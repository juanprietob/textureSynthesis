#ifndef ITKHISTOGRAMMATCHINGFILTER_H
#define ITKHISTOGRAMMATCHINGFILTER_H



#include "itkImageSource.h"
#include "itkMaskedImageToHistogramFilter.h"
#include "itkImageRegionIterator.h"

#include "vtkColorTransferFunction.h"
#include "vtkSmartPointer.h"

#include "itkImageToListSampleFilter.h"

using namespace std;

namespace itk{

template<class TInputImage,class TOutputImage>
class HistogramMatchingFilter
        : public ImageSource<TOutputImage>{
public:
    /** Standard class typedefs. */
    typedef HistogramMatchingFilter         Self;
    typedef ImageSource<TOutputImage>  Superclass;
    typedef SmartPointer<Self>         Pointer;
    typedef SmartPointer<const Self>   ConstPointer;


    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(HistogramMatchingFilter,ImageSource);

    /** Superclass typedefs. */
    typedef typename Superclass::OutputImageRegionType OutputImageRegionType;
    typedef typename Superclass::OutputImagePixelType  OutputImagePixelType;

    /** Some convenient typedefs. */
    typedef TOutputImage                             OutputImageType;
    typedef typename OutputImageType::Pointer        OutputImagePointer;
    typedef typename OutputImageType::ConstPointer   OutputImageConstPointer;
    typedef typename OutputImageType::IndexType      OutputIndexType;
    typedef typename OutputImageType::PixelType      OutputPixelType;
    typedef typename OutputPixelType::ComponentType  OutputPixelComponentType;


    typedef TInputImage                             InputImageType;
    typedef typename InputImageType::Pointer        InputImagePointer;
    typedef typename InputImageType::ConstPointer   InputImageConstPointer;
    typedef typename InputImageType::RegionType     InputImageRegionType;
    typedef typename InputImageType::PixelType      InputImagePixelType;
    typedef typename InputImageType::IndexType      IndexType;
    typedef typename InputImageType::PixelType      PixelType;

    typedef ImageRegionIterator< OutputImageType > OutputImageRegionIteratorType;


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



    itkGetMacro(InputImageSource, InputImagePointer)
    itkSetMacro(InputImageSource, InputImagePointer)

    itkGetMacro(InputImageObject, OutputImagePointer)
    itkSetMacro(InputImageObject, OutputImagePointer)

    itkGetMacro(TexelDimension,const unsigned int)
    itkSetMacro(TexelDimension, unsigned int)

    /** Some convenient typedefs. */
    typedef itk::Image<unsigned char,  TOutputImage::ImageDimension >     InputImageMaskType;
    typedef typename InputImageMaskType::Pointer        InputImageMaskPointerType;
    typedef typename InputImageMaskType::ConstPointer   InputImageMaskConstPointerType;
    typedef typename InputImageMaskType::RegionType     InputImageMaskRegionType;
    typedef typename InputImageMaskType::PixelType      InputImageMaskPixelType;
    typedef typename InputImageMaskType::IndexType      InputImageMaskIndexType;

    typedef ImageRegionIterator< InputImageMaskType > InputImageMaskRegionIteratorType;

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
    itkSetMacro(CalculateOutputImage, bool)
    /**
      * set the rgb image to create the transfer function
     */
    itkGetConstMacro(CalculateOutputImage, bool)

    typedef itk::Statistics::ImageToHistogramFilter< InputImageType > HistogramGeneratorSampleType;
    typedef typename HistogramGeneratorSampleType::Pointer HistogramSamplePointerType;
    typedef typename HistogramGeneratorSampleType::HistogramType::MeasurementVectorType HistogramMeasurementVectorType;
    typedef typename HistogramGeneratorSampleType::HistogramType::IndexType HistogramIndexType;


    double GetFrequencySource(int n, HistogramMeasurementVectorType measurement, bool normalize = false);


    typedef itk::Statistics::MaskedImageToHistogramFilter< OutputImageType, InputImageMaskType  > MaskedHistogramGeneratorObjectType;
    typedef typename MaskedHistogramGeneratorObjectType::Pointer MaskedHistogramGeneratorObjectPointerType;
    typedef typename MaskedHistogramGeneratorObjectType::HistogramType::MeasurementVectorType HistogramMeasurementVectorObjectType;
    typedef typename MaskedHistogramGeneratorObjectType::HistogramType::IndexType HistogramIndexObjectType;

    double GetFrequencyObject(int n, HistogramMeasurementVectorObjectType measurement, bool normalize = false);

    /**
      * set the rgb image to create the transfer function
     */
    itkSetMacro(NBins, int)
    /**
      * set the rgb image to create the transfer function
     */
    itkGetConstMacro(NBins, int)

    /**
      * set the rgb image to create the transfer function
     */
    void UpdateFrequencyObject(HistogramMeasurementVectorObjectType measuresample, HistogramMeasurementVectorObjectType measuresample1);

protected:
    HistogramMatchingFilter();
    ~HistogramMatchingFilter();

private:
    unsigned int m_TexelDimension;
    OutputImagePointer m_InputImageObject;
    InputImagePointer m_InputImageSource;


    vector< HistogramSamplePointerType > m_HistogramSampleVect;


    typedef itk::Statistics::ImageToHistogramFilter< OutputImageType  > HistogramGeneratorObjectType;

    typedef typename HistogramGeneratorObjectType::Pointer HistogramObjectPointerType;
    vector< HistogramObjectPointerType > m_HistogramObjectVect;

    vector< vtkSmartPointer<vtkColorTransferFunction> > m_LutSampleVect;
    //vector< vtkSmartPointer< vtkLookupTable > > m_LutSampleVect;

    vector< vtkSmartPointer<vtkColorTransferFunction> > m_LutObjectVect;
    //vector< vtkSmartPointer<vtkLookupTable> > m_LutObjectVect;

    InputImageMaskPointerType m_ImageMask;

    bool m_CalculateOutputImage;

    int m_NBins;


};

}


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkhistogrammatchingfilter.txx"
#endif


#endif // ITKHISTOGRAMMATCHINGFILTER_H
