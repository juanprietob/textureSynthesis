
//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------


#include <qtvtkviewerwidget.h>
#include <QMainWindow>
#include <QApplication>

#include "vtkImageData.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itktovtkimageimport.h"
#include "vtkMetaImageReader.h"
#include "itkImageFileWriter.h"
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

    bool vtkreader = false;
    if(argc > 2){
        if (string(argv[2]) == "-vtkreader"){
            vtkreader = true;
        }
    }



    std::string inputFilename = argv[1];




    vtkImageData* vtkimageobject = 0;

    typedef double PixelType;
    typedef itk::Image< PixelType, 3 > ImageType;

    typedef itk::ImageFileReader< ImageType > ImageReaderType ;

    typedef itk::ITKToVTKImageImport< ImageType > ITKToVTKImageType;
    ITKToVTKImageType::Pointer itktovtk = ITKToVTKImageType::New();

    if(vtkreader){
        vtkMetaImageReader* reader = vtkMetaImageReader::New();
        reader->SetFileName(inputFilename.c_str());
        reader->Update();
        vtkimageobject = reader->GetOutput();

    }else{

        ImageReaderType::Pointer imageReader = ImageReaderType::New() ;
        imageReader->SetFileName(inputFilename.c_str());
        imageReader->Update() ;
        ImageType::Pointer image = imageReader->GetOutput();

        itktovtk->SetITKImage(image);
        itktovtk->Update();
        vtkimageobject = itktovtk->GetOutputVTKImage();


        if(true){
            typedef itk::ImageFileWriter< ImageType > ImageWriterType;
            ImageWriterType::Pointer writer = ImageWriterType::New();
            std::string outputFilename = inputFilename;
            outputFilename = outputFilename.substr(0, outputFilename.find_last_of("."));
            outputFilename.append(".mhd");
            writer->SetFileName(outputFilename.c_str());
            writer->SetInput(image);
            writer->Update();
        }
    }




    QApplication app(argc, argv);


    QMainWindow* mainwindow = new QMainWindow();

    QtVTKViewerWidget *viewer = new QtVTKViewerWidget(mainwindow);

    mainwindow-> setCentralWidget(viewer);

    mainwindow->show();
    viewer->setImage(vtkimageobject);




    return app.exec();
}


