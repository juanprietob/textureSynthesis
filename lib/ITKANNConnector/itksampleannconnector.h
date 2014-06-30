#ifndef ITKSampleANNConnector_H
#define ITKSampleANNConnector_H

#include <cstdlib>						// C standard library
#include <cstdio>						// C I/O (for sscanf)
#include <cstring>						// string manipulation
#include <fstream>						// file I/O
#include <ANN.h>					// ANN declarations

#include <itkVectorImage.h>
//#include <itkSampleAlgorithmBase.h>
#include <itkListSample.h>
#include <itkProcessObject.h>


namespace itk{
namespace Statistics{

template < class TSample >
class SampleANNConnector :
public ProcessObject
{
public:
    typedef SampleANNConnector           Self;    
    typedef SmartPointer<Self>             Pointer;
    typedef SmartPointer<const Self>       ConstPointer;

    /** Standard Macros */
    itkTypeMacro(SampleANNConnector, ProcessObject);
    itkNewMacro(Self);

    typedef TSample       InputSampleType;
    typedef typename InputSampleType::Pointer InputSamplePointerType;
    /** Length of a measurement vector */
    typedef typename InputSampleType::MeasurementVectorSizeType MeasurementVectorSizeType;

    /** Measurement vector type */
    typedef typename InputSampleType::MeasurementVectorType MeasurementVectorType;


    virtual void Update();


    /** Updates the ANN Structure with the current sample input data */
    void GenerateData( void );


    /** Evalutate the  in the given dimension at specified point */
    virtual inline const void Evaluate(const MeasurementVectorType& point);
    virtual inline const void Evaluate(const vnl_vector<double> point);
    virtual inline const void Evaluate(const vnl_vector<double> point, int* NearestNeighbours, double* distance);

    itkGetConstMacro(NearestNeighbours, int*)
    itkGetConstMacro(Distances, double*)


    itkSetMacro(KNearNeighbours, int)
    itkGetMacro(KNearNeighbours, int)

    itkSetMacro(ErrorBound, double)
    itkGetMacro(ErrorBound, double)

    itkSetMacro(InputSample, InputSamplePointerType)
    itkGetMacro(InputSample, InputSamplePointerType)


protected:
    SampleANNConnector();
    ~SampleANNConnector();

private:
    ANNpointArray		dataPts;				// data points

    //ANNidxArray			nnIdx;					// near neighbor indices
    //ANNdistArray		dists;					// near neighbor distances
    ANNkd_tree*			kdTree;					// search structure
    int				m_KNearNeighbours;		// number of nearest neighbors
    double			m_ErrorBound;			// error bound

    int*                        m_NearestNeighbours;         //nearest n found
    double*                     m_Distances;                //distances to the neighbours


    InputSamplePointerType m_InputSample;

    void deleteKdTree();

};

}
}
#ifndef ITK_MANUAL_INSTANTIATION
#ifndef ITKSampleANNConnector_TXX
#include "itksampleannconnector.cpp"
#endif
#endif

#endif // ITKSampleANNConnector_H
