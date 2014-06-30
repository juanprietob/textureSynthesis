
//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------


#include "vtkImageData.h"

#include "vtkMetaImageReader.h"
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itktovtkimageimport.h"

#include <itkGradientMagnitudeImageFilter.h>
#include <itkHessianRecursiveGaussianImageFilter.h>
#include <itkHessianToObjectnessMeasureImageFilter.h>
#include "itkSignedDanielssonDistanceMapImageFilter.h"

#include "itkDiscreteGaussianImageFilter.h"

#include <itkVectorImage.h>

#include <itkImageSliceIteratorWithIndex.h>

#include "vtkMetaImageWriter.h"

#include "itkrandomimagegenerator.h"
#include "itkoptimizationtexture.h"

#include "itkExpandImageFilter.h"

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
                  << std::endl ;
        return EXIT_FAILURE;
      }

    bool featuredistance = false;
    std::string filenamefeature = "";
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
            break;
        }

    }


    std::string inputFilename = argv[1];    


    typedef double PixelType;
    typedef itk::Image< PixelType, 3 > ImageType;

    typedef itk::ImageFileReader< ImageType > ImageReaderType ;
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

    imageReader->SetFileName(inputFilename.c_str());
    imageReader->Update() ;
    ImageType::Pointer image = imageReader->GetOutput();    


    /*typedef itk::ExpandImageFilter< ImageType, ImageType > ExpandimageType;
    ExpandimageType::Pointer expandimage = ExpandimageType::New();
    expandimage->SetExpandFactors(2);
    expandimage->SetInput(image);
    expandimage->Update();
    image = expandimage->GetOutput();*/



    typedef itk::HessianRecursiveGaussianImageFilter< ImageType > HessianType;
    HessianType::Pointer hessian = HessianType::New();
    hessian->SetInput(image);
    hessian->Update();

    typedef itk::HessianToObjectnessMeasureImageFilter< HessianType::OutputImageType, ImageType > HessianObjectnessType;
    HessianObjectnessType::Pointer hessianobject0 = HessianObjectnessType::New();
    hessianobject0->SetInput(hessian->GetOutput());    
    hessianobject0->SetBrightObject(true);
    hessianobject0->SetAlpha(0.5);
    hessianobject0->SetBeta(0.5);
    hessianobject0->SetGamma(5);
    hessianobject0->SetObjectDimension(0);
    hessianobject0->Update();

    HessianObjectnessType::Pointer hessianobject1 = HessianObjectnessType::New();
    hessianobject1->SetInput(hessian->GetOutput());    
    hessianobject1->SetBrightObject(true);
    hessianobject1->SetAlpha(0.5);
    hessianobject1->SetBeta(0.5);
    hessianobject1->SetGamma(5.0);
    hessianobject1->SetObjectDimension(1);
    hessianobject1->Update();

    HessianObjectnessType::Pointer hessianobject2 = HessianObjectnessType::New();
    hessianobject2->SetInput(hessian->GetOutput());    
    hessianobject2->SetBrightObject(true);
    hessianobject2->SetAlpha(0.5);
    hessianobject2->SetBeta(0.5);
    hessianobject2->SetGamma(5.0);
    hessianobject2->SetObjectDimension(2);
    hessianobject2->Update();




    typedef itk::ImageRegionConstIterator< ImageType > ImageIteratorType;
    ImageIteratorType itimage(image, image->GetLargestPossibleRegion() );

    ImageIteratorType ithessian0(hessianobject0->GetOutput(), hessianobject0->GetOutput()->GetLargestPossibleRegion());
    ImageIteratorType ithessian1(hessianobject1->GetOutput(), hessianobject1->GetOutput()->GetLargestPossibleRegion());
    ImageIteratorType ithessian2(hessianobject2->GetOutput(), hessianobject2->GetOutput()->GetLargestPossibleRegion());


    const unsigned int texeldim = 3;
    typedef itk::Vector< double, texeldim > VectorPixelType;
    typedef itk::Image< VectorPixelType, 3 > VectorImageType;
    VectorImageType::Pointer vectimage = VectorImageType::New();

    vectimage->SetRegions(image->GetLargestPossibleRegion());    
    vectimage->Allocate();


    typedef itk::ImageRegionIterator< VectorImageType > ImageVectorIteratorType;


    ImageVectorIteratorType itvector(vectimage, vectimage->GetLargestPossibleRegion());



    itimage.GoToBegin();        
    ithessian0.GoToBegin();
    ithessian1.GoToBegin();
    ithessian2.GoToBegin();

    itvector.GoToBegin();

    while(!itvector.IsAtEnd()){

        VectorPixelType vectorpixel;
        vectorpixel.Fill(0);

        vectorpixel[0] = itimage.Get();
        vectorpixel[1] = itimage.Get();
        vectorpixel[2] = itimage.Get();

        //vectorpixel[1] = ithessian0.Get();
        //vectorpixel[2] = ithessian1.Get();
        //vectorpixel[3] = ithessian2.Get();



        itvector.Set(vectorpixel);


        ++itimage;
        ++ithessian0;
        ++ithessian1;
        ++ithessian2;
        ++itvector;

    }


    typedef itk::Image< VectorPixelType, 2 > VectorImage2DType;
    VectorImage2DType::Pointer vectimage2D = VectorImage2DType::New();
    VectorImage2DType::RegionType region;
    region.SetIndex(0, 0);
    region.SetIndex(1, 0);
    region.SetSize(0, vectimage->GetLargestPossibleRegion().GetSize()[0]);
    region.SetSize(1, vectimage->GetLargestPossibleRegion().GetSize()[2]);

    vectimage2D->SetRegions(region);
    vectimage2D->Allocate();

    typedef itk::ImageSliceIteratorWithIndex< VectorImage2DType > SliceIterator2DType;
    SliceIterator2DType itslice2d(vectimage2D, vectimage2D->GetLargestPossibleRegion() );
    itslice2d.SetFirstDirection(0);
    itslice2d.SetSecondDirection(1);
    itslice2d.GoToBegin();


    typedef itk::ImageSliceIteratorWithIndex< VectorImageType > SliceIteratorType;
    SliceIteratorType itslice(vectimage, vectimage->GetLargestPossibleRegion());
    itslice.SetFirstDirection(0);
    itslice.SetSecondDirection(2);


    VectorImageType::IndexType index;
    index[0] = 0;
    index[1] = 0;
    index[2] = 0;


    itslice.SetIndex(index);

    index[0] = 0;
    index[1] = 46;
    index[2] = 0;

    itslice.SetIndex(index);

    while(!itslice.IsAtEndOfSlice()){

        while(!itslice.IsAtEndOfLine() && !itslice2d.IsAtEndOfLine()){

            itslice2d.Set(itslice.Get());

            ++itslice;
            ++itslice2d;
        }

        itslice.NextLine();
        itslice2d.NextLine();

    }


    VectorImage2DType::Pointer imageobject = VectorImage2DType::New();
    VectorImage2DType::RegionType regionobject;
    regionobject.SetSize(0, 600);
    regionobject.SetSize(1, 600);
    regionobject.SetIndex(0,0);
    regionobject.SetIndex(1,0);
    imageobject->SetRegions(regionobject);
    imageobject->Allocate();


    typedef itk::RandomImageGenerator<VectorImage2DType> RandomImageType;
    RandomImageType::Pointer randomgenerator = RandomImageType::New();
    randomgenerator->SetInputImageSample(vectimage2D);
    randomgenerator->SetInput(imageobject);
    randomgenerator->SetSamplesNumber(1000);
    //randomgenerator->DebugOn();
    randomgenerator->Update();
    imageobject = randomgenerator->GetOutput();




    typedef itk::OptimizationTexture<VectorImage2DType> OptimizationType;
    OptimizationType::Pointer optimization = OptimizationType::New();
    //optimization->DebugOn();
    //optimization->SetInputImageSample(vectimage2D);
    optimization->SetInput(imageobject);

    string filename = inputFilename.substr(inputFilename.find_last_of("/") + 1);
    filename = filename.substr(0, filename.size()-4);
    optimization->SetFilename(filename.c_str());
    optimization->SetExtension(".mhd");
    optimization->SetTexelDimension(texeldim);

    optimization->Update();

    imageobject = optimization->GetOutput();


    typedef itk::Image< VectorImage2DType::PixelType::ComponentType, VectorImage2DType::ImageDimension > Image2DType;
    Image2DType::Pointer outimage = Image2DType::New();
    outimage->SetRegions(imageobject->GetLargestPossibleRegion());
    outimage->Allocate();

    typedef itk::ImageRegionIterator< Image2DType > ImageIteratorType2D;
    ImageIteratorType2D itout(outimage, outimage->GetLargestPossibleRegion());

    typedef itk::ImageRegionIterator< VectorImage2DType > ImageVectorIteratorType2D;
    ImageVectorIteratorType2D itvector2(imageobject, imageobject->GetLargestPossibleRegion());

    itout.GoToBegin();
    itvector2.GoToBegin();

    while(!itvector2.IsAtEnd()){

        itout.Set(itvector2.Get()[0]);

        ++itvector2;
        ++itout;

    }


    typedef itk::ITKToVTKImageImport< Image2DType > ITKToVTKImageType;
    ITKToVTKImageType::Pointer itktovtk = ITKToVTKImageType::New();
    itktovtk->SetITKImage(outimage);
    itktovtk->Update();

    vtkImageData* vtkimageobject = itktovtk->GetOutputVTKImage();


    string mhdfile = "";
    mhdfile.append(filename);
    mhdfile.append("OptimizationTextureHDR.mhd");

    vtkMetaImageWriter* writervtk =   vtkMetaImageWriter::New();

    writervtk->SetFileName(mhdfile.c_str());
    writervtk->SetInput(vtkimageobject);
    writervtk->Write();
    writervtk->Delete();

    return 0;
}



