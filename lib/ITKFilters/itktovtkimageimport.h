#ifndef __ITKToVTKImageImport_h_
#define __ITKToVTKImageImport_h_

#include "itkImage.h"
#include "itkVTKImageImport.h"
#include "itkVTKImageExport.h"
#include <itkProcessObject.h>

#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkImageExport.h"

#define DIM 3
#define TRY_CLAUSE_ITK		try{
#define CATCH_CLAUSE_ITK	}catch(itk::ExceptionObject &e){		\
                                                                std::cout<<e<<std::endl;			\
                                                                std::string ex = e.GetDescription();}\

namespace itk{

//------------------------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------------------------
template<typename TypeImage>
class ITK_EXPORT ITKToVTKImageImport :
public ProcessObject
{

    public:

        /** Standard class typedefs. */
        typedef ITKToVTKImageImport           Self;

        /** Standard "Superclass" typedef */
        typedef ProcessObject Superclass;

        /** Smart pointer typedef support. */
        typedef SmartPointer<Self>            Pointer;
        typedef SmartPointer<const Self>      ConstPointer;

        /** Method for creation through the object factory. */
        itkNewMacro(Self);

        /** Run-time type information (and related methods). */
        itkTypeMacro( ITKToVTKImageImport, ProcessObject );

        /** InputImageType typedef support. */
        typedef TypeImage                                   InputImageType;
        typedef typename InputImageType::Pointer            InputImagePointerType;
        typedef typename InputImageType::PixelType          PixelType;
        typedef typename InputImageType::IndexType          IndexType;

        /*
        *	type definition for the itk image import
        */
        typedef itk::VTKImageImport<InputImageType> ImageImportType;

        typedef typename ImageImportType::Pointer ImageImportPointer;
        /*
        *	type definition for the itk image import
        */
        typedef itk::VTKImageExport<InputImageType> ImageExportType;
        /*
        *	type definition for the pointer of the image import
        */
        typedef typename ImageExportType::Pointer ImageExportPointer;

        /*
        *	set the vtkImage data to connect to itk pipeline
        */
        void SetVTKImage(vtkImageData* img);

        /*
        *	Get the vtkImageData from the itk image
        */
        vtkImageData* GetOutputVTKImage();


        void SetITKImage(InputImagePointerType itkimage);
        /*
        *
        */
        InputImagePointerType GetOutputITKImage() ;

//------------------------------------------------------------------------------------------------------------
// Attributes
//------------------------------------------------------------------------------------------------------------
protected:
        /**
        ** Default constructor, allocates the vtkImageExport and itkImageImport
        **	Connects the pipelines and set them ready to be use
        **/
        ITKToVTKImageImport();
        ~ITKToVTKImageImport();


private:


        vtkImageData* m_VTKImage;

        InputImagePointerType m_ITKImage;

        /*
        *	Pointer to vtkImageExport
        */
        vtkImageExport * vtkimageexport;
        /*
        *	itk::VTKImageImport pointer
        */
        ImageImportPointer imageimportpointer;
        /*
        *	Pointer to vtkImageExport
        */
        vtkImageImport * vtkimageimport;
        /*
        *	Pointer to Export the itk image into vtk
        */
        ImageExportPointer imageexportpointer;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itktovtkimageimport.txx"
#endif


#endif
