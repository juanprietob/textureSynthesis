//----------------------------------------------------------------------
//		File:			testITKFilters.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

using namespace std;					// make std:: accessible

#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryDilateImageFilter.h"


int main(int argc, char **argv)
{


    if (argc < 3){

        std::cout <<"Perfroms AND operation between to images, ImageMask is an image with background pixels = 0."<<std::endl;
        std::cout<<" The output image contains the pixels in the image model only in the positions where the "<<std::endl;
        std::cout<<"ImageMask is not 0. Usage:"<<std::endl;
        std::cout<<argv[0] << " ImageModel ImageMask"<<std::endl;
        std::cout<<"<options> -x :   x2u map. Perfroms the AND operation using an RGB image X2U map"<<std::endl;
        std::cout<<"-inv <bool> do inverse and operation. "<<std::endl;
        std::cout<<"\t Preservs pixels that are under blue < 255."<<std::endl;
        std::cout<<"-outfn <filename out> "<<std::endl;


        return EXIT_FAILURE;    
    }

    bool binary = true;
    std::string inputFilenamMask = "";
    std::string outfilename = "";
    int inverse = 0;
    int dilate = 0;
    for(int i = 1; i < argc; i++){

        if(string(argv[i]) == "-x"){
            binary = false;
            inputFilenamMask = string(argv[i+1]);
        }else if("-outfn" == string(argv[i]) || "-ofn" == string(argv[i])){
            outfilename = string(argv[i+1]);
        }else if("-inv" == string(argv[i])){
            inverse = atoi(argv[i+1]);
        }else if("-dilate" == string(argv[i])){
            dilate = atoi(argv[i+1]);
        }
    }




    if(binary){
        std::string inputFilename = argv[1];
        std::string inputFilenameMask = argv[2];



        cout<<"Mask filename: "<<inputFilenameMask<<endl;
        cout<<"Object filename: "<<inputFilename<<endl;


        typedef double PixelType;
        typedef itk::Image< PixelType, 3 > InputImageType ;
        typedef itk::ImageFileReader< InputImageType > ImageReaderType ;
        ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

        imageReader->SetFileName(inputFilename.c_str());
        imageReader->Update() ;
        InputImageType::Pointer image = imageReader->GetOutput();

        typedef unsigned char MaskPixelType;
        typedef itk::Image< MaskPixelType, 3 > InputImageMaskType ;
        typedef itk::ImageFileReader< InputImageMaskType > ImageMaskReaderType ;
        ImageMaskReaderType::Pointer imageMaskReader = ImageMaskReaderType::New() ;

        imageMaskReader->SetFileName(inputFilenameMask.c_str());
        imageMaskReader->Update() ;
        InputImageMaskType::Pointer imagemask = imageMaskReader->GetOutput();

        if(dilate){

            typedef itk::BinaryBallStructuringElement<InputImageMaskType::PixelType,3> StructuringElementType;
            StructuringElementType structuringElement;
            structuringElement.SetRadius(dilate);
            structuringElement.CreateStructuringElement();

            typedef itk::BinaryDilateImageFilter <InputImageMaskType, InputImageMaskType, StructuringElementType> BinaryDilateImageFilterType;

            BinaryDilateImageFilterType::Pointer dilateFilter = BinaryDilateImageFilterType::New();
            dilateFilter->SetInput(imagemask);
            dilateFilter->SetKernel(structuringElement);
            dilateFilter->Update();

            imagemask = dilateFilter->GetOutput();

        }


        InputImageType::Pointer outimage = InputImageType::New();
        outimage->SetRegions(imagemask->GetLargestPossibleRegion());
        outimage->SetSpacing(imagemask->GetSpacing());
        outimage->SetOrigin(imagemask->GetOrigin());
        outimage->Allocate();
        outimage->FillBuffer(0);


        typedef itk::ImageRegionIterator< InputImageType >          ImageIteratorType;
        typedef itk::ImageRegionIterator< InputImageMaskType >      IteratorType;

        ImageIteratorType itimage(image, imagemask->GetLargestPossibleRegion());        

        IteratorType  itmask(imagemask, imagemask->GetLargestPossibleRegion());
        ImageIteratorType  itoutobj(outimage, outimage->GetLargestPossibleRegion());

        itimage.GoToBegin();
        itmask.GoToBegin();
        itoutobj.GoToBegin();

        while(!itimage.IsAtEnd() && !itmask.IsAtEnd()){
            if(itmask.Get() != 0){
                if(!inverse){
                    itoutobj.Set(itimage.Get());
                }
            }else{
                if(inverse){
                    itoutobj.Set(itimage.Get());
                }
            }
            ++itimage;
            ++itmask;
            ++itoutobj;
        }


        typedef itk::ImageFileWriter< InputImageType > ImageWriterType;
        ImageWriterType::Pointer map_writer = ImageWriterType::New();

        if(outfilename == ""){
            outfilename = inputFilenameMask.substr(0, inputFilenameMask.find_last_of("."));
            outfilename.append("AND.mhd");
        }

        cout<<"Writing image to: "<<outfilename<<endl;

        map_writer->SetFileName(outfilename.c_str());
        map_writer->SetInput( outimage  );
        map_writer->Update();
    }else{


        std::string inputFilename = argv[1];



        typedef unsigned char PixelType;
        typedef itk::RGBPixel< PixelType > RGBPixelType;
        typedef itk::Image< RGBPixelType, 3 > InputImageType ;
        typedef itk::ImageFileReader< InputImageType > ImageReaderType ;
        ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

        imageReader->SetFileName(inputFilename.c_str());
        imageReader->Update() ;
        InputImageType::Pointer image = imageReader->GetOutput();


        /*typedef unsigned short PixelTypeObject;
        typedef itk::Image< PixelTypeObject, 3 > InputImageObjectType ;
        typedef itk::ImageFileReader< InputImageObjectType > ImageObjectReaderType ;
        ImageObjectReaderType::Pointer imageObjectReader = ImageObjectReaderType::New() ;

        imageObjectReader->SetFileName(inputFilenamObjecte.c_str());
        imageObjectReader->Update() ;
        InputImageObjectType::Pointer imageobj = imageObjectReader->GetOutput();*/

        typedef itk::Image< RGBPixelType, 3 > InputImageObjectType ;
        typedef InputImageObjectType::PixelType  ImageObjectPixelType;
        typedef itk::ImageFileReader< InputImageObjectType > ImageObjectReaderType ;
        ImageObjectReaderType::Pointer imageObjectReader = ImageObjectReaderType::New() ;

        imageObjectReader->SetFileName(inputFilenamMask.c_str());
        imageObjectReader->Update() ;
        InputImageObjectType::Pointer imageobj = imageObjectReader->GetOutput();

        ImageObjectPixelType pixelobj;
        pixelobj.Fill(0);

        RGBPixelType pixel;
        pixel.Fill(0);

        InputImageType::Pointer outimage = InputImageType::New();
        outimage->SetRegions(image->GetLargestPossibleRegion());
        outimage->SetSpacing(image->GetSpacing());
        outimage->SetOrigin(image->GetOrigin());
        outimage->Allocate();
        outimage->FillBuffer(pixel);


        typedef itk::ImageRegionIterator< InputImageType >          RGBIteratorType;
        typedef itk::ImageRegionIterator< InputImageObjectType >    IteratorType;

        RGBIteratorType itimage(image, imageobj->GetLargestPossibleRegion());

        IteratorType  itobj(imageobj, imageobj->GetLargestPossibleRegion());
        RGBIteratorType  itoutobj(outimage, outimage->GetLargestPossibleRegion());

        itimage.GoToBegin();
        itobj.GoToBegin();
        itoutobj.GoToBegin();

        while(!itimage.IsAtEnd() && !itobj.IsAtEnd()){
            if(itobj.Get()[2] <= 255 && itobj.Get()!=pixelobj){
                if(!inverse){
                    itoutobj.Set(itimage.Get());
                }
            }else{
                if(inverse){
                    itoutobj.Set(itimage.Get());
                }
            }
            ++itimage;
            ++itobj;
            ++itoutobj;
        }


        typedef itk::ImageFileWriter< InputImageType > ImageWriterType;
        ImageWriterType::Pointer map_writer = ImageWriterType::New();

        if(outfilename == ""){
            outfilename = inputFilename.substr(0, inputFilename.find_last_of("."));
            outfilename.append("Object.mhd");
        }

        cout<<"Writing image to: "<<outfilename<<endl;

        map_writer->SetFileName(outfilename.c_str());
        map_writer->SetInput( outimage  );
        map_writer->Update();
    }


    return 0;
}

