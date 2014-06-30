//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------


#include <qtvtkviewerwidget.h>
#include <QMainWindow>
#include <QApplication>

#include "vtkImageData.h"

#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"

#include "itkvtkcolortransferfunction.h"
#include "itktovtkimageimport.h"


using namespace std;					// make std:: accessible

//----------------------------------------------------------------------
// ann_sample
//
// This is a simple sample program for the ANN library.	 After compiling,
// it can be run as follows.
//
// ann_sample [-d dim] [-max mpts] [-nn k] [-e eps] [-df data] [-qf query]
//
// where
//		dim				is the dimension of the space (default = 2)
//		mpts			maximum number of data points (default = 1000)
//		k				number of nearest neighbors per query (default 1)
//		eps				is the error bound (default = 0.0)
//		data			file containing data points
//		query			file containing query points
//
// Results are sent to the standard output.
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//	Parameters that are set in getArgs()
//----------------------------------------------------------------------


int main(int argc, char **argv)
{


    if (argc < 3)
      {
        std::cout << "ERROR: data file name argument missing."
                  << std::endl ;
        return EXIT_FAILURE;
      }

    bool rgbtogrey = false;
    if(argc >= 4){
        if (string(argv[3]) == "-rgbtogrey"){
            rgbtogrey = true;
        }
    }


    std::string inputFilename = argv[1];


    typedef unsigned char PixelType;
    typedef itk::RGBPixel< PixelType > RGBPixelType;
    typedef itk::Image< RGBPixelType, 2 > RGBImageType;

    typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

    imageReader->SetFileName(inputFilename.c_str());
    imageReader->Update() ;
    RGBImageType::Pointer image = imageReader->GetOutput();




    std::string inputFilename2 = argv[2];

    vtkImageData* vtkimageobject = 0;

    typedef itk::Image< RGBPixelType, 3 > RGBImage3DType;
    typedef itk::ImageFileReader< RGBImage3DType > ImageReader3DType ;
    typedef itk::Image< RGBImage3DType::PixelType::ComponentType, RGBImage3DType::ImageDimension > OutLumType;
    typedef itk::RGBToLuminanceImageFilter< RGBImage3DType, OutLumType > RGBToLumType;
    typedef itk::ITKToVTKImageImport< RGBToLumType::OutputImageType > ITKToVTKImageType;
    ITKToVTKImageType::Pointer itktovtk = 0;

    if(!rgbtogrey){


        vtkMetaImageReader* imgreader = vtkMetaImageReader::New();
        imgreader->SetFileName(inputFilename2.c_str());

        imgreader->Update();


        vtkimageobject = imgreader->GetOutput();
    }else{


        ImageReader3DType::Pointer imageReader3D = ImageReader3DType::New() ;

        imageReader3D->SetFileName(inputFilename2.c_str());
        imageReader3D->Update() ;
        RGBImage3DType::Pointer imageobject = imageReader3D->GetOutput();

        RGBToLumType::Pointer rgbtolum = RGBToLumType::New();
        rgbtolum->SetInput(imageobject);
        rgbtolum->Update();
        RGBToLumType::OutputImageType::Pointer lumimageobject = rgbtolum->GetOutput();


        itktovtk = ITKToVTKImageType::New();
        itktovtk->SetITKImage(lumimageobject);
        itktovtk->Update();

        vtkimageobject =itktovtk->GetOutputVTKImage();

        vtkMetaImageWriter* writer = vtkMetaImageWriter::New();
        writer->SetFileName("zebraOptimization.mhd");
        writer->SetInput(vtkimageobject);
        writer->Write();

    }



    typedef itk::VTKColorTransferFunction< RGBImageType > VTKColorTransferType;
    VTKColorTransferType::Pointer vtkcolortransfer = VTKColorTransferType::New();

    vtkcolortransfer->SetInput(image);
    vtkcolortransfer->Update();
    vtkColorTransferFunction* colortransfer = vtkcolortransfer->GetOutput();



    QApplication app(argc, argv);


    QMainWindow* mainwindow = new QMainWindow();

    QtVTKViewerWidget *viewer = new QtVTKViewerWidget(mainwindow);

    mainwindow-> setCentralWidget(viewer);

    mainwindow->show();
    viewer->setImage(vtkimageobject);
    viewer->SetLookupTable((vtkLookupTable*)colortransfer);

    return app.exec();
}


