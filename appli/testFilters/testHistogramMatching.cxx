
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkhistogrammatchingfilter.h"


using namespace std;

int main(int argc, char ** argv)
{

    if( argc < 3 )
      {
      cout << "Performs histogram matching between two images by computing a CDF of image source and match."<<endl;
      cout << "The algorithm takes the values from match and searches the correct ones in source. "<<endl;
      cout << "Usage: " << argv[0] << " ImageSource ImageToMatch" <<endl;
      cout  << "options : "<<endl;
      cout  << "-mask maskfilename"<< endl;

      return EXIT_FAILURE;
      }

    //bool exec2d = false;
    string maskfilename = "";

    for(int i = 3; i < argc; i++){
        if (string(argv[i]) == "-mask"){
            maskfilename = string(argv[i+1]);
        }
    }

    static const int texeldim = 1;
    typedef double PixelType;
    //typedef itk::RGBPixel< unsigned char > RGBPixelType;

    typedef unsigned char PixMaskType;
    typedef itk::Image<PixMaskType, 3>  ImageMaskType;
    ImageMaskType::Pointer maskimage = 0;

    if(maskfilename.compare("")!=0){


        typedef itk::ImageFileReader< ImageMaskType >  ReaderMaskType;

        ReaderMaskType::Pointer reader = ReaderMaskType::New();
        reader->SetFileName(maskfilename.c_str());
        reader->Update();
        maskimage = reader->GetOutput();

    }


    typedef itk::Vector< PixelType, texeldim > RGBPixelType;
    typedef itk::Image<RGBPixelType, 3>  ImageType;

    typedef itk::ImageFileReader< ImageType >  ReaderType;

    string filename(argv[1]);

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename.c_str());
    reader->Update();

    string filename1(argv[2]);
    ReaderType::Pointer reader1 = ReaderType::New();
    reader1->SetFileName(filename1.c_str());
    reader1->Update();



    typedef itk::HistogramMatchingFilter< ImageType, ImageType > HistogramMatchingType;

    HistogramMatchingType::Pointer histomatch = HistogramMatchingType::New();

    histomatch->SetInputImageSource(reader->GetOutput());
    histomatch->SetInputImageObject(reader1->GetOutput());
    if(maskimage){
        histomatch->SetImageMask(maskimage);

    }
    histomatch->SetTexelDimension(texeldim);
    histomatch->Update();


    typedef itk::ImageFileWriter< ImageType >  WriterType;

    WriterType::Pointer writer = WriterType::New();
    string extension = filename1.substr(filename1.find_last_of("."), 4);
    filename1 = filename1.substr(0, filename1.find_last_of("."));
    filename1.append("Match");
    filename1.append(extension);
    writer->SetFileName(filename1.c_str());
    writer->SetInput(histomatch->GetOutput());
    writer->Update();

    return EXIT_SUCCESS;
}

