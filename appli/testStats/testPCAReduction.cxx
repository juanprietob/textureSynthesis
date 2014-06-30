//----------------------------------------------------------------------
//		File:			ann_sample.cpp
//		Programmer:		Sunil Arya and David Mount
//		Last modified:	03/04/98 (Release 0.1)
//		Description:	Sample program for ANN
//----------------------------------------------------------------------
// Copyright (c) 1997-2005 University of Maryland and Sunil Arya and
// David Mount.  All Rights Reserved.
//
// This software and related documentation is part of the Approximate
// Nearest Neighbor Library (ANN).  This software is provided under
// the provisions of the Lesser GNU Public License (LGPL).  See the
// file ../ReadMe.txt for further information.
//
// The University of Maryland (U.M.) and the authors make no
// representations about the suitability or fitness of this software for
// any purpose.  It is provided "as is" without express or implied
// warranty.
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "itkrgbimagetovectorfeatures.h"
#include "itkrgbimagetovector.h"
#include "itkvectortorgbimage.h"
#include "itkoptimizationtexture.h"

#include "itkImageConstIterator.h"

#include "itkrgbimagetosamplefeatures.h"
#include "itksamplepcareduction.h"


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

    typedef unsigned char PixelType;
    typedef itk::RGBPixel< PixelType > RGBPixelType;
    typedef itk::Image< RGBPixelType, 2 > RGBImageType ;
    typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

    imageReader->SetFileName(inputFilename.c_str());
    imageReader->Update() ;
    RGBImageType::Pointer image = imageReader->GetOutput();


    typedef itk::Statistics::RGBImageToSampleFeatures< RGBImageType > ImageFeaturesType;
    ImageFeaturesType::Pointer imagefeatures = ImageFeaturesType::New();
    imagefeatures->SetInput(image);
    imagefeatures->SetNeighborhoodSize(8);
    imagefeatures->Update();

    typedef ImageFeaturesType::ListSampleType ListSampleType;

    ListSampleType *list = const_cast<ListSampleType*>(imagefeatures->GetListSample());

    typedef itk::Statistics::SamplePCAReduction< ListSampleType > SamplePCAType;
    SamplePCAType::Pointer samplepca = SamplePCAType::New();
    samplepca->DebugOn();
    samplepca->SetKeepValues(0.95);
    samplepca->SetInputSample(list);
    samplepca->Update();




    return 0;
}
