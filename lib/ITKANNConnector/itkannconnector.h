#ifndef ITKANNCONNECTOR_H
#define ITKANNCONNECTOR_H

#include <cstdlib>						// C standard library
#include <cstdio>						// C I/O (for sscanf)
#include <cstring>						// string manipulation
#include <fstream>						// file I/O
#include <ANN/ANN.h>					// ANN declarations

#include <itkVectorImage.h>
#include <itkProcessObject.h>


namespace itk{
namespace Statistics{

template < class TInputImage >
class ANNConnector :
public ProcessObject
{

public:

    /** Standard class typedefs. */
    typedef ANNConnector           Self;

    typedef TInputImage VectorImageType;

    typedef typename TInputImage::InternalPixelType InternalPixelType;

    /** Standard "Superclass" typedef */
    typedef ProcessObject Superclass;

    /** Smart pointer typedef support. */
    typedef SmartPointer<Self>            Pointer;
    typedef SmartPointer<const Self>      ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro( ANNConnector, ProcessObject );

    /** InputImageType typedef support. */
    typedef TInputImage                                 InputImageType;
    typedef typename InputImageType::Pointer                     InputImagePointerType;
    typedef typename TInputImage::PixelType             PixelType;
    typedef typename InputImageType::IndexType          IndexType;

    /** Evalutate the  in the given dimension at specified point */
    virtual inline IndexType Evaluate(const PixelType& point);

    /** Set the input image.
     * \warning this method caches BufferedRegion information.
     * If the BufferedRegion has changed, user must call
     * SetInputImage again to update cached values. */
    virtual void SetInputImage( const InputImageType * ptr );


    itkSetMacro(KNearNeighbours, int);
    itkGetMacro(KNearNeighbours, int);

    itkSetMacro(ErrorBound, double);
    itkGetMacro(ErrorBound, double);

protected:
    ANNConnector();
    ~ANNConnector();

private:
    ANNpointArray		dataPts;				// data points
    ANNpoint			queryPt;				// query point
    ANNidxArray			nnIdx;					// near neighbor indices
    ANNdistArray		dists;					// near neighbor distances
    ANNkd_tree*			kdTree;					// search structure
    int				m_KNearNeighbours;		// number of nearest neighbors
    double			m_ErrorBound;			// error bound


    IndexType _pointindex;
    PixelType _vectorPoint;
    InputImagePointerType m_input;

};

}
}
#ifndef ITK_MANUAL_INSTANTIATION
#ifndef ITKANNCONNECTOR_TXX
#include "itkannconnector.cpp"
#endif
#endif

#endif // ITKANNCONNECTOR_H
