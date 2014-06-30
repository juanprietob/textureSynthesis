#ifndef ITKOPTIMIZATIONPHASE_H
#define ITKOPTIMIZATIONPHASE_H

#include "itkImageSource.h"
#include "itksamplepcareduction.h"

#include "itkImageRegionIterator.h"

#include "itkVectorImage.h"

#include "itkListSample.h"
#include "itkSampleMeanShiftClusteringFilter.h"

#include "itkvectorimagetosamplefeatures.h"

#include "itkhistogrammatchingfilter.h"

#include "itkImageRandomNonRepeatingIteratorWithIndex.h"



namespace itk{

template<   class TInputImage,
            class TInputImageSample = itk::Image< itk::RGBPixel< unsigned char >, 2 >,
            class TOutputImage = TInputImage,
            class TInputArrayIndexImage = itk::Image< Array< int >, TInputImage::ImageDimension >,//when rotations
            class TX2UImageMask = Image< unsigned char, TInputImage::ImageDimension >
        >
class OptimizationPhase
    : public ImageSource<TOutputImage>{

public:
  /** Standard class typedefs. */
  typedef OptimizationPhase         Self;
  typedef ImageSource<TInputImage>  Superclass;
  typedef SmartPointer<Self>         Pointer;
  typedef SmartPointer<const Self>   ConstPointer;


  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(OptimizationPhase,ImageSource);

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


  typedef TInputImageSample                             InputImageSampleType;
  typedef typename InputImageSampleType::Pointer        InputImageSamplePointer;
  typedef typename InputImageSampleType::ConstPointer   InputImageSampleConstPointer;
  typedef typename InputImageSampleType::RegionType     InputImageSampleRegionType;
  typedef typename InputImageSampleType::PixelType      InputImageSamplePixelType;
  typedef typename InputImageSampleType::IndexType      IndexSampleType;
  typedef typename InputImageSampleType::PixelType      PixelSampleType;


  typedef TInputArrayIndexImage                              InputArrayIndexImageType;
  typedef typename InputArrayIndexImageType::Pointer         InputArrayIndexImagePointerType;
  typedef typename InputArrayIndexImageType::ConstPointer    InputArrayIndexImageConstPointerType;
  typedef typename InputArrayIndexImageType::RegionType      InputArrayIndexImageRegionType;
  typedef typename InputArrayIndexImageType::PixelType       InputArrayIndexImagePixelType;
  typedef typename InputArrayIndexImageType::IndexType       InputArrayIndexType;

  itkSetMacro(ArrayIndexImage, InputArrayIndexImagePointerType);
  itkGetMacro(ArrayIndexImage, const InputArrayIndexImagePointerType);


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



  /** Set the "outside" pixel value. The default value
   * NumericTraits<PixelType>::Zero. */
  itkSetMacro(Input,InputImagePointer);

  /** Get the "Position to compare with the object */
  itkGetConstMacro(Input,InputImagePointer);


  typedef vector< InputImageSamplePointer > VectorImageSamplesType;
  /** Set the samples of the image the images are used to create the histograms
   * NumericTraits<PixelType>::Zero. */
  itkSetObjectMacro(InputSamples,VectorImageSamplesType);

  /** Get the "Position to compare with the object */
  itkGetConstMacro(InputSamples,VectorImageSamplesType*);

  /**
   * Additional typedefs
   */
  /**
   *  Vectors to hold the textures for each neighborhood
   */
  typedef vector< vnl_vector<double> > TexturesVectorsType;
  /**
   *  Weights of each neighborhood
   */
  typedef vector< double > WeightsVectorsType;

  typedef vector< int > SampleIndexVectorsType;

  typedef vector< IndexType >     ImageIndexVectorType;


  itkSetObjectMacro(Wp, WeightsVectorsType);
  itkGetConstMacro(Wp, WeightsVectorsType*);

  itkSetObjectMacro(EvSampleIndex, SampleIndexVectorsType);
  itkGetConstMacro(EvSampleIndex, SampleIndexVectorsType*);

  itkSetMacro(NeighborhoodSize, unsigned int);
  itkSetMacro(TexelDimension, unsigned int);
  itkSetMacro(ClusterThreshold, double );


  typedef itk::Statistics::VectorImageToSampleFeatures< InputImageType > ImageFeaturesType;
  typedef typename ImageFeaturesType::ListSampleType ListSampleType;
  typedef itk::Statistics::SamplePCAReduction< ListSampleType > PCAReductionType;
  typedef typename PCAReductionType::Pointer PCAReductionPointerType;
  typedef vector< ListSampleType* > VectorListSampleType;

  itkSetObjectMacro(ColorSamples, VectorListSampleType);


  typedef typename itk::NeighborhoodIterator< InputImageType > NeighborhoodIteratorType;

  typedef typename itk::ImageRegionIterator< InputImageType > RegionIteratorInputType;

  typedef typename NeighborhoodIteratorType::OffsetType OffsetType;

  typedef vector<  vector< OffsetType > > VectorOffsetType;



  typedef typename itk::NeighborhoodIterator< OutputImageType > NeighborhoodIteratorOutputType;



  typedef typename itk::ImageRegionIterator< OutputImageType > RegionIteratorOutputType;
  typedef typename itk::ImageRandomNonRepeatingIteratorWithIndex< OutputImageType > RandomIteratorType;


  typedef itk::Image< Vector< int, 4 >, InputImageType::ImageDimension >  InputIndexImageType;
  typedef typename InputIndexImageType::PixelType           InputIndexImagePixelType;
  typedef typename InputIndexImageType::Pointer             InputIndexImagePointerType;
  typedef typename InputIndexImageType::IndexType           InputIndexImageIndexType;
  typedef typename itk::NeighborhoodIterator< InputIndexImageType > NeighborhoodIteratorInputIndexImageType;

  itkSetMacro(InputIndexImage, InputIndexImagePointerType)

  typedef itk::VariableLengthVector< double >                SampleVectorType;
  typedef itk::Statistics::ListSample< SampleVectorType >    ListSampleDoubleType;

  typedef itk::Statistics::SampleMeanShiftClusteringFilter< ListSampleDoubleType >  SampleMeanShiftType;


  typedef TX2UImageMask                                   X2UImageMaskType;
  typedef typename X2UImageMaskType::Pointer              X2UImageMaskPointerType;
  typedef typename X2UImageMaskType::RegionType           X2UImageMaskRegionType;
  typedef typename X2UImageMaskType::IndexType            X2UImageMaskIndexType;
  typedef typename X2UImageMaskType::PixelType            X2UImageMaskPixelType;


  /** Set the filename of the rotation image
   */
  itkSetMacro(X2UImageMask,X2UImageMaskPointerType);

  /** Get the texel dimension */
  itkGetConstMacro(X2UImageMask,const X2UImageMaskPointerType);

protected:
    OptimizationPhase();
    ~OptimizationPhase();
private:

    unsigned int m_NeighborhoodSize;
    unsigned int m_NeighborhoodSizOld;
    unsigned int m_TexelDimension;    
    double  m_ClusterThreshold;    

    InputImagePointer   m_Input;
    InputIndexImagePointerType m_InputIndexImage;

    VectorImageSamplesType* m_InputSamples;

    WeightsVectorsType* m_Wp;

    SampleIndexVectorsType* m_EvSampleIndex;

    VectorOffsetType m_OffsetVector;

    /**
      * sample of texels
      */    
    VectorListSampleType* m_ColorSamples;

    InputImageRegionType m_ImageRegion;

    /*typedef itk::Statistics::ImageToHistogramGenerator< InputImageSampleType > HistogramGeneratorSampleType;
    typedef typename HistogramGeneratorSampleType::Pointer HistogramSamplePointerType;
    vector< HistogramSamplePointerType > m_HistogramSampleVect;

    typedef itk::Statistics::ImageToHistogramGenerator< InputImageType  > HistogramGeneratorObjectType;
    typedef typename HistogramGeneratorObjectType::Pointer HistogramObjectPointerType;
    typedef typename HistogramGeneratorObjectType::HistogramType HistogramObjectType;
    vector< HistogramObjectType* > m_HistogramObjectVect;
    vector< HistogramObjectPointerType > m_HistogramObjectVectTemp;*/

    InputArrayIndexImagePointerType m_ArrayIndexImage;

    X2UImageMaskPointerType m_X2UImageMask;

    typedef itk::HistogramMatchingFilter<InputImageSampleType, InputImageType > HistogramMatchingType;
    typename HistogramMatchingType::Pointer m_HistogramMatch;
    typedef typename HistogramMatchingType::HistogramMeasurementVectorType MeasurementVectorType;
    typedef typename HistogramMatchingType::HistogramMeasurementVectorObjectType MeasurementVectorObjectType;
};


}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkoptimizationphase.txx"
#endif


#endif // ITKOPTIMIZATIONPHASE_H
