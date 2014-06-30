
#ifndef ITKVECTORTORGBIMAGE_TXX
#define ITKVECTORTORGBIMAGE_TXX

using namespace std;


namespace itk {


template< class TInputImage, unsigned int VImageDimension >
VectorToRGBImage< TInputImage, VImageDimension > ::VectorToRGBImage(){

}

template< class TInputImage, unsigned int VImageDimension >
void VectorToRGBImage<TInputImage, VImageDimension>::GenerateData(void){
    // Override the method in itkImageSource, so we can set the vector length of
    // the output itk::VectorImage

    InputImageType* inputimage = const_cast< InputImageType*>(this->GetInput());
    OutputImageType* outputimage = this->GetOutput();

    if(inputimage == 0){
        itkExceptionMacro(<< "Input rgbImage not set!");
    }

    outputimage->SetRegions(inputimage->GetLargestPossibleRegion());
    outputimage->Allocate();

    typedef itk::ImageRegionIterator< InputImageType >    IteratorType;
    typedef itk::ImageRegionIterator< OutputImageType >    RGBIteratorType;


    IteratorType  imageIt(  inputimage, inputimage->GetLargestPossibleRegion() );
    RGBIteratorType rgbIt(outputimage,outputimage->GetLargestPossibleRegion() );

    imageIt.GoToBegin();
    rgbIt.GoToBegin();

    while( !rgbIt.IsAtEnd() )
    {
        RGBPixelType rgbpixel;
        rgbpixel.SetRed(imageIt.Get()[0] );
        rgbpixel.SetGreen(imageIt.Get()[1] );
        rgbpixel.SetBlue(imageIt.Get()[2] );

        if(this->GetDebug()){
            itkDebugMacro("image "<<imageIt.Get()[0] <<" "<<imageIt.Get()[1]<<" "<<imageIt.Get()[2]<<"\n");
        }

        rgbIt.Set(rgbpixel);

        ++imageIt;
        ++rgbIt;
    }

}


}//itk namespace

#endif
