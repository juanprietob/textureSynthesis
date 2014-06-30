#ifndef ITKRGBImageToVectorFeatures_H
#define ITKRGBImageToVectorFeatures_H

#include "itkImageToVectorImageFilter.h"



namespace itk {
/** \class itkRGBImageToVectorFeatures
 * \brief This class takes as input an image and composes it into
 * a single itk::VectorImage with features R, G, B, Lumminosity, GradientMagnitud,
*   GuassianGradientMagnitud with different 2 sigma.
 *
 * \par Inputs and Usage
 * \code
 *    filter->SetInput( 0, image0 );
 *    filter->SetInput( 1, image1 );
 *    ...
 *    filter->Update();
 *    itk::VectorImage< PixelType, dimension >::Pointer = filter->GetOutput();
 * \endcode
 * All input images are expected to have the same template parameters and have
 * the same size and origin.
 *
 * \sa VectorImage
 */

template< class TInputRGBImage,class TInputImage = itk::Image<double, 2> >
class ITK_EXPORT RGBImageToVectorFeatures :
    public ImageToVectorImageFilter <TInputImage>
{

public:
    typedef RGBImageToVectorFeatures                     Self;
    typedef SmartPointer<Self>                        Pointer;
    typedef SmartPointer<const Self>                  ConstPointer;
    itkNewMacro(Self);
    itkTypeMacro(RGBImageToVectorFeatures, ImageToVectorImageFilter);

    itkStaticConstMacro(Dimension, unsigned int, TInputImage::ImageDimension);

    typedef typename TInputImage::PixelType   PixelType;

    typedef ImageToVectorImageFilter< TInputImage>     Superclass;    

    typedef TInputRGBImage    InputRGBImageType;
    typedef typename InputRGBImageType::Pointer  InputRGBImagePointerType;

    typedef TInputImage       InputImageType;
    typedef typename InputImageType::Pointer InputImagePointerType;



     //virtual void SetInput(const InputVectorImageType *input);

     /** Set the "outside" pixel value. The default value
      * NumericTraits<PixelType>::Zero. */
     itkSetMacro(Input,InputRGBImagePointerType);

     /** Get the "Position to compare with the object */
     itkGetConstMacro(Input,InputRGBImagePointerType);

protected:
    RGBImageToVectorFeatures();
    virtual void GenerateOutputInformation(void);

private:
    InputRGBImagePointerType m_Input;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkrgbimagetovectorfeatures.txx"
#endif


#endif // ITKRGBImageToVectorFeatures_H
