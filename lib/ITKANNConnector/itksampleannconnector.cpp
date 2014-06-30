

#ifndef ITKSampleANNConnector_TXX
#define ITKSampleANNConnector_TXX

#ifndef ITKSampleANNConnector_H
#include "itksampleannconnector.h"
#endif

#include "itkImageRegionIterator.h"

using namespace std;

namespace itk{
namespace Statistics{

    template < class TSample >
    SampleANNConnector< TSample >::SampleANNConnector()
    {
        dataPts = 0;
        //queryPt = 0;
        kdTree = 0;

        this->m_KNearNeighbours = 1;
        this->m_ErrorBound = 2;
        this->m_NearestNeighbours = 0;
        this->m_Distances = 0;

    }

    template < class TSample >
    SampleANNConnector< TSample >::~SampleANNConnector(){        
        deleteKdTree();
        annClose();
    }

    template < class TSample >
    void SampleANNConnector< TSample >::deleteKdTree(){
        if(m_NearestNeighbours){
            delete [] this->m_NearestNeighbours;							// clean things up
            this->m_NearestNeighbours = 0;
        }
        if(m_Distances){
            delete [] this->m_Distances;
            this->m_Distances = 0;
        }
        if(kdTree){
            delete kdTree;        
            this->kdTree = 0;
        }
        if(dataPts){
            annDeallocPts(dataPts);
            //delete dataPts;
            this->dataPts = 0;
        }
    }

    template < class TSample >
    void SampleANNConnector<TSample>::Update(){
        GenerateData( );
    }

    template < class TSample >
    void SampleANNConnector<TSample>::GenerateData( ){

        deleteKdTree();

        int nPts = 1;
        int dim = 0;

        InputSamplePointerType inputsample = this->GetInputSample();

        dim =  inputsample->GetMeasurementVectorSize();
        nPts = inputsample->Size();

        itkDebugMacro("ANN (nPts, dim) =  ("<<nPts<<","<<dim<<")\n");


        dataPts = annAllocPts(nPts, dim);			// allocate data points

        this->m_NearestNeighbours = new ANNidx[m_KNearNeighbours];						// allocate near neigh indices  (vector int)
        this->m_Distances = new ANNdist[m_KNearNeighbours];						// allocate near neighbor dists (vector double);

        for(unsigned i = 0; i < inputsample->Size(); i++){
            for(unsigned j = 0; j < inputsample->GetMeasurementVectorSize(); j++){
                dataPts[i][j] = inputsample->GetMeasurementVector(i)[j];
            }
        }        

        kdTree = new ANNkd_tree(					// build search structure
                                        dataPts,					// the data points
                                        nPts,						// number of points
                                        dim);						// dimension of space
    }


    template < class TSample >
    const void  SampleANNConnector< TSample >::Evaluate(const MeasurementVectorType& point){

        ANNpoint queryPt = annAllocPt(point.Size());					// allocate query point

        for(unsigned i=0; i < point.Size();i++){
            queryPt[i] = point[i];
        }

        kdTree->annkSearch(						// search
                        queryPt,						// query point
                        this->m_KNearNeighbours,				// number of near neighbors
                        this->m_NearestNeighbours,							// nearest neighbors (returned)
                        this->m_Distances,							// distance (returned)
                        this->m_ErrorBound);							// error bound

                                                                                // done with ANN

        delete[] queryPt;

        if(this->GetDebug()){

            InputSamplePointerType inputsample = this->GetInputSample();
            string stringindex = "";
            string vectorvalues = "";
            char buffer[10];
            int currentindex;

            for(int i = 0; i < this->m_KNearNeighbours; i++){

                currentindex = m_NearestNeighbours[i];
                stringindex = "";
                sprintf(buffer, "%d", currentindex);
                stringindex.append(buffer);
                stringindex.append(",");

                vectorvalues = "";
                for(unsigned j = 0; j < inputsample->GetMeasurementVectorSize(); j++){
                    sprintf(buffer, "%f", inputsample->GetMeasurementVector(currentindex)[j]);
                    vectorvalues.append(buffer);
                    vectorvalues.append(",");
                }

                itkDebugMacro("ANN index " << stringindex << "Values " << vectorvalues << "\n");
            }

        }        
    }

    template < class TSample >
    const void  SampleANNConnector< TSample >::Evaluate(const vnl_vector<double> point){

        ANNpoint queryPt = annAllocPt(point.size());					// allocate query point

        for(unsigned i=0; i < point.size();i++){
            queryPt[i] = point[i];
        }

        kdTree->annkSearch(						// search
                        queryPt,						// query point
                        this->m_KNearNeighbours,				// number of near neighbors
                        this->m_NearestNeighbours,							// nearest neighbors (returned)
                        this->m_Distances,							// distance (returned)
                        this->m_ErrorBound);							// error bound

                                                                                // done with ANN

        delete[] queryPt;

        if(this->GetDebug()){

            InputSamplePointerType inputsample = this->GetInputSample();
            string stringindex = "";
            string vectorvalues = "";
            char buffer[10];
            int currentindex;

            for(int i = 0; i < this->m_KNearNeighbours; i++){

                currentindex = m_NearestNeighbours[i];
                stringindex = "";
                sprintf(buffer, "%d", currentindex);
                stringindex.append(buffer);
                stringindex.append(",");

                vectorvalues = "";
                for(unsigned j = 0; j < inputsample->GetMeasurementVectorSize(); j++){
                    sprintf(buffer, "%f", inputsample->GetMeasurementVector(currentindex)[j]);
                    vectorvalues.append(buffer);
                    vectorvalues.append(",");
                }

                itkDebugMacro("ANN index " << stringindex << "Values " << vectorvalues << "\n");
            }

        }
    }


    template < class TSample >
    const void  SampleANNConnector< TSample >::Evaluate(const vnl_vector<double> point, int* NearestNeighbours, double* distance){

        for(int i = 0; i < m_KNearNeighbours; i++){
            NearestNeighbours[i] = -1;
            distance[i] = -1;
        }

        ANNpoint queryPt = annAllocPt(point.size());					// allocate query point

        for(unsigned i=0; i < point.size();i++){
            queryPt[i] = point[i];
        }


        kdTree->annkSearchMultiThread(						// search
                        queryPt,						// query point
                        this->m_KNearNeighbours,				// number of near neighbors
                        NearestNeighbours,							// nearest neighbors (returned)
                        distance,							// distance (returned)
                        this->m_ErrorBound);	 						// error bound

                                                                                // done with ANN


        delete[] queryPt;


        if(this->GetDebug()){

            InputSamplePointerType inputsample = this->GetInputSample();
            string stringindex = "";
            string vectorvalues = "";
            char buffer[10];
            int currentindex;

            for(int i = 0; i < this->m_KNearNeighbours; i++){

                currentindex = m_NearestNeighbours[i];
                stringindex = "";
                sprintf(buffer, "%d", currentindex);
                stringindex.append(buffer);
                stringindex.append(",");

                vectorvalues = "";
                for(unsigned j = 0; j < inputsample->GetMeasurementVectorSize(); j++){
                    sprintf(buffer, "%f", inputsample->GetMeasurementVector(currentindex)[j]);
                    vectorvalues.append(buffer);
                    vectorvalues.append(",");
                }

                itkDebugMacro("ANN index " << stringindex << "Values " << vectorvalues << "\n");
            }

        }
    }

}
}

#endif
