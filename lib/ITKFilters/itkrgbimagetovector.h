#ifndef ITKRGBIMAGETOVECTOR_H
#define ITKRGBIMAGETOVECTOR_H

#include "itkImageToVectorImageFilter.h"

namespace itk {
/** \class itkImageToVectorFeatures
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

template< class TInputRGBImage,class TInputImage>
class ITK_EXPORT  RGBImageToVector :
        public ImageToVectorImageFilter <TInputImage>
    {

    public:
        typedef RGBImageToVector                      Self;
        typedef SmartPointer<Self>                        Pointer;
        typedef SmartPointer<const Self>                  ConstPointer;
        itkNewMacro(Self);
        itkTypeMacro(RGBImageToVector, ImageToVectorImageFilter);

        itkStaticConstMacro(Dimension, unsigned int, TInputImage::ImageDimension);

        typedef typename TInputImage::InternalPixelType   PixelType;

        typedef ImageToVectorImageFilter< TInputImage>     Superclass;

        typedef TInputImage       InputImageType;

        typedef TInputRGBImage    InputRGBImageType;

        typedef typename Superclass::OutputImageType OutputImageType;

        virtual void SetInput(const InputRGBImageType *input);

    protected:
         RGBImageToVector();
        virtual void GenerateOutputInformation(void);

    private:
        InputRGBImageType* m_rgbimage;
    };

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkrgbimagetovector.txx"
#endif


#endif // ITKRGBIMAGETOVECTOR_H








