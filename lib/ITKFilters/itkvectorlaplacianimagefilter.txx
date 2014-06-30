#ifndef ITKVECTORLAPLACIANIMAGEFILTER_TXX
#define ITKVECTORLAPLACIANIMAGEFILTER_TXX

#include <itkNeighborhoodIterator.h>

using namespace std;

namespace itk{

template<class TInputImage,class TOutputImage>
VectorLaplacianImageFilter<TInputImage, TOutputImage>::VectorLaplacianImageFilter()
{
    m_Variance = 0;
}
template<class TInputImage,class TOutputImage>
VectorLaplacianImageFilter<TInputImage, TOutputImage>::~VectorLaplacianImageFilter()
{
}

template<class TInputImage,class TOutputImage>
void VectorLaplacianImageFilter<TInputImage, TOutputImage>::GenerateData(){


    InputImagePointer imagevector = const_cast< InputImageType * >(this->GetInput());

    if(!imagevector){
        itkExceptionMacro(<< "Input not set!");
    }


    typedef itk::NeighborhoodIterator<InputImageType> NeighborhoodIteratorType;
    typename NeighborhoodIteratorType::RadiusType radius;
    radius.Fill(1);
    NeighborhoodIteratorType itimage (radius, imagevector, imagevector->GetLargestPossibleRegion());
    itimage.GoToBegin();


    OutputImagePointer imageout = this->GetOutput();
    imageout->SetRegions(imagevector->GetLargestPossibleRegion());
    imageout->SetSpacing(imagevector->GetSpacing());
    imageout->SetOrigin(imagevector->GetOrigin());
    imageout->Allocate();

    typedef itk::NeighborhoodIterator<OutputImageType> NeighborhoodIteratorOutputType;
    typename NeighborhoodIteratorOutputType::RadiusType outradius;
    outradius.Fill(1);
    NeighborhoodIteratorOutputType imgoutit(outradius, imageout, imageout->GetLargestPossibleRegion());

    imgoutit.GoToBegin();


    while(!itimage.IsAtEnd()){
        OutputImagePixelType outpix;
        outpix.Fill(0);

        for(unsigned i = 0; i < itimage.Size(); i++){
            for(unsigned j = 0; j < outpix.Size(); j++){
                outpix[j] += itimage.GetPixel(i)[j];
            }
        }
        for(unsigned j = 0; j < outpix.Size(); j++){
            outpix[j] /= itimage.Size();
        }

        imgoutit.SetCenterPixel(outpix);

        ++itimage;
        ++imgoutit;
    }



}

}

#endif
