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

#include <itkListSample.h>
#include "itksamplepcareduction.h"
#include <itkVariableLengthVector.h>


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


    std::vector< std::vector< double > > datacsv;

    string line;
    ifstream myfile (inputFilename.c_str());
    if (myfile.is_open())
    {
        while ( myfile.good() ){
            getline (myfile,line);
            int pos = 0, pos1 = 0;
            pos1 = line.find("\t");

            string num = line.substr(pos, pos1);
            std::vector<double> current;
            while(pos1 > 0){
                current.push_back((double)atof(num.c_str()));

                pos = pos1 + 1;
                pos1 = line.find("\t", pos);
                num = "";
                num = line.substr(pos, pos1-pos);
            }
            if(current.size() > 0){
                if(num.size() > 0){
                    current.push_back(atof(num.c_str()));
                }
                datacsv.push_back(current);
            }
        }
        myfile.close();
    }else{
         cout << "Unable to open file";
         return 0;
    }



    typedef itk::VariableLengthVector< double > VectorType;
    typedef itk::Statistics::ListSample< VectorType > ListSampleType;
    ListSampleType::Pointer list = ListSampleType::New();

    unsigned measurementsize = datacsv[0].size();

    list->SetMeasurementVectorSize( measurementsize );

    for(unsigned i = 0; i < datacsv.size(); i++){
        VectorType vector;
        vector.SetSize(measurementsize);
        vector.Fill(-1);
        for(unsigned j = 0; j < measurementsize; j++){
            vector[j] = datacsv[i][j];
        }
        list->PushBack(vector);
    }

    typedef itk::Statistics::SamplePCAReduction< ListSampleType > SamplePCAType;
    SamplePCAType::Pointer samplepca = SamplePCAType::New();
    samplepca->DebugOn();
    samplepca->SetKeepValues(1);
    samplepca->SetInputSample(list);
    samplepca->Update();




    return 0;
}
