#ifndef ITKVTKCOLORTRANSFERFUNCTION_TXX
#define ITKVTKCOLORTRANSFERFUNCTION_TXX

namespace itk{


template< class TypeImage, typename OutType>
VTKColorTransferFunction<TypeImage, OutType>::VTKColorTransferFunction()
{

    m_GenerateShort = false;
}

template< class TypeImage, typename OutType>
VTKColorTransferFunction<TypeImage, OutType>::~VTKColorTransferFunction()
{
    m_Output->Delete();
}

template< class TypeImage, typename OutType>
void VTKColorTransferFunction<TypeImage, OutType>::Update(){
    GenerateOutputInformation();
    GenerateData();
}

template< class TypeImage, typename OutType>
void VTKColorTransferFunction<TypeImage, OutType>::GenerateOutputInformation(){
     m_Output = vtkColorTransferFunction::New();
     m_Output->SetColorSpaceToRGB();
}



template< class TypeImage,typename OutType >
void VTKColorTransferFunction<TypeImage, OutType>::GenerateData(){



    InputImagePointerType imagergb = this->GetInput();

    if(!imagergb){
        itkExceptionMacro(<<"Set the image first!");
    }


    typedef itk::ImageRegionIterator< InputImageType > rgbiteratorType;
    rgbiteratorType rgbit(imagergb, imagergb->GetLargestPossibleRegion());


    rgbit.GoToBegin();


    m_Output->RemoveAllPoints ();





        while(!rgbit.IsAtEnd()){


            unsigned char lum = (unsigned char)(0.299*rgbit.Get()[0] + 0.587*rgbit.Get()[1] + 0.114*rgbit.Get()[2]);

            m_Output->AddRGBPoint(lum, rgbit.Get()[0]/255.0, rgbit.Get()[1]/255.0, rgbit.Get()[2]/255.0);

            ++rgbit;

        }









}

}
#endif
