//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "itkrgbimagetovector.h"
#include "itkvectortorgbimage.h"
#include "itkoptimizationtexture.h"
#include "itkrandomimagegenerator.h"



#include <qtvtkviewerwidget.h>
#include <QMainWindow>
#include <QApplication>
#include "itktovtkimageimport.h"

#include "vtkImageData.h"
#include "itkRGBToLuminanceImageFilter.h"
#include "itkvtkcolortransferfunction.h"

#include "vtkMetaImageWriter.h"
#include "vtkMetaImageReader.h"

#include "itkSignedMaurerDistanceMapImageFilter.h"


using namespace std;					// make std:: accessible

//----------------------------------------------------------------------
// ann_sample
//
// This is a simple sample program for the ANN library.	 After compiling,
// it can be run as follows.
//
// ann_sample [-d dim] [-max mpts] [-nn k] [-e eps] [-df data] [-qf query]
//
// where
//		dim				is the dimension of the space (default = 2)
//		mpts			maximum number of data points (default = 1000)
//		k				number of nearest neighbors per query (default 1)
//		eps				is the error bound (default = 0.0)
//		data			file containing data points
//		query			file containing query points
//
// Results are sent to the standard output.
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//	Parameters that are set in getArgs()
//----------------------------------------------------------------------


int main(int argc, char **argv)
{


    if (argc < 2)
      {
        std::cout << "ERROR: data file name argument missing."
                  << "optimizationTexture3D <textureimage> [options] -fd <mask>"
                  << std::endl ;
        return EXIT_FAILURE;
      }

    bool featuredistance = false;
    std::string filenamefeature = "";
    vector< std::string > vectfilenamesample;
    std::string inputFilename = "";
    for(int i = 1; i < argc; i++){

        if (string(argv[i]) == "-fd"){
            featuredistance = true;
            if(i + 1 >= argc){
                std::cout << "ERROR: Feature distance filename missing."
                          <<std::endl
                          <<"-fd <filename>"
                          << std::endl ;
                return EXIT_FAILURE;
            }
            filenamefeature = argv[i + 1];
        }else if (string(argv[i]) == "-sp"){
            if(i + 1 >= argc){
                std::cout << "ERROR: Sample filename missing."
                          <<std::endl
                          <<"-fd <filename>"
                          << std::endl ;
                return EXIT_FAILURE;
            }
            if(inputFilename.compare("") == 0){
                inputFilename = argv[i + 1];
            }
            vectfilenamesample.push_back(argv[i + 1]);
        }

    }

    if(vectfilenamesample.size() == 0){
        std::cout<<"Image sample not set use -sp <image sample filname>"<<endl;
        return EXIT_FAILURE;
    }
    typedef unsigned char PixelType;
    typedef itk::RGBPixel< PixelType > RGBPixelType;
    typedef itk::Image< RGBPixelType, 2 > RGBImageType;
    typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;        

    vector< RGBImageType::Pointer > vectsampleimages;

    for(unsigned i = 0; i < vectfilenamesample.size(); i++){
        ImageReaderType::Pointer imageReader = ImageReaderType::New();

        imageReader->SetFileName(vectfilenamesample[i]);
        imageReader->Update() ;
        RGBImageType::Pointer image = imageReader->GetOutput();
        vectsampleimages.push_back(image);
    }

    if(featuredistance){

        /*typedef itk::Vector< double, 4 > VectorPixelType;
        typedef itk::Image< VectorPixelType, 2 > VectorImageType;

        typedef itk::Image< unsigned char, 2 > MaskImageType;
        typedef itk::ImageFileReader< MaskImageType > ImageReaderMaskType ;
        ImageReaderMaskType::Pointer imageReadermask = ImageReaderMaskType::New() ;

        imageReadermask->SetFileName(filenamefeature.c_str());
        imageReadermask->Update() ;
        MaskImageType::Pointer imagemask = imageReadermask->GetOutput();


        typedef itk::Image< double, 2 > DistanceImageType;
        typedef itk::SignedMaurerDistanceMapImageFilter< MaskImageType, DistanceImageType > DistanceType;
        DistanceType::Pointer distance = DistanceType::New();
        distance->SetInput(imagemask);
        distance->InsideIsPositiveOn();
        distance->Update();

        typedef itk::ImageFileWriter< DistanceImageType > ImageWriterLumType;
        ImageWriterLumType::Pointer distwriter = ImageWriterLumType::New();
        distwriter->SetFileName("distance.mhd");
        distwriter->SetInput( distance->GetOutput()  );
        distwriter->Update();


        VectorImageType::Pointer vectimage = VectorImageType::New();
        vectimage->SetRegions(image->GetLargestPossibleRegion());
        vectimage->Allocate();


        typedef itk::ImageRegionIterator< DistanceImageType > DistanceImageIterator;
        DistanceImageIterator itdistance(distance->GetOutput(), distance->GetOutput()->GetLargestPossibleRegion());

        typedef itk::ImageRegionIterator< VectorImageType > VectorImageIterator;
        VectorImageIterator itvect(vectimage, vectimage->GetLargestPossibleRegion());

        typedef itk::ImageRegionIterator< RGBImageType > RGBIteratorType;
        RGBIteratorType itrgb(image, image->GetLargestPossibleRegion());

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



        typedef itk::Image< VectorPixelType, 3 > VectorImage3DType;
        VectorImage3DType::Pointer imageobject = VectorImage3DType::New();
        VectorImage3DType::RegionType region;
        region.SetSize(0, 128);
        region.SetSize(1, 128);
        region.SetSize(2, 128);
        region.SetIndex(0,0);
        region.SetIndex(1,0);
        region.SetIndex(2,0);

        imageobject->SetRegions(region);

        imageobject->Allocate();



        typedef itk::RandomImageGenerator<VectorImageType, VectorImage3DType> RandomImageType;
        RandomImageType::Pointer randomgenerator = RandomImageType::New();
        randomgenerator->SetInputImageSample(vectimage);
        randomgenerator->SetInput(imageobject);
        randomgenerator->SetSamplesNumber(4000);
        //randomgenerator->DebugOn();
        randomgenerator->Update();
        imageobject = randomgenerator->GetOutput();


        typedef itk::OptimizationTexture<VectorImageType, VectorImage3DType> OptimizationType;
        OptimizationType::Pointer optimization = OptimizationType::New();
        //optimization->DebugOn();
        optimization->SetInputImageSample(vectimage);
        optimization->SetInput(imageobject);

        string filename = inputFilename.substr(inputFilename.find_last_of("/") + 1);
        filename = filename.substr(0, filename.size()-4);
        optimization->SetFilename(filename.c_str());
        optimization->SetExtension(".mhd");
        optimization->SetTexelDimension(4);
        optimization->Update();
        imageobject = optimization->GetOutput();



        typedef itk::Image< RGBPixelType, 3 > OutRGBImageType;

        OutRGBImageType::Pointer rgboutimage = OutRGBImageType::New();
        rgboutimage->SetRegions(imageobject->GetLargestPossibleRegion());
        rgboutimage->Allocate();

        typedef itk::ImageRegionIterator< OutRGBImageType > RGBOutIteratorType;
        RGBOutIteratorType itrgbout(rgboutimage, rgboutimage->GetLargestPossibleRegion());

        typedef itk::ImageRegionIterator< VectorImage3DType > VectorImageIterator3D;
        VectorImageIterator3D itvectout(imageobject, imageobject->GetLargestPossibleRegion());


        itrgbout.GoToBegin();
        itvectout.GoToBegin();

        while(!itrgbout.IsAtEnd()){
            OutRGBImageType::PixelType pixel = itrgbout.Get();
            for(unsigned i = 0; i < 3; i++){
                pixel[i]  =(unsigned char) itvectout.Get()[i];
            }
            itrgbout.Set(pixel);
            ++itvectout;
            ++itrgbout;

        }


        typedef itk::ImageFileWriter< OutRGBImageType > ImageWriterType;
        ImageWriterType::Pointer map_writer = ImageWriterType::New();

        //char buffer[10];
        //sprintf(buffer, "%d", times);
        //string outfile = "testOptimizationTexture";
        //outfile.append(buffer);
        //outfile.append(".png");
        //map_writer->SetFileName(outfile.c_str());
        filename.append("OptimizationTexture.png");
        map_writer->SetFileName(filename.c_str());
        map_writer->SetInput( rgboutimage  );
        map_writer->Update();


        return 0;*/
    }else{


        typedef itk::Image< RGBPixelType, 3 > RGBImage3DType;
        RGBImage3DType::Pointer imageobject = RGBImage3DType::New();
        RGBImage3DType::RegionType region;
        region.SetSize(0, 128);
        region.SetSize(1, 128);
        region.SetSize(2, 128);
        region.SetIndex(0,0);
        region.SetIndex(1,0);
        region.SetIndex(2,0);

        imageobject->SetRegions(region);

        imageobject->Allocate();

        typedef itk::OptimizationTexture<RGBImageType, RGBImage3DType> OptimizationType;
        OptimizationType::Pointer optimization = OptimizationType::New();
        //optimization->DebugOn();
        optimization->SetInputImageSamples(&vectsampleimages);
        optimization->SetInput(imageobject);

        string filename = inputFilename.substr(inputFilename.find_last_of("/") + 1);
        filename = filename.substr(0, filename.size()-4);
        optimization->SetFilename(filename.c_str());
        optimization->SetExtension(".mhd");
        optimization->Update();
        imageobject = optimization->GetOutput();


        typedef itk::Image< RGBImage3DType::PixelType::ComponentType, RGBImage3DType::ImageDimension > OutLumType;
        typedef itk::RGBToLuminanceImageFilter< RGBImage3DType, OutLumType > RGBToLumType;
        RGBToLumType::Pointer rgbtolum = RGBToLumType::New();
        rgbtolum->SetInput(imageobject);
        rgbtolum->Update();
        RGBToLumType::OutputImageType::Pointer lumimageobject = rgbtolum->GetOutput();


        typedef itk::ITKToVTKImageImport< RGBToLumType::OutputImageType > ITKToVTKImageType;
        ITKToVTKImageType::Pointer itktovtk = ITKToVTKImageType::New();
        itktovtk->SetITKImage(lumimageobject);
        itktovtk->Update();

        vtkImageData* vtkimageobject =itktovtk->GetOutputVTKImage();


        string mhdfile = filename;
        mhdfile.append("OptimizationTexture3D.mhd");

        vtkMetaImageWriter* writervtk =   vtkMetaImageWriter::New();

        writervtk->SetFileName(mhdfile.c_str());
        writervtk->SetInput(vtkimageobject);
        writervtk->Write();
        writervtk->Delete();

        return 0;
    }
}

