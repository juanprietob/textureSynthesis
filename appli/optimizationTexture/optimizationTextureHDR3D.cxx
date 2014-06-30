

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


    std::string inputFilename = argv[1];


    typedef double PixelType;
    typedef itk::Image< PixelType, 3 > ImageType;

    typedef itk::ImageFileReader< ImageType > ImageReaderType ;
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

    imageReader->SetFileName(inputFilename.c_str());
    imageReader->Update() ;
    ImageType::Pointer image = imageReader->GetOutput();


    const unsigned int texeldim = 3;
    typedef itk::Vector< double, texeldim > VectorPixelType;
    typedef itk::Image< VectorPixelType, 3 > VectorImageType;
    VectorImageType::Pointer vectimage = VectorImageType::New();

    vectimage->SetRegions(image->GetLargestPossibleRegion());
    vectimage->Allocate();


    typedef itk::ImageRegionIterator< VectorImageType > ImageVectorIteratorType;


    ImageVectorIteratorType itvector(vectimage, vectimage->GetLargestPossibleRegion());

    typedef itk::ImageRegionConstIterator< ImageType > ImageIteratorType;
    ImageIteratorType itimage(image, image->GetLargestPossibleRegion() );


    itimage.GoToBegin();
    itvector.GoToBegin();

    while(!itvector.IsAtEnd()){

        VectorPixelType vectorpixel;
        vectorpixel.Fill(0);

        vectorpixel[0] = itimage.Get();
        vectorpixel[1] = itimage.Get();
        vectorpixel[2] = itimage.Get();


        itvector.Set(vectorpixel);


        ++itimage;
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


    VectorImageType::Pointer imageobject = VectorImageType::New();
    VectorImageType::RegionType regionobject;
    regionobject.SetSize(0, 128);
    regionobject.SetSize(1, 128);
    regionobject.SetSize(2, 128);
    regionobject.SetIndex(0,0);
    regionobject.SetIndex(1,0);
    regionobject.SetIndex(2,0);
    imageobject->SetRegions(regionobject);
    imageobject->Allocate();


    typedef itk::RandomImageGenerator<VectorImage2DType, VectorImageType> RandomImageType;
    RandomImageType::Pointer randomgenerator = RandomImageType::New();
    randomgenerator->SetInputImageSample(vectimage2D);
    randomgenerator->SetInput(imageobject);
    randomgenerator->SetSamplesNumber(1000);
    //randomgenerator->DebugOn();
    randomgenerator->Update();
    imageobject = randomgenerator->GetOutput();




    typedef itk::OptimizationTexture<VectorImage2DType, VectorImageType> OptimizationType;
    OptimizationType::Pointer optimization = OptimizationType::New();
    //optimization->DebugOn();
    //optimization->SetInputImageSamples(vectimage2D);
    optimization->SetInput(imageobject);

    string filename = inputFilename.substr(inputFilename.find_last_of("/") + 1);
    filename = filename.substr(0, filename.size()-4);
    optimization->SetFilename(filename.c_str());
    optimization->SetExtension(".mhd");
    optimization->SetTexelDimension(texeldim);

    optimization->Update();

    imageobject = optimization->GetOutput();


    typedef itk::ITKToVTKImageImport< VectorImageType > ITKToVTKImageType;
    ITKToVTKImageType::Pointer itktovtk = ITKToVTKImageType::New();
    itktovtk->SetITKImage(imageobject);
    itktovtk->Update();

    vtkImageData* vtkimageobject =itktovtk->GetOutputVTKImage();


    string mhdfile = filename;
    mhdfile.append("OptimizationTextureHDR3D.mhd");

    vtkMetaImageWriter* writervtk =   vtkMetaImageWriter::New();

    writervtk->SetFileName(mhdfile.c_str());
    writervtk->SetInput(vtkimageobject);
    writervtk->Write();
    writervtk->Delete();


    return 0;
}



