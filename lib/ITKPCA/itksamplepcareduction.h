#ifndef ITKSAMPLEPCAREDUCTION_H
#define ITKSAMPLEPCAREDUCTION_H

#include <itkListSample.h>
#include "itkVariableSizeMatrix.h"
#include <itkListSample.h>

#include "itkProcessObject.h"

namespace itk{
namespace Statistics{


    /** \class SamplePCAReduction
     * \brief This class computes the eigenvalues and eigenvectors from the covariance matrix
     * calculated from some data, the number of dimensions is preserve according to the number
     * of vectors the user wants to keep, using SetKeepValues [0, 1].
     *
     * \footnote
     *    In general, once eigenvectors are found from the covariance matrix, the next step
     *    is to order them by eigenvalue, highest to lowest. This gives you the components in
     *    order of significance. Now, if you like, you can decide to ignore the components of
     *    lesser significance. You do lose some information, but if the eigenvalues are small, you
     *    don’t lose much. If you leave out some components, the final data set will have less
     *    dimensions than the original. To be precise, if you originally have n dimensions in
     *    your data, and so you calculate eigenvectors and eigenvalues, and then you choose
     *    only the first p eigenvectors, then the final data set has only p dimensions.
     */



template< class TSample, class TValueSample = double>
class SamplePCAReduction:
    public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef SamplePCAReduction           Self;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  /** Standard Macros */
  itkTypeMacro(SamplePCAReduction, ProcessObject);
  itkNewMacro(Self);


  typedef TSample       InputSampleType;
  typedef typename InputSampleType::Pointer InputSamplePointerType;
  /** Length of a measurement vector */
  typedef typename InputSampleType::MeasurementVectorSizeType MeasurementVectorSizeType;

  /** Measurement vector type */
  typedef typename InputSampleType::MeasurementVectorType MeasurementVectorType;


  itkSetMacro(InputSample, InputSamplePointerType)
  itkGetMacro(InputSample, InputSamplePointerType)


  typedef itk::VariableLengthVector< TValueSample >                     MeasurementVectorSampleType;
  typedef itk::Statistics::ListSample< MeasurementVectorSampleType >    OutputSampleType;
  typedef typename OutputSampleType::MeasurementVectorType              OutMeasurementVectorType;

  /** Returns the Reduced sample by PCA of the target sample data */
  typedef typename OutputSampleType::Pointer OutputSamplePointerType;
  itkGetMacro(Output, OutputSamplePointerType)



  /**
   * Keep the percentage of eigenvalues to preserve variability
   * default 95 percent. Dimensionality of the dataset is reduced
   * for a dataset of 192 dimensions, it usually is reduced to 10-30 dimensions
   * value between [0, 1].
   */
  itkSetMacro(KeepValues, double)
  itkGetMacro(KeepValues, double)

  /**
   * feature vector (array of eigen vectors or matrix) to keep the coeficients calculated from the covariance matrix and the eigen analysis
   * the number of features is preserved from the KeepValues parameter
   */

  typedef typename itk::VariableSizeMatrix< double >  MatrixType;
  itkGetMacro(FeatureVector, MatrixType)

  /**
   *    Multiplies samplevector with the current m_FeatureVector if it exists
   *    reduces the dimensionality of the vector
   */
  inline vnl_vector<double> GetPCAReducedVector(const MeasurementVectorType vector);

  /**
   *    Given an index converts the pca vector in m_Output into the original vector
   *    OriginalData =  (m_FeatureVector ^(-1) x FinalData) + OriginalMean
   */
  inline vnl_vector<double> GetOriginalMeasurementVector(const int index);

  /**
   *    Given a vector converts the pca vector in m_Output into the original vector
   *    OriginalData =  (m_FeatureVector ^(-1) x FinalData) + OriginalMean
   */
  inline vnl_vector<double> GetOriginalMeasurementVector(vnl_vector<double> vector);


  inline vnl_vector<double> GetMeasurementVector(const int index);

  virtual void Update();

protected:
  SamplePCAReduction();
  virtual ~SamplePCAReduction();
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Calculates the covariance and save it. This method calls
   * ComputeCovarianceWithGivenMean, if the user provides mean vector
   * using SetMean method. Otherwise, it calls
   * ComputeCovarianceWithoutGivenMethod depending on */
  void GenerateData( void );


private:
  OutputSamplePointerType m_Output;
  vnl_matrix<double> m_finaldata;

  double m_KeepValues;
  MatrixType m_FeatureVector;  

  vnl_vector<double> m_MeanArray;

  InputSamplePointerType m_InputSample;

}; // end of class

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#ifndef ITKSAMPLEPCAREDUCTION_TXX
#include "itksamplepcareduction.txx"
#endif
#endif

#endif // ITKSAMPLEPCAREDUCTION_H
