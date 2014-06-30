#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkVectorNearestNeighborInterpolateImageFunction.h>


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

using namespace std;


int m_Size[3] = {255, 255, 255};
string m_Outfilename = "textureAppend.mhd";
int regularspc = 1;

void help(){
    std::cout <<"Append images by computing a larger region and writting the content of each image into the new region."<<endl
            <<endl
            <<"usage: testAppendImages -fn <filename> <options>"
            <<endl
            << "options:" <<endl
            <<"-fn <filename> add as many images you want."<<endl            
            <<"-ofn <filename> Output filename default: "<<m_Outfilename<<endl
           <<"-regularspc <bool> Set a regular spacing on the output image (output size is modified), default = "<<regularspc<<endl
            <<"-size <int int int> size of the output image (Note that if regular spc is set, the output size is modified to all images and have regular spc. Default: -size "<<m_Size[0]<<" "<<m_Size[1]<<" "<<m_Size[2]<<endl
            <<std::endl ;
}

int main(int argc, char **argv)
{


    if (argc < 2){
        help();
        return EXIT_FAILURE;
    }

    vector<string> inputFilenamevector;    
    int cin = 3;

    for(int i = 1; i < argc; i++){

        if(string(argv[i]) == "--h" || string(argv[i]) == "--help"){
            help();
            return 0;
        }else if (string(argv[i]) == "-fn"){
            inputFilenamevector.push_back(string(argv[i+1]));
        }else if (string(argv[i]) == "-size"){
            m_Size[0] = atoi(argv[i+1]);
            m_Size[1] = atoi(argv[i+2]);
            m_Size[2] = atoi(argv[i+3]);
        }else if(string(argv[i]) == "-ofn" || string(argv[i]) == "-outfn"){
            m_Outfilename = string(argv[i+1]);
        }else if(string(argv[i]) == "-regularspc"){
            regularspc = atoi(argv[i+1]);
        }else if(string(argv[i]) == "-cin"){
            cin = atoi(argv[i+1]);
        }
    }

    cout<<"components in: "<<cin<<endl;
    cout<<"outfn: "<<m_Outfilename<<endl;
    cout<<"size: "<<m_Size[0]<<" "<<m_Size[1]<<" "<<m_Size[2]<<endl;
    cout<<"regularspc: "<<regularspc<<endl;

    if(cin==3){



        typedef double Pixel;
        typedef itk::Vector<Pixel, 3> PixelType;
        //typedef unsigned char PixelType;
        typedef itk::Image<PixelType, 3> ImageType;
        typedef ImageType::Pointer ImagePointer;

        typedef itk::VectorNearestNeighborInterpolateImageFunction< ImageType > InterpolateImage;
        typedef InterpolateImage::Pointer InterpolateImagePointer;
        vector<InterpolateImagePointer> inputVectorInterpolateImages;


        ImageType::PointType pmin_origin;
        pmin_origin.Fill(999999);
        ImageType::PointType pmax_end;
        pmax_end.Fill(-999999);
        ImageType::SpacingType spacing;


        for(unsigned i = 0; i < inputFilenamevector.size(); i++){

            cout<<"Reading image: "<<inputFilenamevector[i]<<endl;

            typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
            ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
            reader->SetFileName( inputFilenamevector[i].c_str() );
            reader->Update();
            ImagePointer image = reader->GetOutput();

            InterpolateImage::Pointer interpolate = InterpolateImage::New();
            interpolate->SetInputImage(image);

            inputVectorInterpolateImages.push_back(interpolate);

            ImageType::PointType origin = image->GetOrigin();
            //cout<<origin<<endl;
            spacing = image->GetSpacing();
            ImageType::SizeType size  = image->GetLargestPossibleRegion().GetSize();

            for(unsigned j = 0; j < origin.Size(); j++){
                if(origin[j] < pmin_origin[j]){
                    pmin_origin[j] = origin[j];
                }
                double max = size[j]*spacing[j] + origin[j];
                if(max > pmax_end[j]){
                    pmax_end[j] = max;
                }
            }

        }



        double maxspc = 0;
        for(unsigned j = 0; j < pmin_origin.Size(); j++){
            spacing[j] = (pmax_end[j] - pmin_origin[j])/m_Size[j];
            maxspc = max(spacing[j], maxspc);
        }

        ImagePointer outimage = ImageType::New();

        if(regularspc){

            for(unsigned j = 0; j < pmin_origin.Size(); j++){
                m_Size[j] = m_Size[j]*spacing[j]/maxspc;
                spacing[j] = maxspc;
            }

        }

        ImageType::RegionType imageregion;
        ImageType::SizeType imagesize;
        imagesize[0] = m_Size[0];
        imagesize[1] = m_Size[1];
        imagesize[2] = m_Size[2];
        imageregion.SetSize(imagesize);
        outimage->SetRegions(imageregion);

        outimage->Allocate();
        ImageType::PixelType defpix;
        defpix.Fill(0);
        outimage->FillBuffer(defpix);


        cout<<"Computed Region: "<<endl;
        cout<<"\t Origin= "<<pmin_origin[0]<<" "<<pmin_origin[1]<<" "<<pmin_origin[2]<<endl;
        cout<<"\t Spacing= "<<spacing[0]<<" "<<spacing[1]<<" "<<spacing[2]<<endl;
        cout<<"\t Size= "<<m_Size[0]<<" "<<m_Size[1]<<" "<<m_Size[2]<<endl;

        outimage->SetOrigin(pmin_origin);
        outimage->SetSpacing(spacing);

        cout<<"Image append...";


        itk::ImageRegionIterator< ImageType > itout(outimage, outimage->GetLargestPossibleRegion());
        itout.GoToBegin();


        while(!itout.IsAtEnd()){

            ImageType::IndexType currentindex = itout.GetIndex();
            ImageType::PointType point;
            for(unsigned j = 0; j < point.Size(); j++){
                point[j] = pmin_origin[j] + currentindex[j]*spacing[j];
            }

            for(unsigned i = 0; i < inputVectorInterpolateImages.size(); i++){
                //if(itout.Get() == defpix){
                    InterpolateImage::ContinuousIndexType index;
                    inputVectorInterpolateImages[i]->ConvertPointToContinuousIndex(point, index);
                    if(inputVectorInterpolateImages[i]->IsInsideBuffer(index)){
                        ImageType::PixelType pixel = inputVectorInterpolateImages[i]->EvaluateAtContinuousIndex(index);
                        if(pixel != defpix){
                            itout.Set(pixel);
                        }
                    }
                //}
            }
            ++itout;

        }
        cout<<endl;
        cout<<"Writting to: "<<m_Outfilename<<endl;

        typedef  itk::ImageFileWriter< ImageType  > WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(m_Outfilename.c_str());
        writer->SetInput(outimage);
        writer->Update();
    }else if(cin == 1){

        typedef double Pixel;
        typedef itk::Vector<Pixel, 1> PixelType;
        //typedef unsigned char PixelType;
        typedef itk::Image<PixelType, 3> ImageType;
        typedef ImageType::Pointer ImagePointer;

        typedef itk::VectorNearestNeighborInterpolateImageFunction< ImageType > InterpolateImage;
        typedef InterpolateImage::Pointer InterpolateImagePointer;
        vector<InterpolateImagePointer> inputVectorInterpolateImages;


        ImageType::PointType pmin_origin;
        pmin_origin.Fill(999999);
        ImageType::PointType pmax_end;
        pmax_end.Fill(-999999);
        ImageType::SpacingType spacing;


        for(unsigned i = 0; i < inputFilenamevector.size(); i++){

            cout<<"Reading image: "<<inputFilenamevector[i]<<endl;

            typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
            ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
            reader->SetFileName( inputFilenamevector[i].c_str() );
            reader->Update();
            ImagePointer image = reader->GetOutput();

            InterpolateImage::Pointer interpolate = InterpolateImage::New();
            interpolate->SetInputImage(image);

            inputVectorInterpolateImages.push_back(interpolate);

            ImageType::PointType origin = image->GetOrigin();
            //cout<<origin<<endl;
            spacing = image->GetSpacing();
            ImageType::SizeType size  = image->GetLargestPossibleRegion().GetSize();

            for(unsigned j = 0; j < origin.Size(); j++){
                if(origin[j] < pmin_origin[j]){
                    pmin_origin[j] = origin[j];
                }
                double max = size[j]*spacing[j] + origin[j];
                if(max > pmax_end[j]){
                    pmax_end[j] = max;
                }
            }

        }



        double maxspc = 0;
        for(unsigned j = 0; j < pmin_origin.Size(); j++){
            spacing[j] = (pmax_end[j] - pmin_origin[j])/m_Size[j];
            maxspc = max(spacing[j], maxspc);
        }

        ImagePointer outimage = ImageType::New();

        if(regularspc){

            for(unsigned j = 0; j < pmin_origin.Size(); j++){
                m_Size[j] = ((double)m_Size[j])*spacing[j]/maxspc;
                spacing[j] = maxspc;
            }

        }

        ImageType::RegionType imageregion;
        ImageType::SizeType imagesize;
        imagesize[0] = m_Size[0];
        imagesize[1] = m_Size[1];
        imagesize[2] = m_Size[2];
        imageregion.SetSize(imagesize);
        outimage->SetRegions(imageregion);

        outimage->Allocate();
        ImageType::PixelType defpix;
        defpix.Fill(0);
        outimage->FillBuffer(defpix);


        cout<<"Computed Region: "<<endl;
        cout<<"\t Origin= "<<pmin_origin[0]<<" "<<pmin_origin[1]<<" "<<pmin_origin[2]<<endl;
        cout<<"\t Spacing= "<<spacing[0]<<" "<<spacing[1]<<" "<<spacing[2]<<endl;
        cout<<"\t Size= "<<m_Size[0]<<" "<<m_Size[1]<<" "<<m_Size[2]<<endl;

        outimage->SetOrigin(pmin_origin);
        outimage->SetSpacing(spacing);

        cout<<"Image append...";


        itk::ImageRegionIterator< ImageType > itout(outimage, outimage->GetLargestPossibleRegion());
        itout.GoToBegin();


        while(!itout.IsAtEnd()){

            ImageType::IndexType currentindex = itout.GetIndex();
            ImageType::PointType point;
            for(unsigned j = 0; j < point.Size(); j++){
                point[j] = pmin_origin[j] + currentindex[j]*spacing[j];
            }

            for(unsigned i = 0; i < inputVectorInterpolateImages.size(); i++){
                //if(itout.Get() == defpix){                    

                    if(inputVectorInterpolateImages[i]->IsInsideBuffer(point)){
                        ImageType::PixelType pixel = inputVectorInterpolateImages[i]->Evaluate(point);
                        if(pixel != defpix){
                            itout.Set(pixel);
                        }
                    }
                //}
            }
            ++itout;

        }
        cout<<endl;
        cout<<"Writting to: "<<m_Outfilename<<endl;

        typedef  itk::ImageFileWriter< ImageType  > WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(m_Outfilename.c_str());
        writer->SetInput(outimage);
        writer->Update();


    }




    return 0;
}
