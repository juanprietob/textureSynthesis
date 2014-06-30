#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkConstantPadImageFilter.h>
#include <itkSignedDanielssonDistanceMapImageFilter.h>

#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkErodeObjectMorphologyImageFilter.h>
#include <itkDilateObjectMorphologyImageFilter.h>
#include <itkRegionOfInterestImageFilter.h>


#include "itkBinaryThinningImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

//#include <itkResampleImageFilter.h>
//#include <itkAffineTransform.h>
//#include <itkNearestNeighborInterpolateImageFunction.h>

#include <stack>
#include <iomanip>

//#include "common.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkvectordiscretegaussianimagefilter.h"

using namespace std;

typedef unsigned short Pixel;
typedef itk::Image<Pixel, 3> ImageType;
typedef ImageType::Pointer ImagePointer;


ImagePointer closeImage( ImagePointer image, const double radius )
{
        // The ball structuring element:
        typedef itk::BinaryBallStructuringElement<Pixel, 3> Kernel;
        // The dilation filter
        //typedef DilateObjectMorphologyImageFilter<ImageType, ImageType, Kernel> DilateFilter;
        typedef itk::BinaryDilateImageFilter<ImageType, ImageType, Kernel> DilateFilter;
        // The erosion filter
        //typedef ErodeObjectMorphologyImageFilter<ImageType, ImageType, Kernel> ErodeFilter;
        typedef itk::BinaryErodeImageFilter<ImageType, ImageType, Kernel> ErodeFilter;

        // Create the structuring element:
        ImageType::SpacingType spacing = image->GetSpacing();
        Kernel ball;
        Kernel::SizeType ballSize;
        ballSize[0] = (unsigned int)ceil(radius/spacing[0]);
        ballSize[1] = (unsigned int)ceil(radius/spacing[1]);
        ballSize[2] = (unsigned int)ceil(radius/spacing[2]);
        cout << "Creating structuring element ["
                 << ballSize[0] << ", " << ballSize[1] << ", " << ballSize[2]
                 << "] ... " << flush;
        ball.SetRadius(ballSize);
        ball.CreateStructuringElement();
        cout << "done" << endl << flush;
//        writeImage<ImageType>(image, "original.mhd");

        // Before dilating, pad the image, so that structures near the edges do not get messed up.
        typedef itk::ConstantPadImageFilter<ImageType, ImageType> PadFilter;

        PadFilter::Pointer padFilter = PadFilter::New();
        padFilter->SetInput(image);
        padFilter->SetConstant(0);
        const unsigned long pad = max(max(ballSize[0],ballSize[1]),ballSize[2]) + 1;
        const unsigned long padding[3] = { pad, pad, pad };
        padFilter->SetPadLowerBound(padding);
        padFilter->SetPadUpperBound(padding);
        padFilter->Update();

        // Now do the close operation
        cout << "Dilating ... " << flush;
        DilateFilter::Pointer closeDilate        = DilateFilter::New();
        //closeDilate->SetObjectValue(1);
        closeDilate->SetForegroundValue(1);
        closeDilate->SetKernel(ball);
        closeDilate->SetInput(padFilter->GetOutput());
        closeDilate->Update();
        cout << "done" << endl << flush;
//        writeImage<ImageType>(closeDilate->GetOutput(), "dilated.mhd");

        cout << "Eroding ... " << flush;
        ErodeFilter::Pointer closeErode        = ErodeFilter::New();
        //closeErode->SetObjectValue(1);
        closeErode->SetForegroundValue(1);
        closeErode->SetKernel(ball);
        closeErode->SetInput(closeDilate->GetOutput());
        closeErode->Update();
        cout << "done" << endl << flush;
//        writeImage<ImageType>(closeErode->GetOutput(), "closed.mhd");

        // Remove the extra padding that we had added.
        ImageType::RegionType desiredRegion;
        // ConstantPadImageFilter is crazy -- it creates images with negative indices.
        ImageType::IndexType idxLower = {{ 0, 0, 0 }};
        desiredRegion.SetIndex( idxLower );
        desiredRegion.SetSize( image->GetLargestPossibleRegion().GetSize() );
        // Setup the region of interest filter
        typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> ROIFilter;
        ROIFilter::Pointer roi        = ROIFilter::New();
        roi->SetInput( closeErode->GetOutput() );
        roi->SetRegionOfInterest( desiredRegion );
        roi->Update();

        return roi->GetOutput();
}

ImagePointer openImage( ImagePointer image, const double radius )
{
        // The ball structuring element:
        typedef itk::BinaryBallStructuringElement<Pixel, 3> Kernel;
        // The dilation filter
        //typedef DilateObjectMorphologyImageFilter<ImageType, ImageType, Kernel> DilateFilter;
        typedef itk::BinaryDilateImageFilter<ImageType, ImageType, Kernel> DilateFilter;
        // The erosion filter
        //typedef ErodeObjectMorphologyImageFilter<ImageType, ImageType, Kernel> ErodeFilter;
        typedef itk::BinaryErodeImageFilter<ImageType, ImageType, Kernel> ErodeFilter;

        // Create the structuring element:
        ImageType::SpacingType spacing = image->GetSpacing();
        Kernel ball;
        Kernel::SizeType ballSize;
        ballSize[0] = (unsigned int)ceil(radius/spacing[0]);
        ballSize[1] = (unsigned int)ceil(radius/spacing[1]);
        ballSize[2] = (unsigned int)ceil(radius/spacing[2]);
        cout << "Creating structuring element ["
                 << ballSize[0] << ", " << ballSize[1] << ", " << ballSize[2]
                 << "] ... " << flush;
        ball.SetRadius(ballSize);
        ball.CreateStructuringElement();
        cout << "done" << endl << flush;
//        writeImage<ImageType>(image, "original.mhd");

        // Now do the open operation
        cout << "Eroding ... " << flush;
        ErodeFilter::Pointer openErode        = ErodeFilter::New();
        //openErode->SetObjectValue(1);
        openErode->SetForegroundValue(1);
        openErode->SetKernel(ball);
        openErode->SetInput(image);
        openErode->Update();
        cout << "done" << endl << flush;
//        writeImage<ImageType>(openErode->GetOutput(), "eroded.mhd");

        cout << "Dilating ... " << flush;
        DilateFilter::Pointer openDilate        = DilateFilter::New();
        //openDilate->SetObjectValue(1);
        openDilate->SetForegroundValue(1);
        openDilate->SetKernel(ball);
        openDilate->SetInput(openErode->GetOutput());
        openDilate->Update();
        cout << "done" << endl << flush;
//        writeImage<ImageType>(openDilate->GetOutput(), "opened.mhd");

        return openDilate->GetOutput();
}



int main (int argc, char **argv)
{
    if(argc < 2) {
        cout << "testITKFilters <image> [option] <ext> "<<endl;
        return 1;
    }

    const string inputImageName = string(argv[1]);

    string outDDMMap = inputImageName.substr(0, inputImageName.find_last_of("."));
    if(argc > 2){
        string ext = string(argv[2]);

        if(ext.c_str()[0] != '.'){
            outDDMMap.append(".");
        }
        outDDMMap.append(string(argv[2]));

    }else{
        outDDMMap.append(".mhd");
    }
	cout<<"OutFilename "<<outDDMMap<<endl;

    try {

        typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
        ImageFileReaderType::Pointer reader = ImageFileReaderType::New();

        reader->SetFileName( inputImageName.c_str() );
        reader->Update();
        ImagePointer image = reader->GetOutput();

        typedef  itk::ImageFileWriter< ImageType  > WriterType;
		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName(outDDMMap.c_str());
        writer->SetInput(image);
        writer->Update();

    }catch( itk::ExceptionObject & e ) {
        cerr << "Exception caught : " << e.GetDescription() << endl;
    }

    return 0;
}

// vi: set ts=4 sw=4:

