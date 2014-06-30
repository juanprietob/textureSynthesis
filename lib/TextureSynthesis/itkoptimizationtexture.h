#ifndef OPTIMIZATIONTEXTURE_H
#define OPTIMIZATIONTEXTURE_H

#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"

#include "itksampleannconnector.h"
#include "itkvectorimagetosamplefeatures.h"
#include "itksamplepcareduction.h"
//#include <itkvectorimagemeanshiftclusteringfilter.h>
#include "itksearchphase.h"
#include "itkoptimizationphase.h"

#include "itkVectorImage.h"
#include "itkListSample.h"

#include "itkx2umaprotationmatriximagefilter.h"

namespace itk{

template <  class TImageSample,
            class TImageObject = TImageSample,
            class TX2UImage = Image<Vector<unsigned char, 3>, TImageObject::ImageDimension >
         >
class ITK_EXPORT OptimizationTexture
: public ImageToImageFilter<TImageObject,TImageObject>
{
public:
    /** Standard class typedefs. */
    typedef OptimizationTexture             Self;
    typedef ImageToImageFilter<TImageObject,TImageObject>      Superclass;
    typedef SmartPointer<Self>                                Pointer;
    typedef SmartPointer<const Self>                          ConstPointer;


    /** Pixel Type of the input image */
    typedef TImageSample                                       InputImageSampleType;
    typedef typename InputImageSampleType::PixelType           InputImageSamplePixelType;
    typedef typename InputImageSampleType::Pointer             InputImageSamplePointerType;
    typedef typename InputImageSampleType::RegionType          InputImageSampleRegionType;
    typedef typename InputImageSampleType::IndexType           InputimageSampleIndexType;


    /** Pixel Type of the input image */
    typedef TImageObject                                        InputImageObjectType;
    typedef typename InputImageObjectType::Pointer              InputImageObjectPointerType;
    typedef typename InputImageObjectType::RegionType           InputImageObjectRegionType;
    typedef typename InputImageObjectType::IndexType            InputImageObjectIndexType;
    typedef typename InputImageObjectType::PixelType            InputImageObjectPixelType;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(OptimizationTexture, ImageToImageFilter);


    /** Some additional typedefs.  */


    /** Some additional typedefs.  */
    typedef typename Superclass::OutputImageType                OutputImageObjectType;
    typedef typename OutputImageObjectType::Pointer             OutputImageObjectPointerType;
    typedef typename OutputImageObjectType::RegionType          OutputImageObjectRegionType;
    typedef typename OutputImageObjectType::PixelType           OutputImageObjectPixelType;

    typedef typename itk::ImageRegionIterator<  OutputImageObjectType > RegionIteratorType;
    typedef typename itk::NeighborhoodIterator< OutputImageObjectType > NeighborhoodIteratorType;


    /** Set the input image samples
     * */
    typedef vector< InputImageSamplePointerType > VectorImageSamples;
    itkSetObjectMacro(InputImageSamples, VectorImageSamples);

    /** Get the "Position to compare with the object */
    itkGetConstMacro(InputImageSamples, VectorImageSamples*);

    /** Set the "outside" pixel value. The default value
     * NumericTraits<PixelType>::Zero. */
    typedef vector< InputImageSamplePointerType > VectorImageFeatureSamples;
    itkSetObjectMacro(InputImageFeatureSamples, VectorImageFeatureSamples);

    /** Get the "Position to compare with the object */
    itkGetConstMacro(InputImageFeatureSamples, VectorImageFeatureSamples*)


    /** Set the texel dimension the default is 3 for RGB textures
     * NumericTraits<PixelType>::Zero. */
    itkSetMacro(TexelDimension,unsigned int);

    /** Get the texel dimension */
    itkGetConstMacro(TexelDimension,unsigned int);

    /** Set the exponent to make the optimization more robust against auliers [Kwatra et al. 2005]
      * default 0.8
     * NumericTraits<PixelType>::Zero. */
    itkSetMacro(R,double);

    /** Get the texel dimension */
    itkGetConstMacro(R,double);


    /** Set the filename to save image during the progress
     */
    itkSetMacro(Filename,char*);

    /** Get the texel dimension */
    itkGetConstMacro(Filename,const char*);

    /** Set the filename to save image during the progress
     */
    itkSetMacro(Extension,char*);

    /** Get the texel dimension */
    itkGetConstMacro(Extension,const char*);

    /** Set the filename to save image during the progress
     */
    itkSetMacro(SmoothHistogram,bool);

    /** Get the texel dimension */
    itkGetConstMacro(SmoothHistogram,bool);

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


    typedef TX2UImage                                   X2UImageType;
    typedef typename X2UImageType::Pointer              X2UImagePointerType;
    typedef typename X2UImageType::RegionType           X2UImageRegionType;
    typedef typename X2UImageType::IndexType            X2UImageIndexType;
    typedef typename X2UImageType::PixelType            X2UImagePixelType;


    /** Set the filename of the rotation image
     */
    itkSetMacro(X2UImage,X2UImagePointerType);

    /** Get the texel dimension */
    itkGetConstMacro(X2UImage,const X2UImagePointerType);

protected:
        OptimizationTexture();
        ~OptimizationTexture(){}

        //InputImageSamplePointerType m_InputImageSample;
        VectorImageSamples* m_InputImageSamples;
        VectorImageSamples* m_InputImageFeatureSamples;

        unsigned int m_TexelDimension;

        double m_R;

        virtual void GenerateData();

        bool m_SearchZ;
        bool m_SearchY;
        bool m_SearchX;
private:

        /**
         * typedefinitions
        */
        typedef itk::Statistics::VectorImageToSampleFeatures< InputImageSampleType > ImageFeaturesType;
        typedef typename ImageFeaturesType::ListSampleType ListSampleType;

        typedef itk::Statistics::SamplePCAReduction< ListSampleType > PCAReductionType;
        typedef typename  PCAReductionType::OutputSampleType ListSamplePCAType;

        typedef itk::Statistics::SampleANNConnector< ListSamplePCAType > SampleANNConnectorType;
        //typedef itk::Statistics::VectorImageMeanShiftClusteringFilter< InputImageSampleType > VectorImageClusteringType;
        typedef itk::SearchPhase< InputImageObjectType > SearchPhaseType;
        typename SearchPhaseType::Pointer m_SearchPhase;
        typedef typename SearchPhaseType::RotationImageType RotationImageType;
        typedef typename RotationImageType::Pointer RotationImagePointerType;

        typedef itk::X2UMapRotationMatrixImageFilter< X2UImageType > X2UMapRotationMatrixImageFilterType;
        typedef typename X2UMapRotationMatrixImageFilterType::Pointer X2UMapRotationMatrixImageFilterPointerType;
        typedef typename X2UMapRotationMatrixImageFilterType::InputImageMaskType InputImageMaskType;
        typedef typename InputImageMaskType::Pointer InputImageMaskPointerType;

        X2UImagePointerType m_X2UImage;

        typedef itk::OptimizationPhase< InputImageObjectType,
                                        InputImageSampleType,
                                        InputImageObjectType,
                                        typename SearchPhaseType::ArrayIndexImageType,
                                        typename X2UMapRotationMatrixImageFilterType::InputImageMaskType
                                      > OptimizationPhaseType;

        typename OptimizationPhaseType::Pointer m_Optimization;

        /**
         *  Vectors to hold the textures for each neighborhood
         */
        typedef vector< vnl_vector<double>  > TexturesVectorsType;
        /**
         *  Weights of each neighborhood
         */
        typedef vector< double > WeightsVectorsType;

        TexturesVectorsType m_Ev;
        WeightsVectorsType  m_Wp;

        typedef vector< int > SampleIndexVectorsType;
        SampleIndexVectorsType m_EvSampleIndex;        

        typedef typename SampleANNConnectorType::Pointer SampleAnnConnectorPointerType;
        /**
         *  ann conector pointers
         *  this object performs the highdimensional search
         */
        vector< SampleAnnConnectorPointerType > m_ANNConnectors;

        /**
          Vector that holds the original color sample vectors
          **/
        vector< ListSampleType *> m_ColorSamples;
        unsigned int m_ColorSampleSize;


        /**
         * PCA reduction pointers
         * Perform the dimensionality reduction of the sample vector feature space
         */
        typedef typename PCAReductionType::Pointer PCAReductionPointerType;        
        vector< PCAReductionPointerType > m_PCAReductions;

        /**
          * Neighborhood to perform the optimization
        */
        unsigned int m_NeighborhoodSize;
        /**
         * Number of steps to move inside the image to perform the closest neighborhood search
         */
        unsigned int m_StepSize;


        /**
         *  Filename to save the image during the progress
         */
        const char* m_Filename;
        const char* m_Extension;        

        bool m_SmoothHistogram;




        /////METHODS

        /**
         * optimization algorithm to find the texture at multiple neighborhoods
         * Uses multithread
         */
        void optimizationPhaseMultiThread(
                    InputImageObjectPointerType& imageobject,
                    VectorImageSamples* imagesample,
                    double clusterthreshold = 500
                );
        /**
         * Search the ANN space for the closest neighborhoods in the exemplar
         * Uses multithread and a modification of ann lib
         * @returns bool if the current search is equal to the previous one
         *          this usually means the current level has converged
         */
        double searchPhase(InputImageObjectPointerType imageobject, InputImageSamplePointerType imagesample, bool random = false, RotationImagePointerType rotationimage = 0);

        /**
         *  Setup the closest neighborhood space of the vector of imagesample
         */
        void setupANN(vector< InputImageSamplePointerType >*);


};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkoptimizationtexture.txx"
#endif

#endif // OPTIMIZATIONTEXTURE_H
