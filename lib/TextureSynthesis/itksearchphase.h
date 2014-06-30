#ifndef ITKSEARCHPHASE_H
#define ITKSEARCHPHASE_H

#include "itkImageSource.h"

#include "itksampleannconnector.h"
#include "itksamplepcareduction.h"

#include "itkImageRegionIteratorWithIndex.h"
#include "itkvectorimagetosamplefeatures.h"

namespace itk{

template< class TInputImage, class TOutput = itk::Image<Vector< int, 4 >, TInputImage::ImageDimension > >
class SearchPhase
    : public ImageSource< TOutput >{

public:
  /** Standard class typedefs. */
  typedef SearchPhase         Self;
  typedef ImageSource<TInputImage>  Superclass;
  typedef SmartPointer<Self>         Pointer;
  typedef SmartPointer<const Self>   ConstPointer;


  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SearchPhase,ImageSource);

  /** Superclass typedefs. */
  typedef TOutput                                       OutputImageType;
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
  //virtual void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId );
  virtual void GenerateData();


  /** The GenerateData method normally allocates the buffers for all of the
   * outputs of a filter. Some filters may want to override this default
   * behavior. For example, a filter may have multiple outputs with
   * varying resolution. Or a filter may want to process data in place by
   * grafting its input to its output. */
  //virtual void AllocateOutputs();
  virtual void GenerateOutputInformation();

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
  //virtual void BeforeThreadedGenerateData();

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
  //virtual void AfterThreadedGenerateData();



  /** Set the "outside" pixel value. The default value
   * NumericTraits<PixelType>::Zero. */
  itkSetMacro(Input,InputImagePointer);

  /** Get the "Position to compare with the object */
  itkGetConstMacro(Input,InputImagePointer);  


  /**
   * Additional typedefs
   */
  /**
   *  Vectors to hold the textures for each neighborhood
   */
  typedef vector< vnl_vector< double >  > TexturesVectorsType;
  /**
   *  Weights of each neighborhood
   */
  typedef vector< double > WeightsVectorsType;

  typedef vector< int > SampleIndexVectorsType;

  typedef vector< IndexType >     ImageIndexVectorType;



  //itkSetObjectMacro(Ev, TexturesVectorsType);
  //itkGetConstMacro(Ev, TexturesVectorsType*);

  //itkSetObjectMacro(Sv, TexturesVectorsType);
  //itkGetConstMacro(Sv, TexturesVectorsType*);

  itkSetObjectMacro(Wp, WeightsVectorsType);
  itkGetConstMacro(Wp, WeightsVectorsType*);

  itkSetObjectMacro(EvSampleIndex, SampleIndexVectorsType);
  itkGetConstMacro(EvSampleIndex, SampleIndexVectorsType*);

  //itkSetObjectMacro(EvImageIndex, ImageIndexVectorType);
  //itkGetConstMacro(EvImageIndex, ImageIndexVectorType*);


  itkSetMacro(NeighborhoodSize, unsigned int);
  itkSetMacro(TexelDimension, unsigned int);

  itkSetMacro(StepSize, unsigned int);

  itkSetMacro(SearchState, bool);
  itkGetMacro(SearchState, bool);

  itkSetMacro(Energy, double);
  itkGetMacro(Energy, double);

  typedef itk::Statistics::VectorImageToSampleFeatures< InputImageType > ImageFeaturesType;
  typedef typename ImageFeaturesType::ListSampleType ListSampleType;

  typedef itk::Statistics::SamplePCAReduction< ListSampleType > PCAReductionType;
  typedef typename  PCAReductionType::OutputSampleType ListSamplePCAType;
  typedef typename PCAReductionType::Pointer PCAReductionPointerType;


  typedef itk::Statistics::SampleANNConnector< ListSamplePCAType > SampleANNConnectorType;
  typedef typename SampleANNConnectorType::Pointer SampleANNConnectorPointerType;

  typedef VariableLengthVector< typename PixelType::ComponentType >       MeasurementVectorDoubleType;
  typedef itk::Statistics::ListSample< MeasurementVectorDoubleType >  ListSampleCharType;


  //itkSetMacro(ColorSample, ListSampleType*);
  itkSetMacro(ColorSampleSize, unsigned int);
  //itkSetMacro(PCAReduction, PCAReductionPointerType );
  //itkSetMacro(ANNConnector, SampleANNConnectorPointerType);

  itkSetObjectMacro(PCAReductions, vector< PCAReductionPointerType > );
  itkSetObjectMacro(ANNConnectors, vector< SampleANNConnectorPointerType >);



  typedef typename itk::NeighborhoodIterator< InputImageType > NeighborhoodIteratorType;

  typedef typename NeighborhoodIteratorType::OffsetType OffsetType;

  typedef vector<  vector< OffsetType > > VectorOffsetType;


  itkSetMacro(RandomSearch, bool);
  itkGetMacro(RandomSearch, bool);

  /**
    Set and get macro for searching for planes perpendicular to Z direction
    **/
  itkSetMacro(SearchZ, bool);
  itkGetMacro(SearchZ, bool);

  /**
    Set and get macro for searching for planes perpendicular to Y direction
    **/
  itkSetMacro(SearchY, bool);
  itkGetMacro(SearchY, bool);

  /**
    Set and get macro for searching for planes perpendicular to X direction
    **/
  itkSetMacro(SearchX, bool);
  itkGetMacro(SearchX, bool);



  typedef itk::Image< Matrix< double, InputImageType::ImageDimension, InputImageType::ImageDimension >, InputImageType::ImageDimension > RotationImageType;
  typedef typename RotationImageType::Pointer        RotationImagePointerType;
  typedef typename RotationImageType::ConstPointer   RotationImageConstPointer;
  typedef typename RotationImageType::RegionType     RotationImageRegionType;
  typedef typename RotationImageType::PixelType      RotationImagePixelType;

  /**
    Set and get macro for searching for planes perpendicular to X direction
    **/
  itkSetMacro(RotationImage, RotationImagePointerType);
  itkGetMacro(RotationImage, const RotationImagePointerType);

  static const int MAXVECT = 51;//size in pos 0, 25 values for 1 dimension, sample index and position in the sample
  typedef itk::Image< Matrix<int , 3, MAXVECT >, InputImageType::ImageDimension > ArrayIndexImageType;
  typedef typename ArrayIndexImageType::Pointer        ArrayIndexImagePointerType;
  typedef typename ArrayIndexImageType::ConstPointer   ArrayIndexImageConstPointer;
  typedef typename ArrayIndexImageType::RegionType     ArrayIndexImageRegionType;
  typedef typename ArrayIndexImageType::PixelType      ArrayIndexImagePixelType;

  itkSetMacro(ArrayIndexImage, ArrayIndexImagePointerType);
  itkGetMacro(ArrayIndexImage, const ArrayIndexImagePointerType);

protected:
    SearchPhase();
    ~SearchPhase();


  private:

      unsigned int m_NeighborhoodSize;
      unsigned int m_NeighborhoodSizOld;
      unsigned int m_TexelDimension;
      unsigned int m_iterationsize;
      unsigned int m_ColorSampleSize;
      unsigned int m_StepSize;
      unsigned int m_StepSizeOld;
      bool m_SearchState;
      double m_Energy;
      bool m_RandomSearch;
      /* searching for direction perpendicular to Z Y or X*/
      bool m_SearchZ;
      bool m_SearchY;
      bool m_SearchX;

      InputImagePointer   m_Input;      

      //TexturesVectorsType* m_Ev;
      WeightsVectorsType*  m_Wp;      

      SampleIndexVectorsType* m_EvSampleIndex;      

      VectorOffsetType m_OffsetVector;


      /**
       *  ann conector pointer
       */
      //SampleANNConnectorPointerType m_ANNConnector;
      vector< SampleANNConnectorPointerType >* m_ANNConnectors;
      /**
       * image feature pointer, this contains the list with the vectorized versions of the neighborhoods
       * in the texture sample, the output is the set to m_Colorsample
       */
      typename ImageFeaturesType::Pointer m_ImageFeatures;
      /**
        * sample of texels
        */
      //const ListSampleType *m_ColorSample;


      /**
       * PCA reduction pointer
       */
      //PCAReductionPointerType m_PCAReduction;
      vector< PCAReductionPointerType >* m_PCAReductions;


      InputImageRegionType m_ImageRegion;

      RotationImagePointerType m_RotationImage;
      ArrayIndexImagePointerType m_ArrayIndexImage;

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itksearchphase.txx"
#endif

#endif // ITKSEARCHPHASE_H
