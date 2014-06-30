#ifndef ITKX2UMAPROTATIONMATRIXIMAGEFILTER_H
#define ITKX2UMAPROTATIONMATRIXIMAGEFILTER_H

#include "itkImageToImageFilter.h"
#include "itkMatrix.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"

#include <itkGradientImageFilter.h>
#include "itkvectordiscretegaussianimagefilter.h"
//#include "itkDiscreteGaussianImageFilter.h"
#include "itkvectorlaplacianimagefilter.h"

using namespace std;

namespace itk{

template<   class TInputImage,
            class TOutputImage = itk::Image< Matrix< double, TInputImage::ImageDimension, TInputImage::ImageDimension >, TInputImage::ImageDimension >
        >
class ITK_EXPORT X2UMapRotationMatrixImageFilter
        : public ImageToImageFilter< TInputImage,  TOutputImage>
{

public:
    typedef X2UMapRotationMatrixImageFilter                   Self;
    typedef ImageToImageFilter<TInputImage,TOutputImage>      Superclass;
    typedef SmartPointer<Self>                                Pointer;
    typedef SmartPointer<const Self>                          ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(X2UMapRotationMatrixImageFilter, ImageToImageFilter);


    /** Superclass typedefs. */
    typedef TOutputImage                                  OutputImageType;
    typedef typename OutputImageType::Pointer             OutputImagePointerType;
    typedef typename OutputImageType::RegionType          OutputImageRegionType;
    typedef typename OutputImageType::PixelType           OutputImagePixelType;
    typedef typename OutputImageType::IndexType           OutputImageIndexType;

    typedef itk::ImageRegionIteratorWithIndex< OutputImageType > OutputRegionIteratorType;

    /** Some convenient typedefs. */
    typedef TInputImage                             InputImageType;
    typedef typename InputImageType::Pointer        InputImagePointer;
    typedef typename InputImageType::ConstPointer   InputImageConstPointer;
    typedef typename InputImageType::RegionType     InputImageRegionType;
    typedef typename InputImageType::PixelType      InputImagePixelType;
    typedef typename InputImageType::IndexType      IndexType;
    typedef typename TInputImage::PixelType         PixelType;

    typedef itk::ImageRegionIteratorWithIndex< InputImageType > InputRegionIteratorType;

    /** If an imaging filter can be implemented as a multithreaded
     * algorithm, the filter will provide an implementation of
     * ThreadedGenerateData().  This superclass will automatically split
     * the output image into a number of pieces, spawn multiple threads,
     * and call ThreadedGenerateData() in each thread. Prior to spawning
     * threads, the BeforeThreadedGenerateData() method is called. After
     * all the threads have completed, the AfterThreadedGenerateData()
     * method is called. If an image processing filter cannot support
     * threading, that filter should provide an implementation of the
     * GenerateData() method instead of providing an implementation of
     * ThreadedGenerateData().  If a filter provides a GenerateData()
     * method as its implementation, then the filter is responsible for
     * allocating the output data.  If a filter provides a
     * ThreadedGenerateData() method as its implementation, then the
     * output memory will allocated automatically by this superclass.
     * The ThreadedGenerateData() method should only produce the output
     * specified by "outputThreadRegion"
     * parameter. ThreadedGenerateData() cannot write to any other
     * portion of the output image (as this is responsibility of a
     * different thread).
     *
     * \sa GenerateData(), SplitRequestedRegion() */
    virtual void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId );


    /** The GenerateData method normally allocates the buffers for all of the
     * outputs of a filter. Some filters may want to override this default
     * behavior. For example, a filter may have multiple outputs with
     * varying resolution. Or a filter may want to process data in place by
     * grafting its input to its output. */
    virtual void AllocateOutputs();

    /** If an imaging filter needs to perform processing after the buffer
     * has been allocated but before threads are spawned, the filter can
     * can provide an implementation for BeforeThreadedGenerateData(). The
     * execution flow in the default GenerateData() method will be:
     *      1) Allocate the output buffer
     *      2) Call BeforeThreadedGenerateData()
     *      3) Spawn threads, calling ThreadedGenerateData() in each thread.
     *      4) Call AfterThreadedGenerateData()
     * Note that this flow of control is only available if a filter provides
     * a ThreadedGenerateData() method and NOT a GenerateData() method. */
    virtual void BeforeThreadedGenerateData();

    /** If an imaging filter needs to perform processing after all
     * processing threads have completed, the filter can can provide an
     * implementation for AfterThreadedGenerateData(). The execution
     * flow in the default GenerateData() method will be:
     *      1) Allocate the output buffer
     *      2) Call BeforeThreadedGenerateData()
     *      3) Spawn threads, calling ThreadedGenerateData() in each thread.
     *      4) Call AfterThreadedGenerateData()
     * Note that this flow of control is only available if a filter provides
     * a ThreadedGenerateData() method and NOT a GenerateData() method. */
    virtual void AfterThreadedGenerateData();


    /** Some convenient typedefs. */
    typedef itk::Image<unsigned char,  TInputImage::ImageDimension >     InputImageMaskType;
    typedef typename InputImageMaskType::Pointer        InputImageMaskPointerType;
    typedef typename InputImageMaskType::ConstPointer   InputImageMaskConstPointerType;
    typedef typename InputImageMaskType::RegionType     InputImageMaskRegionType;
    typedef typename InputImageMaskType::PixelType      InputImageMaskPixelType;
    typedef typename InputImageMaskType::IndexType      InputImageMaskIndexType;

    /**
      * set the rgb image to create the transfer function
     */
    itkSetMacro(ImageMask, InputImageMaskPointerType)
    /**
      * set the rgb image to create the transfer function
     */
    itkGetConstMacro(ImageMask, InputImageMaskPointerType)

    /**
      * set the rgb image to create the transfer function
     */
    itkSetMacro(RotatePos, unsigned int)
    /**
      * set the rgb image to create the transfer function
     */
    itkGetConstMacro(RotatePos, unsigned int)

protected:
    X2UMapRotationMatrixImageFilter();
    ~X2UMapRotationMatrixImageFilter();

private:
    typedef double  GradientPixelType;
    typedef itk::Image< GradientPixelType, TInputImage::ImageDimension > DoubleImageType;
    typedef GradientImageFilter< DoubleImageType, GradientPixelType, GradientPixelType > GradientImageFilterType;

    typedef VectorLaplacianImageFilter< typename GradientImageFilterType::OutputImageType, typename GradientImageFilterType::OutputImageType > VectorLaplacianImageFilterType;
    vector< typename VectorLaplacianImageFilterType::Pointer > m_VectorgGadientFilter;

    InputImageMaskPointerType m_ImageMask;
    unsigned int m_RotatePos;
};

}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkx2umaprotationmatriximagefilter.txx"
#endif

#endif // ITKX2UMAPROTATIONMATRIXIMAGEFILTER_H
