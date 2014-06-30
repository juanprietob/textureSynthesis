


#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "itkrgbimagetovectorfeatures.h"
#include "itkrgbimagetovector.h"
#include "itkvectortorgbimage.h"
#include "itkoptimizationtexture.h"

#include "itkImageConstIterator.h"

#include "itkvectorimagetosamplefeatures.h"


using namespace std;					// make std:: accessible




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


    typedef itk::Statistics::VectorImageToSampleFeatures< RGBImageType > ImageFeaturesType;
    ImageFeaturesType::Pointer imagefeatures = ImageFeaturesType::New();
    imagefeatures->SetInput(image);
    imagefeatures->SetNeighborhoodSize(8);
    imagefeatures->Update();
    ImageFeaturesType::ListSampleType *list = const_cast<ImageFeaturesType::ListSampleType*>(imagefeatures->GetListSample());



    if(list->Size() != 9){
        std::cout<<"Error "<<list->Size()<<endl;
        return 1;
    }

    for(unsigned i = 0; i < list->Size();i++){
        ImageFeaturesType::MeasurementVectorType measurevector = list->GetMeasurementVector(i);
        std::cout<<"vector "<<i;
        for(unsigned j = 0; j < measurevector.Size();j++){
            std::cout<<"; "<<(double)measurevector[j];
        }

        std::cout<<std::endl;
    }




    return 0;
}
