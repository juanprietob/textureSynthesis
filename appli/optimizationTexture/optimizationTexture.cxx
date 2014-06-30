//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "itkoptimizationtexture.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"
//#include "itkApproximateSignedDistanceMapImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "itkThresholdImageFilter.h"

using namespace std;					// make std:: accessible

#define TEMPLATE_EXEC(P, V, I)               \
typedef P PixelType;                    \
typedef itk::Vector< PixelType , V> RGBPixelType;   \
typedef itk::Image< RGBPixelType, 2 > RGBImageType; \
typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;   \
typedef itk::Image< RGBPixelType, I > VectorImageType;         \
typedef itk::Image< itk::Vector< double, 3 >, I > X2UImageType;\
typedef itk::OptimizationTexture<RGBImageType, VectorImageType, X2UImageType> OptimizationType; \
typedef itk::ImageFileWriter< VectorImageType > ImageWriterType;    \
vector< RGBImageType::Pointer > vectsampleimages;\
vector< RGBImageType::Pointer > vectfeaturesampleimages;\
for(unsigned i = 0; i < vectfilenamesample.size(); i++){           \
    ImageReaderType::Pointer imageReader = ImageReaderType::New(); \
    imageReader->SetFileName(vectfilenamesample[i]);               \
    imageReader->Update() ;                                         \
    RGBImageType::Pointer image = imageReader->GetOutput();         \
    vectsampleimages.push_back(image);                              \
}                                                                   \
VectorImageType::Pointer imageobject = VectorImageType::New();  \
VectorImageType::RegionType region;                                    \
for(unsigned i = 0; i < region.GetSize().GetSizeDimension(); i++){ \
    region.SetSize(i, imagesize[i]);                                             \
    region.SetIndex(i, 0);                                               \
}\
X2UImageType::Pointer x2uimage = 0;\
if(x2umapfilename.compare("") != 0){ \
    typedef itk::ImageFileReader< X2UImageType >        X2UImageReaderType ;\
    X2UImageReaderType::Pointer x2ureader = X2UImageReaderType::New();\
    x2ureader->SetFileName(x2umapfilename.c_str());\
    x2ureader->Update();\
    imagesize[0] = x2ureader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];\
    imagesize[1] = x2ureader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];\
    if(I == 3)\
        imagesize[2] = x2ureader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];\
    x2uimage = x2ureader->GetOutput();\
    imageobject->SetRegions(x2uimage->GetLargestPossibleRegion());\
    imageobject->SetSpacing(x2uimage->GetSpacing());\
    imageobject->SetOrigin(x2uimage->GetOrigin());\
}else{\
    imageobject->SetRegions(region);\
}\
imageobject->Allocate();                                            \
VectorImageType::PixelType pixel;                                      \
pixel.Fill(0);                                                      \
imageobject->FillBuffer(pixel);                                     \
OptimizationType::Pointer optimization = OptimizationType::New();   \
optimization->SetInputImageSamples(&vectsampleimages);\
optimization->SetInput(imageobject);\
optimization->SetTexelDimension(vectsize);\
optimization->SetExtension(extension.c_str());\
optimization->SetFilename(filename.c_str());\
optimization->SetSmoothHistogram(histosmoothing);\
optimization->SetSearchZ(search[0]);\
optimization->SetSearchY(search[1]);\
optimization->SetSearchX(search[2]);\
optimization->SetX2UImage( x2uimage );\
optimization->Update();\
imageobject = optimization->GetOutput();\
vectsampleimages.clear();\


void featureDistance(vector< string >& vectfilenamesample, vector< string >& vectfeaturefilenamesample, string& filename){
typedef unsigned char PixelType;
typedef itk::RGBPixel< PixelType > RGBPixelType;
typedef itk::Image< RGBPixelType, 2 > RGBImageType;
typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;

typedef itk::Image< unsigned char, 2 > MaskImageType;
typedef itk::ImageFileReader< MaskImageType > MaskImageReaderType ;

vector< RGBImageType::Pointer > vectsampleimages;
vector< MaskImageType::Pointer > vectfeaturesampleimages;

filename.append("fd");
for(unsigned i = 0; i < vectfilenamesample.size(); i++){
    ImageReaderType::Pointer imageReader = ImageReaderType::New();
    imageReader->SetFileName(vectfilenamesample[i]);
    imageReader->Update() ;
    RGBImageType::Pointer image = imageReader->GetOutput();
    vectsampleimages.push_back(image);
}

for(unsigned i = 0; i < vectfeaturefilenamesample.size(); i++){
    MaskImageReaderType::Pointer imageReader = MaskImageReaderType::New();
    imageReader->SetFileName(vectfeaturefilenamesample[i]);
    imageReader->Update() ;
    MaskImageType::Pointer image = imageReader->GetOutput();
    vectfeaturesampleimages.push_back(image);
}

typedef itk::Vector< double, 4 > VectorPixelType;
typedef itk::Image< VectorPixelType, 2 > VectorImageType;

vectfilenamesample.clear();

for(unsigned i = 0; i < vectsampleimages.size(); i++){

    typedef itk::Image< double, 2 > DistanceImageType;

    typedef  itk::SignedDanielssonDistanceMapImageFilter< MaskImageType, DistanceImageType  > DistanceMapImageFilter;
    DistanceMapImageFilter::Pointer distancefilter = DistanceMapImageFilter::New();
    distancefilter->SetInput(vectfeaturesampleimages[i]);
    //DistanceMapImageFilter->SetInsideValue(255);
    //DistanceMapImageFilter->SetOutsideValue(0);
    distancefilter->Update();

    typedef itk::RescaleIntensityImageFilter< DistanceImageType, MaskImageType > RescaleFilterType;
    RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
    rescaleFilter->SetInput(distancefilter->GetOutput());
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();


    VectorImageType::Pointer vectimage = VectorImageType::New();
    vectimage->SetRegions(vectfeaturesampleimages[i]->GetLargestPossibleRegion());
    vectimage->Allocate();


    typedef itk::ImageRegionIterator< MaskImageType > DistanceImageIterator;
    DistanceImageIterator itdistance(rescaleFilter->GetOutput(), rescaleFilter->GetOutput()->GetLargestPossibleRegion());

    typedef itk::ImageRegionIterator< VectorImageType > VectorImageIterator;
    VectorImageIterator itvect(vectimage, vectimage->GetLargestPossibleRegion());

    typedef itk::ImageRegionIterator< RGBImageType > RGBIteratorType;
    RGBIteratorType itrgb(vectsampleimages[i], vectsampleimages[i]->GetLargestPossibleRegion());

    itrgb.GoToBegin();
    itdistance.GoToBegin();
    itvect.GoToBegin();

    while(!itvect.IsAtEnd()){
        VectorImageType::PixelType pixel = itvect.Get();

        for(unsigned i = 0; i < 3; i++){
            pixel[i] = itrgb.Get()[i];
        }

        pixel[3] = itdistance.Get();
        itvect.Set(pixel);

        ++itvect;
        ++itdistance;
        ++itrgb;
    }

    string filenamefeature = "fdRGBD";
    char buffer [10];
    sprintf (buffer, "%d", i);
    filenamefeature.append(buffer);
    filenamefeature.append(".mhd");
    vectfilenamesample.push_back(filenamefeature);
    typedef itk::ImageFileWriter< VectorImageType > ImageWriterType;
    ImageWriterType::Pointer map_writer = ImageWriterType::New();
    map_writer->SetFileName(filenamefeature.c_str());
    map_writer->SetInput( vectimage  );
    map_writer->Update();


}
}

void help(){
cout<<"Usage: optimizationTexture -sp <filename sample texture> [options]"<<endl;
cout<<"options:"<<endl;
cout<<"--h --help show help menu"<<endl;
cout<<"-sp <filename> Constraint the view of the volume with another sample texture max 3"<<endl;
cout<<"-t <N> type image unsigned char = 0, short = 1, unsigned short = 2, int = 3, unsigned int = 4, double = 5: default 0"<<endl;
cout<<"-fd <filename> Provide a binary mask from the sample to perform a distance transform and use it as an extra channel RGB+D"<<endl;
cout<<"-x2u <filename> Provide a x2u map to produce object guided texture synthesis."<<endl;
cout<<"The number of sp images must be equal to fd images, the size of images sp must be equal to fd"<<endl;
cout<<"-2d perform 2D texture sampling instead of 3D"<<endl;
cout<<"-hs histogram smoothing"<<endl;
cout<<"-size <int int int> Size of the output image X, Y, Z -> default 128 128 128 ex. -size 256 256 128"<<endl;
cout<<"-search <bool bool bool> perform search on following slices: perpendicular to Z (xy slices), Y (xz slices) and X (zy slices) ex. -search 1 0 1"<<endl;
cout<<"-dim <int> default 3"<<endl;
cout<<"-t <type> type image"<<endl;
}

int main(int argc, char **argv)
{


    if (argc == 1){
        help();
        return 0;
    }
    bool featuredistance = false;    
    vector< std::string > vectfilenamesample;
    vector< std::string > vectfeaturefilenamesample;
    std::string inputFilename = "";
    bool image2d = false;
    string extension = ".mhd";//this is for the output extension

    string filename = "", x2umapfilename = "";
    bool histosmoothing = false;
    int imagesize[3];
    imagesize[0] = 128;
    imagesize[1] = 128;
    imagesize[2] = 128;
    bool search[3];
    search[0] = true;
    search[1] = true;
    search[2] = true;
    int sampledim = 3;
    int typeimage = 0;

    for(int i = 1; i < argc; i++){

        if(string(argv[i]) == "--h" || string(argv[i]) == "--help"){
            help();
            return 0;
        }else if (string(argv[i]) == "-fd"){
            featuredistance = true;
            if(i + 1 >= argc){
                std::cout << "ERROR: Feature distance filename missing."
                          <<std::endl
                          <<"-fd <filename>"
                          << std::endl ;
                return EXIT_FAILURE;
            }
            vectfeaturefilenamesample.push_back(argv[i + 1]);
        }else if (string(argv[i]) == "-sp"){
            if(i + 1 >= argc){
                std::cout << "ERROR: Sample filename missing."
                          <<std::endl
                          <<"-sp <filename>"
                          << std::endl ;
                return EXIT_FAILURE;
            }

            string infilename = argv[i + 1];

            filename.append(infilename.substr(infilename.find_last_of("/") + 1));
            filename = filename.substr(0, filename.size()-4);

            vectfilenamesample.push_back(argv[i + 1]);
        }else if (string(argv[i]) == "-2d" || string(argv[i]) == "-2D"){
            image2d = true;
        }else if(string(argv[i]) == "-hs"){
            histosmoothing = true;
        }else if(string(argv[i]) == "-size"){

            imagesize[0] = atoi(argv[i+1]);
            imagesize[1] = atoi(argv[i+2]);
            imagesize[2] = atoi(argv[i+3]);

        }else if(string(argv[i]) == "-search"){
            search[0] = atoi(argv[i+1]);
            search[1] = atoi(argv[i+2]);
            search[2] = atoi(argv[i+3]);
        }else if(string(argv[i]) == "-x2u"){
            x2umapfilename = argv[i+1];
        }else if(string(argv[i]) == "-dim"){
            sampledim = atoi(argv[i+1]);
        }else if(string(argv[i]) == "-t"){
            typeimage = atoi(argv[i+1]);
        }

    }

    if(vectfilenamesample.size() == 0){
        help();
        return EXIT_FAILURE;
    }

    if(featuredistance && vectfeaturefilenamesample.size() != vectfilenamesample.size() ){
        std::cout<<"Number of images are different for every -sp <image sample filname> -fd <image feature filename > "<<endl;
        help();
        return EXIT_FAILURE;
    }


    if(image2d){

        if(featuredistance){
            const int vectsize = 4;
            const int imagedim = 2;            
            featureDistance(vectfilenamesample, vectfeaturefilenamesample, filename);
            TEMPLATE_EXEC(double, vectsize, imagedim)
        }else{
            const int vectsize = 3;
            const int imagedim = 2;            

            if(typeimage == 0){
                cout<<"exec with unsigned char"<<endl;
                extension = ".png";//this is for the output extension
                TEMPLATE_EXEC(unsigned char, vectsize, imagedim)
            }else if(typeimage == 1){
                cout<<"exec with unsigned short"<<endl;
                TEMPLATE_EXEC(short, vectsize, imagedim)
            }else if(typeimage == 2){
                cout<<"exec with unsigned short"<<endl;
                TEMPLATE_EXEC(unsigned short, vectsize, imagedim)
            }else if(typeimage == 3){
                cout<<"exec with int"<<endl;
                TEMPLATE_EXEC(int, vectsize, imagedim)
            }if(typeimage == 4){
                cout<<"exec with unsigned int"<<endl;
                TEMPLATE_EXEC(unsigned int, vectsize, imagedim)
            }else if(typeimage == 5){
                cout<<"exec with double"<<endl;
                TEMPLATE_EXEC(double, vectsize, imagedim)
            }
        }
    }else{

        if(featuredistance || sampledim == 4){
            const int vectsize = 4;
            const int imagedim = 3;
            if(featuredistance){
                cout<<"exec fd"<<endl;
                featureDistance(vectfilenamesample, vectfeaturefilenamesample, filename);
            }
            cout<<"exec with double and dim 4"<<endl;
            TEMPLATE_EXEC(double, vectsize, imagedim)
        }else{
            /*typedef double PixelType;
            typedef itk::Vector< PixelType , vectsize> RGBPixelType;
            typedef itk::Image< RGBPixelType, 2 > RGBImageType;
            typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;
            typedef itk::Image< RGBPixelType, imagedim > VectorImageType;
            typedef itk::Image< itk::Vector< double, 3 >, imagedim > X2UImageType;
            typedef itk::OptimizationTexture<RGBImageType, VectorImageType, X2UImageType> OptimizationType;
            typedef itk::ImageFileWriter< VectorImageType > ImageWriterType;
            vector< RGBImageType::Pointer > vectsampleimages;
            vector< RGBImageType::Pointer > vectfeaturesampleimages;
            for(unsigned i = 0; i < vectfilenamesample.size(); i++){
                ImageReaderType::Pointer imageReader = ImageReaderType::New();
                imageReader->SetFileName(vectfilenamesample[i]);
                imageReader->Update() ;
                RGBImageType::Pointer image = imageReader->GetOutput();
                vectsampleimages.push_back(image);
            }
            VectorImageType::Pointer imageobject = VectorImageType::New();
            VectorImageType::RegionType region;
            for(unsigned i = 0; i < region.GetSize().GetSizeDimension(); i++){
                region.SetSize(i, imagesize[i]);
                region.SetIndex(i, 0);
            }
            X2UImageType::Pointer x2uimage = 0;
            if(x2umapfilename.compare("") != 0){
                typedef itk::ImageFileReader< X2UImageType >        X2UImageReaderType ;
                X2UImageReaderType::Pointer x2ureader = X2UImageReaderType::New();
                x2ureader->SetFileName(x2umapfilename.c_str());
                x2ureader->Update();
                imagesize[0] = x2ureader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
                imagesize[1] = x2ureader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
                if(imagedim == 3)
                    imagesize[2] = x2ureader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
                x2uimage = x2ureader->GetOutput();
                imageobject->SetRegions(x2uimage->GetLargestPossibleRegion());
                imageobject->SetSpacing(x2uimage->GetSpacing());
                imageobject->SetOrigin(x2uimage->GetOrigin());
            }else{
                imageobject->SetRegions(region);
            }
            imageobject->Allocate();
            VectorImageType::PixelType pixel;
            pixel.Fill(0);
            imageobject->FillBuffer(pixel);
            OptimizationType::Pointer optimization = OptimizationType::New();
            optimization->SetInputImageSamples(&vectsampleimages);
            optimization->SetInput(imageobject);
            optimization->SetTexelDimension(vectsize);
            optimization->SetExtension(extension.c_str());
            optimization->SetFilename(filename.c_str());
            optimization->SetSmoothHistogram(histosmoothing);
            optimization->SetSearchZ(search[0]);
            optimization->SetSearchY(search[1]);
            optimization->SetSearchX(search[2]);
            optimization->SetX2UImage( x2uimage );
            optimization->Update();
            imageobject = optimization->GetOutput();*/


            const int vectsize = 3;
            const int imagedim = 3;

            if(typeimage == 0){
                cout<<"exec with unsigned char"<<endl;
                TEMPLATE_EXEC(unsigned char, vectsize, imagedim)
            }else if(typeimage == 1){
                cout<<"exec with unsigned short"<<endl;
                TEMPLATE_EXEC(short, vectsize, imagedim)
            }else if(typeimage == 2){
                cout<<"exec with unsigned short"<<endl;
                TEMPLATE_EXEC(unsigned short, vectsize, imagedim)
            }else if(typeimage == 3){
                cout<<"exec with int"<<endl;
                TEMPLATE_EXEC(int, vectsize, imagedim)
            }if(typeimage == 4){
                cout<<"exec with unsigned int"<<endl;
                TEMPLATE_EXEC(unsigned int, vectsize, imagedim)
            }else if(typeimage == 5){
                cout<<"exec with double"<<endl;
                TEMPLATE_EXEC(double, vectsize, imagedim)
            }

        }

    }

    return 0;
}


