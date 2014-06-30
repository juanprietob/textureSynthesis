#ifndef ITKSEARCHPHASE_TXX
#define ITKSEARCHPHASE_TXX


#include "itkImageSliceConstIteratorWithIndex.h"


#include "itkImageFileWriter.h"
#include <itkVectorLinearInterpolateImageFunction.h>
#include "itkVectorNearestNeighborInterpolateImageFunction.h"

#include "itkImageRegionIteratorWithIndex.h"

namespace itk{

template< class TInputImage, class TOutput >
SearchPhase<TInputImage, TOutput>::SearchPhase(){

    m_NeighborhoodSize = 0;
    m_NeighborhoodSizOld = 0;
    m_TexelDimension = 0;

    m_ColorSampleSize = 0;
    m_StepSize = 0;
    m_StepSizeOld = 0;
    m_SearchState = 0;

    m_Input = 0;

    //m_Ev = 0;

    m_EvSampleIndex = 0;


    m_RandomSearch = false;

    //m_ANNConnector = 0;
    //m_PCAReduction = 0;

    m_SearchZ = true;
    m_SearchY = true;
    m_SearchX = true;

    m_RotationImage = 0;
    m_ArrayIndexImage = 0;
}

template< class TInputImage, class TOutput >
SearchPhase<TInputImage, TOutput>::~SearchPhase(){

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
template< class TInputImage, class TOutput >
//void SearchPhase<TInputImage, TOutput>::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId ){
void SearchPhase<TInputImage, TOutput>::GenerateData(){


    InputImagePointer imageobject = this->GetInput();    

    unsigned int neighborhoodsize = m_NeighborhoodSize/2;


    typedef itk::ImageRegionIteratorWithIndex< RotationImageType > RotationNeighborhoodIteratorType;
    RotationNeighborhoodIteratorType rotationit;

    typedef itk::VectorNearestNeighborInterpolateImageFunction< InputImageType > VectorInterpolateType;
    typedef typename VectorInterpolateType::Pointer VectorInterpolatePointerType;
    VectorInterpolatePointerType interpolate = 0;


    typedef itk::ImageRegionIteratorWithIndex< ArrayIndexImageType > ImageIndexIteratorType;
    ImageIndexIteratorType imageindexit;

    if(m_RotationImage){
        RotationNeighborhoodIteratorType temp(m_RotationImage, m_RotationImage->GetLargestPossibleRegion());
        temp.GoToBegin();

        rotationit = temp;        

        interpolate = VectorInterpolateType::New();
        
        interpolate->SetInputImage(imageobject);


        ImageIndexIteratorType tempindexit(m_ArrayIndexImage,m_ArrayIndexImage->GetLargestPossibleRegion() );
        tempindexit.GoToBegin();
        imageindexit = tempindexit;


    }

    typedef itk::NeighborhoodIterator< InputImageType > NeighborhoodIteratorType;
    typename NeighborhoodIteratorType::RadiusType radius;
    radius.Fill(neighborhoodsize);
    NeighborhoodIteratorType objectit(radius, imageobject, imageobject->GetLargestPossibleRegion());
    objectit.NeedToUseBoundaryConditionOn();
    objectit.GoToBegin();


    TexturesVectorsType Sv;

    //OutputRegionIteratorType outit( this->GetOutput(), outputRegionForThread);
    OutputRegionIteratorType outit( this->GetOutput(), this->GetOutput()->GetLargestPossibleRegion());

    outit.GoToBegin();

    srand ( time(NULL) );
    //if(threadId == 0)
    {
        while(!outit.IsAtEnd()){            

            OutputImageIndexType currentoutindex = outit.GetIndex();
            objectit.SetLocation(currentoutindex);

            //Creating the vectors s_vi with the corresponding neighborhoods in the object image

            for(unsigned i = 0; i < m_OffsetVector.size();i++){//for the three dimensions
                SampleANNConnectorPointerType annconnector = 0;
                PCAReductionPointerType pcareduction = 0;
                unsigned annsample = 0;
                if(i < m_ANNConnectors->size()){
                    annsample = i;
                }
                
                annconnector = (*m_ANNConnectors)[annsample];
                pcareduction = (*m_PCAReductions)[annsample];
                //annconnector->DebugOn();
                //pcareduction->DebugOn();
                
                RotationImagePixelType rotationmatrix;
		
                if(m_RotationImage){
		    
                    rotationit.SetIndex(currentoutindex);
                    rotationmatrix = rotationit.Get();
		    
                    /*vnl_vector<double> offsetvnl(m_OffsetVector.size(), 0);
                    offsetvnl[i] = 1;

                    //offsetvnl = rotationmatrix.GetTranspose  () * offsetvnl;
                    offsetvnl = rotationmatrix * offsetvnl;
		    
                    rotationmatrix[0][0] = 1 - 2 * (offsetvnl[1]*offsetvnl[1] + offsetvnl[2] * offsetvnl[2] );
                    rotationmatrix[1][0] = 2 * (offsetvnl[0]*offsetvnl[1] );
                    rotationmatrix[2][0] = 2 * (offsetvnl[0] * offsetvnl[2]);

                    rotationmatrix[0][1] = 2 * (offsetvnl[0] * offsetvnl[1]);
                    rotationmatrix[1][1] = 1 - 2 * (offsetvnl[0] * offsetvnl[0] + offsetvnl[2] * offsetvnl[2]);
                    rotationmatrix[2][1] = 2 * (offsetvnl[1] * offsetvnl[2]);

                    rotationmatrix[0][2] = 2 * (offsetvnl[0] * offsetvnl[2]);
                    rotationmatrix[1][2] = 2 * (offsetvnl[1] * offsetvnl[2]);
                    rotationmatrix[2][2] = 1 - 2 * (offsetvnl[0] * offsetvnl[0] + offsetvnl[1] * offsetvnl[1]);*/
                }

                if(outit.Get()[i] >= 0){
                    unsigned positionvector = 0;

                    typename PCAReductionType::MeasurementVectorType s_vxyz;
                    s_vxyz.SetSize(m_ColorSampleSize);
                    s_vxyz.Fill(-1);                    

                    int indexsample = 0;

                    //creation of the vector with the values from the object
                    for(unsigned j = 0; j < m_OffsetVector[i].size();j++){//for all the offset values in the current dimension
                        InputImagePixelType objectpixel;
                        objectpixel.Fill(0);

                        if(m_RotationImage){

                            vnl_vector<double> offsetvnl(m_OffsetVector.size(), 0);
                            offsetvnl[0] = m_OffsetVector[i][j][0];
                            offsetvnl[1] = m_OffsetVector[i][j][1];
                            if(m_TexelDimension > 2)
                                offsetvnl[2] = m_OffsetVector[i][j][2];

                            vnl_vector<double> newoffset = rotationmatrix * offsetvnl;

                            typename VectorInterpolateType::ContinuousIndexType pointinterp;

                            //typename OutputImageType::SpacingType spacing = this->GetOutput()->GetSpacing();

                            pointinterp[0] = (newoffset[0] + currentoutindex[0]);
                            pointinterp[1] = (newoffset[1] + currentoutindex[1]);
                            if(m_TexelDimension > 2)
                                pointinterp[2] = (newoffset[2] + currentoutindex[2]);

                            if(interpolate->IsInsideBuffer (pointinterp)){

                                objectpixel = interpolate->EvaluateAtContinuousIndex(pointinterp);

                                typename VectorInterpolateType::IndexType nearestindex;
                                interpolate->ConvertContinuousIndexToNearestIndex (pointinterp, nearestindex);

                                imageindexit.SetIndex(nearestindex);

                                ArrayIndexImagePixelType arrayindex = imageindexit.Get();				

                                int currentsize = arrayindex[i][0];
                                if(currentsize == -1){
                                    currentsize = 0;
                                }

                                if(currentsize + 2 < MAXVECT){//max size of values in the vector
                                    arrayindex[i][0] = currentsize + 2;//size of the vector
                                    arrayindex[i][currentsize + 1] = outit.Get()[i];//index of the sample
                                    arrayindex[i][currentsize + 2] = positionvector;//position in the vector

                                    imageindexit.Set(arrayindex);
                                }

                            }


                        }else{
                            objectpixel = objectit.GetPixel(m_OffsetVector[i][j]);
                        }

                        for(unsigned k = 0; k < m_TexelDimension; k++){//for multidimension texels
                            s_vxyz.SetElement(positionvector+k, objectpixel[k]);
                        }
                        positionvector+= m_TexelDimension;
                    }

                    vnl_vector<double> reducedvector = pcareduction->GetPCAReducedVector(s_vxyz);//for each vector PCA reduce dimension is used

                    if(!m_RandomSearch){

                        //int* tempindexsample = new int[annconnector->GetKNearNeighbours()];
                        //annconnector->Evaluate(reducedvector, tempindexsample, distance);//evaluate the reduced vector for the closest Neighbour
                        annconnector->Evaluate(reducedvector);
                        indexsample = annconnector->GetNearestNeighbours()[0];
			//cout<<indexsample<<endl;
                        //indexsample = tempindexsample[0];
                        //delete[] tempindexsample;

                        /*if(indexsample == -1){                           

                            for(int l = 1; l < annconnector->GetKNearNeighbours(); l++){
                                if(annconnector->GetNearestNeighbours()[l] != -1){
                                    indexsample = annconnector->GetNearestNeighbours()[l];
                                    break;
                                }
                            }
                            if(indexsample == -1){
                                //annconnector->Evaluate(reducedvector, indexsample, distance);
                                annconnector->Evaluate(reducedvector);
                                indexsample = annconnector->GetNearestNeighbours()[0];
                            }

                        }*/


                        itkDebugMacro(<<"The index sample is: "<<indexsample<<" ");
                    }else{
                        indexsample = rand() % pcareduction->GetInputSample()->Size() ;
                    }

                    vnl_vector< double > reducedclosest = pcareduction->GetMeasurementVector(indexsample);

                    double wp = -1;
                    double tempenergy = 0;

                    for(unsigned j = 0; j < reducedvector.size(); j++){ //for the reduced object vector

                        tempenergy+= pow(reducedvector[j] - reducedclosest[j], 2);

                    }
                    if(tempenergy > 0){
                        wp = sqrt(tempenergy);

                        if(wp > 0){
                            wp = pow(wp, 0.8 - 2.0);
                            m_Energy += wp * tempenergy;
                        }
                    }
                    (*m_Wp)[outit.Get()[i]] = wp ;
                    //(*m_Ev)[outit.Get()[i]] = reducedclosest;
                    (*m_EvSampleIndex)[outit.Get()[i]] = indexsample;                    

                    //delete[] distance;
                }
            }
            ++outit;
        }
        //return energy;

        m_SearchState = true;
    }
}


/** The GenerateData method normally allocates the buffers for all of the
 * outputs of a filter. Some filters may want to override this default
 * behavior. For example, a filter may have multiple outputs with
 * varying resolution. Or a filter may want to process data in place by
 * grafting its input to its output. */
template< class TInputImage, class TOutput >
//void SearchPhase<TInputImage, TOutput>::AllocateOutputs(){
void SearchPhase<TInputImage, TOutput>::GenerateOutputInformation(){

    //InputImagePointer imageobject = this->GetInput();

    if(m_RotationImage){
        m_ArrayIndexImage = ArrayIndexImageType::New();
        m_ArrayIndexImage->SetRegions(m_RotationImage->GetLargestPossibleRegion());
        m_ArrayIndexImage->Allocate();	
	
	ArrayIndexImagePixelType arrayindex;
        arrayindex.Fill(-1);
	
	m_ArrayIndexImage->FillBuffer(arrayindex);
	
    }
//}

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
//template< class TInputImage, class TOutput >
//void SearchPhase<TInputImage, TOutput>::BeforeThreadedGenerateData(){

    InputImagePointer imageobject = this->GetInput();
    imageobject->SetRequestedRegionToLargestPossibleRegion ();

    if(!m_ANNConnectors ){
        itkExceptionMacro(<<"AnnConnector not initialized")

    }
    if(!m_PCAReductions ){
        itkExceptionMacro(<<"PcaReduction not initialized")
    }

    unsigned int neighborhoodsize = m_NeighborhoodSize/2;
    m_Energy = 0;


    if(m_NeighborhoodSizOld != m_NeighborhoodSize || m_StepSizeOld != m_StepSize){
        m_NeighborhoodSizOld = m_NeighborhoodSize;
        m_StepSizeOld = m_StepSize;
        m_OffsetVector.clear();

        m_ImageRegion.SetSize(0, 0);
        m_ImageRegion.SetSize(1, 0);
    }

    unsigned int imagedim = imageobject->GetLargestPossibleRegion().GetImageDimension();
    if(imagedim == 2 && m_OffsetVector.size() == 0){


        vector< OffsetType > offsetx;


        for(int j = -1*((int)neighborhoodsize); j <= (int)neighborhoodsize; j++){
            for(int i = -1*((int)neighborhoodsize); i <= (int)neighborhoodsize; i++){
                OffsetType offset;
                offset[0] = i;
                offset[1] = j;

                offsetx.push_back(offset);
            }
        }

        m_OffsetVector.push_back(offsetx);

    } else if(imagedim == 3 && m_OffsetVector.size() == 0){

        vector< OffsetType > offsetz;
        vector< OffsetType > offsety;
        vector< OffsetType > offsetx;


        for(int j = -1*((int)neighborhoodsize); j <= (int)neighborhoodsize; j++){
            for(int i = -1*((int)neighborhoodsize); i <= (int)neighborhoodsize; i++){


                OffsetType offset;

                //note that this means offsetz -> are the perpendicular planes to z axis (i, j, z)
                offset[0] = i;
                offset[1] = j;
                offset[2] = 0;
                offsetz.push_back(offset);

                //note that this means offsety -> are the perpendicular planes to y axis (i, y, j)
                offset[0] = i;
                offset[1] = 0;
                offset[2] = j;
                offsety.push_back(offset);

                //note that this means offsetx -> are the perpendicular planes to x axis (x, j, i)
                //change to j, i to preserve right hand rule when constructing the neighborhoods in optimization phase
                offset[0] = 0;
                offset[1] = j;
                offset[2] = i;
                offsetx.push_back(offset);

            }
        }



        m_OffsetVector.push_back(offsetz);
        m_OffsetVector.push_back(offsety);
        m_OffsetVector.push_back(offsetx);
    }


    /*for(unsigned i = 0; i < m_Ev->size(); i++){
        (*m_Ev)[i].clear();        
    }*/

    if(m_ImageRegion != imageobject->GetLargestPossibleRegion()){

        m_ImageRegion = imageobject->GetLargestPossibleRegion();

        //m_Ev->clear();
        m_Wp->clear();
        m_EvSampleIndex->clear();        


        if(InputImageType::ImageDimension == 3){
            OutputImageType* output = this->GetOutput();
            //output->SetVectorLength(4);
            output->SetRegions(imageobject->GetLargestPossibleRegion());
            output->Allocate();
            OutputImagePixelType vectorpixel;
            //vectorpixel.SetSize(4);
            vectorpixel.Fill(-1);
            output->FillBuffer(vectorpixel);

            OutputRegionIteratorType outit( output, output->GetLargestPossibleRegion());
            outit.GoToBegin();

            OutputImageIndexType xstartindex = outit.GetIndex();
            xstartindex[0] += neighborhoodsize;
            xstartindex[1] += neighborhoodsize;

            OutputImageIndexType xendindex = outit.GetIndex();
            xendindex[0] += output->GetLargestPossibleRegion().GetSize()[0] - neighborhoodsize;
            xendindex[1] += output->GetLargestPossibleRegion().GetSize()[1] - neighborhoodsize;
            xendindex[2] += output->GetLargestPossibleRegion().GetSize()[2];

            OutputImageIndexType ystartindex = outit.GetIndex();
            ystartindex[0] += neighborhoodsize;
            ystartindex[2] += neighborhoodsize;

            OutputImageIndexType yendindex = outit.GetIndex();
            yendindex[0] += output->GetLargestPossibleRegion().GetSize()[0] - neighborhoodsize;
            yendindex[1] += output->GetLargestPossibleRegion().GetSize()[1];
            yendindex[2] += output->GetLargestPossibleRegion().GetSize()[2] - neighborhoodsize;

            OutputImageIndexType zstartindex = outit.GetIndex();
            zstartindex[1] += neighborhoodsize;
            zstartindex[2] += neighborhoodsize;

            OutputImageIndexType zendindex = outit.GetIndex();
            zendindex[0] += output->GetLargestPossibleRegion().GetSize()[0];
            zendindex[1] += output->GetLargestPossibleRegion().GetSize()[1] - neighborhoodsize;
            zendindex[2] += output->GetLargestPossibleRegion().GetSize()[2] - neighborhoodsize;



            while(!outit.IsAtEnd()){

                OutputImageIndexType outindex = outit.GetIndex();
                bool ztexel = false, ytexel = false, xtexel = false;

                if(         xstartindex[0] <= outindex[0] && outindex[0] <= xendindex[0]
                        &&  xstartindex[1] <= outindex[1] && outindex[1] <= xendindex[1]
                        &&  (outindex[0] - xstartindex[0]) % m_StepSize == 0
                        &&  (outindex[1] - xstartindex[1]) % m_StepSize == 0
                        &&  m_SearchZ){
                    ztexel = true;
                }
                if(         ystartindex[0] <= outindex[0] && outindex[0] <= yendindex[0]
                        &&  ystartindex[2] <= outindex[2] && outindex[2] <= yendindex[2]
                        &&  (outindex[0] - ystartindex[0]) % m_StepSize == 0
                        &&  (outindex[2] - ystartindex[2]) % m_StepSize == 0
                         && m_SearchY){
                    ytexel = true;
                }
                if(         zstartindex[1] <= outindex[1] && outindex[1] <= zendindex[1]
                        &&  zstartindex[2] <= outindex[2] && outindex[2] <= zendindex[2]
                        &&  (outindex[1] - zstartindex[1]) % m_StepSize == 0
                        &&  (outindex[2] - zstartindex[2]) % m_StepSize == 0
                        &&  m_SearchX){
                    xtexel = true;
                }

                OutputImagePixelType vectorxyz;
                //vectorxyz.SetSize(3);
                vectorxyz.Fill(-1);

                if(xtexel || ytexel || ztexel){                    

                    if(ztexel ){
                        vectorxyz[0] =  m_Wp->size();
                        m_Wp->push_back(-1);
                        m_EvSampleIndex->push_back(-1);
                    }
                    if(ytexel ){
                        vectorxyz[1] =  m_Wp->size();
                        m_Wp->push_back(-1);
                        m_EvSampleIndex->push_back(-1);
                    }
                    if(xtexel){
                        vectorxyz[2] =  m_Wp->size();
                        m_Wp->push_back(-1);
                        m_EvSampleIndex->push_back(-1);
                    }
                }

                outit.Set(vectorxyz);

                ++outit;
            }



            typedef itk::Image< int, TInputImage::ImageDimension > ImageINT;


            //for(unsigned i = 0; i < 4; i++){
                typename ImageINT::Pointer tempoutimage = ImageINT::New();
                tempoutimage->SetRegions(output->GetLargestPossibleRegion());
                tempoutimage->Allocate();
                tempoutimage->FillBuffer(0);
                typedef itk::ImageRegionIterator< ImageINT > RegionIT;
                RegionIT tempit(tempoutimage, tempoutimage->GetLargestPossibleRegion());

                outit.GoToBegin();
                tempit.GoToBegin();
                while(!outit.IsAtEnd()){

                    if(outit.Get()[0] > 0 || outit.Get()[1] > 0 || outit.Get()[2] > 0){
                        tempit.Set(1);
                    }else{
                        tempit.Set(0);
                    }
                    ++outit;
                    ++tempit;
                }

                /*typedef itk::ImageFileWriter< ImageINT > writer;
                typename writer::Pointer write = writer::New();
                write->SetInput(tempoutimage);

                string filen = "SearchImage";
                //char c[10];
                //sprintf(c, "%d", i);
                //filen.append(c);
                filen.append(".mhd");
                write->SetFileName(filen.c_str());
                write->Write();*/
            //}





        }else{
            OutputImageType* output = this->GetOutput();
            //output->SetVectorLength(1);
            output->SetRegions(imageobject->GetLargestPossibleRegion());
            output->Allocate();
            OutputImagePixelType vectorpixel;
            //vectorpixel.SetSize(2);
            vectorpixel.Fill(-1);
            output->FillBuffer(vectorpixel);

            OutputRegionIteratorType outit( output, output->GetLargestPossibleRegion());
            outit.GoToBegin();

            OutputImageIndexType xstartindex = outit.GetIndex();
            xstartindex[0] += neighborhoodsize;
            xstartindex[1] += neighborhoodsize;

            OutputImageIndexType xendindex = outit.GetIndex();
            xendindex[0] += output->GetLargestPossibleRegion().GetSize()[0] - neighborhoodsize;
            xendindex[1] += output->GetLargestPossibleRegion().GetSize()[1] - neighborhoodsize;

            while(!outit.IsAtEnd()){

                OutputImageIndexType outindex = outit.GetIndex();
                bool xtexel = false;

                if(         xstartindex[0] <= outindex[0] && outindex[0] <= xendindex[0]
                        &&  xstartindex[1] <= outindex[1] && outindex[1] <= xendindex[1]
                        &&  (outindex[0] - xstartindex[0]) % m_StepSize == 0
                        &&  (outindex[1] - xstartindex[1]) % m_StepSize == 0){
                    xtexel = true;
                }

                if(xtexel){
                    OutputImagePixelType vectorxyz;
                    //vectorxyz.SetSize(1);
                    vectorxyz.Fill(-1);
                    vectorxyz[0] = m_Wp->size();

                    outit.Set(vectorxyz);

                    m_Wp->push_back(-1);
                    m_EvSampleIndex->push_back(-1);
                }
                ++outit;
            }
        }

    }

    int threads = this->GetNumberOfThreads();
    OutputImageRegionType splitRegion;
    threads = this->SplitRequestedRegion(0, threads, splitRegion);

    m_iterationsize = (m_Wp->size()/threads) + 1;



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
/*template< class TInputImage, class TOutput >
void SearchPhase<TInputImage, TOutput>::AfterThreadedGenerateData(){
    m_SearchState = true;
}*/

}

#endif
