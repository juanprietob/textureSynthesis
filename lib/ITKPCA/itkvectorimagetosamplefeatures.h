#ifndef ITKVECTORIMAGETOSAMPLEFEATURES_H
#define ITKVECTORIMAGETOSAMPLEFEATURES_H

#include "itkProcessObject.h"
#include "itkVariableLengthVector.h"
#include "itkListSample.h"
#include "itkDataObject.h"
#include "itkDataObjectDecorator.h"

namespace itk {
namespace Statistics {

/** \class VectorImageToSampleFeatures
 *  \brief The class takes an rgb image and generates a list sample as
 *  output from the neighborhood size set as parameter.
 *
 *  It takes every rgb value in the neighborhood and generates a list
 *  for a 8x8 neighborhood it generates a list with 192 values
 *
 */
template < class TImage,class TListValue = typename TImage::PixelType::ValueType >
class VectorImageToSampleFeatures :
public ProcessObject
{
public:
    /** Standard class typedefs */
    typedef VectorImageToSampleFeatures        Self;
    typedef ProcessObject               Superclass;
    typedef SmartPointer< Self >        Pointer;
    typedef SmartPointer<const Self>    ConstPointer;

    /** Run-time type information (and related methods). */
    itkTypeMacro(VectorImageToSampleFeatures, ProcessObject);

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Image typedefs */
    typedef TImage                           ImageType;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::PixelType    PixelType;
    typedef typename PixelType::ValueType    PixelValueType;

    typedef TListValue               SamplePixelType;
    typedef VariableLengthVector< SamplePixelType >       MeasurementVectorType;

   /** Type of the output list sample */
   typedef ListSample< MeasurementVectorType >  ListSampleType;

   /** Standard itk::ProcessObject subclass method. */

   /** ListSample is not a DataObject, we need to decorate it to push it down
    * a ProcessObject's pipeline */
   typedef DataObjectDecorator< ListSampleType >  ListSampleOutputType;

   /**
     data object pointer for the output
    */
   typedef DataObject::Pointer DataObjectPointer;

    /**
    *  generate the output
    */
   virtual DataObjectPointer MakeOutput(unsigned int idx);


   /**
     * generate the data
    */
    virtual void GenerateData();


    virtual void GenerateOutputInformation();

    /** Method to get the list sample, the generated output. Note that this does
    * not invoke Update(). You should have called update on this class to get
    * any meaningful output. */
    const ListSampleType * GetListSample() const;


    /** Set the "outside" pixel value. The default value
     * NumericTraits<PixelType>::Zero. */
    itkSetMacro(Input,ImagePointer)

    /** Get the "Position to compare with the object */
    itkGetConstMacro(Input, ImagePointer)

    /** Set the "outside" pixel value. The default value
     * NumericTraits<PixelType>::Zero. */
    itkSetMacro(NeighborhoodSize,unsigned int);

    /** Get the "Position to compare with the object */
    itkGetConstMacro(NeighborhoodSize,unsigned int);    

    /** Set the "outside" pixel value. The default value
     * NumericTraits<PixelType>::Zero. */
    itkSetMacro(StepSize,unsigned int);

    /** Get the "Position to compare with the object */
    itkGetConstMacro(StepSize,unsigned int);


    /** Set the number of channels of the texture
     * NumericTraits<PixelType>::Zero. */
    itkSetMacro(TexelDimension,unsigned int);
    /** Set the number of channels of the texture
     * NumericTraits<PixelType>::Zero. */
    itkGetMacro(TexelDimension,unsigned int);

    virtual void Update();

protected:
    VectorImageToSampleFeatures();
    ~VectorImageToSampleFeatures();

    //ListSampleType* m_ListSample;
    ImagePointer m_Input;
    unsigned int m_NeighborhoodSize;
    unsigned int m_StepSize;
    unsigned int m_TexelDimension;
};

}//itk
}//statistics

#ifndef ITK_MANUAL_INSTANTIATION
#ifndef ITKVECTORIMAGETOSAMPLEFEATURES_TXX
#include "itkvectorimagetosamplefeatures.txx"
#endif
#endif

#endif // ITKVECTORIMAGETOSAMPLEFEATURES_H
