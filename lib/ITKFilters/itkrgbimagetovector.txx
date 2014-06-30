
#ifndef ITKRGBIMAGETOVECTOR_TXX
#define ITKRGBIMAGETOVECTOR_TXX

#include <itkGradientMagnitudeImageFilter.h>
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"

using namespace std;


namespace itk {


template< class TInputRGBImage,class TInputImage >
RGBImageToVector< TInputRGBImage, TInputImage > ::RGBImageToVector(){
    this->m_rgbimage = 0;    
}

template< class TInputRGBImage,class TInputImage >
void RGBImageToVector<TInputRGBImage, TInputImage>::GenerateOutputInformation(void){
    // Override the method in itkImageSource, so we can set the vector length of
    // the output itk::VectorImage

    if(this->m_rgbimage == 0){
        itkExceptionMacro(<< "Input rgbImage not set!");
    }

    typedef itk::ImageRegionIterator< InputRGBImageType >    RGBIteratorType;
    typedef itk::ImageRegionIterator< InputImageType >    IteratorType;

    RGBIteratorType rgbIt(this->m_rgbimage,this->m_rgbimage->GetLargestPossibleRegion() );

    for(unsigned i = 0; i < 4; i++){

        typename InputImageType::Pointer imagecolor = InputImageType::New();
        imagecolor->SetRegions(this->m_rgbimage->GetLargestPossibleRegion());
        imagecolor->Allocate();
        IteratorType  imageIt(  imagecolor, imagecolor->GetLargestPossibleRegion() );

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
    this->Superclass::GenerateOutputInformation();

}

template< class TInputRGBImage,class TInputImage >
void RGBImageToVector< TInputRGBImage, TInputImage > ::SetInput(const InputRGBImageType *input){
    this->m_rgbimage = (InputRGBImageType*) input;
}


}//itk namespace

#endif
