#ifndef ITKRGBImageToVectorFeatures_TXX
#define ITKRGBImageToVectorFeatures_TXX

#include <itkGradientMagnitudeImageFilter.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>

#include <itkRecursiveMultiResolutionPyramidImageFilter.h>

#include "itkImageFileWriter.h"

using namespace std;


namespace itk {


template< class TInputRGBImage,class TInputImage >
RGBImageToVectorFeatures< TInputRGBImage, TInputImage > ::RGBImageToVectorFeatures(){

}

template< class TInputRGBImage,class TInputImage >
void RGBImageToVectorFeatures<TInputRGBImage, TInputImage>::GenerateOutputInformation(void){
    // Override the method in itkImageSource, so we can set the vector length of
    // the output itk::VectorImage

    InputRGBImagePointerType image =  this->GetInput();

    if(!image){
        itkExceptionMacro(<< "Input rgbImage not set!");
    }

    typedef itk::ImageRegionIterator< InputRGBImageType >    RGBIteratorType;
    typedef itk::ImageRegionIterator< InputImageType >    IteratorType;

    RGBIteratorType rgbIt(image,image->GetRequestedRegion() );
    rgbIt.GoToBegin();    

    for(unsigned i = 0; i < 4; i++){

        typename InputImageType::Pointer imagecolor = InputImageType::New();
        imagecolor->SetRegions(image->GetRequestedRegion());
        imagecolor->Allocate();
        IteratorType  imageIt(  imagecolor, imagecolor->GetRequestedRegion() );

        imageIt.GoToBegin();
        rgbIt.GoToBegin();

        while( !imageIt.IsAtEnd() )
        {

            switch (i){
            case 0: imageIt.Set(rgbIt.Get().GetRed());
                break;
            case 1: imageIt.Set(rgbIt.Get().GetGreen());
                break;
            case 2: imageIt.Set(rgbIt.Get().GetBlue());
                break;
            case 3: imageIt.Set(rgbIt.Get().GetLuminance());

                break;
            default: itkExceptionMacro(<< "case not defined!");
                break;
            }

            ++imageIt;
            ++rgbIt;
        }

        this->Superclass::SetInput(i, imagecolor);

    }

    InputImageType* imageluminance = const_cast<InputImageType*>(  Superclass::GetInput(3));


    typedef itk::SmoothingRecursiveGaussianImageFilter<InputImageType, InputImageType >  SmoothingFilterType;
    typename SmoothingFilterType::Pointer smoothfilter = SmoothingFilterType::New();
    smoothfilter->SetInput(imageluminance);
    smoothfilter->SetSigma(0.5);
    smoothfilter->Update();
    this->Superclass::SetInput(4, smoothfilter->GetOutput());

    smoothfilter = SmoothingFilterType::New();
    smoothfilter->SetInput(imageluminance);
    smoothfilter->SetSigma(1);
    smoothfilter->Update();
    this->Superclass::SetInput(5, smoothfilter->GetOutput());

    smoothfilter = SmoothingFilterType::New();
    smoothfilter->SetInput(imageluminance);
    smoothfilter->SetSigma(1.5);
    smoothfilter->Update();
    this->Superclass::SetInput(6, smoothfilter->GetOutput());

    smoothfilter = SmoothingFilterType::New();
    smoothfilter->SetInput(imageluminance);
    smoothfilter->SetSigma(2);
    smoothfilter->Update();
    this->Superclass::SetInput(7, smoothfilter->GetOutput());

    smoothfilter = SmoothingFilterType::New();
    smoothfilter->SetInput(imageluminance);
    smoothfilter->SetSigma(2.5);
    smoothfilter->Update();
    this->Superclass::SetInput(8, smoothfilter->GetOutput());

    smoothfilter = SmoothingFilterType::New();
    smoothfilter->SetInput(imageluminance);
    smoothfilter->SetSigma(3);
    smoothfilter->Update();
    this->Superclass::SetInput(9, smoothfilter->GetOutput());

    smoothfilter = SmoothingFilterType::New();
    smoothfilter->SetInput(imageluminance);
    smoothfilter->SetSigma(3.5);
    smoothfilter->Update();
    this->Superclass::SetInput(10, smoothfilter->GetOutput());



    this->Superclass::GenerateOutputInformation();



    /*typedef itk::RecursiveMultiResolutionPyramidImageFilter<InputImageType, InputImageType> RecursivePyramidType;
    typename RecursivePyramidType::Pointer pyramid = RecursivePyramidType::New();
    pyramid->SetInput(imageluminance);
    pyramid->SetNumberOfLevels(4);    
    pyramid->Update();

    for(unsigned i = 0; i < pyramid->GetNumberOfOutputs(); i++){


        typedef itk::Image<unsigned char, 2> tempimagetype;
        typename tempimagetype::Pointer tempimageout = tempimagetype::New();
        //typename InputImageType::Pointer tempimageout = InputImageType::New();
        tempimageout->SetRegions(pyramid->GetOutput(i)->GetRequestedRegion());
        tempimageout->Allocate();
        typedef itk::ImageRegionIterator< tempimagetype >    TempIteratorType;
        //typedef itk::ImageRegionIterator< InputImageType >    TempIteratorType;
        TempIteratorType tempit(tempimageout, tempimageout->GetRequestedRegion());
        tempit.GoToBegin();

        IteratorType  imageit(  pyramid->GetOutput(i), pyramid->GetOutput(i)->GetRequestedRegion() );
        imageit.GoToBegin();

        while( !tempit.IsAtEnd() )
        {

            tempit.Set(imageit.Get());
            ++tempit;
            ++imageit;


        }

        typedef itk::ImageFileWriter< tempimagetype > ImageWriterType;
        typename ImageWriterType::Pointer map_writer = ImageWriterType::New();
        std::string filename = "testPyramid";
        char buffer[10];
        sprintf(buffer, "%d", i);
        filename.append( buffer );
        filename.append(".png");
        map_writer->SetFileName(filename.c_str());
        map_writer->SetInput( tempimageout );
        map_writer->Update();
    }



    this->Superclass::GenerateOutputInformation();*/

}


}//itk namespace

#endif
