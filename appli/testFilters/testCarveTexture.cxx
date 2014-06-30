//----------------------------------------------------------------------
//		File:			testITKFilters.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkVectorNearestNeighborInterpolateImageFunction.h>



#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"


using namespace std;					// make std:: accessible


int main(int argc, char **argv)
{


    if (argc < 3)
      {
        std::cout <<"Perfroms a 'carving' operation. ImageModel is a textured image and ImageMask is an image with background pixels = 0.";
        std::cout <<std::endl;
        std::cout <<"The output image contains the pixels in the image model only in the positions where the ";
        std::cout <<"ImageMask is not 0. The Image Model changes the spacing and origin to englobe the bounding box of the object";
        std::cout<<"in the ImageMask. It finally performs interpolation using the positions in the image mask and the values in the ";
        std::cout<<"ImageModel. Usage:"<<std::endl;
        std::cout <<argv[0] << " ImageModel ImageMask";
        std::cout <<std::endl;
        std::cout <<std::endl ;
        return EXIT_FAILURE;
    }


    std::string inputFilename = argv[1];
    std::string inputFilenameMask = argv[2];

    unsigned int dilate = 0;
    if(argc == 4){
        dilate = atoi(argv[3]);
    }


    cout<<"-imageModel = "<<inputFilename<<endl;
    cout<<"-imageMask = "<<inputFilenameMask<<endl;


    typedef double PixelType;
    typedef itk::Vector<PixelType, 3 > VectorPixelType;
    typedef itk::Image< VectorPixelType, 3 > InputImageType ;
    typedef itk::ImageFileReader< InputImageType > ImageReaderType ;
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

    imageReader->SetFileName(inputFilename.c_str());
    imageReader->Update() ;
    InputImageType::Pointer image = imageReader->GetOutput();

    typedef itk::Image< unsigned char, 3 > InputImageMaskType ;
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
    InputImageType::PixelType outpix;
    outpix.Fill(0);
    outimage->FillBuffer(outpix);

    typedef itk::VectorNearestNeighborInterpolateImageFunction< InputImageType > InterpolateImage;
    InterpolateImage::Pointer interpolate = InterpolateImage::New();
    interpolate->SetInputImage(image);

    typedef itk::ImageRegionIterator< InputImageType >          ImageIteratorType;
    typedef itk::ImageRegionIterator< InputImageMaskType >      IteratorType;

    ImageIteratorType itimage(image, image->GetLargestPossibleRegion());

    IteratorType  itmask(imagemask, imagemask->GetLargestPossibleRegion());
    ImageIteratorType  itoutobj(outimage, outimage->GetLargestPossibleRegion());

    itmask.GoToBegin();
    InputImageMaskType::SpacingType spcmask = imagemask->GetSpacing();
    InputImageMaskType::PointType minpoint;
    minpoint.Fill(9999999);
    InputImageMaskType::PointType maxpoint;
    maxpoint.Fill(-9999999);

    while(!itmask.IsAtEnd()){

        if(itmask.Get() != 0){
            InputImageMaskType::IndexType indexmask = itmask.GetIndex();
            InputImageMaskType::PointType point = imagemask->GetOrigin();
            point[0] += indexmask[0]*spcmask[0];
            point[1] += indexmask[1]*spcmask[1];
            point[2] += indexmask[2]*spcmask[2];

            for(unsigned i = 0; i < point.Size(); i++){
                if(point[i]<minpoint[i]){
                    minpoint[i] = point[i];
                }
                if(point[i] > maxpoint[i]){
                    maxpoint[i] = point[i];
                }
            }
        }
        ++itmask;
    }


    image->SetOrigin(minpoint);
    InputImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();

    double maxspc = 0;
    for(unsigned i = 0; i < maxpoint.Size(); i++){
        double tempspc = (maxpoint[i] - minpoint[i])/size[i];
        maxspc = max(maxspc, tempspc);
    }

    if(maxspc != 0){

        InputImageType::SpacingType spc = image->GetSpacing();
        spc.Fill(maxspc);
        image->SetSpacing(spc);

        cout<<"Minpoint = "<<minpoint<<endl;
        cout<<"Spc = "<<spc<<endl;

        itimage.GoToBegin();
        itmask.GoToBegin();
        itoutobj.GoToBegin();

        cout<<"Carving Image..."<<endl;

        while(!itmask.IsAtEnd()){

            if(itmask.Get() != 0){
                InputImageMaskType::IndexType indexmask = itmask.GetIndex();
                InputImageMaskType::PointType point = imagemask->GetOrigin();
                point[0] += indexmask[0]*spcmask[0];
                point[1] += indexmask[1]*spcmask[1];
                point[2] += indexmask[2]*spcmask[2];


                InputImageType::IndexType index;
                interpolate->ConvertPointToNearestIndex (point, index);
                if(interpolate->IsInsideBuffer(index)){
                    InputImageType::PixelType outpix = interpolate->EvaluateAtIndex(index);

                    itoutobj.Set(outpix);
                }
            }

            ++itmask;
            ++itoutobj;
        }


        typedef itk::ImageFileWriter< InputImageType > ImageWriterType;
        ImageWriterType::Pointer map_writer = ImageWriterType::New();

        string filename = inputFilenameMask.substr(0, inputFilenameMask.find_last_of("."));
        filename.append("Texture.mhd");
        map_writer->SetFileName(filename.c_str());
        map_writer->SetInput( outimage  );
        map_writer->Update();

        cout<<"Done!" <<endl;
        cout<<"image written to : "<<filename<<endl;
    }else{


        cout<<"ERROR, the spacing is 0, the image mask is probably empty."<<endl;
        cout<<"No output generated."<<endl;
        cout<<endl;
        return -1;

    }
    cout<<endl;
    return 0;
}


