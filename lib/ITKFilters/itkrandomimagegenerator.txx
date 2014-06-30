
#ifndef ITKRANDOMIMAGEGENERATOR_TXX
#define ITKRANDOMIMAGEGENERATOR_TXX

namespace itk
{

template <class TInputImage,class TOutputImage>
RandomImageGenerator<TInputImage, TOutputImage>::RandomImageGenerator()
{
    m_InputImageSample = 0;
    m_PositionValue = 0;
    m_SamplesNumber = 0;    

    m_NeighborhoodSize = 32;
}

template <class TInputImage,class TOutputImage>
void RandomImageGenerator<TInputImage, TOutputImage>::GenerateData(){


    itkDebugMacro(<<"Actually executing");

    // Get the input and output pointers
    OutputImagePointer  inputobject  = const_cast< OutputImageType * >(this->GetInput());

    if(!inputobject){
        itkExceptionMacro(<< "Input object not set!");
    }

    if(!m_InputImageSample){
        itkExceptionMacro(<< "Input sample not set!");
    }


    OutputImagePointer outputobject    = this->GetOutput();
    outputobject->SetRegions(inputobject->GetLargestPossibleRegion());
    outputobject->Allocate();

    OutputImagePixelType initpixel;
    initpixel.Fill(0);
    outputobject->FillBuffer(initpixel);


    RandomConstIteratorType sampleIt(  m_InputImageSample,  m_InputImageSample->GetLargestPossibleRegion() );
    sampleIt.SetNumberOfSamples( this->m_SamplesNumber );
    sampleIt.ReinitializeSeed();
    sampleIt.GoToBegin();


    OutputRegionIterator outit(outputobject, outputobject->GetLargestPossibleRegion());

    outit.GoToBegin();

    while(!outit.IsAtEnd()){

        if(sampleIt.IsAtEnd()){
            sampleIt.ReinitializeSeed();
            sampleIt.GoToBegin();
        }

        outit.Set(sampleIt.Get());
        ++outit;
        ++sampleIt;

    }



    /*

    typename InputNeighborhoodIteratorType::RadiusType inputradius;
    inputradius.Fill(m_NeighborhoodSize/2);
    InputNeighborhoodIteratorType sampleItneigh(inputradius, m_InputImageSample, m_InputImageSample->GetLargestPossibleRegion());


    InputImagePointer imageslice = InputImageType::New();
    InputImageRegionType region;
    region.SetSize(0, inputobject->GetLargestPossibleRegion().GetSize(0));
    region.SetSize(1, inputobject->GetLargestPossibleRegion().GetSize(1));
    region.SetIndex(0,0);
    region.SetIndex(1,0);
    imageslice->SetRegions(region);
    imageslice->Allocate();

    InputNeighborhoodIteratorType imagesliceobjectItneigh(inputradius, imageslice, imageslice->GetLargestPossibleRegion());
    InputSliceIteratorType        imagesliceobjectit(imageslice, imageslice->GetLargestPossibleRegion());
    imagesliceobjectit.SetFirstDirection(0);
    imagesliceobjectit.SetSecondDirection(1);



    sampleIt.GoToBegin();
    imagesliceobjectit.GoToBegin();

    while(!imagesliceobjectit.IsAtEnd()){

       while(!imagesliceobjectit.IsAtEndOfSlice()){

           bool moveline = false;

           while(!imagesliceobjectit.IsAtEndOfLine()){

               imagesliceobjectItneigh.SetLocation(imagesliceobjectit.GetIndex());

               if(imagesliceobjectItneigh.InBounds()){

                   moveline = true;

                   if(sampleIt.IsAtEnd()){
                       sampleIt.GoToBegin();
                   }

                   sampleItneigh.SetLocation(sampleIt.GetIndex());
                   while(!sampleItneigh.InBounds()){
                       ++sampleIt;
                       sampleItneigh.SetLocation(sampleIt.GetIndex());
                   }

                   for(unsigned i = 0; i < imagesliceobjectItneigh.Size(); i++){
                       imagesliceobjectItneigh.SetPixel(i, sampleItneigh.GetPixel(i));
                   }

                    ++sampleIt;

                   for(unsigned i = 0; i < m_NeighborhoodSize-1 ; i++){
                       ++imagesliceobjectit;
                   }
               }else{                   
                   ++imagesliceobjectit;
               }
           }

           if(moveline){
               for(unsigned i = 0; i < m_NeighborhoodSize-1;i++){
                   imagesliceobjectit.NextLine();
               }
           }else{
               imagesliceobjectit.NextLine();
           }
       }
        imagesliceobjectit.NextSlice();
   }


    if(OutputImageType::ImageDimension == 3){

        for(unsigned i = 0; i < 3; i++){


            InputSliceIteratorType  imagesliceobjectit(imageslice, imageslice->GetLargestPossibleRegion());
            imagesliceobjectit.SetFirstDirection(0);
            imagesliceobjectit.SetSecondDirection(1);


            int first = i, second = first+1;

            if(second == 3){
                second = 0;
            }

            OutputSliceIteratorType outputsliceit(outputobject, outputobject->GetLargestPossibleRegion());
            outputsliceit.SetFirstDirection(first);
            outputsliceit.SetSecondDirection(second);


            outputsliceit.GoToBegin();


            while(!outputsliceit.IsAtEnd()){

                imagesliceobjectit.GoToBegin();

                while(!outputsliceit.IsAtEndOfSlice() && !imagesliceobjectit.IsAtEndOfSlice()){

                    while(!outputsliceit.IsAtEndOfLine() && !imagesliceobjectit.IsAtEndOfLine()){

                        OutputImagePixelType outpixel;
                        if(i==0){
                            outpixel = imagesliceobjectit.Get();
                        }else if(i==1){
                            outpixel = imagesliceobjectit.Get();
                            for(unsigned j = 0; j < outpixel.Size(); j++){
                                outpixel[j] =(outputsliceit.Get()[j] + outpixel[j])/2;
                            }
                        }else if(i == 2){
                            outpixel = imagesliceobjectit.Get();
                            for(unsigned j = 0; j < outpixel.Size(); j++){
                                outpixel[j] =(outputsliceit.Get()[j]*2 + outpixel[j])/3;
                            }
                        }
                        outputsliceit.Set(outpixel);

                        ++outputsliceit;
                        ++imagesliceobjectit;
                    }
                    imagesliceobjectit.NextLine();
                    outputsliceit.NextLine();
                }

                outputsliceit.NextSlice();
            }
        }
    }else{
        InputSliceIteratorType  imagesliceobjectit(imageslice, imageslice->GetLargestPossibleRegion());
        imagesliceobjectit.SetFirstDirection(0);
        imagesliceobjectit.SetSecondDirection(1);


        OutputSliceIteratorType outputsliceit(outputobject, outputobject->GetLargestPossibleRegion());
        outputsliceit.SetFirstDirection(0);
        outputsliceit.SetSecondDirection(1);


        outputsliceit.GoToBegin();


         while(!outputsliceit.IsAtEnd()){

            imagesliceobjectit.GoToBegin();

            while(!outputsliceit.IsAtEndOfSlice() && !imagesliceobjectit.IsAtEndOfSlice()){

                while(!outputsliceit.IsAtEndOfLine() && !imagesliceobjectit.IsAtEndOfLine()){


                    outputsliceit.Set(imagesliceobjectit.Get());

                    ++outputsliceit;
                    ++imagesliceobjectit;
                }
                imagesliceobjectit.NextLine();
                outputsliceit.NextLine();
            }
            outputsliceit.NextSlice();
        }
    }*/

}


}
#endif
