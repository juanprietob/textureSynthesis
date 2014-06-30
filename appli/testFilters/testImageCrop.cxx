
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkCropImageFilter.h"


using namespace std;

int main(int argc, char ** argv)
{

    if( argc < 4 )
      {
      std::cerr << "Usage: " << argv[0];
      std::cerr << " filename";
      std::cerr << " -index x y z "<< std::endl;
      std::cerr << " -size x y z "<< std::endl;
      std::cerr<< "ex: "<<argv[0]<<"-index 10 10 10 -size 5 5 5"<<endl;
      return EXIT_FAILURE;
      }

    typedef itk::Image<double, 3>  ImageType;

    typedef itk::ImageFileReader< ImageType >  ReaderType;

    string filename(argv[1]);

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    ImageType::Pointer image = reader->GetOutput();


    int xindex = atoi(argv[3]);
    int yindex = atoi(argv[4]);
    int zindex = atoi(argv[5]);

    int xsize = atoi(argv[7]);
    int ysize = atoi(argv[8]);
    int zsize = atoi(argv[9]);

    ImageType::RegionType cropregion;
    cropregion.SetIndex(0, xindex);
    cropregion.SetIndex(1, yindex);
    cropregion.SetIndex(2, zindex);

    cropregion.SetSize(0, xsize);
    cropregion.SetSize(1, ysize);
    cropregion.SetSize(2, zsize);


    ImageType::Pointer outimage = ImageType::New();
    outimage->SetRegions(cropregion);
    outimage->SetOrigin(image->GetOrigin());
    outimage->SetSpacing(image->GetSpacing());
    outimage->Allocate();
    outimage->FillBuffer(0);

    typedef itk::ImageRegionIterator<ImageType> ImageIteratorType;
    ImageIteratorType it(image, cropregion);
    ImageIteratorType outit(outimage, outimage->GetLargestPossibleRegion());

    it.GoToBegin();
    outit.GoToBegin();

    while(!outit.IsAtEnd()){
        outit.Set(it.Get());

        ++it;
        ++outit;
    }

    typedef itk::ImageFileWriter< ImageType >  WriterType;

    WriterType::Pointer writer = WriterType::New();
    //string extension = filename.substr(filename.find_last_of("."), 4);
    filename = filename.substr(0, filename.find_last_of("."));
    filename.append("Crop.mhd");
    //filename.append(extension);
    writer->SetFileName(filename.c_str());
    writer->SetInput(outimage);
    writer->Update();


  return EXIT_SUCCESS;
}
