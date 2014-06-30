//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "itkrgbimagetovector.h"
#include "itkvectortorgbimage.h"
#include "itkrandomimagegenerator.h"


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
    typedef itk::Image< RGBPixelType, 2 > RGBImageType ;
    typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

    imageReader->SetFileName(inputFilename.c_str());
    imageReader->Update() ;
    RGBImageType::Pointer image = imageReader->GetOutput();

    RGBImageType::Pointer imageobject = RGBImageType::New();
    RGBImageType::RegionType region;
    region.SetSize(0, 600);
    region.SetSize(1, 600);
    region.SetIndex(0,0);
    region.SetIndex(1,0);
    imageobject->SetRegions(region);
    imageobject->Allocate();


    typedef double PixelTypeDouble ;
    typedef itk::Image< PixelTypeDouble, 2 > ImageType ;


    typedef itk::RandomImageGenerator<RGBImageType> RandomImageTypeRGB;
    RandomImageTypeRGB::Pointer randomgenerator = RandomImageTypeRGB::New();
    randomgenerator->SetInputImageSample(image);
    randomgenerator->SetInput(imageobject);
    randomgenerator->SetSamplesNumber(3000);
    //randomgenerator->DebugOn();
    randomgenerator->Update();
    RGBImageType::Pointer imagerandom = randomgenerator->GetOutput();

    typedef itk::ImageFileWriter< RGBImageType > ImageWriterType;
    ImageWriterType::Pointer map_writer = ImageWriterType::New();
    map_writer->SetFileName("testRandomRGB.png");
    map_writer->SetInput( imagerandom  );
    map_writer->Update();


    return 0;
}
