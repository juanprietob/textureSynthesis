#ifndef ITKVectorDiscreteGaussianImageFilter_TXX
#define ITKVectorDiscreteGaussianImageFilter_TXX

#include <itkDiscreteGaussianImageFilter.h>

using namespace std;

namespace itk{

template<class TInputImage,class TOutputImage>
VectorDiscreteGaussianImageFilter<TInputImage, TOutputImage>::VectorDiscreteGaussianImageFilter()
{
    m_Variance = 0;
}
template<class TInputImage,class TOutputImage>
VectorDiscreteGaussianImageFilter<TInputImage, TOutputImage>::~VectorDiscreteGaussianImageFilter()
{
}

template<class TInputImage,class TOutputImage>
void VectorDiscreteGaussianImageFilter<TInputImage, TOutputImage>::GenerateData(){


    InputImagePointer imagevector = const_cast< InputImageType * >(this->GetInput());

    if(!imagevector){
        itkExceptionMacro(<< "Input not set!");
    }


    RegionIteratorType itimage (imagevector, imagevector->GetLargestPossibleRegion());
    itimage.GoToBegin();



    vector< DiscreteImagePointerType > vectorimages;

    for(unsigned i = 0; i < itimage.Get().Size(); i++){
        DiscreteImagePointerType imagedim = DiscreteImageType::New();
        imagedim->SetRegions(imagevector->GetLargestPossibleRegion());
        imagedim->Allocate();

        vectorimages.push_back(imagedim);
    }

    while(!itimage.IsAtEnd()){


        for(unsigned i = 0; i < itimage.Get().Size(); i++){

            RegionIteratorDiscreteType itdiscreteimage(vectorimages[i], vectorimages[i]->GetLargestPossibleRegion());

            itdiscreteimage.SetIndex(itimage.GetIndex());

            itdiscreteimage.Set(itimage.Get()[i]);

        }



        ++itimage;
    }


    typedef itk::DiscreteGaussianImageFilter< DiscreteImageType, DiscreteImageType > DiscreteGaussianType;
    vector< DiscreteImagePointerType > vectordiscreteimages;

    for(unsigned i = 0; i < vectorimages.size(); i++){

        typename DiscreteGaussianType::Pointer discretegaussian = DiscreteGaussianType::New();
        discretegaussian->SetVariance(m_Variance);
        discretegaussian->SetInput(vectorimages[i]);
        discretegaussian->Update();        

        vectordiscreteimages.push_back(discretegaussian->GetOutput());

    }

    OutputImagePointer imageout = this->GetOutput();
    imageout->SetRegions(imagevector->GetLargestPossibleRegion());
    imageout->Allocate();

    RegionIteratorType imgoutit(imageout, imageout->GetLargestPossibleRegion());

    imgoutit.GoToBegin();

    while(!imgoutit.IsAtEnd()){

        PixelType outpixel;

        for(unsigned i = 0; i < vectordiscreteimages.size(); i++){

            RegionIteratorDiscreteType itdiscreteimage(vectordiscreteimages[i], vectordiscreteimages[i]->GetLargestPossibleRegion());

            itdiscreteimage.SetIndex(imgoutit.GetIndex());

            outpixel[i] = itdiscreteimage.Get();
        }

        imgoutit.Set( outpixel );
        ++imgoutit;        
    }

}

}

#endif
