//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "itkrgbimagetovectorfeatures.h"
#include "itkrgbimagetovector.h"
#include "itkvectortorgbimage.h"
#include "itkoptimizationtexture.h"

#include "itkImageConstIterator.h"


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


    typedef itk::RGBImageToVectorFeatures< RGBImageType > ImageFeaturesType;
    ImageFeaturesType::Pointer imagefeatures = ImageFeaturesType::New();
    imagefeatures->SetInput(image);
    imagefeatures->Update();

    typedef itk::VectorImage<double, 2> VectorImageType;
    typedef VectorImageType::Pointer VectorImagePointer;
    VectorImagePointer imagevector = imagefeatures->GetOutput();

    typedef itk::ImageRegionConstIterator< VectorImageType > ImageConstIterationType;
    ImageConstIterationType it(imagevector, imagevector->GetLargestPossibleRegion());


    typedef itk::Image<unsigned char, 2> ImageOutType;
    typedef itk::ImageRegionIterator< ImageOutType > ImageIterationType;


    typedef itk::ImageFileWriter< ImageOutType > ImageWriterType;
    ImageWriterType::Pointer map_writer = ImageWriterType::New();



    for(unsigned i = 0; i < 8; i++){

        ImageOutType::Pointer imageout = ImageOutType::New();
        imageout->SetRegions(imagevector->GetLargestPossibleRegion());
        imageout->Allocate();

        ImageIterationType outit(imageout, imageout->GetLargestPossibleRegion());

        outit.GoToBegin();
        it.GoToBegin();

        while(!it.IsAtEnd()){
            outit.Set(it.Get()[i]);

            ++it;
            ++outit;
        }

        std::string filename = "testVectorFeatures";
        char buffer[10];
        sprintf(buffer, "%d", i);
        filename.append( buffer );
        filename.append(".png");
        map_writer->SetFileName(filename.c_str());
        map_writer->SetInput( imageout  );
        map_writer->Update();
    }



    return 0;
}
