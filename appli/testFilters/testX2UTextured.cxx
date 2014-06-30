//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <itkVector.h>

#include "itkImageRegionIteratorWithIndex.h"

#include <itkVectorNearestNeighborInterpolateImageFunction.h>
#include <itkVectorExpandImageFilter.h>
#include <itkvectordiscretegaussianimagefilter.h>
#include <itkMedianImageFilter.h>
#include "itkCastImageFilter.h"

using namespace std;

void help(){
    cout<<"Usage: textX2UTextured -x2u <filename x2u map> -texture <texture volume filename> [options]"<<endl;
    cout<<"options:"<<endl;
    cout<<"--h --help show help menu"<<endl;
    cout<<"-thresh  <double double double double double double> Apply the texture on the range following u, v, tau or red, green blue object coordinates."<<endl;
    cout<<"\t       Different textures can be applied but their range and texture filename must be specified also with the flags -thresh and -texture."<<endl;
    cout<<"-texture <texture volume filnename>"<<endl;
    //cout<<"-mirror <bool> mirror the texture following the the 'v' coordinate from the x2u map. Useful to correctly warp a block of texture."<<endl;
}

int main(int argc, char **argv)
{


    if (argc < 3)
      {
        help();
        return EXIT_FAILURE;
      }

    std::string inputFilename = "";

    vector<string> vectorTextures;
    vector<double*> vectorThresh;    
    int mirror = 1;


    for(int i = 1; i < argc; i++){

        if(string(argv[i]) == "--h" || string(argv[i]) == "--help"){
            help();
            return 0;
        }else if(string(argv[i]) == "-texture"){
            vectorTextures.push_back(string(argv[i+1]));
        }else if(string(argv[i]) == "-x2u"){
            inputFilename = argv[i+1];
        }else if(string(argv[i]) == "-thresh"){
            double *thresh;
            thresh = new double[6];
            thresh[0] = atof(argv[i+1]);
            thresh[1] = atof(argv[i+2]);
            thresh[2] = atof(argv[i+3]);
            thresh[3] = atof(argv[i+4]);
            thresh[4] = atof(argv[i+5]);
            thresh[5] = atof(argv[i+6]);
            vectorThresh.push_back(thresh);
        }else if(string(argv[i]) == "-mirror"){
            mirror = atoi(argv[i+1]);
        }
    }

    cout<<endl<<"Texturing object: "<<endl;
    cout<<"X2U map: "<<inputFilename<<endl;
    cout<<"mirror: "<<mirror<<endl;

    if(vectorTextures.size() == 0){
        cout<<"Error, must specify at least one textures."<<endl;
        return 0;
    }

    for(unsigned i = 0; i < vectorTextures.size(); i++){
        cout<<"Texture: "<<vectorTextures[i]<<endl;
    }

    if(vectorThresh.size() != 0){

        if(vectorThresh.size() != vectorTextures.size()){
            cout<<"Warning! the threshold for all textures is not specified. Applying texture for the first ranges specified."<<endl;
        }

        cout<<"Applying the textures in the range: "<<endl;
    }

    for(unsigned i = 0; i < vectorThresh.size(); i++){
        cout<<"Thresh: u=["<<vectorThresh[i][0]<<", "<<vectorThresh[i][1]<<"] v=["<<vectorThresh[i][2]<<", "<<vectorThresh[i][3]<<"] tau=["<<vectorThresh[i][4]<<", "<<vectorThresh[i][5]<<"]"<<endl;
    }


    if(inputFilename != ""){


        typedef double X2UDoubleType;
        typedef itk::Vector< X2UDoubleType, 3 > X2UPixelType;
        typedef itk::Image< X2UPixelType, 3 > X2UImageType ;
        typedef itk::ImageFileReader< X2UImageType > X2UImageReaderType ;

        X2UImageReaderType::Pointer imageReaderx2u = X2UImageReaderType::New() ;

        imageReaderx2u->SetFileName(inputFilename.c_str());
        imageReaderx2u->Update() ;
        X2UImageType::Pointer imagex2u = imageReaderx2u->GetOutput();


        //typedef unsigned char PixelType;
        typedef double PixelType;
        static const int NumChannelsOutTexture = 3;
        static const int NumChannelsInTexture = 3;


        typedef itk::Vector< PixelType, NumChannelsOutTexture > VectorPixelType;
        typedef itk::Image< VectorPixelType, 3 > VectorImageType ;

        VectorImageType::Pointer outimage = VectorImageType::New();
        outimage->SetRegions(imagex2u->GetLargestPossibleRegion());
        outimage->SetSpacing(imagex2u->GetSpacing());
        outimage->SetOrigin(imagex2u->GetOrigin());
        outimage->Allocate();

        VectorImageType::PixelType pixel;
        pixel.Fill(0);

        outimage->FillBuffer(pixel);



        for(unsigned i = 0; i < vectorTextures.size(); i++){

            double range[6] = {0,255,0,255,0,255};

            if(vectorThresh.size() != 0){
                if(i < vectorThresh.size()){
                    for(unsigned j = 0; j < 6; j++){
                        range[j] = vectorThresh[i][j];
                    }
                }else{
                    break;
                }
            }


            typedef itk::Vector< PixelType, NumChannelsInTexture> RGBPixelType;
            typedef itk::Image< RGBPixelType, 3 > RGBImageType ;
            typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;

            ImageReaderType::Pointer readertexture = ImageReaderType::New() ;

            readertexture->SetFileName(vectorTextures[i].c_str());
            readertexture->Update() ;
            RGBImageType::Pointer imagetexture = readertexture->GetOutput();
            RGBImageType::PointType origin;
            origin.Fill(0);
            imagetexture->SetOrigin(origin);

            RGBImageType::SizeType size = imagetexture->GetLargestPossibleRegion().GetSize();
            RGBImageType::SpacingType spc;

            for(unsigned j = 0; j < origin.Size(); j++){
                if(size[j] > 1){
                    spc[j] = range[j*2+1]/(size[j]-1);
                }else{
                    spc[j] = 256;
                }
            }

            cout<<"Changing texture's spacing to: "<<spc[0]<<", "<<spc[1]<<" "<<spc[2]<<endl;
            imagetexture->SetSpacing(spc);



            typedef itk::ImageRegionIteratorWithIndex< X2UImageType >    X2UIteratorType;
            typedef itk::ImageRegionIteratorWithIndex< RGBImageType >    RGBIteratorType;
            typedef itk::ImageRegionIteratorWithIndex< VectorImageType > VectorIteratorType;


            X2UIteratorType x2uIt(imagex2u, imagex2u->GetLargestPossibleRegion() );
            RGBIteratorType  imagetextureIt(  imagetexture, imagetexture->GetLargestPossibleRegion() );
            VectorIteratorType  outIt(  outimage, outimage->GetLargestPossibleRegion() );

            x2uIt.GoToBegin();
            imagetextureIt.GoToBegin();

            X2UImageType::PixelType pixelx2u;
            pixelx2u.Fill(0);

            typedef itk::VectorNearestNeighborInterpolateImageFunction< RGBImageType > InterpolateImage;
            InterpolateImage::Pointer interpolate = InterpolateImage::New();
            interpolate->SetInputImage(imagetexture);

            cout<<"Texturing...";

            double midrange[3] = {(range[0] + range[1])/2.0, (range[2] + range[3])/2.0, (range[4] + range[5])/2.0};

            while( !x2uIt.IsAtEnd() )
            {

                //if(x2uIt.Get() != pixelx2u){

                    VectorPixelType outpix = outIt.Get();


                    if(x2uIt.Get() != pixelx2u
                            && (range[0] <= x2uIt.Get()[0] && x2uIt.Get()[0] <= range[1])
                            && (range[2] <= x2uIt.Get()[1] && x2uIt.Get()[1] <= range[3])
                            && (range[4] <= x2uIt.Get()[2] && x2uIt.Get()[2] <= range[5])){
                        //cout<<x2uIt.Get()<<endl;

                        InterpolateImage::PointType continuouspoint;

                        continuouspoint[0] = x2uIt.Get()[0];

                        if(mirror){
                            continuouspoint[1] = x2uIt.Get()[1];
                            if(continuouspoint[1] <= midrange[1]){
                                continuouspoint[1] *= 2.0;
                                continuouspoint[2] = midrange[2] + (x2uIt.Get()[2])/2.0;
                            }else{
                                continuouspoint[1] = 2.0*(range[3] - continuouspoint[1]);
                                continuouspoint[2] = midrange[2] - (x2uIt.Get()[2])/2.0;
                            }

                        }else{
                            continuouspoint[1] = x2uIt.Get()[1];
                            continuouspoint[2] = x2uIt.Get()[2];
                        }


                        InterpolateImage::IndexType index;

                        interpolate->ConvertPointToNearestIndex (continuouspoint, index);

                        if(interpolate->IsInsideBuffer(index)){
                            RGBPixelType pix = interpolate->EvaluateAtIndex(index);
                            for(unsigned i = 0; i < pix.Size(); i++){
                                outpix[i] = pix[i];
                            }

                        }else{
                            cout<<"pixel outside buffer "<<continuouspoint;
                        }


                        if( outpix.Size() == 4){
                            outpix[3] = 255;
                        }
                        //outpix[0] = pix[0];
                        //outpix[1] = pix[1];
                        //outpix[2] = pix[2];
                        //outpix[3] = pix[3];
                        //outpix[3] = 255;
                    }

                    //outIt.SetIndex(x2uIt.GetIndex());

                    outIt.Set(outpix);

                //}

                ++x2uIt;
                ++outIt;

            }
            cout<<endl;
        }

        /*typedef itk::VectorExpandImageFilter< RGBImageType, RGBImageType > VectorExpandImageType;
        VectorExpandImageType::Pointer vectorexpandimage = VectorExpandImageType::New();
        vectorexpandimage->SetExpandFactors(4);
        vectorexpandimage->SetInput(outimage);
        vectorexpandimage->Update();
        outimage = vectorexpandimage->GetOutput();


        typedef typename itk::VectorDiscreteGaussianImageFilter< RGBImageType, RGBImageType > VectorDiscreteGaussianImageFilterType;
        typename VectorDiscreteGaussianImageFilterType::Pointer discretegauss = VectorDiscreteGaussianImageFilterType::New();

        discretegauss->SetInput(outimage);
        discretegauss->SetVariance(2);
        discretegauss->Update();
        outimage = discretegauss->GetOutput();*/


        string filenametexture = inputFilename.substr(0, inputFilename.find_last_of("."));
        filenametexture.append("Texture.mhd");

        cout<<"Writing textured object: "<<filenametexture<<endl;


        typedef itk::ImageFileWriter< VectorImageType > ImageWriterType;
        ImageWriterType::Pointer map_writer = ImageWriterType::New();
        map_writer->SetFileName(filenametexture.c_str());
        map_writer->SetInput( outimage  );
        map_writer->Update();

        for(unsigned i = 0; i < vectorThresh.size(); i++){
            delete[] vectorThresh[i];
        }
        cout<<" Done!"<<endl<<endl;
    }else{

        cout<<"X2U map not specified. Adding luminance =255 to the texture. "<<endl;

        typedef unsigned char PixelType;
        static const int NumChannelsOutTexture = 4;
        static const int NumChannelsInTexture = 3;


        typedef itk::Vector< PixelType, NumChannelsOutTexture > VectorPixelType;
        typedef itk::Image< VectorPixelType, 3 > VectorImageType;


        for(unsigned i = 0; i < vectorTextures.size(); i++){



            typedef itk::Vector< PixelType, NumChannelsInTexture> RGBPixelType;
            typedef itk::Image< RGBPixelType, 3 > RGBImageType ;
            typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;

            ImageReaderType::Pointer readertexture = ImageReaderType::New() ;

            readertexture->SetFileName(vectorTextures[i].c_str());
            readertexture->Update() ;
            RGBImageType::Pointer imagetexture = readertexture->GetOutput();

            VectorImageType::Pointer outimage = VectorImageType::New();
            outimage->SetRegions(imagetexture->GetLargestPossibleRegion());
            outimage->SetSpacing(imagetexture->GetSpacing());
            outimage->SetOrigin(imagetexture->GetOrigin());
            outimage->Allocate();

            typedef itk::ImageRegionIteratorWithIndex< RGBImageType >    RGBIteratorType;
            typedef itk::ImageRegionIteratorWithIndex< VectorImageType > VectorIteratorType;

            RGBIteratorType  imagetextureIt(  imagetexture, imagetexture->GetLargestPossibleRegion() );
            VectorIteratorType  outIt(  outimage, outimage->GetLargestPossibleRegion() );

            imagetextureIt.GoToBegin();
            outIt.GoToBegin();

            while(!imagetextureIt.IsAtEnd()){

                VectorImageType::PixelType pixel;
                pixel.Fill(0);

                for(unsigned i = 0; i < imagetextureIt.Get().Size(); i++){
                    pixel[i] = imagetextureIt.Get()[i];
                }
                pixel[3] = 255;

                outIt.Set(pixel);

                ++outIt;
                ++imagetextureIt;
            }

            string filenametexture = vectorTextures[i].substr(0, vectorTextures[i].find_last_of("."));
            filenametexture.append("Lum.mhd");

            cout<<"Writing textured object: "<<filenametexture<<endl;

            typedef itk::ImageFileWriter< VectorImageType > ImageWriterType;
            ImageWriterType::Pointer map_writer = ImageWriterType::New();
            map_writer->SetFileName(filenametexture.c_str());
            map_writer->SetInput( outimage  );
            map_writer->Update();

            cout<<" Done!"<<endl<<endl;
        }



    }

    return 0;
}
