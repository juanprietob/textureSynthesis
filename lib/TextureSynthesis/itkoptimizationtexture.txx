 #ifndef OPTIMIZATIONTEXTURE_TXX
#define OPTIMIZATIONTEXTURE_TXX


#include "itkdiscreteshrinkimagefilter.h"
#include <itkVectorExpandImageFilter.h>
#include <itkImageSliceConstIteratorWithIndex.h>
#include "itkImageFileWriter.h"

#include "itkhistogrammatchingfilter.h"
#include "itkvectordiscretegaussianimagefilter.h"

//#include <sys/time.h>

//#include <itkrgbtolumimagefilter.h>

namespace itk{

template <class TImageSample, class TImageObject, class TX2UImage>
OptimizationTexture <TImageSample, TImageObject,  TX2UImage>::OptimizationTexture()
{

    m_TexelDimension = 3;
    m_R = 0.8;

    m_NeighborhoodSize = 8;
    m_StepSize = 2;

    m_Filename = 0;
    m_Extension = ".png";    

    //m_InputImageSample = 0;
    m_SearchPhase = SearchPhaseType::New();
    //m_SearchPhase->SetEv(&m_Ev);
    m_SearchPhase->SetWp(&m_Wp);    
    m_SearchPhase->SetEvSampleIndex(&m_EvSampleIndex);

    m_Optimization = OptimizationPhaseType::New();

    //m_Optimization->SetEv(&m_Ev);
    m_Optimization->SetWp(&m_Wp);

    m_Optimization->SetEvSampleIndex(&m_EvSampleIndex);

    m_InputImageSamples = 0;


    m_SmoothHistogram = false;

    m_SearchZ = true;
    m_SearchY = true;
    m_SearchX = true;

    m_X2UImage = 0;
}


template <class TImageSample, class TImageObject, class TX2UImage>
void OptimizationTexture <TImageSample, TImageObject,  TX2UImage>::GenerateData(){

    //InputImageSamplePointerType imagesample = this->m_InputImageSample;
    VectorImageSamples *imagesamples = this->m_InputImageSamples;

    if(!imagesamples || imagesamples->size() == 0){
        itkExceptionMacro(<< "Input samples not set!");
    }


    typedef itk::DiscreteShrinkImageFilter< InputImageSampleType, InputImageSampleType > ShrinkFilterSampleType;
    typedef typename ShrinkFilterSampleType::Pointer ShrinkFilterSamplePointerType;
    vector< ShrinkFilterSamplePointerType > vectshrinkfiltersample;

    m_PCAReductions.reserve(imagesamples->size());
    m_ANNConnectors.reserve(imagesamples->size());
    vectshrinkfiltersample.reserve(imagesamples->size());
    for(unsigned i = 0; i < imagesamples->size(); i++){
        m_PCAReductions.push_back(PCAReductionType::New());
        m_PCAReductions[i]->SetKeepValues(0.95);

        m_ANNConnectors.push_back(SampleANNConnectorType::New());

        vectshrinkfiltersample.push_back(ShrinkFilterSampleType::New());
        vectshrinkfiltersample[i]->SetInput((*imagesamples)[i]);
    }


    InputImageObjectPointerType imageobject = const_cast< InputImageObjectType * >(this->GetInput());

    typename X2UImageType::SpacingType x2uSpacing;
    typename X2UImageType::PointType x2uOrigin;
    if(m_X2UImage){
        x2uSpacing = m_X2UImage->GetSpacing();
        x2uOrigin = m_X2UImage->GetOrigin();
        double spacing[3] ={1,1,1};
        m_X2UImage->SetSpacing(spacing);
        double origin[3] ={0,0,0};
        m_X2UImage->SetOrigin(origin);
    }else{
        x2uSpacing.Fill(1);
        x2uOrigin.Fill(0);
    }

    if(!imageobject){
        itkExceptionMacro(<< "Input imageobject not set!");
    }

    typedef itk::ImageFileWriter< InputImageObjectType > ImageWriterType;
    typename ImageWriterType::Pointer map_writer = ImageWriterType::New();

    typedef itk::DiscreteShrinkImageFilter< InputImageObjectType, InputImageObjectType > ShrinkFilterObjectType;
    typename ShrinkFilterObjectType::Pointer shrinkfilterobject = ShrinkFilterObjectType::New();
    shrinkfilterobject->SetShrinkFactors(4);
    shrinkfilterobject->SetInput(imageobject);
    shrinkfilterobject->Update();
    InputImageObjectPointerType imageobjectdown = shrinkfilterobject->GetOutput();


    typedef itk::VectorExpandImageFilter< InputImageObjectType, InputImageObjectType > VectorExpandImageType;


    int i = 0;
    //imageobjectdown = imageobject;


    int numi = 3;


    unsigned int numiter0 = 6;
    unsigned int numiter = numiter0;


    int startclust = 1000;
    double clusterthresh  = startclust;

    int endclust = 1;
    double stepcluster = (startclust - endclust)/(numiter*numi);


    while(i <= numi){

        bool random = false;

        vector< InputImageSamplePointerType >vectshrinkfilteroutput;


        RotationImagePointerType rotationmatriximage = 0;
        InputImageMaskPointerType maskimage = 0;


        if(i == 0){
            random = true;
            numiter = 1;
            m_NeighborhoodSize = 16;
            m_StepSize = 8;

            for(unsigned j = 0; j < vectshrinkfiltersample.size(); j++){
                vectshrinkfiltersample[j]->SetShrinkFactors( 4 );
                //vectshrinkfiltersample[j]->SetVariance(0.75);
                vectshrinkfiltersample[j]->SetVariance(0);
                vectshrinkfiltersample[j]->Update();
                vectshrinkfilteroutput.push_back(vectshrinkfiltersample[j]->GetOutput());
            }


            if(m_X2UImage){

                typedef itk::DiscreteShrinkImageFilter< X2UImageType, X2UImageType > ShrinkFilterX2UType;
                typename ShrinkFilterX2UType::Pointer x2ushrinkfilter = ShrinkFilterX2UType::New();
                x2ushrinkfilter->SetInput(m_X2UImage);
                x2ushrinkfilter->SetShrinkFactors( 4 );
                x2ushrinkfilter->SetVariance(0);
                x2ushrinkfilter->Update();


                X2UMapRotationMatrixImageFilterPointerType x2urotationfilter = X2UMapRotationMatrixImageFilterType::New();

                x2urotationfilter->SetInput(x2ushrinkfilter->GetOutput());
                x2urotationfilter->Update();
                rotationmatriximage = x2urotationfilter->GetOutput();
                maskimage = x2urotationfilter->GetImageMask();
            }




        }else if (i <= 3){

            numiter = numiter0;
            m_NeighborhoodSize = 8;
            m_StepSize = 2;

            for(unsigned j = 0; j < vectshrinkfiltersample.size(); j++){
                vectshrinkfiltersample[j]->SetShrinkFactors( pow((double)2,(int)(3 - i) ) );
                //vectshrinkfiltersample[j]->SetVariance((3-i)/4.0);
                vectshrinkfiltersample[j]->SetVariance(0);
                vectshrinkfiltersample[j]->Update();

                vectshrinkfilteroutput.push_back(vectshrinkfiltersample[j]->GetOutput());
            }

            if(m_X2UImage){

                typedef itk::DiscreteShrinkImageFilter< X2UImageType, X2UImageType > ShrinkFilterX2UType;
                typename ShrinkFilterX2UType::Pointer x2ushrinkfilter = ShrinkFilterX2UType::New();
                x2ushrinkfilter->SetInput(m_X2UImage);
                x2ushrinkfilter->SetShrinkFactors( pow((double)2, (int)(3 - i)) );
                x2ushrinkfilter->SetVariance(0);
                x2ushrinkfilter->Update();

                X2UMapRotationMatrixImageFilterPointerType x2urotationfilter = X2UMapRotationMatrixImageFilterType::New();

                x2urotationfilter->SetInput(x2ushrinkfilter->GetOutput());
                x2urotationfilter->Update();
                rotationmatriximage = x2urotationfilter->GetOutput();
                maskimage = x2urotationfilter->GetImageMask();
            }

        }



        //setupANN(vectshrinkfiltersample[0]->GetOutput());


        setupANN(&vectshrinkfilteroutput);


        cout<<"iter = "<<i<<":"<<endl;


        for(unsigned j = 0; j < numiter ; j++){

            double energy = searchPhase(imageobjectdown, vectshrinkfilteroutput[vectshrinkfilteroutput.size() - 1], random, rotationmatriximage);

            //cout<<j<<"\t, "<<energy<<endl;
            printf("%d \t, %f\n",j, energy);

            //timeval tim;
            //gettimeofday(&tim, NULL);
            //double t1=tim.tv_sec+(tim.tv_usec/1000000.0);

            optimizationPhaseMultiThread(imageobjectdown, &vectshrinkfilteroutput, clusterthresh );

            clusterthresh -= stepcluster;

            /*if(m_Filename){
		    std::string filename(m_Filename);
		    char buffer[10];
		    sprintf(buffer, "%d-0-%d", i, j);
		    filename.append( buffer );
		    filename.append(m_Extension);
		    map_writer->SetFileName(filename.c_str());
		    map_writer->SetInput( imageobjectdown );
		    map_writer->Update();
                }*/


            //if(j < numiter -1 || i < numi){
                typedef itk::HistogramMatchingFilter<InputImageSampleType, InputImageObjectType > HistogramMatchingType;
                typename HistogramMatchingType::Pointer histogrammatch = HistogramMatchingType::New();

                histogrammatch->SetInputImageSource(vectshrinkfilteroutput[vectshrinkfilteroutput.size() - 1]);
                //histogrammatch->SetInputImageSource(vectshrinkfilteroutput[0]);
                histogrammatch->SetInputImageObject(imageobjectdown);
                //if(maskimage){
                //    histogrammatch->SetImageMask(maskimage);
                //}
                histogrammatch->SetTexelDimension(m_TexelDimension);
                histogrammatch->Update();
                if(histogrammatch->GetCalculateOutputImage()){
                    imageobjectdown = histogrammatch->GetOutput();
                }
            //}
                


		

            //gettimeofday(&tim, NULL);
            //double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
            //printf("%.6lf seconds elapsed\n", t2-t1);


        }


        if(i < numi && i != 0){

            typename VectorExpandImageType::Pointer vectorexpandimage = VectorExpandImageType::New();
            vectorexpandimage->SetExpandFactors(2);
            vectorexpandimage->SetInput(imageobjectdown);
            vectorexpandimage->Update();
            imageobjectdown = vectorexpandimage->GetOutput();
        }


        if(m_Filename){
            std::string filename(m_Filename);
            char buffer[10];
            sprintf(buffer, "%d-1", i);
            filename.append( buffer );
            filename.append(m_Extension);
            map_writer->SetFileName(filename.c_str());

            map_writer->SetInput( imageobjectdown );
            map_writer->Update();
        }

        i++;
    }

    if(m_SmoothHistogram){
        typedef typename itk::VectorDiscreteGaussianImageFilter< OutputImageObjectType, OutputImageObjectType > VectorDiscreteGaussianImageFilterType;
        typename VectorDiscreteGaussianImageFilterType::Pointer discretegauss = VectorDiscreteGaussianImageFilterType::New();

        discretegauss->SetInput(imageobjectdown);
        discretegauss->SetVariance(0.25);
        discretegauss->Update();
        imageobjectdown = discretegauss->GetOutput();
    }

    OutputImageObjectPointerType imageout = this->GetOutput();
    imageout->SetRegions(imageobjectdown->GetLargestPossibleRegion());
    imageout->Allocate();

    RegionIteratorType imgoutit(imageout, imageout->GetLargestPossibleRegion());
    RegionIteratorType imageobjectdownit(imageobjectdown, imageobjectdown->GetLargestPossibleRegion());

    imageobjectdownit.GoToBegin();
    imgoutit.GoToBegin();

    while(!imgoutit.IsAtEnd()){
        imgoutit.Set(imageobjectdownit.Get());

        ++imgoutit;
        ++imageobjectdownit;
    }

    imageout->SetSpacing(x2uSpacing);
    imageout->SetOrigin(x2uOrigin);



}

/**
 *  Setup the closest neighborhood space of the imagesample
 */
template <class TImageSample, class TImageObject, class TX2UImage>
void OptimizationTexture <TImageSample, TImageObject,  TX2UImage>::setupANN(vector< InputImageSamplePointerType >* imagesamples){
    //typename ImageFeaturesType::Pointer imageFeatures;

    m_ColorSamples.clear();
    for(unsigned i = 0; i < imagesamples->size(); i++){
        typename ImageFeaturesType::Pointer imagefeatures = ImageFeaturesType::New();

        imagefeatures->SetNeighborhoodSize(m_NeighborhoodSize);
        imagefeatures->SetStepSize(2);
        imagefeatures->SetTexelDimension(m_TexelDimension);
        imagefeatures->SetInput((*imagesamples)[i]);
        imagefeatures->Update();

        /**
          * sample of texels
          */
        //ListSampleType *colorSample;
        ListSampleType* colorsample = const_cast<ListSampleType*>(imagefeatures->GetListSample());
        m_ColorSampleSize = colorsample->GetMeasurementVectorSize();

        m_PCAReductions[i]->SetInputSample(colorsample);
        //m_PCAReductions[i]->DebugOn();
        m_PCAReductions[i]->Update();

        m_ANNConnectors[i]->SetInputSample(m_PCAReductions[i]->GetOutput());
        m_ANNConnectors[i]->Update();
        m_ANNConnectors[i]->SetKNearNeighbours(2);

        m_ColorSamples.push_back(const_cast<ListSampleType*>(colorsample));
    }

}

/**
 * optimization algorithm to find the texture at multiple neighborhoods
 * Uses multithread
 */
template <class TImageSample, class TImageObject, class TX2UImage>
void OptimizationTexture <TImageSample, TImageObject,  TX2UImage>::optimizationPhaseMultiThread(
                                                                        InputImageObjectPointerType& imageobject,
                                                                        VectorImageSamples*  imagesamples,
                                                                        double  clusterthreshold
                                                                        ){


    m_Optimization->SetInput(imageobject);
    m_Optimization->SetInputSamples(imagesamples);

    m_Optimization->SetNeighborhoodSize(m_NeighborhoodSize);
    m_Optimization->SetTexelDimension(m_TexelDimension);

    //m_Optimization->SetColorSample(m_ColorSample);
    m_Optimization->SetColorSamples(&m_ColorSamples);
    m_Optimization->SetInputIndexImage(m_SearchPhase->GetOutput());//image with the map of indexes for the vectors m_Sv m_Ev m_EvSampleIndex m_EvImageIndex
    m_Optimization->SetClusterThreshold(clusterthreshold);    
    m_Optimization->SetArrayIndexImage(m_SearchPhase->GetArrayIndexImage());

    m_Optimization->Modified();
    m_Optimization->Update();

    imageobject = m_Optimization->GetOutput();



}

/**
 * Search the ANN space for the closest neighborhoods in the exemplar
 * Uses multithread and a modification of ann lib
 * @returns bool if the current search is equal to the previous one
 *          this usually means the current level has converged
 */
template <class TImageSample, class TImageObject, class TX2UImage>
double OptimizationTexture <TImageSample, TImageObject,  TX2UImage>::searchPhase(InputImageObjectPointerType imageobject, InputImageSamplePointerType imagesample, bool random, RotationImagePointerType rotationimage){


    m_SearchPhase->SetInput(imageobject);
    m_SearchPhase->Modified();


    m_SearchPhase->SetNeighborhoodSize(m_NeighborhoodSize);
    m_SearchPhase->SetStepSize(m_StepSize);
    m_SearchPhase->SetTexelDimension(m_TexelDimension);

    m_SearchPhase->SetANNConnectors(&m_ANNConnectors);
    m_SearchPhase->SetPCAReductions(&m_PCAReductions);

    m_SearchPhase->SetColorSampleSize(m_ColorSampleSize);
    m_SearchPhase->SetRandomSearch(random);
    m_SearchPhase->SetSearchZ(m_SearchZ);
    m_SearchPhase->SetSearchY(m_SearchY);
    m_SearchPhase->SetSearchX(m_SearchX);

    m_SearchPhase->SetRotationImage(rotationimage);

    m_SearchPhase->Update();


    return m_SearchPhase->GetEnergy();
}


}//namespace itk

#endif
