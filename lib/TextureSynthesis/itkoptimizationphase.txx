#ifndef ITKOPTIMIZATIONPHASE_TXX
#define ITKOPTIMIZATIONPHASE_TXX


#include "itkVectorLinearInterpolateImageFunction.h"
//#include "itkVectorNearestNeighborInterpolateImageFunction.h"


namespace itk{

template< class TInputImage, class TInputImageSample, class TOutputImage, class TInputIndexImage, class TX2UImageMask >
OptimizationPhase<TInputImage,TInputImageSample, TOutputImage, TInputIndexImage, TX2UImageMask >::OptimizationPhase()
{
    m_NeighborhoodSize = 8;
    m_NeighborhoodSizOld = 0;
    m_TexelDimension = 3;
    m_ClusterThreshold = 500;

    //m_Ev = 0;
    m_Wp = 0;

    m_EvSampleIndex = 0;
    //m_ColorSample = 0;
    m_ColorSamples = 0;

    m_ArrayIndexImage = 0;
}


template< class TInputImage, class TInputImageSample, class TOutputImage, class TInputIndexImage, class TX2UImageMask >
OptimizationPhase<TInputImage,TInputImageSample, TOutputImage, TInputIndexImage, TX2UImageMask >::~OptimizationPhase()
{
}


/** If an imaging filter can be implemented as a multithreaded
 * algorithm, the filter will provide an implementation of
 * ThreadedGenerateData().  This superclass will automatically split
 * the output image into a number of pieces, spawn multiple threads,
 * and call ThreadedGenerateData() in each thread. Prior to spawning
 * threads, the BeforeThreadedGenerateData() method is called. After
 * all the threads have completed, the AfterThreadedGenerateData()
 * method is called. If an image processing filter cannot support
 * threading, that filter should provide an implementation of the
 * GenerateData() method instead of providing an implementation of
 * ThreadedGenerateData().  If a filter provides a GenerateData()
 * method as its implementation, then the filter is responsible for
 * allocating the output data.  If a filter provides a
 * ThreadedGenerateData() method as its implementation, then the
 * output memory will allocated automatically by this superclass.
 * The ThreadedGenerateData() method should only produce the output
 * specified by "outputThreadRegion"
 * parameter. ThreadedGenerateData() cannot write to any other
 * portion of the output image (as this is responsibility of a
 * different thread).
 *
 * \sa GenerateData(), SplitRequestedRegion() */
template< class TInputImage, class TInputImageSample, class TOutputImage, class TInputIndexImage, class TX2UImageMask >
void OptimizationPhase<TInputImage,TInputImageSample, TOutputImage, TInputIndexImage, TX2UImageMask >::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId ){


    unsigned int neighborhoodsize = m_NeighborhoodSize/2;    
    OutputImagePointer imageobject = this->GetOutput();    
    unsigned int imagedim = imageobject->GetLargestPossibleRegion().GetImageDimension();

    typename NeighborhoodIteratorInputIndexImageType::RadiusType radius;
    radius.Fill(neighborhoodsize);
    NeighborhoodIteratorInputIndexImageType indexit(radius, m_InputIndexImage, m_InputIndexImage->GetLargestPossibleRegion());
    indexit.NeedToUseBoundaryConditionOn();

    typedef itk::ImageRegionIterator< InputArrayIndexImageType > ArrayIndexImageIteratorType;
    ArrayIndexImageIteratorType arrayindexit;

    if(m_ArrayIndexImage){
        ArrayIndexImageIteratorType temp(m_ArrayIndexImage, outputRegionForThread);
        temp.GoToBegin();
        arrayindexit = temp;
    }    

    //if(threadId == 0)
    {
        //RandomIteratorType outputit(imageobject, outputRegionForThread);
        //outputit.SetNumberOfSamples(outputRegionForThread.GetNumberOfPixels());
        //outputit.ReinitializeSeed();

        RegionIteratorOutputType outputit(imageobject, outputRegionForThread);
        //RegionIteratorOutputType outputit(imageobject, imageobject->GetLargestPossibleRegion());
        outputit.GoToBegin();

        while(!outputit.IsAtEnd()){


            IndexType currentposition = outputit.GetIndex();            
            indexit.SetLocation(currentposition);

            OutputPixelType finaltexel;
            finaltexel.Fill(0);

            vector< double > vectorweight;
            typename ListSampleDoubleType::Pointer texelvector = ListSampleDoubleType::New();
            texelvector->SetMeasurementVectorSize(m_TexelDimension);

            if(m_ArrayIndexImage){
                arrayindexit.SetIndex(currentposition);

                InputArrayIndexImagePixelType indexarray = arrayindexit.Get();

                for(unsigned i = 0; i < 3; i++){
		  
                    int colorsampleindex = 0;
                    if(i < m_ColorSamples->size()){
                         colorsampleindex = i;
                    }

                    const ListSampleType* colorsample = (*m_ColorSamples)[colorsampleindex];

                    for(int j = 0; j < indexarray[i][0]; j+=2){

                        int objectposition = indexarray[i][j + 1];
                        int position = indexarray[i][j + 2];

                        typename PCAReductionType::MeasurementVectorType closesttexture;
                        closesttexture = colorsample->GetMeasurementVector((*m_EvSampleIndex)[objectposition]);


                        SampleVectorType texel;//for clustering
                        texel.SetSize(m_TexelDimension);
                        texel.Fill(0);

                        for(unsigned k = 0; k < m_TexelDimension; k++){
                            double t = (double)(closesttexture[position + k]);
                            texel[k] = t;
                        }

                        texelvector->PushBack(texel);


                        double weight = (*m_Wp)[objectposition];//weight of the neighborhood calculated in energy

                        vectorweight.push_back(weight);
                    }
                }

            }else{


                for(unsigned off = 0; off < m_OffsetVector.size(); off++){

                    const ListSampleType* colorsample = 0;
                    if(off < m_ColorSamples->size()){
                         colorsample = (*m_ColorSamples)[off];
                    }else{
                         colorsample = (*m_ColorSamples)[0];
                    }

                    for (unsigned i = 0; i < m_OffsetVector[off].size(); i++){

                        bool inbounds = false;

                        InputIndexImagePixelType indexSv;
                        indexSv.Fill(-1);


                        OffsetType offset = m_OffsetVector[off][i];
                        indexSv = indexit.GetPixel(offset, inbounds);
                        double distoffset = 0;
                        for(unsigned k = 0; k < offset.GetOffsetDimension(); k++){
                            distoffset += pow(offset[k], 2.0);
                        }
                        distoffset = sqrt(distoffset);

                        if(inbounds && indexSv[off] >= 0){

                            int objectposition = indexSv[off];
                            //cout<<objectposition<<endl;
                            double weight = (*m_Wp)[objectposition];//weight of the neighborhood calculated in energy
                            //weight *= exp(-distoffset);//Gaussian fallout

                            vectorweight.push_back(weight);

                            typename PCAReductionType::MeasurementVectorType closesttexture;
                            //cout<<(*m_EvSampleIndex)[objectposition]<<endl;
                            closesttexture = colorsample->GetMeasurementVector((*m_EvSampleIndex)[objectposition]);


                            //calculating the position in the closestexture
                            int position = 0, x = 0, y = 0;
                            IndexType centerindexpos = indexit.GetIndex(m_OffsetVector[off][i]);

                            if(imagedim == 3){

                                if(off == 0){
                                    x = currentposition[0] - (centerindexpos[0] - neighborhoodsize);
                                    y = currentposition[1] - (centerindexpos[1] - neighborhoodsize);
                                }else if(off == 1){
                                    x = currentposition[0] - (centerindexpos[0] - neighborhoodsize);
                                    y = currentposition[2] - (centerindexpos[2] - neighborhoodsize);
                                }else if(off == 2){
                                    //x = currentposition[1] - (centerindexpos[1] - neighborhoodsize);
                                    //y = currentposition[2] - (centerindexpos[2] - neighborhoodsize);
                                    x = currentposition[2] - (centerindexpos[2] - neighborhoodsize);
                                    y = currentposition[1] - (centerindexpos[1] - neighborhoodsize);
                                }
                            }else if(imagedim == 2){
                                x = currentposition[0] - (centerindexpos[0] - neighborhoodsize);
                                y = currentposition[1] - (centerindexpos[1] - neighborhoodsize);
                            }

                            position = y*(m_NeighborhoodSize + 1);
                            position += x;
                            position *= m_TexelDimension;

                            //vector< double > texel;

                            SampleVectorType texel;//for clustering
                            texel.SetSize(m_TexelDimension);
                            texel.Fill(0);

                            for(unsigned k = 0; k < m_TexelDimension; k++){
                                double t = (double)(closesttexture[position + k]);
                                texel[k] = t;
                            }

                            texelvector->PushBack(texel);

                        }

                    }

                }
            }

            if(texelvector->Size() > 0){


                typename SampleMeanShiftType::Pointer meanshift = SampleMeanShiftType::New();
                meanshift->SetInputSample(texelvector);
                meanshift->SetMinimumClusterSize( 1 );
                meanshift->SetThreshold( m_ClusterThreshold ) ;//radius
                meanshift->Update() ;

                typename SampleMeanShiftType::ClusterLabelsType texelvectorlabels = meanshift->GetOutput();
                map< int, int> clustermap;

                int clusterlabel = -1;

                for(unsigned i = 0; i < texelvectorlabels.size(); i++){
                    if(clustermap.find(texelvectorlabels[i]) != clustermap.end()){
                        clustermap[texelvectorlabels[i]] += 1;
                    }else{
                        clustermap[texelvectorlabels[i]] = 1;
                    }
                }

                map<int ,int >::iterator it;

                int clustercount = 0;
                for ( it=clustermap.begin() ; it != clustermap.end(); it++ ){
                    if((*it).second > clustercount){
                        clusterlabel = (*it).first;
                        clustercount = (*it).second;
                    }
                }

                if(clusterlabel == -1){
                    itkExceptionMacro(<<"Cluster label does not exist");
                }



                double sumweight = 0;
                double *sumtexel = new double[m_TexelDimension];
                for(unsigned i = 0; i < m_TexelDimension; i++) sumtexel[i] = 0;

                for(unsigned i = 0; i < vectorweight.size();i++){

                    if(texelvectorlabels[i] == clusterlabel || clusterlabel == -1){

                        double weight = vectorweight[i];
                        SampleVectorType samplevect = texelvector->GetMeasurementVector(i);

                        MeasurementVectorType measuresample(samplevect.Size());
                        for(unsigned j = 0; j < samplevect.Size(); j++){
                            measuresample[j] = samplevect[j];
                        }

                        //histo matching does not work
                        /*double sumhist = 0;
                        for(unsigned j = 0; j < samplevect.Size(); j++){

                            double freqsource = m_HistogramMatch->GetFrequencySource(j, measuresample, true);
                            freqsource*=256;
                            double freqobject = m_HistogramMatch->GetFrequencyObject(j, measuresample, true);
                            freqobject*=256;

                            sumhist += max(0.0, freqobject - freqsource);
                        }
                        sumhist += 1;
                        weight = weight/sumhist;*/

                        sumweight += weight;
                        for(unsigned j = 0; j < samplevect.Size(); j++){
                            sumtexel[j] += ((double)samplevect[j]) * weight;
                        }

                    }

                }



                for(unsigned i = 0; i < m_TexelDimension && i < finaltexel.Size(); i++){
                    finaltexel[i] =(OutputPixelComponentType)(sumtexel[i] / sumweight);
                }
                delete[] sumtexel;

            }

            vectorweight.clear();
            texelvector->Clear();


            //histo matching does not work
            /*OutputPixelType oldpix = outputit.Get();

            MeasurementVectorType measuresample(oldpix.Size());
            for(unsigned j = 0; j < oldpix.Size(); j++){
                measuresample[j] = oldpix[j];
            }
            MeasurementVectorType measuresample1(finaltexel.Size());
            for(unsigned j = 0; j < finaltexel.Size(); j++){
                measuresample1[j] = finaltexel[j];
            }
            m_HistogramMatch->UpdateFrequencyObject(measuresample, measuresample1);*/

            outputit.Set(finaltexel);
            ++outputit;            
        }
    }    
}


/** The GenerateData method normally allocates the buffers for all of the
 * outputs of a filter. Some filters may want to override this default
 * behavior. For example, a filter may have multiple outputs with
 * varying resolution. Or a filter may want to process data in place by
 * grafting its input to its output. */
template< class TInputImage, class TInputImageSample, class TOutputImage, class TInputIndexImage, class TX2UImageMask >
void OptimizationPhase<TInputImage,TInputImageSample, TOutputImage, TInputIndexImage, TX2UImageMask >::AllocateOutputs(){

    OutputImagePointer imageobject = this->GetOutput();

    if(m_ImageRegion != m_Input->GetLargestPossibleRegion()){
        m_ImageRegion = m_Input->GetLargestPossibleRegion();

        imageobject->SetRegions(m_ImageRegion);
        imageobject->SetOrigin(m_Input->GetOrigin());
        imageobject->SetSpacing(m_Input->GetSpacing());
        imageobject->Allocate();

        RegionIteratorOutputType inputit(m_Input, m_Input->GetLargestPossibleRegion());
        inputit.GoToBegin();

        RegionIteratorOutputType outputit(imageobject, imageobject->GetLargestPossibleRegion());
        outputit.GoToBegin();

        while(!outputit.IsAtEnd()){
            outputit.Set(inputit.Get());
            ++outputit;
            ++inputit;
        }
    }

    InputImagePixelType texel;
    texel.Fill(0);
    imageobject->FillBuffer(texel);
}

/** If an imaging filter needs to perform processing after the buffer
 * has been allocated but before threads are spawned, the filter can
 * can provide an implementation for BeforeThreadedGenerateData(). The
 * execution flow in the default GenerateData() method will be:
 *      1) Allocate the output buffer
 *      2) Call BeforeThreadedGenerateData()
 *      3) Spawn threads, calling ThreadedGenerateData() in each thread.
 *      4) Call AfterThreadedGenerateData()
 * Note that this flow of control is only available if a filter provides
 * a ThreadedGenerateData() method and NOT a GenerateData() method. */
template< class TInputImage, class TInputImageSample, class TOutputImage, class TInputIndexImage, class TX2UImageMask >
void OptimizationPhase<TInputImage,TInputImageSample, TOutputImage, TInputIndexImage, TX2UImageMask >::BeforeThreadedGenerateData(){

    //The following image are initialized to hold the values of each pixel on the object
    //m_Imagesumsv will hold the values of the Sum of the weigh * texture according to equation (4) of Solid Texture Synthesis from 2D exemplars [Kopf et al]
    //unsigned int neighborhoodsize = m_NeighborhoodSize/2;    

    //srand(time(0));

    if(!m_ColorSamples){
        itkExceptionMacro(<<"ColorSamples Not Set")
    }

    InputImagePointer imageobject = this->GetInput();
    unsigned int imagedim = imageobject->GetLargestPossibleRegion().GetImageDimension();

    int neighborhoodsize = (m_NeighborhoodSize)/2;

    if(m_NeighborhoodSizOld != m_NeighborhoodSize){
        m_NeighborhoodSizOld = m_NeighborhoodSize;
        m_OffsetVector.clear();
    }

    if(imagedim == 2 && m_OffsetVector.size() == 0){

        vector< OffsetType > offsetxy;

        for(int j = -1*((int)neighborhoodsize); j <= (int)neighborhoodsize; j++){
            for(int i = -1*((int)neighborhoodsize); i <= (int)neighborhoodsize; i++){
                OffsetType offset;
                offset[0] = i;
                offset[1] = j;

                offsetxy.push_back(offset);
            }
        }

        m_OffsetVector.push_back(offsetxy);

    } else if(imagedim == 3 && m_OffsetVector.size() == 0){

        vector< OffsetType > offsetz;
        vector< OffsetType > offsety;
        vector< OffsetType > offsetx;


        for(int j = -1*((int)neighborhoodsize); j <= (int)neighborhoodsize; j++){
            for(int i = -1*((int)neighborhoodsize); i <= (int)neighborhoodsize; i++){


                OffsetType offset;

                offset[0] = i;
                offset[1] = j;
                offset[2] = 0;
                offsetz.push_back(offset);

                offset[0] = i;
                offset[1] = 0;
                offset[2] = j;

                offsety.push_back(offset);

                offset[0] = 0;
                offset[1] = j;
                offset[2] = i;

                offsetx.push_back(offset);

            }
        }


        m_OffsetVector.clear();
        m_OffsetVector.push_back(offsetz);
        m_OffsetVector.push_back(offsety);
        m_OffsetVector.push_back(offsetx);

    }

    m_HistogramMatch = HistogramMatchingType::New();
    m_HistogramMatch->SetInputImageSource((*m_InputSamples)[(*m_InputSamples).size() - 1]);
    m_HistogramMatch->SetInputImageObject(imageobject);
    m_HistogramMatch->SetTexelDimension(m_TexelDimension);
    m_HistogramMatch->SetCalculateOutputImage(false);
    m_HistogramMatch->SetNBins(256);
    m_HistogramMatch->Update();

}

/** If an imaging filter needs to perform processing after all
 * processing threads have completed, the filter can can provide an
 * implementation for AfterThreadedGenerateData(). The execution
 * flow in the default GenerateData() method will be:
 *      1) Allocate the output buffer
 *      2) Call BeforeThreadedGenerateData()
 *      3) Spawn threads, calling ThreadedGenerateData() in each thread.
 *      4) Call AfterThreadedGenerateData()
 * Note that this flow of control is only available if a filter provides
 * a ThreadedGenerateData() method and NOT a GenerateData() method. */
template< class TInputImage, class TInputImageSample, class TOutputImage, class TInputIndexImage, class TX2UImageMask >
void OptimizationPhase<TInputImage,TInputImageSample, TOutputImage, TInputIndexImage, TX2UImageMask >::AfterThreadedGenerateData(){


}


}
#endif
