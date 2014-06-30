#ifndef ITKDISCRETESHRINKIMAGEFILTER_TXX
#define ITKDISCRETESHRINKIMAGEFILTER_TXX

#include "itkvectordiscretegaussianimagefilter.h"
#include "itkScaleTransform.h"
#include "itkVectorResampleImageFilter.h"
#include "itkVectorNearestNeighborInterpolateImageFunction.h"

namespace itk{

template <class TInputImage, class TOutputImage >
DiscreteShrinkImageFilter<TInputImage, TOutputImage >::DiscreteShrinkImageFilter()
{
    m_ShrinkFactors = 1;
    m_Variance = -1;
}
template <class TInputImage, class TOutputImage >
DiscreteShrinkImageFilter<TInputImage, TOutputImage >::~DiscreteShrinkImageFilter(){

}

template <class TInputImage, class TOutputImage >
void DiscreteShrinkImageFilter<TInputImage, TOutputImage >::GenerateData(){


    InputImagePointer inputimage = const_cast< InputImageType * >(this->GetInput());

    if(!inputimage){
        itkExceptionMacro(<< "Input sample not set!");
    }

    if(m_ShrinkFactors == 0){
        itkExceptionMacro(<< "The shrink factor should be greater than 0");
    }else if(m_ShrinkFactors == 1){
       itkDebugMacro(<<"ShrinkFactor = 1, the output image will be blured"<<endl);
    }

    typedef itk::VectorDiscreteGaussianImageFilter< InputImageType, InputImageType> VectorDiscreteGaussian;
    typename VectorDiscreteGaussian::Pointer vectordiscrete = VectorDiscreteGaussian::New();
    if(m_Variance == -1){
        m_Variance = 0.5;
    }

    vectordiscrete->SetVariance(m_Variance);
    vectordiscrete->SetInput(inputimage);
    vectordiscrete->Update();

    typedef itk::VectorResampleImageFilter< InputImageType, InputImageType> ResampleShrinkerType;
    typename ResampleShrinkerType::Pointer resampleShrinker = ResampleShrinkerType::New();

    typedef itk::VectorLinearInterpolateImageFunction< InputImageType, double >  VectorLinearInterpolatorType;
    typename VectorLinearInterpolatorType::Pointer vectorinterpolator = VectorLinearInterpolatorType::New();

    //typedef itk::VectorNearestNeighborInterpolateImageFunction< InputImageType, double >  VectorNearestNeighborInterpolateImageFunction;
    //typename VectorNearestNeighborInterpolateImageFunction::Pointer vectorinterpolator = VectorNearestNeighborInterpolateImageFunction::New();


    resampleShrinker->SetInterpolator( vectorinterpolator );



    typedef itk::ScaleTransform< double, InputImageType::ImageDimension>  IdentityTransformType;
    typename IdentityTransformType::Pointer identityTransform = IdentityTransformType::New();
    typename IdentityTransformType::ScaleType scale;
    for(unsigned i = 0; i < scale.Size(); i++ ){
        scale[i] = m_ShrinkFactors;
    }
    identityTransform->SetScale(scale);
    resampleShrinker->SetTransform(identityTransform);

    resampleShrinker->SetOutputSpacing(vectordiscrete->GetOutput()->GetSpacing());
    resampleShrinker->SetOutputOrigin(vectordiscrete->GetOutput()->GetOrigin());
    //resampleShrinker->SetOutputDirection(imageobject->GetOutputDirection());
    resampleShrinker->SetOutputStartIndex(vectordiscrete->GetOutput()->GetLargestPossibleRegion().GetIndex());

    typename InputImageType::SizeType size = vectordiscrete->GetOutput()->GetLargestPossibleRegion().GetSize();
    for(unsigned i = 0; i < size.GetSizeDimension(); i++ ){
        size[i] /= m_ShrinkFactors;
    }
    resampleShrinker->SetSize(size);
    resampleShrinker->SetInput(vectordiscrete->GetOutput());
    resampleShrinker->Update();
    InputImagePointer resampleoutput = resampleShrinker->GetOutput();



    OutputImagePointer imageout = this->GetOutput();
    imageout->SetRegions(resampleoutput->GetLargestPossibleRegion());
    imageout->SetSpacing(resampleoutput->GetSpacing());
    imageout->SetOrigin(resampleoutput->GetOrigin());
    imageout->Allocate();


    RegionIteratorType imgoutit(imageout, imageout->GetLargestPossibleRegion());
    RegionIteratorType imageresampleit(resampleoutput, resampleoutput->GetLargestPossibleRegion());

    imageresampleit.GoToBegin();
    imgoutit.GoToBegin();

    while(!imgoutit.IsAtEnd()){
        imgoutit.Set(imageresampleit.Get());

        ++imgoutit;
        ++imageresampleit;
    }


}

}

#endif
