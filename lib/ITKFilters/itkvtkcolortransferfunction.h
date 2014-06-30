#ifndef ITKVTKCOLORTRANSFERFUNCTION_H
#define ITKVTKCOLORTRANSFERFUNCTION_H


#include <itkProcessObject.h>

#include <vtkColorTransferFunction.h>

#include <itkRGBToLuminanceImageFilter.h>
#include <itkImageRegionConstIterator.h>

#include "limits.h"

namespace itk{

template< class TypeImage, typename OutType = unsigned char>
class VTKColorTransferFunction :
public ProcessObject
{

    public:

        /** Standard class typedefs. */
        typedef VTKColorTransferFunction           Self;

        /** Standard "Superclass" typedef */
        typedef ProcessObject Superclass;

        /** Smart pointer typedef support. */
        typedef SmartPointer<Self>            Pointer;
        typedef SmartPointer<const Self>      ConstPointer;

        /** Method for creation through the object factory. */
        itkNewMacro(Self);

        /** Run-time type information (and related methods). */
        itkTypeMacro( VTKColorTransferFunction, ProcessObject );

        /** InputImageType typedef support. */
        typedef TypeImage                                   InputImageType;
        typedef typename InputImageType::Pointer            InputImagePointerType;
        typedef typename InputImageType::PixelType          PixelType;
        typedef typename InputImageType::IndexType          IndexType;


        typedef Image< OutType,   InputImageType::ImageDimension > OutputImageType;
        typedef typename OutputImageType::Pointer            OutputImagePointerType;
        typedef typename OutputImageType::PixelType          OutputImagePixelType;
        typedef typename OutputImageType::IndexType          OutputImageIndexType;

        /**
          * set the rgb image to create the transfer function
         */
        itkSetMacro(Input, InputImagePointerType)
        /**
          * set the rgb image to create the transfer function
         */
        itkGetConstMacro(Input, InputImagePointerType)

        /**
          get the color transfer created from the image
         */
        itkGetMacro(Output, vtkColorTransferFunction*)

        /**
          get the short image
         */
        itkGetMacro(OutputImage, OutputImagePointerType)

        virtual void Update();

        /**
          * set the rgb image to create the transfer function
         */
        itkSetMacro(GenerateShort, bool)
protected:
    VTKColorTransferFunction();
    ~VTKColorTransferFunction();

    virtual void GenerateData ();

    virtual void GenerateOutputInformation();

private:
    vtkColorTransferFunction* m_Output;
    InputImagePointerType m_Input;
    OutputImagePointerType m_OutputImage;
    bool m_GenerateShort;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkvtkcolortransferfunction.txx"
#endif

#endif // ITKVTKCOLORTRANSFERFUNCTION_H
