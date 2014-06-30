#ifndef ITKRANDOMIMAGEGENERATOR_H
#define ITKRANDOMIMAGEGENERATOR_H

#include "itkImageToImageFilter.h"
#include "itkImageRandomNonRepeatingConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkImageSliceIteratorWithIndex.h"

namespace itk
{

/** \class Random image generator
 * \brief Computes the object image with random pixels chosen from the sample image.
 *        The sample image is set with SetInput(sampleimage) and the object image
 *        wich is considered to have pixels greater than 0 for the object and 0 for the
 *       background.
 *
 * This filter is implemented using the random iterator for the sample image and
 * ImageToImageFilter
 *
 *
 * \ingroup Singlethreaded
 */
template <class TInputImage, class TOutputImage= TInputImage >
class ITK_EXPORT RandomImageGenerator :
    public ImageToImageFilter<TOutputImage,TOutputImage>
{
public:
    /** Standard class typedefs. */
    typedef RandomImageGenerator             Self;
    typedef ImageToImageFilter<TInputImage,TOutputImage>      Superclass;
    typedef SmartPointer<Self>                                Pointer;
    typedef SmartPointer<const Self>                          ConstPointer;


    /** Pixel Type of the input image */
    typedef TInputImage                                       InputImageType;
    typedef TOutputImage                                      OutputImageType;
    typedef typename TInputImage::PixelType                   PixelType;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(RandomImageGenerator, ImageToImageFilter);


    /** Some additional typedefs.  */    
    typedef typename InputImageType::Pointer      InputImagePointer;
    typedef typename InputImageType::RegionType   InputImageRegionType;
    typedef typename InputImageType::PixelType    InputImagePixelType;
    typedef typename InputImageType::IndexType    InputImageIndexType;

    /** Some additional typedefs.  */
    typedef typename OutputImageType::Pointer     OutputImagePointer;
    typedef typename OutputImageType::RegionType  OutputImageRegionType;
    typedef typename OutputImageType::PixelType   OutputImagePixelType;
    typedef typename OutputImageType::IndexType   OutputImageIndexType;

    /** Iterators def**/    
    typedef typename itk::ImageRandomNonRepeatingConstIteratorWithIndex< InputImageType >  RandomConstIteratorType;
    typedef typename itk::ImageSliceIteratorWithIndex< InputImageType > InputSliceIteratorType;
    typedef typename itk::NeighborhoodIterator< InputImageType > InputNeighborhoodIteratorType;
    typedef typename itk::ImageRegionIterator< InputImageType > InputRegionIteratorType;

    typedef typename itk::ImageSliceIteratorWithIndex< OutputImageType > OutputSliceIteratorType;
    typedef typename itk::NeighborhoodIterator< OutputImageType > OutputNeighborhoodIteratorType;
    typedef typename itk::ImageRegionIterator < OutputImageType > OutputRegionIterator;




    /** Set the "outside" pixel value. The default value
     * NumericTraits<PixelType>::Zero. */
    itkSetMacro(InputImageSample,InputImagePointer);

    /** Get the "Position to compare with the object */
    itkGetConstMacro(InputImageSample,InputImagePointer);

    /** Set the "outside" pixel value. The default value
     * NumericTraits<PixelType>::Zero. */
    itkSetMacro(PositionValue,unsigned int);

    /** Get the "position to compare with the object" */
    itkGetConstMacro(PositionValue,unsigned int);

    /** Set the "outside" pixel value. The default value
     * NumericTraits<PixelType>::Zero. */
    itkSetMacro(SamplesNumber,unsigned int);

    /** Get the "position to compare with the object" */
    itkGetConstMacro(SamplesNumber,unsigned int);    



protected:
    RandomImageGenerator();
    ~RandomImageGenerator(){};

    virtual void GenerateData();
private:
    InputImagePointer m_InputImageSample;
    unsigned int m_PositionValue;
    unsigned int m_SamplesNumber;
    RandomImageGenerator(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    /**
      * Neighborhood to perform the optimization
    */
    unsigned int m_NeighborhoodSize;    

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkrandomimagegenerator.txx"
#endif

#endif // ITKRANDOMIMAGEGENERATOR_H
