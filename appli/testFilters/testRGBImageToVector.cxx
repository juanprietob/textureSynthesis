//----------------------------------------------------------------------
//		File:			testRGBImageToVector.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <itkVector.h>

#include "itkImageRegionIteratorWithIndex.h"

#include <itkVectorNearestNeighborInterpolateImageFunction.h>
#include <itkVectorExpandImageFilter.h>
#include <itkvectordiscretegaussianimagefilter.h>

using namespace std;


int main(int argc, char **argv)
{


    if (argc < 2)
      {
        std::cout << argv[0]
                  <<" filename texture volume"
                  <<std::endl
                  <<"Output lumminance all in 255"
                  <<std::endl ;
        return EXIT_FAILURE;
      }

    std::string inputFilename = argv[1];


    typedef unsigned char PixelType;
    typedef itk::RGBPixel< PixelType > RGBPixelType;
    typedef itk::Image< RGBPixelType, 3 > RGBImageType ;
    typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;

    ImageReaderType::Pointer readertexture = ImageReaderType::New() ;

    readertexture->SetFileName(inputFilename.c_str());
    readertexture->Update() ;
    RGBImageType::Pointer imagetexture = readertexture->GetOutput();

    typedef itk::Vector< PixelType, 4 > VectorPixelType;
    typedef itk::Image< VectorPixelType, 3 > VectorImageType ;

    VectorImageType::Pointer outimage = VectorImageType::New();
    outimage->SetRegions(imagetexture->GetLargestPossibleRegion());
    outimage->SetSpacing(imagetexture->GetSpacing());
    outimage->SetOrigin(imagetexture->GetOrigin());
    outimage->Allocate();

    VectorImageType::PixelType pixel;
    pixel.Fill(0);

    outimage->FillBuffer(pixel);

    typedef itk::ImageRegionIteratorWithIndex< RGBImageType >    RGBIteratorType;
    typedef itk::ImageRegionIteratorWithIndex< VectorImageType > VectorIteratorType;

    RGBIteratorType  imagetextureIt(  imagetexture, imagetexture->GetLargestPossibleRegion() );
    VectorIteratorType  outIt(  outimage, outimage->GetLargestPossibleRegion() );

    imagetextureIt.GoToBegin();
    outIt.GoToBegin();

    while( !imagetextureIt.IsAtEnd() )
    {


        RGBPixelType pix = imagetextureIt.Get();

        VectorPixelType outpix;
        outpix.Fill(0);

        outpix[0] = pix[0];
        outpix[1] = pix[1];
        outpix[2] = pix[2];
        outpix[3] = 255;


        outIt.Set(outpix);

        ++imagetextureIt;
        ++outIt;
    }

    string filenametexture = inputFilename.substr(0, inputFilename.size()-4);
    filenametexture.append("Texture.mhd");


    typedef itk::ImageFileWriter< VectorImageType > ImageWriterType;
    ImageWriterType::Pointer map_writer = ImageWriterType::New();
    map_writer->SetFileName(filenametexture.c_str());
    map_writer->SetInput( outimage  );
    map_writer->Update();

    return 0;
}
