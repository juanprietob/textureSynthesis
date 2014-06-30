//----------------------------------------------------------------------
//		File:			testRandomImageGenerator3D.cxx
//              Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------






#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "itksampleannconnector.h"
#include "itkvectorimagetosamplefeatures.h"
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


    if (argc < 3)
      {
        std::cout << "ERROR: data file name argument missing. <2D rgb image> <query points in rgb space>"
                  << std::endl ;
        return EXIT_FAILURE;
      }

    std::string inputFilename = argv[1];
    std::string inputFilename2 = argv[2];


    typedef unsigned char PixelType;
    typedef itk::RGBPixel< PixelType > RGBPixelType;
    typedef itk::Image< RGBPixelType, 2 > RGBImageType ;
    typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

    imageReader->SetFileName(inputFilename.c_str());
    imageReader->Update() ;
    RGBImageType::Pointer image = imageReader->GetOutput();

    /*imageReader = ImageReaderType::New() ;
    imageReader->SetFileName(inputFilename2.c_str());
    imageReader->Update();
    RGBImageType::Pointer imageobject = imageReader->GetOutput();   */


    typedef itk::Statistics::VectorImageToSampleFeatures< RGBImageType > ImageFeaturesType;
    ImageFeaturesType::Pointer imagefeatures = ImageFeaturesType::New();
    int neighborhoodsize = 8;
    //imagefeatures->DebugOn();
    imagefeatures->SetNeighborhoodSize(neighborhoodsize);
    imagefeatures->SetInput(image);
    imagefeatures->Update();
    typedef ImageFeaturesType::ListSampleType ListSampleType;
    ListSampleType *colorsample = const_cast<ListSampleType*>(imagefeatures->GetListSample());

    for(unsigned i = 0; i < colorsample->Size(); i++){
        cout<<"vector "<<i<<": ";
        for(unsigned j = 0; j < colorsample->GetMeasurementVector(i).GetSize(); j++){
            cout<<(int)colorsample->GetMeasurementVector(i)[j]<<", ";
        }
        cout<<endl;
    }

    typedef itk::Statistics::SamplePCAReduction< ListSampleType > PCAReductionType;
    PCAReductionType::Pointer pcareduction = PCAReductionType::New();
    pcareduction->SetKeepValues(0.95);
    pcareduction->SetInputSample(colorsample);
    pcareduction->DebugOn();
    pcareduction->Update();
    typedef PCAReductionType::OutputSampleType ListSamplePCAType;

    ListSamplePCAType *colorsamplepca = pcareduction->GetOutput();


    typedef itk::Statistics::SampleANNConnector< ListSamplePCAType > SampleANNConnectorType;
    SampleANNConnectorType::Pointer annconnector = SampleANNConnectorType::New();
    annconnector->SetInputSample(colorsamplepca);
    annconnector->Update();



    std::vector< std::vector< double > > datacsv;

    string line;
    ifstream myfile (inputFilename2.c_str());
    if (myfile.is_open())
    {
        while ( myfile.good() ){
            getline (myfile,line);
            int pos = 0, pos1 = 0;
            pos = line.find(":") + 1;
            pos1 = line.find(",", pos);

            string num = line.substr(pos, pos1 - pos);
            std::vector<double> current;

            while(pos1 > 0){                
                current.push_back((double)atof(num.c_str()));

                pos = pos1 + 1;
                pos1 = line.find(",", pos);
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
    typedef itk::Statistics::ListSample< VectorType > ListSampleDoubleType;
    ListSampleDoubleType::Pointer list = ListSampleDoubleType::New();

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


    for(unsigned i = 0; i < list->Size();i++){

        annconnector->Evaluate(pcareduction->GetPCAReducedVector(list->GetMeasurementVector(i)));//the closest neighbour is found
        int foundindex = annconnector->GetNearestNeighbours()[0];

        cout<<"index "<<foundindex<<endl;

        if(foundindex == -1){
            cout<<"Test not passed"<<endl;
            return 1;
        }
    }


    cout<<"test passed"<<endl;


    /*
    typedef itk::ImageFileWriter< RGBImageType > ImageWriterType;
    ImageWriterType::Pointer map_writer = ImageWriterType::New();

    //char buffer[10];
    //sprintf(buffer, "%d", times);
    //string outfile = "testOptimizationTexture";
    //outfile.append(buffer);
    //outfile.append(".png");
    //map_writer->SetFileName(outfile.c_str());
    map_writer->SetFileName("testANN.png");
    map_writer->SetInput( image  );
    map_writer->Update();
*/


    return 0;
}
