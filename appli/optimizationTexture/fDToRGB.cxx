//----------------------------------------------------------------------
//		File:			fdToRGB.cxx
//  convert a 4 component image into an 3 component image
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------





#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include <itkVectorImageToImageAdaptor.h>
#include "itkImageFileWriter.h"
#include "itkVectorRescaleIntensityImageFilter.h"

using namespace std;					// make std:: accessible

string outext = "mhd";
int cincomp = 4;

#define TEMPLATE_EXEC(P, N)\
typedef itk::Image< P, 3 > OutImageType;\
OutImageType::Pointer outimage = OutImageType::New();\
outimage->SetRegions(image->GetLargestPossibleRegion());\
outimage->SetSpacing(image->GetSpacing());\
outimage->SetOrigin(image->GetOrigin());\
outimage->Allocate();\
typedef itk::ImageRegionIterator< OutImageType > OutImageIteratorType;\
OutImageIteratorType outit(outimage, outimage->GetLargestPossibleRegion());\
it.GoToBegin();\
outit.GoToBegin();\
while(!it.IsAtEnd()){\
   VectorPixelType pix = it.Get();\
   P outpix;\
    for(unsigned i = 0; i < N; i++){\
        outpix[i] = pix[i];\
    }\
    outit.Set(outpix);\
   ++it;\
   ++outit;\
}\
string filenamefeature = inputFilename.substr(0, inputFilename.size()-4);\
filenamefeature.append("Comp0.");\
if(outfn!=""){\
    filenamefeature=outfn;\
    filenamefeature.append(".");\
}\
filenamefeature.append(outext);\
cout<<"Writing image: "<<filenamefeature<<endl;\
typedef itk::ImageFileWriter< OutImageType > ImageWriterType;\
ImageWriterType::Pointer map_writer = ImageWriterType::New();\
map_writer->SetFileName(filenamefeature.c_str());\
map_writer->SetInput(  outimage );\
map_writer->Update();\


#define TEMPLATE_NUMC(TIN, N)\
typedef TIN PixelType;\
typedef itk::Vector< PixelType , N  > VectorPixelType;\
typedef itk::Image< VectorPixelType, 3 > VectorImageType;\
typedef itk::ImageFileReader< VectorImageType > ImageReaderType ;\
ImageReaderType::Pointer imageReader = ImageReaderType::New() ;\
imageReader->SetFileName(inputFilename.c_str());\
imageReader->Update() ;\
VectorImageType::Pointer image = imageReader->GetOutput();\
typedef itk::ImageRegionIterator< VectorImageType > ImageIteratorType;\
ImageIteratorType it(image, image->GetLargestPossibleRegion());\
if(divide == 1){\
    typedef itk::Image< PixelType, 3 > OutImageType;\
    OutImageType::Pointer outimage = OutImageType::New();\
    outimage->SetRegions(image->GetLargestPossibleRegion());\
    outimage->SetSpacing(image->GetSpacing());\
    outimage->SetOrigin(image->GetOrigin());\
    outimage->Allocate();\
    typedef itk::ImageRegionIterator< OutImageType > OutImageIteratorType;\
    OutImageIteratorType outit(outimage, outimage->GetLargestPossibleRegion());\
    VectorPixelType v;\
    for(unsigned i = 0; i < v.GetNumberOfComponents(); i++){\
        it.GoToBegin();\
        outit.GoToBegin();\
        while(!it.IsAtEnd()){\
           VectorPixelType pix = it.Get();\
           double outpix;\
           outpix = pix[i];\
           outit.Set(outpix);\
           ++it;\
           ++outit;\
        }\
        string filenamefeature;\
        if(outfn != ""){\
            filenamefeature = outfn;\
        }else{\
            filenamefeature = inputFilename.substr(0, inputFilename.size()-4);\
        }\
        char buf[50];\
        sprintf(buf, "Comp%d.", i);\
        filenamefeature.append(buf);\
        filenamefeature.append(outext);\
        cout<<"Writing image: "<<filenamefeature<<endl;\
        typedef itk::ImageFileWriter< OutImageType > ImageWriterType;\
        ImageWriterType::Pointer map_writer = ImageWriterType::New();\
        map_writer->SetFileName(filenamefeature.c_str());\
        map_writer->SetInput(  outimage );\
        map_writer->Update();\
    }\
}else if(rescale == 1){\
    typedef itk::Vector< unsigned char , VectorPixelType::Length > OutVectorPixelType;\
    typedef itk::Image< OutVectorPixelType, 3 > OutImageType;\
    OutImageType::Pointer outimage = OutImageType::New();\
    outimage->SetRegions(image->GetLargestPossibleRegion());\
    outimage->SetSpacing(image->GetSpacing());\
    outimage->SetOrigin(image->GetOrigin());\
    outimage->Allocate();\
    typedef itk::ImageRegionIterator< OutImageType > OutImageIteratorType;\
    OutImageIteratorType outit(outimage, outimage->GetLargestPossibleRegion());\
    typedef itk::Image< OutVectorPixelType, 3 > OutputVectorImageType;\
    it.GoToBegin();\
    VectorPixelType max;\
    max.Fill(-9999999);\
    if(!maxrescalebo){\
        while(!it.IsAtEnd()){\
            for(unsigned i = 0; i < max.Size(); i++){\
                if(it.Get()[i] > max[i] ){\
                    max[i]=it.Get()[i];\
                }\
            }\
            ++it;\
        }\
        cout<<"Max: "<<max<<endl;\
    }else{\
        max[0] = maxrescale[0];\
        max[1] = maxrescale[1];\
        max[2] = maxrescale[2];\
    }\
    outit.GoToBegin();\
    it.GoToBegin();\
    while(!it.IsAtEnd()){\
        OutVectorPixelType out;\
        for(unsigned i = 0; i < out.Size(); i++){\
            out[i]=(it.Get()[i]/max[i])*255;\
        }\
        outit.Set(out);\
        ++it;\
        ++outit;\
    }\
    string filenamefeature = inputFilename.substr(0, inputFilename.find_last_of("."));\
    filenamefeature.append("Rescale.");\
    filenamefeature.append(outext);\
    cout<<"Writing image: "<<filenamefeature<<endl;\
    typedef itk::ImageFileWriter< OutputVectorImageType > ImageWriterType;\
    ImageWriterType::Pointer map_writer = ImageWriterType::New();\
    map_writer->SetFileName(filenamefeature.c_str());\
    map_writer->SetInput( outimage );\
    map_writer->Update();\
}else if(numcomponents == 4){\
    cout<<"numcomponents "<<numcomponents<<endl;\
    if(typeimage == 0){\
        typedef itk::Vector< unsigned char , 4  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,4);\
    }else if(typeimage == 1){\
        typedef itk::Vector< short , 4  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,4);\
    }else if(typeimage == 4){\
        typedef itk::Vector< int , 4  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,4);\
    }else if(typeimage == 4){\
        typedef itk::Vector< double , 4  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType, 4);\
    }\
}else if(numcomponents == 3){\
    cout<<"numcomponents "<<numcomponents<<endl;\
    if(typeimage == 0){\
        typedef itk::Vector< unsigned char , 3  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,3);\
    }else if(typeimage == 1){\
        typedef itk::Vector< short , 3  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,3);\
    }else if(typeimage == 3){\
        typedef itk::Vector< int , 3  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,3);\
    }else if(typeimage == 5){\
        typedef itk::Vector< double , 3  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,3);\
    }\
}else if(numcomponents == 2){\
    if(typeimage == 0){\
        typedef itk::Vector< unsigned char , 2  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,2);\
    }else if(typeimage == 1){\
        typedef itk::Vector< short , 2  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,2);\
    }else if(typeimage == 3){\
        typedef itk::Vector< int , 2  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,2);\
    }else if(typeimage == 5){\
        typedef itk::Vector< double , 2  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,3);\
    }\
}else{\
    if(typeimage == 0){\
        typedef itk::Vector< unsigned char , 1  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,1);\
    }else if(typeimage == 1){\
        typedef itk::Vector< short , 1 > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,1);\
    }else if(typeimage == 3){\
        typedef itk::Vector< int , 1  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,1);\
    }else if(typeimage == 5){\
        typedef itk::Vector< double , 1  > OutVectorPixelType;\
        TEMPLATE_EXEC(OutVectorPixelType,1);\
    }\
}\


void help(){
    std::cout <<"convert an image with multiple components to single component default double 4 to uchar 3 (rgb) or";
    std::cout <<endl;
    std::cout <<"usage: fDToRGB -fn <filename> <options>";
    std::cout <<endl;
    std::cout << "options:" <<endl;
    std::cout <<"-c <N> get the first n components of the image only, example: -c 1"<<endl;
    std::cout <<"-cin <N> number of components for the input image, default= "<<cincomp<<", example: -cin 2"<<endl;
    std::cout <<"-tin [uchar 0, short 1, int 3, double 5] intput image type (int), example for short: -tin 1, default: double"<<endl;
    std::cout <<"-tout [uchar 0, short 1, int 3, double 5] output image type (int), example for short: -tout 1, default: double"<<endl;
    std::cout <<"-r <bool> rescales the intensity of an image, all components between [0, 255], converts them to char, example: -r 1"<<endl;
    std::cout <<"-rval <double double double> max value to rescale the components"<<endl;
    std::cout <<"-d <bool> divides all of the components in different files, example: -d 1"<<endl;
    std::cout <<"-outext <string> Set the output extension. default = "<<outext<<endl;
    std::cout <<"-outfn <string> Set the output filename. default = <fn>+[<Comp<N>|Rescale]."<<outext<<endl;
    std::cout <<std::endl;
}

int main(int argc, char **argv)
{


    if (argc < 2)
      {
        help();
        return EXIT_FAILURE;
      }


    int numcomponents = 3;
    int typeimage = 0;
    std::string inputFilename ="";
    int rescale = 0;
    double maxrescale[3] = {0,0,0};
    bool maxrescalebo = false;
    int divide = 0;
    string outfn = "";
    int tin = 5;



    for(int i = 1; i < argc; i++){

        if(string(argv[i]) == "--h" || string(argv[i]) == "--help"){
            help();
            return 0;
        }else if (string(argv[i]) == "-c"){
            numcomponents = atoi(argv[i+1]);
        }else if (string(argv[i]) == "-tout"){
            typeimage = atoi(argv[i+1]);;
        }else if (string(argv[i]) == "-fn"){
            inputFilename = argv[i+1];
        }else if (string(argv[i]) == "-r"){
            rescale = atoi(argv[i+1]);
        }else if (string(argv[i]) == "-rval"){
            maxrescalebo = true;
            maxrescale[0] = atof(argv[i+1]);
            maxrescale[1] = atof(argv[i+2]);
            maxrescale[2] = atof(argv[i+3]);
            cout<<"maxr "<<maxrescale[0]<<", "<<maxrescale[1]<<", "<<maxrescale[2]<<endl;
        }else if (string(argv[i]) == "-d"){
            divide = atoi(argv[i+1]);
        }else if (string(argv[i]) == "-outext" || string(argv[i]) == "-oext"){
            outext = string(argv[i+1]);
        }else if(string(argv[i]) == "-outfn" || string(argv[i]) == "-ofn"){
            outfn = string(argv[i+1]);
        }else if(string(argv[i]) == "-cin"){
            cincomp = atoi(argv[i+1]);
        }else if(string(argv[i]) == "-tin"){
            tin = atoi(argv[i+1]);
        }
    }


    if(tin == 5){

        typedef double TIN;

        if(cincomp == 4){

            TEMPLATE_NUMC(TIN, 4);

        }else if(cincomp == 3){

            TEMPLATE_NUMC(TIN, 3);

        }else if(cincomp == 2){

            TEMPLATE_NUMC(TIN, 2);

        }else if(cincomp == 1){

            TEMPLATE_NUMC(TIN, 1);

        }
    }else if(tin == 0){

        typedef unsigned char TIN;

        if(cincomp == 4){

            TEMPLATE_NUMC(TIN, 4);

        }else if(cincomp == 3){

            TEMPLATE_NUMC(TIN, 3);

        }else if(cincomp == 2){

            TEMPLATE_NUMC(TIN, 2);

        }else if(cincomp == 1){

            TEMPLATE_NUMC(TIN, 1);

        }
    }


    return 0;
}



