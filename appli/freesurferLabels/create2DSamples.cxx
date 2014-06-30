//----------------------------------------------------------------------
//		File:                   create2DSamples.cxx
//      Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------


#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <itkVector.h>

#include <itkThresholdImageFilter.h>
#include <itkMaskImageFilter.h>

#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageRandomNonRepeatingIteratorWithIndex.h>
#include <itkImageRandomIteratorWithIndex.h>
#include <itkBSplineInterpolateImageFunction.h>

using namespace std;					// make std:: accessible

#define TEMPLATE_NUMCOMP(N)\
typedef itk::Vector< InputMapPixelType, 3 > OutputPixelType;\
typedef itk::Image< OutputPixelType, 2 > OutputImageType ;\
typedef OutputImageType::IndexType  OutputImageIndexType;\
OutputImageType::Pointer outimage = OutputImageType::New();\
OutputImageType::RegionType outimageregion;\
outimageregion.SetSize(0, 128);\
outimageregion.SetSize(1, 128);\
outimage->SetRegions(outimageregion);\
outimage->Allocate();\
OutputPixelType initpixel;\
initpixel.Fill(0);\
outimage->FillBuffer(initpixel);\
typedef itk::ImageRandomNonRepeatingIteratorWithIndex< OutputImageType > OutputImageRegionIteratorType;\
OutputImageRegionIteratorType ot(outimage, outimage->GetLargestPossibleRegion());\
ot.SetNumberOfSamples(pow(128,2));\
ot.ReinitializeSeed();\
ot.GoToBegin();\
while(!ot.IsAtEnd()){\
    itr.GoToBegin();\
    while(!itr.IsAtEnd() && !ot.IsAtEnd()){\
        if(itr.Get() != 0 || !mask){\
            InputImageIndexType index = itr.GetIndex();\
            OutputPixelType pout;\
            for(unsigned i=0; i < vectmapimages.size();i++){\
                pout[i]=vectmapimages[i]->GetPixel(index);\
            }\
            ot.Set(pout);\
            ++ot;\
        }\
        ++itr;\
    }\
}\
string outimagefilename = imageMaskFilename;\
outimagefilename = outimagefilename.substr(0, outimagefilename.size()-4);\
outimagefilename.append("Sample.mhd");\
cout<<"Writing sample to: "<<outimagefilename<<endl;\
typedef itk::ImageFileWriter< OutputImageType > ImageWriterType;\
ImageWriterType::Pointer imagewriter = ImageWriterType::New();\
imagewriter->SetFileName(outimagefilename.c_str());\
imagewriter->SetInput(outimage);\
imagewriter->Update();\




void help(char* execname){
    cout<<"Usage: "<<execname<<"-i <image filename> -im <image mask>"<<endl;
    cout<<"options:"<<endl;
    cout<<"--h --help show help menu"<<endl;
    cout<<"-i <image filename> image to produce the 2D image sample. If more images are added, this will be put following the third dimension (result image with multiple components as RGB)."<<endl;
    cout<<"-im <image filename> to perform the AND operation with the image files"<<endl;
    cout<<"-random <bool> specify this option in case the mask does not contain any pixel in order to generate a random sample."<<endl;
    cout<<"-x2u <filename> specify an x2u map to produce the texture sample"<<endl;
    cout<<"-u2x <filename> specify an u2x file to produce the texture sample"<<endl;
    cout<<"-append <bool> append the componets to the image and write the output"<<endl;
}

int main(int argc, char **argv)
{


    if (argc < 7){
        help(argv[0]);
        return 0;
    }

    vector< string > vectimagefilename;
    string imageMaskFilename = "";
    bool random = 0;
    string imagex2uFilename = "";
    string imageu2xFilename = "";
    bool append = 0;


    for(int i = 1; i < argc; i++){

        if(string(argv[i]) == "--h" || string(argv[i]) == "--help"){
            help(argv[0]);
            return 0;
        }else if (string(argv[i]) == "-i"){
            if(i + 1 >= argc){
                std::cout << "ERROR: image filename missing."
                          <<std::endl
                          <<" -i <filename>"
                          << std::endl ;
                return EXIT_FAILURE;
            }

            string infilename = argv[i + 1];
            vectimagefilename.push_back(infilename);

        }else if (string(argv[i]) == "-im"){
            if(i + 1 >= argc){
                std::cout << "ERROR: image filename missing."
                          <<std::endl
                          <<"-im <filename>"
                          << std::endl ;
                return EXIT_FAILURE;
            }

            imageMaskFilename = argv[i + 1];

        }else if (string(argv[i]) == "-random"){

            random = atoi(argv[i + 1]);

        }else if (string(argv[i]) == "-x2u"){

            imagex2uFilename = string(argv[i + 1]);

        }else if (string(argv[i]) == "-u2x"){

            imageu2xFilename = string(argv[i + 1]);

        }else if (string(argv[i]) == "-append"){

            append = atoi(argv[i + 1]);

        }
    }


    typedef double InputMapPixelType;
    typedef itk::Image< InputMapPixelType, 3 > InputMapImageType ;

    typedef itk::ImageFileReader< InputMapImageType > ImageMapReaderType ;

    vector< InputMapImageType::Pointer > vectmapimages;

    for(unsigned i=0; i < vectimagefilename.size();i++){

        cout<<"Reading Sample: "<<vectimagefilename[i]<<endl;

        ImageMapReaderType::Pointer imageMapReader = ImageMapReaderType::New();
        imageMapReader->SetFileName(vectimagefilename[i].c_str());
        imageMapReader->Update() ;
        InputMapImageType::Pointer image = imageMapReader->GetOutput();

        vectmapimages.push_back(image);
    }


    if(imagex2uFilename == "" && imageu2xFilename == "" && !append){

        typedef unsigned char PixelType;
        typedef itk::Image< PixelType, 3 > InputImageType ;
        typedef InputImageType::IndexType  InputImageIndexType;
        typedef itk::ImageFileReader< InputImageType > ImageReaderType ;
        ImageReaderType::Pointer imageReader = ImageReaderType::New();

        cout<<"Creating sample from: "<<imageMaskFilename<<endl;

        imageReader->SetFileName(imageMaskFilename.c_str());
        imageReader->Update() ;
        InputImageType::Pointer image = imageReader->GetOutput();


        typedef itk::ImageRegionIteratorWithIndex< InputImageType > InputImageRegionIteratorType;

        InputImageRegionIteratorType it(image, image->GetLargestPossibleRegion());
        it.GoToBegin();


        int boundingbox[6] = {999999, -1, 999999, -1, 999999, -1};

        bool mask = false;

        while( !it.IsAtEnd() ){
            if(it.Get()!=0){

                mask = true;
                InputImageIndexType index = it.GetIndex();

                if(index[0] < boundingbox[0]){
                    boundingbox[0] = index[0];
                }
                if(index[0] > boundingbox[1]){
                    boundingbox[1] = index[0];
                }

                if(index[1] < boundingbox[2]){
                    boundingbox[2] = index[1];
                }
                if(index[1] > boundingbox[3]){
                    boundingbox[3] = index[1];
                }

                if(index[2] < boundingbox[4]){
                    boundingbox[4] = index[2];
                }
                if(index[2] > boundingbox[5]){
                    boundingbox[5] = index[2];
                }
            }
            ++it;
        }

        if(!mask){
            cout<<"ERROR: the mask is empty. fn= "<<imageMaskFilename<<endl;

            if(random){
                cout<<"Random sample specified by the user."<<endl;
            }else{

                cout<<"No Sample Generated."<<endl;
                return 1;
            }
        }

        int xsize = boundingbox[1] - boundingbox[0];
        int ysize = boundingbox[3] - boundingbox[2];
        int zsize = boundingbox[5] - boundingbox[4];

        cout<<"Calculated region: "<<endl;
        cout<<"\t origin: "<<boundingbox[0]<<", "<<boundingbox[1]<<", "<<boundingbox[2]<<endl;
        cout<<"\t size: "<<xsize<<", "<<ysize<<", "<<zsize<<endl;


        InputImageType::RegionType imageregion;
        InputImageType::SizeType imagesize;
        imagesize[0] = xsize;
        imagesize[1] = ysize;
        imagesize[2] = zsize;
        imageregion.SetSize(imagesize);
        InputImageType::IndexType index0;
        index0[0] = boundingbox[0];
        index0[1] = boundingbox[2];
        index0[2] = boundingbox[4];
        imageregion.SetIndex(index0);


        typedef itk::ImageRandomNonRepeatingIteratorWithIndex< InputImageType > ImageRandomIteratorType;
        ImageRandomIteratorType itr;

        if(!mask){
            itr = ImageRandomIteratorType(image, image->GetLargestPossibleRegion());
            InputImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
            itr.SetNumberOfSamples(size[0]*size[1]*size[2]);
        }else{
            itr = ImageRandomIteratorType(image, imageregion);
            itr.SetNumberOfSamples(xsize*ysize*zsize);
        }


        itr.ReinitializeSeed();


        cout<<"Creating sample..."<<endl;

        /*if(vectimagefilename.size() == 5){
            TEMPLATE_NUMCOMP(5);
        }else if(vectimagefilename.size() == 4){
            TEMPLATE_NUMCOMP(4);
        }else if(vectimagefilename.size() == 3){
            TEMPLATE_NUMCOMP(3);
        }if(vectimagefilename.size() == 2){
            TEMPLATE_NUMCOMP(2);
        }*/

        typedef itk::Vector< InputMapPixelType, 3 > OutputPixelType;
        typedef itk::Image< OutputPixelType, 2 > OutputImageType ;
        typedef OutputImageType::IndexType  OutputImageIndexType;
        OutputImageType::Pointer outimage = OutputImageType::New();
        OutputImageType::RegionType outimageregion;
        outimageregion.SetSize(0, 128);
        outimageregion.SetSize(1, 128);
        outimage->SetRegions(outimageregion);
        outimage->Allocate();
        OutputPixelType initpixel;
        initpixel.Fill(0);
        outimage->FillBuffer(initpixel);
        typedef itk::ImageRandomNonRepeatingIteratorWithIndex< OutputImageType > OutputImageRegionIteratorType;
        OutputImageRegionIteratorType ot(outimage, outimage->GetLargestPossibleRegion());
        ot.SetNumberOfSamples(pow(128,2));
        ot.ReinitializeSeed();
        ot.GoToBegin();
        while(!ot.IsAtEnd()){
            itr.GoToBegin();
            while(!itr.IsAtEnd() && !ot.IsAtEnd()){
                if(itr.Get() != 0 || !mask){
                    InputImageIndexType index = itr.GetIndex();
                    OutputPixelType pout;
                    for(unsigned i=0; i < vectmapimages.size();i++){
                        pout[i]=vectmapimages[i]->GetPixel(index);
                    }
                    ot.Set(pout);
                    ++ot;
                }
                ++itr;
            }
        }
        string outimagefilename = imageMaskFilename;
        outimagefilename = outimagefilename.substr(0, outimagefilename.size()-4);
        outimagefilename.append("Sample.mhd");
        cout<<"Writing sample to: "<<outimagefilename<<endl;
        typedef itk::ImageFileWriter< OutputImageType > ImageWriterType;
        ImageWriterType::Pointer imagewriter = ImageWriterType::New();
        imagewriter->SetFileName(outimagefilename.c_str());
        imagewriter->SetInput(outimage);
        imagewriter->Update();
    }else{

        if(imageu2xFilename != ""){

            typedef double U2XDoubleType;
            typedef itk::Vector< U2XDoubleType, 3 > U2XPixelType;
            typedef itk::Image< U2XPixelType, 3 > U2XImageType ;
            typedef itk::ImageFileReader< U2XImageType > U2XImageReaderType ;

            U2XImageReaderType::Pointer imageReaderu2x = U2XImageReaderType::New() ;

            cout<<"Reading u2x map: "<<imageu2xFilename<<endl;

            imageReaderu2x->SetFileName(imageu2xFilename.c_str());
            imageReaderu2x->Update() ;
            U2XImageType::Pointer imageu2x = imageReaderu2x->GetOutput();


            typedef itk::ImageRegionIteratorWithIndex< U2XImageType > InputImageRegionIteratorType;

            InputImageRegionIteratorType itu2x(imageu2x, imageu2x->GetLargestPossibleRegion());
            itu2x.GoToBegin();


            typedef itk::Vector< InputMapPixelType, 3 > OutputPixelType;
            typedef itk::Image< OutputPixelType, 3 > OutputImageType ;
            typedef OutputImageType::IndexType  OutputImageIndexType;
            OutputImageType::Pointer outimage = OutputImageType::New();
            outimage->SetRegions(imageu2x->GetLargestPossibleRegion());
            outimage->Allocate();
            OutputPixelType initpixel;
            initpixel.Fill(0);
            outimage->FillBuffer(initpixel);

            typedef itk::ImageRegionIteratorWithIndex< OutputImageType > OutputImageRegionIteratorType;
            OutputImageRegionIteratorType itout(outimage, outimage->GetLargestPossibleRegion());
            itout.GoToBegin();

            typedef itk::BSplineInterpolateImageFunction< InputMapImageType > InterpolatorType;
            vector<InterpolatorType::Pointer> vectmapinterpolator;

            cout<<"Creating interpolators... "<<endl;
            for(unsigned i=0; i < vectmapimages.size();i++){
                vectmapinterpolator.push_back(InterpolatorType::New());
                vectmapinterpolator[i]->SetInputImage(vectmapimages[i]);

            }

            cout<<"U2X... "<<endl;

            while(!itu2x.IsAtEnd()){

                U2XPixelType currentpix = itu2x.Get();

                U2XImageType::PointType point;
                for(unsigned j = 0; j < currentpix.Size(); j++){
                    point[j] = currentpix[j];
                }

                OutputPixelType pout;
                pout.Fill(0);

                for(unsigned i=0; i < vectmapinterpolator.size();i++){

                    InterpolatorType::ContinuousIndexType continuousindex;
                    vectmapinterpolator[i]->ConvertPointToContinuousIndex(point, continuousindex);
                    if(vectmapinterpolator[i]->IsInsideBuffer(continuousindex)){
                        pout[i] = vectmapinterpolator[i]->EvaluateAtContinuousIndex(continuousindex);
                    }
                }


                itout.Set(pout);
                ++itu2x;
                ++itout;
            }
            string outimagefilename = imageu2xFilename;
            outimagefilename = outimagefilename.substr(0, outimagefilename.size()-4);
            outimagefilename.append("Texture.mhd");
            cout<<"Writing sample to: "<<outimagefilename<<endl;
            typedef itk::ImageFileWriter< OutputImageType > ImageWriterType;
            ImageWriterType::Pointer imagewriter = ImageWriterType::New();
            imagewriter->SetFileName(outimagefilename.c_str());
            imagewriter->SetInput(outimage);
            imagewriter->Update();

        }else if(imagex2uFilename != ""){

            typedef double X2UDoubleType;
            typedef itk::Vector< X2UDoubleType, 3 > X2UPixelType;
            typedef itk::Image< X2UPixelType, 3 > X2UImageType ;
            typedef itk::ImageFileReader< X2UImageType > X2UImageReaderType ;

            X2UImageReaderType::Pointer imageReaderx2u = X2UImageReaderType::New() ;

            cout<<"Reading x2u map: "<<imagex2uFilename<<endl;

            imageReaderx2u->SetFileName(imagex2uFilename.c_str());
            imageReaderx2u->Update() ;
            X2UImageType::Pointer imagex2u = imageReaderx2u->GetOutput();


            typedef itk::ImageRegionIteratorWithIndex< X2UImageType > InputImageRegionIteratorType;

            InputImageRegionIteratorType itx2u(imagex2u, imagex2u->GetLargestPossibleRegion());
            itx2u.GoToBegin();


            typedef itk::Vector< InputMapPixelType, 3 > OutputPixelType;
            typedef itk::Image< OutputPixelType, 3 > OutputImageType ;
            typedef OutputImageType::IndexType  OutputImageIndexType;
            OutputImageType::Pointer outimage = OutputImageType::New();
            OutputImageType::RegionType outimageregion;
            outimageregion.SetSize(0, 256);
            outimageregion.SetSize(1, 256);
            outimageregion.SetSize(2, 256);
            outimage->SetRegions(outimageregion);
            outimage->Allocate();
            OutputPixelType initpixel;
            initpixel.Fill(0);
            outimage->FillBuffer(initpixel);

            X2UPixelType def;
            def.Fill(0);

            X2UImageType::SpacingType x2uspacing = imagex2u->GetSpacing();
            X2UImageType::PointType x2uorigin = imagex2u->GetOrigin();

            typedef itk::BSplineInterpolateImageFunction< InputMapImageType > InterpolatorType;
            vector<InterpolatorType::Pointer> vectmapinterpolator;

            cout<<"Creating interpolators... "<<endl;
            for(unsigned i=0; i < vectmapimages.size();i++){
                vectmapinterpolator.push_back(InterpolatorType::New());
                vectmapinterpolator[i]->SetInputImage(vectmapimages[i]);

            }

            cout<<"U2X... "<<endl;

            while(!itx2u.IsAtEnd()){

                X2UPixelType currentpix = itx2u.Get();

                OutputImageType::IndexType outindex;
                for(unsigned i = 0; i < 3; i++){
                    outindex[i] = currentpix[i];
                }                

                if(currentpix != def && currentpix[2]<=255){

                    X2UImageType::IndexType currentindex = itx2u.GetIndex();
                    X2UImageType::PointType point;
                    for(unsigned j = 0; j < x2uorigin.Size(); j++){
                        point[j] = x2uorigin[j] + currentindex[j]*x2uspacing[j];
                    }

                    OutputPixelType pout;// = outimage->GetPixel(outindex);
                    pout.Fill(0);

                    for(unsigned i=0; i < vectmapinterpolator.size();i++){

                        InterpolatorType::ContinuousIndexType continuousindex;
                        vectmapinterpolator[i]->ConvertPointToContinuousIndex(point, continuousindex);
                        if(vectmapinterpolator[i]->IsInsideBuffer(continuousindex)){
                            pout[i] = vectmapinterpolator[i]->EvaluateAtContinuousIndex(continuousindex);
                        }
                    }



                    OutputImageType::IndexType outindex;
                    outindex[0] = currentpix[0];
                    outindex[1] = currentpix[1];
                    if(outindex[1] < 128){
                        outindex[1] *= 2.0;
                        outindex[2] = 127.5 + (currentpix[2])/2.0;
                    }else{
                        outindex[1] = 2*(255.0 - outindex[1]);
                        outindex[2] = 127.5 - (currentpix[2])/2.0;
                    }


                    outimage->SetPixel(outindex, pout);

                }

                ++itx2u;
            }
            string outimagefilename = imagex2uFilename;
            outimagefilename = outimagefilename.substr(0, outimagefilename.size()-4);
            outimagefilename.append(".u2x.mhd");
            cout<<"Writing sample to: "<<outimagefilename<<endl;
            typedef itk::ImageFileWriter< OutputImageType > ImageWriterType;
            ImageWriterType::Pointer imagewriter = ImageWriterType::New();
            imagewriter->SetFileName(outimagefilename.c_str());
            imagewriter->SetInput(outimage);
            imagewriter->Update();


        }else if(append && vectmapimages.size() > 0){


            typedef unsigned char PixelType;
            typedef itk::Image< PixelType, 3 > InputImageType ;
            typedef InputImageType::IndexType  InputImageIndexType;
            typedef itk::ImageFileReader< InputImageType > ImageReaderType ;

            InputImageType::Pointer image = 0;

            if(imageMaskFilename != ""){
                ImageReaderType::Pointer imageReader = ImageReaderType::New();

                cout<<"Creating sample from: "<<imageMaskFilename<<endl;

                imageReader->SetFileName(imageMaskFilename.c_str());
                imageReader->Update() ;
                image = imageReader->GetOutput();
            }


            typedef itk::ImageRegionIteratorWithIndex< InputImageType > InputImageRegionIteratorType;

            InputImageRegionIteratorType it;

            if(image){
                it = InputImageRegionIteratorType(image, image->GetLargestPossibleRegion());
                it.GoToBegin();
            }

            typedef itk::Vector< InputMapPixelType, 3 > OutputPixelType;
            typedef itk::Image< OutputPixelType, 3 > OutputImageType ;
            typedef OutputImageType::IndexType  OutputImageIndexType;
            OutputImageType::Pointer outimage = OutputImageType::New();
            outimage->SetRegions(vectmapimages[0]->GetLargestPossibleRegion());
            outimage->SetSpacing(vectmapimages[0]->GetSpacing());
            outimage->SetOrigin(vectmapimages[0]->GetOrigin());
            outimage->Allocate();

            typedef itk::ImageRegionIteratorWithIndex< OutputImageType > OutputImageRegionIteratorType;

            OutputImageRegionIteratorType itout(outimage, outimage->GetLargestPossibleRegion());
            itout.GoToBegin();

            cout<<"append images... "<<endl;

            while(!itout.IsAtEnd()){

                bool mask = true;

                if(image){
                    mask = it.Get() != 0;
                }

                OutputPixelType outpix;
                outpix.Fill(0);

                if(mask){
                    for(unsigned i=0; i < vectmapimages.size();i++){

                        outpix[i] = vectmapimages[i]->GetPixel(itout.GetIndex());

                    }
                }

                itout.Set(outpix);
                ++itout;
                if(image){
                    ++it;
                }
            }
            string outimagefilename = imageMaskFilename;

            if(outimagefilename.find_last_of(".")!=string::npos){
                outimagefilename = outimagefilename.substr(0, outimagefilename.find_last_of("."));
                outimagefilename.append(".");
            }
            outimagefilename.append("AppendParams.mhd");
            cout<<"Writing sample to: "<<outimagefilename<<endl;
            typedef itk::ImageFileWriter< OutputImageType > ImageWriterType;
            ImageWriterType::Pointer imagewriter = ImageWriterType::New();
            imagewriter->SetFileName(outimagefilename.c_str());
            imagewriter->SetInput(outimage);
            imagewriter->Update();
        }

    }


    return 0;
}

