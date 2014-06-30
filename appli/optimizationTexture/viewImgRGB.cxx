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


    if (argc < 2)
      {
        std::cout << "ERROR: data file name argument missing."
                  << std::endl ;
        return EXIT_FAILURE;
      }


    std::string inputFilename = argv[1];


    typedef unsigned char PixelType;
    typedef itk::RGBPixel< PixelType > RGBPixelType;
    typedef itk::Image< RGBPixelType, 3 > RGBImageType;

    typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

    imageReader->SetFileName(inputFilename.c_str());
    imageReader->Update() ;
    RGBImageType::Pointer image = imageReader->GetOutput();

    typedef itk::ITKToVTKImageImport< RGBImageType > ITKToVTKImageType;
    //typedef itk::ITKToVTKImageImport< RGBImageType > ITKToVTKImageType;
    ITKToVTKImageType::Pointer itktovtk = 0;
    itktovtk = ITKToVTKImageType::New();
    itktovtk->SetITKImage(image);
    itktovtk->Update();

    vtkImageData* vtkimageobject = itktovtk->GetOutputVTKImage();


    QApplication app(argc, argv);


    QMainWindow* mainwindow = new QMainWindow();

    QtVTKViewerWidget *viewer = new QtVTKViewerWidget(mainwindow);

    mainwindow-> setCentralWidget(viewer);

    mainwindow->show();
    viewer->setImage(vtkimageobject);    


    return app.exec();
}


