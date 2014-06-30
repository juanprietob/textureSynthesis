#ifndef ITKDISCRETESHRINKIMAGEFILTER_H
#define ITKDISCRETESHRINKIMAGEFILTER_H

#include "itkImageToImageFilter.h"

namespace itk{

template <class TInputImage, class TOutputImage >
class DiscreteShrinkImageFilter :
public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
    /** Standard class typedefs. */
    typedef DiscreteShrinkImageFilter             Self;
    typedef ImageToImageFilter<TInputImage,TOutputImage>      Superclass;
    typedef SmartPointer<Self>                                Pointer;
    typedef SmartPointer<const Self>                          ConstPointer;


    /** Pixel Type of the input image */
    typedef TInputImage                                       InputImageType;
    typedef TOutputImage                                      OutputImageType;
    typedef typename TInputImage::PixelType                   PixelType;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(DiscreteShrinkImageFilter, ImageToImageFilter);


    /** Some additional typedefs.  */
    typedef typename InputImageType::Pointer      InputImagePointer;
    typedef typename InputImageType::RegionType   InputImageRegionType;
    typedef typename InputImageType::PixelType    InputImagePixelType;

    /** Some additional typedefs.  */
    typedef typename OutputImageType::Pointer     OutputImagePointer;
    typedef typename OutputImageType::RegionType  OutputImageRegionType;
    typedef typename OutputImageType::PixelType   OutputImagePixelType;


    typedef typename itk::ImageRegionIterator<  InputImageType > RegionIteratorType;

    /** Set the texel dimension the default is 3 for RGB textures
     * NumericTraits<PixelType>::Zero. */
    itkSetMacro(ShrinkFactors,double);

    /** Get the texel dimension */
    itkGetConstMacro(ShrinkFactors,double);

    /** Set the variance of the Discrete Gaussian Image Filter*/
    itkSetMacro(Variance,double);

    /** Get the variance of the Discrete Gaussian Image Filter*/
     itkGetConstMacro(Variance,double);

protected:
    DiscreteShrinkImageFilter();
    ~DiscreteShrinkImageFilter();
    virtual void GenerateData();

private:

    double m_ShrinkFactors;
    double m_Variance;
};

}


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkdiscreteshrinkimagefilter.txx"
#endif


#endif // ITKDISCRETESHRINKIMAGEFILTER_H
