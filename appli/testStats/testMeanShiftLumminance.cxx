
#include <stdio.h>

#include <itkImageFileReader.h>
#include <itkImageRegionIterator.h>
#include "itkrgbimagetosamplefeatures.h"
#include <itkSampleMeanShiftClusteringFilter.h>

#include <itkImageFileWriter.h>
#include <itkRGBToLuminanceImageFilter.h>

#include <itkvectorimagemeanshiftclusteringfilter.h>

int main(int argc, char* argv[] )
{
  if (argc < 2)
    {
      std::cout << "ERROR: data file name argument missing."
                << std::endl ;
      return EXIT_FAILURE;
    }

  std::string inputFilename = argv[1];

  typedef itk::RGBPixel< unsigned char > PixelTypeRGB;
  typedef itk::Image< PixelTypeRGB, 2 > ImageTypeRGB ;
  typedef itk::ImageFileReader< ImageTypeRGB > ImageReaderType ;
  ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

  imageReader->SetFileName(inputFilename.c_str());
  imageReader->Update() ;
  ImageTypeRGB::Pointer imagergb = imageReader->GetOutput() ;


  typedef ImageTypeRGB OutputImageType ;


  typedef itk::Statistics::RGBImageToSampleFeatures< ImageTypeRGB > RGBToSampleType;
  RGBToSampleType::Pointer rgbtosample = RGBToSampleType::New();

  rgbtosample->DebugOn();
  rgbtosample->SetInput(imagergb);
  rgbtosample->SetNeighborhoodSize(0);
  rgbtosample->Update();

  typedef RGBToSampleType::ListSampleType ListAdaptorType;
  ListAdaptorType* listadaptor =  const_cast<ListAdaptorType*>(rgbtosample->GetListSample());

  typedef itk::Statistics::SampleMeanShiftClusteringFilter< ListAdaptorType > ClusteringMethodType;

  ClusteringMethodType::Pointer clustering2 = ClusteringMethodType::New();

  clustering2->SetInputSample(listadaptor);
  clustering2->SetThreshold( 50 ) ;//radius
  clustering2->SetMinimumClusterSize( 10 ) ;
  clustering2->DebugOn() ;
  clustering2->Update() ;


  OutputImageType::Pointer clusterMap = OutputImageType::New() ;
  clusterMap->SetRegions( imagergb->GetLargestPossibleRegion() ) ;
  clusterMap->Allocate() ;

  typedef itk::ImageRegionIterator< OutputImageType > ImageIteratorType ;
  ImageIteratorType m_iter( clusterMap,
                            clusterMap->GetLargestPossibleRegion() ) ;
  m_iter.GoToBegin() ;

  ClusteringMethodType::ClusterLabelsType clusterLabels =
    clustering2->GetOutput() ;

  ClusteringMethodType::ClusterLabelsType::iterator co_iter =
    clusterLabels.begin() ;

  while ( co_iter != clusterLabels.end() )
    {



    m_iter.Set( *co_iter ) ;

    PixelTypeRGB label = m_iter.Get();
    std::cout<<(int)label[0]<<std::endl;

    //printf("%s \t %s \t %s\n", pixel[0], pixel[1], pixel[2]);
    //printf("%d \n", pixel);
    ++co_iter ;
    ++m_iter ;
    }

  typedef itk::ImageFileWriter< OutputImageType > ImageWriterType ;
  ImageWriterType::Pointer map_writer = ImageWriterType::New() ;
  map_writer->SetFileName("clusteredImage0.png") ;
  map_writer->SetInput( clusterMap ) ;
  map_writer->Update() ;



  typedef itk::Statistics::VectorImageMeanShiftClusteringFilter< ImageTypeRGB, ImageTypeRGB > VectorMeanShiftType;
  VectorMeanShiftType::Pointer meanshiftvector = VectorMeanShiftType::New();
  meanshiftvector->SetInput(imagergb);
  meanshiftvector->SetThreshold(100);

  map_writer->SetFileName("clusteredImage1.png") ;
  map_writer->SetInput( meanshiftvector->GetOutput() ) ;
  map_writer->Update() ;



  return EXIT_SUCCESS;
}
