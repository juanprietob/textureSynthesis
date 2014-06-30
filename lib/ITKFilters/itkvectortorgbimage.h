#ifndef ITKVECTORTORGBIMAGE_H
#define ITKVECTORTORGBIMAGE_H

#include <itkRGBPixel.h>
#include <itkImageToImageFilter.h>

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

template< class TInputImage, unsigned int VImageDimension=2 >
class ITK_EXPORT  VectorToRGBImage :
        public ImageToImageFilter <TInputImage, itk::Image<itk::RGBPixel< unsigned char >, VImageDimension> >
    {

    public:
        typedef VectorToRGBImage                      Self;
        typedef SmartPointer<Self>                        Pointer;
        typedef SmartPointer<const Self>                  ConstPointer;
        itkNewMacro(Self);
        itkTypeMacro(VectorToRGBImage, ImageToVectorImageFilter);

        itkStaticConstMacro(Dimension, unsigned int, TInputImage::ImageDimension);

        typedef TInputImage       InputImageType;
        typedef typename InputImageType::PixelType PixelType;
        typedef typename InputImageType::InternalPixelType InternalPixelType;
        typedef typename InputImageType::IndexType  IndexType;

        typedef unsigned char                       RGBType;
        typedef itk::RGBPixel< RGBType >            RGBPixelType;


        typedef itk::Image<itk::RGBPixel< unsigned char >, VImageDimension> OutputImageType;


        typedef ImageToImageFilter<TInputImage, OutputImageType >     Superclass;


    protected:
        VectorToRGBImage();
        ~VectorToRGBImage(){};
        virtual void GenerateData(void);

    private:

    };

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkvectortorgbimage.txx"
#endif

#endif // ITKVECTORTORGBIMAGE_H
