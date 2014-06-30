#include "itkMedianImageFilter.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkSubtractImageFilter.h"
#include "itkVector.h"
#include "itkImageFileWriter.h"

#include "itksys/SystemTools.hxx"

#include <sstream>

#include "itkImageRegionIterator.h"
#include "itkvectordiscretegaussianimagefilter.h"

using namespace std;

int main(int argc, char * argv[])
{
    // Verify command line arguments
    if( argc < 2 )
    {
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " InputImageFile [radius] [type]" << std::endl;
        return EXIT_FAILURE;
    }
    std::string inputFilename = argv[1];    

    // Setup types
    typedef double PixelType;
    static const int DIM = 3;
    int type = 0;

    typedef itk::Vector<PixelType, DIM>   VectorPixelType;
    typedef itk::Image<VectorPixelType, 3 > VectorImageType;


    cout<<"Reading Image= "<<inputFilename<<endl;

    typedef itk::ImageFileReader<VectorImageType>                     ReaderType;
    // Create and setup a reader
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inputFilename );
    reader->Update();
    VectorImageType::Pointer image = reader->GetOutput();

     VectorImageType::Pointer outimage = 0;

    if(argc == 4){
        type = atoi(argv[3]);
    }

    if(type){

        typedef itk::Image<PixelType, 3 > ImageType;
        vector< ImageType::Pointer > vectorimages;

        for(unsigned i = 0; i < DIM; i++){
            ImageType::Pointer imagedim = ImageType::New();
            imagedim->SetRegions(image->GetLargestPossibleRegion());
            imagedim->SetSpacing(image->GetSpacing());
            imagedim->SetOrigin(image->GetOrigin());
            imagedim->Allocate();

            vectorimages.push_back(imagedim);
        }


        cout<<"Separating components ..."<<endl;
        itk::ImageRegionIterator< VectorImageType > it(image, image->GetLargestPossibleRegion());
        it.GoToBegin();

        while(!it.IsAtEnd()){
            for(unsigned i = 0; i < it.Get().Size(); i++){
                vectorimages[i]->SetPixel(it.GetIndex(), it.Get()[i]);
            }
            ++it;
        }

        // Create and setup a median filter

        cout<<"Filtering ... "<<endl;
        typedef itk::MedianImageFilter<ImageType, ImageType> MedianFilterType;

        for(unsigned i = 0; i < vectorimages.size(); i++){

            MedianFilterType::Pointer medianFilter = MedianFilterType::New();
            MedianFilterType::InputSizeType radius;
            radius.Fill(2);
            if (argc > 2)
            {
                radius.Fill(atoi(argv[2]));
            }

            medianFilter->SetRadius(radius);
            medianFilter->SetInput(vectorimages[i]);
            medianFilter->Update();
            ImageType::Pointer outimage = medianFilter->GetOutput();

            vectorimages[i] = outimage;

        }

        cout<<"Gathering components ..."<<endl;
        outimage = VectorImageType::New();
        outimage->SetRegions(image->GetLargestPossibleRegion());
        outimage->SetSpacing(image->GetSpacing());
        outimage->SetOrigin(image->GetOrigin());
        outimage->Allocate();

        itk::ImageRegionIterator< VectorImageType > outimageit(outimage, outimage->GetLargestPossibleRegion());

        outimageit.GoToBegin();

        while(!outimageit.IsAtEnd()){

            VectorPixelType outpixel;

            for(unsigned i = 0; i < vectorimages.size(); i++){
                outpixel[i] = vectorimages[i]->GetPixel(outimageit.GetIndex());
            }

            outimageit.Set( outpixel );
            ++outimageit;
        }



    }else{

        typedef itk::VectorDiscreteGaussianImageFilter< VectorImageType, VectorImageType> DiscreteType;
        DiscreteType::Pointer discrete = DiscreteType::New();
        discrete->SetInput(image);
        discrete->SetVariance(2);

        if (argc > 2)
        {
            discrete->SetVariance(atof(argv[2]));
        }

        discrete->Update();
        outimage = discrete->GetOutput();

    }

    string outFilename = inputFilename.substr(0, inputFilename.size()-4);
    outFilename.append("Smooth.mhd");

    cout<<"Done! "<<endl;
    cout<<"Filtered image: "<<outFilename<<endl;

    typedef itk::ImageFileWriter< VectorImageType > ImageWriterType;
    ImageWriterType::Pointer map_writer = ImageWriterType::New();
    map_writer->SetFileName(outFilename.c_str());
    map_writer->SetInput( outimage  );
    map_writer->Update();

    cout<<endl;

    return EXIT_SUCCESS;
}
