

#ifndef ITKANNCONNECTOR_TXX
#define ITKANNCONNECTOR_TXX

#ifndef ITKANNCONNECTOR_H
#include "itkannconnector.h"
#endif

#include "itkImageRegionIterator.h"

using namespace std;

namespace itk{
namespace Statistics{

    template < class TInputImage >
    ANNConnector< TInputImage >::ANNConnector()
    {
        dataPts = 0;
        queryPt = 0;
        nnIdx = 0;
        dists = 0;
        kdTree = 0;

        this->m_KNearNeighbours = 1;
        this->m_ErrorBound = 2;

    }

    template < class TInputImage >
    ANNConnector< TInputImage >::~ANNConnector(){
        if(nnIdx)
            delete [] nnIdx;							// clean things up
        if(dists)
            delete [] dists;
        if(kdTree)
            delete kdTree;

          annClose();
    }

    template < class TInputImage >
    void ANNConnector<TInputImage>::SetInputImage( const InputImageType * ptr ){

        int nPts = 1;
        int dim = 0;
        this->m_input = const_cast<InputImageType*>(ptr);
        InputImageType* vectorimage = const_cast<InputImageType*>(ptr);

        dim = (int) vectorimage->GetVectorLength();

        for(unsigned i = 0; i < vectorimage->GetLargestPossibleRegion().GetSize().GetSizeDimension(); i++){
            nPts*= vectorimage->GetLargestPossibleRegion().GetSize()[i];
        }

        itkDebugMacro("ANN (nPts, dim) =  ("<<nPts<<","<<dim<<")\n");

        dataPts = annAllocPts(nPts, dim);			// allocate data points

        nnIdx = new ANNidx[m_KNearNeighbours];						// allocate near neigh indices  (vector int)
        dists = new ANNdist[m_KNearNeighbours];						// allocate near neighbor dists (vector double)

        nPts = 0;									// read data points

        typedef itk::ImageRegionIterator< VectorImageType > VectorImageRegionIteratorType;
        VectorImageRegionIteratorType rit( vectorimage, vectorimage->GetLargestPossibleRegion() );
        rit.GoToBegin();

        PixelType vector;

        while( !rit.IsAtEnd() )
        {
            vector = rit.Get();
            for(int i = 0; i < dim; i++){
                dataPts[nPts][i] = vector[i];
            }
            nPts++;
            ++rit;
        }

        kdTree = new ANNkd_tree(					// build search structure
                                        dataPts,					// the data points
                                        nPts,						// number of points
                                        dim);						// dimension of space
    }


    template < class TInputImage >
    typename ANNConnector<TInputImage>::IndexType
    ANNConnector< TInputImage >::Evaluate(const PixelType& point){

        queryPt = annAllocPt(point.Size());					// allocate query point

        for(unsigned i=0; i < point.Size();i++){
            queryPt[i] = point[i];
        }

        kdTree->annkSearch(						// search
                        queryPt,						// query point
                        this->m_KNearNeighbours,								// number of near neighbors
                        nnIdx,							// nearest neighbors (returned)
                        dists,							// distance (returned)
                        this->m_ErrorBound);							// error bound

                                                                                // done with ANN

        typedef itk::ImageRegionConstIterator<InputImageType> ConstIteratorType;
        ConstIteratorType it(this->m_input, this->m_input->GetRequestedRegion());
        it.GoToBegin();

        for(int i = 0; i < nnIdx[0]; i++)
            ++it;

        this->_pointindex = it.GetIndex();



        if(this->GetDebug()){
            string res = " ";
            for(unsigned i = 0; i < it.Get().GetSize(); i++){
                char buffer[10];
                sprintf(buffer, "%f", it.Get()[i]);
                res.append(buffer);
                res.append(",");
            }

            itkDebugMacro("ANN index " << nnIdx[0] <<" index " << _pointindex[0] <<" "<< _pointindex[1]<< "Values " << res << "\n");
        }





        return this->_pointindex;
    }

}
}

#endif
