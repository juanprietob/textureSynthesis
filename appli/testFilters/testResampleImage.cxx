#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkConstNeighborhoodIterator.h>


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkShrinkImageFilter.h"

using namespace std;

typedef double Pixel;
typedef itk::Vector<Pixel, 3> PixelType;
typedef itk::Image<PixelType, 3> ImageType;
typedef ImageType::Pointer ImagePointer;

int m_Size[3] = {196, 196, 44};
string m_Outfilename = "Resample.mhd";
double m_Scale[3] = {1,1,1};
double m_Translate[3] = {0,0,0};

void help(){
    cout<<"Resample an image."<<endl;
    cout<<"usage: testResampleImage -fn <filename> <options>"<<endl;
    cout<< "options:" <<endl;
    cout<<"-ofn <filename> Output filename default: <inputImage>+"<<m_Outfilename<<endl;
    cout<<"-scale <double double double> scale the image by factor, default = "<<m_Scale[0]<<","<<m_Scale[2]<<","<<m_Scale[1]<<endl;
    cout<<"-translate <double double double> translate the image, default = "<<m_Translate[0]<<","<<m_Translate[1]<<","<<m_Translate[2]<<endl;
    cout<<"-shrink <double double double> Shrink the image at specified dimension."<<endl;
    cout<<endl;
}

int main(int argc, char **argv)
{


    if (argc < 2){
        help();
        return EXIT_FAILURE;
    }

    string inputFilename;
    vector<ImagePointer> inputVectorImages;
    double m_Shrink[3];
    bool shrinkt=false;

    for(int i = 1; i < argc; i++){

        if(string(argv[i]) == "--h" || string(argv[i]) == "--help"){
            help();
            return 0;
        }else if (string(argv[i]) == "-fn"){
            inputFilename = string(argv[i+1]);
        }else if (string(argv[i]) == "-scale"){
            m_Scale[0] = atof(argv[i+1]);
            m_Scale[1] = atof(argv[i+2]);
            m_Scale[2] = atof(argv[i+3]);
        }else if(string(argv[i]) == "-ofn"){
            m_Outfilename = string(argv[i+1]);
        }else if(string(argv[i]) == "-translate"){
            m_Translate[0] = atof(argv[i+1]);
            m_Translate[1] = atof(argv[i+2]);
            m_Translate[2] = atof(argv[i+3]);
        }else if(string(argv[i]) == "-shrink"){
            m_Shrink[0] = atof(argv[i+1]);
            m_Shrink[1] = atof(argv[i+2]);
            m_Shrink[2] = atof(argv[i+3]);
            shrinkt=true;
        }
    }

    cout<<"Reading image: "<<inputFilename<<endl;

    typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
    ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
    reader->SetFileName( inputFilename.c_str() );
    reader->Update();
    ImagePointer image = reader->GetOutput();

    ImageType::PointType origin = image->GetOrigin();

    ImageType::SpacingType spacing = image->GetSpacing();




    for(unsigned j = 0; j < origin.Size(); j++){
        origin[j] = origin[j]*m_Scale[j] + m_Translate[j];
        spacing[j] = spacing[j]*m_Scale[j];
    }

    ImagePointer outimage = ImageType::New();
    outimage->SetRegions(image->GetLargestPossibleRegion());
    outimage->Allocate();
    PixelType pix;
    pix.Fill(0);
    outimage->FillBuffer(pix);


    cout<<"Output image: "<<endl;
    cout<<"\t Origin= "<<origin[0]<<" "<<origin[1]<<" "<<origin[2]<<endl;
    cout<<"\t Spacing= "<<spacing[0]<<" "<<spacing[1]<<" "<<spacing[2]<<endl;

    outimage->SetOrigin(origin);
    outimage->SetSpacing(spacing);



    itk::ImageRegionIterator< ImageType > it(image, image->GetLargestPossibleRegion());
    itk::ImageRegionIterator< ImageType > itout(outimage, outimage->GetLargestPossibleRegion());
    it.GoToBegin();
    itout.GoToBegin();

    cout<<"Image resample"<<endl;

    while(!itout.IsAtEnd()){

        itout.Set(it.Get());

        ++itout;
        ++it;
    }
    cout<<endl;

    if(m_Outfilename == "Resample.mhd"){
        m_Outfilename = ((inputFilename.substr(0, inputFilename.find_last_of(".")))).append(m_Outfilename);
    }

    if(shrinkt){
        cout<<"Shrinking image... "<<endl;
        typedef itk::ShrinkImageFilter <ImageType, ImageType> ShrinkImageFilterType;
        ShrinkImageFilterType::Pointer shrinkFilter = ShrinkImageFilterType::New();
        shrinkFilter->SetInput(image);
        shrinkFilter->SetShrinkFactor(0, m_Shrink[0]);
        shrinkFilter->SetShrinkFactor(1, m_Shrink[1]);
        shrinkFilter->SetShrinkFactor(2, m_Shrink[2]);
        shrinkFilter->Update();
        image = shrinkFilter->GetOutput();
    }

    cout<<"Writting to: "<<m_Outfilename<<endl;

    typedef  itk::ImageFileWriter< ImageType  > WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_Outfilename.c_str());
    writer->SetInput(outimage);
    writer->Update();




    return 0;
}

