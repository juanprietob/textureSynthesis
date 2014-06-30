
//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------


#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "itkrgbimagetovector.h"
#include "itkvectortorgbimage.h"
#include "itkrandomimagegenerator.h"
#include "itkhistogrammatchingfilter.h"

#include "itkRGBToLuminanceImageFilter.h"

#include "itktovtkimageimport.h"
#include "itkvtkcolortransferfunction.h"

#include "vtkMetaImageWriter.h"
#include "vtkImageData.h"


using namespace std;

int main(int argc, char **argv)
{


    if (argc < 2)
      {
        std::cout << "ERROR: data file name argument missing."
                  << std::endl ;
        return EXIT_FAILURE;
      }

    std::string inputFilename = argv[1];


    typedef double PixelType;
    typedef itk::Vector< PixelType, 3 > RGBPixelType;
    typedef itk::Image< RGBPixelType, 2 > RGBImageType ;
    typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

    imageReader->SetFileName(inputFilename.c_str());
    imageReader->Update() ;
    RGBImageType::Pointer image = imageReader->GetOutput();



    typedef itk::Image< RGBPixelType, 3 > RGBImage3DType;
    RGBImage3DType::Pointer imageobject = RGBImage3DType::New();
    RGBImage3DType::RegionType region;
    if(argc == 5){
        int x[3] = {atoi(argv[2]), atoi(argv[3]), atoi(argv[4])};
        cout<<"Size: "<<x[0]<<" "<<x[1]<<" "<<x[2]<<endl;
        region.SetSize(0, x[0]);
        region.SetSize(1, x[1]);
        region.SetSize(2, x[2]);
    }else{
        cout<<"Size: "<<512<<endl;
        region.SetSize(0, 512);
        region.SetSize(1, 512);
        region.SetSize(2, 512);
    }
    imageobject->SetRegions(region);

    imageobject->Allocate();


    typedef itk::RandomImageGenerator<RGBImageType, RGBImage3DType> RandomImageType;
    RandomImageType::Pointer randomgenerator = RandomImageType::New();
    randomgenerator->SetInputImageSample(image);
    randomgenerator->SetInput(imageobject);
    randomgenerator->SetSamplesNumber(4000);
    //randomgenerator->DebugOn();
    randomgenerator->Update();
    imageobject = randomgenerator->GetOutput();


    /*typedef itk::HistogramMatchingFilter<  RGBImageType, RGBImage3DType > HistogramMatchingType;
    HistogramMatchingType::Pointer histogrammatch = HistogramMatchingType::New();
    histogrammatch->SetInputImageSource(image);
    histogrammatch->SetInputImageObject(imageobject);
    histogrammatch->Update();
    imageobject = histogrammatch->GetOutput();*/

    /*typedef itk::ITKToVTKImageImport< HistogramMatchingType::OutputImageType > ITKToVTKImageHistoType;
    ITKToVTKImageHistoType::Pointer itktovtkhisto = ITKToVTKImageHistoType::New();
    itktovtkhisto->SetITKImage(histogrammatch->GetOutput());
    itktovtkhisto->Update();

    vtkImageData* vtkimageobjecthisto =itktovtkhisto->GetOutputVTKImage();*/    



    string mhdfile = inputFilename.substr(0, inputFilename.find_last_of("."));
    mhdfile.append("Random3D.mhd");

    cout<<"Writting to: "<<mhdfile<<endl;

    typedef  itk::ImageFileWriter< RGBImage3DType  > WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(mhdfile.c_str());
    writer->SetInput(imageobject);
    writer->Update();




    return 0;
}
