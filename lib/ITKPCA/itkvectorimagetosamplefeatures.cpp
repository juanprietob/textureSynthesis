
#ifndef ITKVECTORIMAGETOSAMPLEFEATURES_TXX
#define ITKVECTORIMAGETOSAMPLEFEATURES_TXX

#ifndef ITKVECTORIMAGETOSAMPLEFEATURES_H
#include "itkvectorimagetosamplefeatures.h"
#endif

#include "itkImageLinearConstIteratorWithIndex.h"
#include "itkNeighborhoodIterator.h"

namespace itk {
namespace Statistics {

using namespace std;

template < class TImage,class TListValue >
VectorImageToSampleFeatures<TImage, TListValue>::VectorImageToSampleFeatures(){


    this->SetNumberOfRequiredInputs(1);
    this->SetNumberOfRequiredOutputs(1);
    typename ListSampleOutputType::Pointer listSampleDecorator = static_cast< ListSampleOutputType * >( this->MakeOutput(0).GetPointer() );

    this->ProcessObject::SetNthOutput(0, listSampleDecorator.GetPointer());

    this->m_NeighborhoodSize = 0;
    this->m_StepSize = 1;
    this->m_TexelDimension = 1;
}

template < class TImage,class TListValue >
VectorImageToSampleFeatures<TImage, TListValue>::~VectorImageToSampleFeatures(){

}

template < class TImage, class TListValue >
typename VectorImageToSampleFeatures< TImage, TListValue >::DataObjectPointer
VectorImageToSampleFeatures< TImage, TListValue >::MakeOutput(unsigned int itkNotUsed(idx))
{
  typename ListSampleOutputType::Pointer decoratedOutput =  ListSampleOutputType::New();
  decoratedOutput->Set( ListSampleType::New() );

  return static_cast< DataObject * >(decoratedOutput.GetPointer());
}

template < class TImage, class TListValue >
void
VectorImageToSampleFeatures< TImage, TListValue >
::GenerateOutputInformation()
{
  Superclass::GenerateOutputInformation();

  ListSampleOutputType * decoratedOutput = static_cast< ListSampleOutputType * >(this->ProcessObject::GetOutput(0));
  ListSampleType *output = const_cast< ListSampleType *>( decoratedOutput->Get() );

  int vectorsize = pow((double)m_NeighborhoodSize+1, (double)2)*this->m_TexelDimension;

  output->SetMeasurementVectorSize(vectorsize);
}

template < class TImage,class TListValue >
void VectorImageToSampleFeatures< TImage, TListValue >::Update(){
    GenerateOutputInformation();
    GenerateData();
}

template < class TImage,class TListValue >
void VectorImageToSampleFeatures< TImage, TListValue >::GenerateData(){

    ListSampleOutputType * decoratedOutput =
      static_cast< ListSampleOutputType * >(
        this->ProcessObject::GetOutput(0));

    ListSampleType *listSample =
      const_cast< ListSampleType * >( decoratedOutput->Get() );

    listSample->Clear();

    ImagePointer  inputimage  = this->GetInput();



    typedef itk::ImageLinearConstIteratorWithIndex< ImageType > LinearIteratorType;
    LinearIteratorType linearit(inputimage, inputimage->GetRequestedRegion());
    linearit.SetDirection(0);
    linearit.GoToBegin();

    unsigned int neighborhoodsize = m_NeighborhoodSize/2;   


    typedef itk::NeighborhoodIterator< ImageType > NeighborhoodIteratorType;
    typename NeighborhoodIteratorType::RadiusType radius;    
    radius.Fill(neighborhoodsize);
    NeighborhoodIteratorType imageit(radius, inputimage, inputimage->GetRequestedRegion());

    imageit.GoToBegin();
    int vectorsize = imageit.Size()*this->m_TexelDimension;

    while(!linearit.IsAtEnd()){

        bool moveline = false;

        while( !linearit.IsAtEndOfLine() ){


            imageit.SetLocation(linearit.GetIndex());

            if(imageit.InBounds()){

                moveline = true;

                MeasurementVectorType currentvector(vectorsize);
                currentvector.Fill(-1);

                unsigned positionvector = 0;

                for(unsigned i = 0; i < imageit.Size(); i++){
                    //get all the texel values and pile them up in the current vector
                    for(unsigned j = 0; j < m_TexelDimension && j < imageit.GetPixel(i).Size(); j++){
                        PixelValueType valuecurrentv = imageit.GetPixel(i)[j];
                        #ifdef WIN32
							if(!(valuecurrentv!=valuecurrentv)){
						#else
							if(isnan(valuecurrentv)){
						#endif
                            cout<<imageit.GetPixel(i)[j]<<endl;
                        }
                        currentvector.SetElement(positionvector+j, valuecurrentv);
                    }

                    positionvector+=m_TexelDimension;
                }
                listSample->PushBack( currentvector );

                for(unsigned i = 0; i < m_StepSize && !linearit.IsAtEndOfLine();i++){
                    ++linearit;
                }



            }else{
                ++linearit;
            }
        }
        if(moveline){
            for(unsigned i = 0; i < m_StepSize && !linearit.IsAtEnd();i++){
                linearit.NextLine();
            }
        }else{
            linearit.NextLine();
        }

    }

    itkDebugMacro(<<"list size "<<listSample->Size()<<endl);

}

template < class TImage,class TListValue >
const typename VectorImageToSampleFeatures<TImage, TListValue>::ListSampleType *
VectorImageToSampleFeatures<TImage, TListValue>::GetListSample() const{
    const ListSampleOutputType * decoratedOutput =
      static_cast< const ListSampleOutputType * >(
        this->ProcessObject::GetOutput(0));
    return decoratedOutput->Get();
}

}
}


#endif
