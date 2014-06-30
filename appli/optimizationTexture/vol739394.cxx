
//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "itkrgbimagetovector.h"
#include "itkvectortorgbimage.h"
#include "itkoptimizationtexture.h"
#include "itkrandomimagegenerator.h"
#include "itkRGBToLuminanceImageFilter.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkThresholdImageFilter.h"

#include <iostream>

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
    typedef itk::SignedMaurerDistanceMapImageFilter< MaskImageType, DistanceImageType > DistanceType;
    DistanceType::Pointer distance = DistanceType::New();
    distance->SetInput(vectfeaturesampleimages[i]);
    distance->InsideIsPositiveOn();
    distance->Update();


    VectorImageType::Pointer vectimage = VectorImageType::New();
    vectimage->SetRegions(vectfeaturesampleimages[i]->GetLargestPossibleRegion());
    vectimage->Allocate();


    typedef itk::ImageRegionIterator< DistanceImageType > DistanceImageIterator;
    DistanceImageIterator itdistance(distance->GetOutput(), distance->GetOutput()->GetLargestPossibleRegion());

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
cout<<"-fd <filename> Provide a binary mask from the sample to perform a distance transform and use it as an extra channel RGB+D"<<endl;
cout<<"The number of sp images must be equal to fd images, the size of images sp must be equal to fd"<<endl;
cout<<"-2d perform 2D texture sampling instead of 3D"<<endl;
cout<<"-hs histogram smoothing"<<endl;
cout<<"-size <int int int> Size of the output image X, Y, Z -> default 128 128 128 ex. -size 256 256 128"<<endl;
cout<<"-search <bool bool bool> search for slices perpendicular to Z (xy slices), Y (xz slices) and X (zy slices) ex. -search 1 0 1"<<endl;
}

int main(int argc, char **argv)
{





    srand ( time(NULL) );

    int count = 0;
    while(count < 10){


        vector< std::string > vectfilenamesample;
        vector< std::string > vectfeaturefilenamesample;

        string dirx = "/home/prieto/Images/Echantillons Texture Medicale/vol.73-93-94_volraw-slice_os_trabeculaire_IRM_X/";
        string dirxmask = "/home/prieto/Images/Echantillons Texture Medicale/vol.73-93-94_volraw-slice_os_trabeculaire_IRM_XMask/";

        string diry = "/home/prieto/Images/Echantillons Texture Medicale/vol.73-93-94_volraw-slice_os_trabeculaire_IRM_Y/";
        string dirymask = "/home/prieto/Images/Echantillons Texture Medicale/vol.73-93-94_volraw-slice_os_trabeculaire_IRM_YMask/";

        string dirz = "/home/prieto/Images/Echantillons Texture Medicale/vol.73-93-94_volraw-slice_os_trabeculaire_IRM_Z/";
        string dirzmask = "/home/prieto/Images/Echantillons Texture Medicale/vol.73-93-94_volraw-slice_os_trabeculaire_IRM_ZMask/";

        string filename = "";


        string filex = "vol.73-93-94-slice";
        string filey = "vol.73-93-94-slice";
        string filez = "vol.73-93-94-slice";


        int randomx = rand() % 140;
        int randomy = rand() % 180;
        int randomz = rand() % 92;


        char buffer [50];

        sprintf (buffer, "%d", randomx);

        if(randomx < 10){
            filex.append("00");
        }else if(randomx < 100){
            filex.append("0");
        }
        filex.append(buffer);

        sprintf (buffer, "%d", randomy);

        if(randomy < 10){
            filey.append("00");
        }else if(randomy < 100){
            filey.append("0");
        }
        filey.append(buffer);

        sprintf (buffer, "%d", randomz);

        if(randomz < 10){
            filez.append("00");
        }else if(randomz < 100){
            filez.append("0");
        }
        filez.append(buffer);


        filename.append(filex);
        filename.append(filey);
        filename.append(filez);

        filex.append(".png");
        filey.append(".png");
        filez.append(".png");

        dirx.append(filex);
        dirxmask.append(filex);

        diry.append(filey);
        dirymask.append(filey);

        dirz.append(filez);
        dirzmask.append(filez);        

        vectfilenamesample.push_back(dirx);
        vectfilenamesample.push_back(diry);
        vectfilenamesample.push_back(dirz);


        vectfeaturefilenamesample.push_back(dirxmask);
        vectfeaturefilenamesample.push_back(dirymask);
        vectfeaturefilenamesample.push_back(dirzmask);


        int imagesize[3];
        imagesize[0] = 128;
        imagesize[1] = 128;
        imagesize[2] = 128;
	bool search[3];
	search[0] = true;
	search[1] = true;
	search[2] = false;
	
	string x2umapfilename = "";
	bool histosmoothing = false;


        string extension = ".mhd";


        const int vectsize = 4;
        const int imagedim = 3;
        featureDistance(vectfilenamesample, vectfeaturefilenamesample, filename);

        cout<<filename<<endl;
        TEMPLATE_EXEC(double, vectsize, imagedim)

        count++;
    }






    return 0;
}


