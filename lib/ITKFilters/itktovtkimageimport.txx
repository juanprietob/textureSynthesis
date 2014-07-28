#ifndef __ITKToVTKImageImport_TXX
#define __ITKToVTKImageImport_TXX


namespace itk{

/**
** Default constructor, allocates the vtkImageExport and itkImageImport
**	Connects the pipelines and set them ready to be use
**/
template<typename TypeImage>
ITKToVTKImageImport<TypeImage>::ITKToVTKImageImport(){

        vtkimageexport  = vtkImageExport::New();
        imageimportpointer = ImageImportType::New();

        imageimportpointer->SetUpdateInformationCallback(vtkimageexport->GetUpdateInformationCallback());
        imageimportpointer->SetPipelineModifiedCallback(vtkimageexport->GetPipelineModifiedCallback());
        imageimportpointer->SetWholeExtentCallback(vtkimageexport->GetWholeExtentCallback());
        imageimportpointer->SetSpacingCallback(vtkimageexport->GetSpacingCallback());
        imageimportpointer->SetOriginCallback(vtkimageexport->GetOriginCallback());
        imageimportpointer->SetScalarTypeCallback(vtkimageexport->GetScalarTypeCallback());
        imageimportpointer->SetNumberOfComponentsCallback(vtkimageexport->GetNumberOfComponentsCallback());
        imageimportpointer->SetPropagateUpdateExtentCallback(vtkimageexport->GetPropagateUpdateExtentCallback());
        imageimportpointer->SetUpdateDataCallback(vtkimageexport->GetUpdateDataCallback());
        imageimportpointer->SetDataExtentCallback(vtkimageexport->GetDataExtentCallback());
        imageimportpointer->SetBufferPointerCallback(vtkimageexport->GetBufferPointerCallback());
        imageimportpointer->SetCallbackUserData(vtkimageexport->GetCallbackUserData());


        imageexportpointer = ImageExportType::New();
        vtkimageimport = vtkImageImport::New();

        vtkimageimport->SetUpdateInformationCallback(imageexportpointer->GetUpdateInformationCallback());
        vtkimageimport->SetPipelineModifiedCallback(imageexportpointer->GetPipelineModifiedCallback());
        vtkimageimport->SetWholeExtentCallback(imageexportpointer->GetWholeExtentCallback());
        vtkimageimport->SetSpacingCallback(imageexportpointer->GetSpacingCallback());
        vtkimageimport->SetOriginCallback(imageexportpointer->GetOriginCallback());
        vtkimageimport->SetScalarTypeCallback(imageexportpointer->GetScalarTypeCallback());
        vtkimageimport->SetNumberOfComponentsCallback(imageexportpointer->GetNumberOfComponentsCallback());
        vtkimageimport->SetPropagateUpdateExtentCallback(imageexportpointer->GetPropagateUpdateExtentCallback());
        vtkimageimport->SetUpdateDataCallback(imageexportpointer->GetUpdateDataCallback());
        vtkimageimport->SetDataExtentCallback(imageexportpointer->GetDataExtentCallback());
        vtkimageimport->SetBufferPointerCallback(imageexportpointer->GetBufferPointerCallback());
        vtkimageimport->SetCallbackUserData(imageexportpointer->GetCallbackUserData());

}


template<typename TypeImage >
ITKToVTKImageImport<TypeImage>::~ITKToVTKImageImport(){
        /*std::cout<<"delete ~ITKToVTKImageImport(){"<<std::endl;*/
        vtkimageimport->Delete();
        vtkimageexport->Delete();
}
/*
*	set the vtkImage data to connect to itk pipeline
*/
template<typename TypeImage>
void ITKToVTKImageImport<TypeImage>::SetVTKImage(vtkImageData* img) {
        vtkimageexport->SetInputData( img );
        vtkimageexport->Update();
        imageimportpointer->Update();
}

/*
*
*/
template<typename TypeImage>
typename ITKToVTKImageImport<TypeImage>::InputImagePointerType ITKToVTKImageImport<TypeImage>::GetOutputITKImage() {
        if(vtkimageexport->GetInput()==0){
                throw "Image not set to get the ItkImage, set the VtkImage before.\0";
        }
        return  imageimportpointer->GetOutput();
}

/*
*	Get the vtkImageData from the itk image
*/
template<typename TypeImage >
vtkImageData* ITKToVTKImageImport<TypeImage >::GetOutputVTKImage() {
        return vtkimageimport->GetOutput();
}
/*
*	set the vtkImage data to connect to itk pipeline
*/
template<typename TypeImage>
void ITKToVTKImageImport<TypeImage >::SetITKImage(InputImagePointerType itkimage){
        //std::cout<<"image pointer setITKImage"<<img<<std::endl;

    m_ITKImage = itkimage;
    imageexportpointer->SetInput(itkimage);
    imageexportpointer->Update();
    vtkimageimport->Update();

}

}
#endif
