#ifndef ITKX2UMAPROTATIONMATRIXIMAGEFILTER_TXX
#define ITKX2UMAPROTATIONMATRIXIMAGEFILTER_TXX


#include "itkVectorImage.h"
#include "itkVectorImageToImageAdaptor.h"
#include "itkCastImageFilter.h"
#include <itkNearestNeighborInterpolateImageFunction.h>

#include "itkLaplacianImageFilter.h"

#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_rotation_matrix.h>

namespace itk{

template< class TInputImage, class TOutputImage >
X2UMapRotationMatrixImageFilter< TInputImage, TOutputImage >::X2UMapRotationMatrixImageFilter()
{
    m_RotatePos = 0;
}

template< class TInputImage, class TOutputImage >
X2UMapRotationMatrixImageFilter< TInputImage, TOutputImage >::~X2UMapRotationMatrixImageFilter()
{
}



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


template< class TInputImage, class TOutputImage >
void X2UMapRotationMatrixImageFilter< TInputImage, TOutputImage >::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId ){


    OutputRegionIteratorType outimageit(this->GetOutput(), outputRegionForThread);
    outimageit.GoToBegin();

    InputImageType* inputimage = const_cast<InputImageType*>(this->GetInput());
    InputRegionIteratorType itimage(inputimage, outputRegionForThread);
    itimage.GoToBegin();

    typedef itk::ImageRegionIteratorWithIndex< typename GradientImageFilterType::OutputImageType > GradientImageIteratorType;

    typedef itk::ImageRegionIterator< InputImageMaskType > ImageMaskIteratorType;
    ImageMaskIteratorType itmask(m_ImageMask, outputRegionForThread);
    itmask.GoToBegin();
    //if(threadId == 0)
    {

        while(!outimageit.IsAtEnd()){

            OutputImagePixelType outmatrix;
            outmatrix.SetIdentity ();

            if(itmask.Get() == 1){

                vnl_vector<double> dir(3);
                dir.fill(0);
                dir[m_RotatePos] = 1;

                GradientImageIteratorType gradientimageit(m_VectorgGadientFilter[m_RotatePos]->GetOutput(), outputRegionForThread);
                gradientimageit.SetIndex(outimageit.GetIndex());

                vnl_vector<double> gradient(gradientimageit.Get().GetVnlVector());
                gradient.normalize();

                double angle = acos(dot_product(gradient, dir));
                vnl_vector<double> rotvect = vnl_cross_3d(gradient, dir);

                rotvect.normalize();
                rotvect*=angle;
                vnl_matrix<double> mat(3, 3);
                vnl_rotation_matrix (rotvect, mat);
                outmatrix = mat;
            }

            /*int n = vnl_determinant(outmatrix.GetVnlMatrix());
            cout<<n;
            //outmatrix.GetVnlMatrix().normalize_columns ();

            if(gradientnul){
                outmatrix.SetIdentity ();
            }*/

            //outmatrix.GetVnlMatrix().normalize_columns ();
            //outmatrix.SetIdentity ();
            outimageit.Set(outmatrix);


            ++itmask;
            ++outimageit;
        }
    }

}


/** The GenerateData method normally allocates the buffers for all of the
 * outputs of a filter. Some filters may want to override this default
 * behavior. For example, a filter may have multiple outputs with
 * varying resolution. Or a filter may want to process data in place by
 * grafting its input to its output. */
template< class TInputImage, class TOutputImage >
void X2UMapRotationMatrixImageFilter< TInputImage, TOutputImage >::AllocateOutputs(){

    OutputImagePointerType imageobject = this->GetOutput();

    imageobject->SetRegions(this->GetInput()->GetLargestPossibleRegion());    
    imageobject->SetSpacing (this->GetInput()->GetSpacing());
    imageobject->SetOrigin  (this->GetInput()->GetOrigin());
    imageobject->Allocate();


    m_ImageMask = InputImageMaskType::New();
    m_ImageMask->SetRegions(this->GetInput()->GetLargestPossibleRegion());
    m_ImageMask->SetSpacing (this->GetInput()->GetSpacing());
    m_ImageMask->SetOrigin  (this->GetInput()->GetOrigin());
    m_ImageMask->Allocate();

}

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
template< class TInputImage, class TOutputImage >
void X2UMapRotationMatrixImageFilter< TInputImage, TOutputImage >::BeforeThreadedGenerateData(){

    InputImageType* inputimage = const_cast<InputImageType*>(this->GetInput());
    InputRegionIteratorType itimage(inputimage, inputimage->GetLargestPossibleRegion());
    itimage.GoToBegin();

    vector<typename DoubleImageType::Pointer > vectorimage;

    for(unsigned i = 0; i < InputImageType::PixelType::Dimension; i++){
        vectorimage.push_back(DoubleImageType::New());
        vectorimage[i]->SetRegions(inputimage->GetLargestPossibleRegion());
        vectorimage[i]->Allocate();
    }

    typedef itk::ImageRegionIterator< DoubleImageType > DoubleImageIteratorType;

    typedef itk::ImageRegionIterator< InputImageMaskType > ImageMaskIteratorType;
    ImageMaskIteratorType itmask(m_ImageMask, m_ImageMask->GetLargestPossibleRegion());
    itmask.GoToBegin();

    InputImagePixelType texel;
    texel.Fill(0);

    while(!itimage.IsAtEnd()){

        for(unsigned i = 0; i < InputImageType::PixelType::Dimension; i++){
            DoubleImageIteratorType doubleimageit(vectorimage[i], vectorimage[i]->GetLargestPossibleRegion());
            doubleimageit.SetIndex(itimage.GetIndex());
            doubleimageit.Set(itimage.Get()[i]);

        }
        if(itimage.Get().Size() >= 3 && itimage.Get()[2] <= 255 && itimage.Get()!= texel){
            itmask.Set(1);
        }else{
            itmask.Set(0);
        }

        ++itimage;
        ++itmask;
    }


    for(unsigned i = 0; i < InputImageType::PixelType::Dimension; i++){

        typename GradientImageFilterType::Pointer gradient = GradientImageFilterType::New();
        gradient->SetInput(vectorimage[i]);
        gradient->Update();

        m_VectorgGadientFilter.push_back( VectorLaplacianImageFilterType::New() );
        m_VectorgGadientFilter[i]->SetInput( gradient->GetOutput() );
        m_VectorgGadientFilter[i]->SetVariance(1);
        m_VectorgGadientFilter[i]->Update();

    }
}

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
template< class TInputImage, class TOutputImage >
void X2UMapRotationMatrixImageFilter< TInputImage, TOutputImage >::AfterThreadedGenerateData(){

}

}
#endif
