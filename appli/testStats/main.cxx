#include <stdio.h>

#include <itkImageFileReader.h>
#include <itkImageRegionIterator.h>
#include <itkImageToListAdaptor.h>
#include <itkSampleMeanShiftClusteringFilter.h>

#include <itkImageFileWriter.h>
#include "itkrgbimagetolistadaptor.h"


int main(int argc, char* argv[] )
{
  if (argc < 2)
    {
      std::cout << "ERROR: data file name argument missing."
                << std::endl ;
      return EXIT_FAILURE;
    }

  std::string inputFilename = argv[1];

  typedef itk::RGBPixel< unsigned char > PixelType;
  //typedef unsigned char PixelType ;
  typedef itk::Image< PixelType, 2 > ImageType ;
  typedef itk::ImageFileReader< ImageType > ImageReaderType ;
  ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

  imageReader->SetFileName(inputFilename.c_str());
  imageReader->Update() ;
  ImageType::Pointer image = imageReader->GetOutput() ;


  typedef itk::Statistics::RGBImageToListAdaptor< ImageType >   ListAdaptorType ;
  //typedef itk::Statistics::ScalarImageToListAdaptor< ImageType >   ListAdaptorType ;

  ListAdaptorType::Pointer listadaptor = ListAdaptorType::New();

  listadaptor->SetImage(image);

  typedef itk::Statistics::SampleMeanShiftClusteringFilter< ListAdaptorType > ClusteringMethodType;

  ClusteringMethodType::Pointer clustering2 = ClusteringMethodType::New();

  clustering2->SetInputSample(listadaptor);
  clustering2->SetThreshold( 10 ) ;
  clustering2->SetMinimumClusterSize( 100 ) ;
  clustering2->DebugOn() ;
  clustering2->Update() ;

  typedef ImageType OutputImageType ;

  OutputImageType::Pointer clusterMap = OutputImageType::New() ;
  clusterMap->SetRegions( image->GetLargestPossibleRegion() ) ;
  clusterMap->Allocate() ;

  typedef itk::ImageRegionIterator< OutputImageType > ImageIteratorType ;
  ImageIteratorType m_iter( clusterMap,
                            clusterMap->GetLargestPossibleRegion() ) ;
  m_iter.GoToBegin() ;

  ClusteringMethodType::ClusterLabelsType clusterLabels =
    clustering2->GetOutput() ;

  ClusteringMethodType::ClusterLabelsType::iterator co_iter =
    clusterLabels.begin() ;
PixelType pixel;
  while ( co_iter != clusterLabels.end() )
    {
    pixel.SetRed(((PixelType) *co_iter).GetRed());
    pixel.SetGreen(((PixelType) *co_iter).GetGreen());
    pixel.SetBlue(((PixelType) *co_iter).GetBlue());    
    //pixel.Print(std::cout);
    m_iter.Set( pixel ) ;
    ++co_iter ;
    ++m_iter ;
    }

  typedef itk::ImageFileWriter< OutputImageType > ImageWriterType ;
  ImageWriterType::Pointer map_writer = ImageWriterType::New() ;
  map_writer->SetFileName("clustered_sf2.png") ;
  map_writer->SetInput( clusterMap ) ;
  map_writer->Update() ;




  return EXIT_SUCCESS;
}
