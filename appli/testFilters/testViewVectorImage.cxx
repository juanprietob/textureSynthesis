
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkArrowSource.h>
#include <itkRGBPixel.h>
#include <itkImage.h>
#include <itkImageFileReader.h>

#include "itkx2umaprotationmatriximagefilter.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkImageFileWriter.h"

#include "vtkProperty.h"
#include "vnl/vnl_cross.h"
#include "vtkSphereSource.h"

using namespace std;

#define PI 3.14159265

int main(int argc, char* argv[])
{



    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(1,1,1);
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);


    if (argc < 2)
      {
        std::cout << "ERROR: data file name argument missing."
                  << std::endl ;
        return EXIT_FAILURE;
      }

    std::string inputFilename = argv[1];

    typedef unsigned char PixelTypeUC;
    typedef itk::Vector< PixelTypeUC, 3 > RGBUCPixelType;
    typedef itk::Image< RGBUCPixelType, 3 > RGBUCImageType ;
    typedef itk::ImageFileReader< RGBUCImageType > ImageReaderUCType ;
    ImageReaderUCType::Pointer imageReaderuc = ImageReaderUCType::New() ;
    std::string inputFilenameuc = argv[2];
    imageReaderuc->SetFileName(inputFilenameuc.c_str());
    imageReaderuc->Update();

    RGBUCImageType::Pointer imagergb = imageReaderuc->GetOutput();

    typedef itk::ImageRegionIteratorWithIndex< RGBUCImageType > InputRegionIteratorUCType;
    InputRegionIteratorUCType imagergbit(imagergb, imagergb->GetLargestPossibleRegion());
    imagergbit.GoToBegin();

    typedef itk::VectorLinearInterpolateImageFunction< RGBUCImageType > VectorInterpolateImageType;
    VectorInterpolateImageType::Pointer interpolate = VectorInterpolateImageType::New();
    interpolate->SetInputImage(imagergb);

    RGBUCImageType::Pointer outimage = RGBUCImageType::New();
    outimage->SetRegions(imagergb->GetLargestPossibleRegion());
    outimage->Allocate();

    InputRegionIteratorUCType imagergboutit(outimage, outimage->GetLargestPossibleRegion());
    imagergboutit.GoToBegin();

    typedef double PixelType;
    typedef itk::Vector< PixelType, 3 > RGBPixelType;
    typedef itk::Image< RGBPixelType, 3 > RGBImageType ;
    typedef itk::ImageFileReader< RGBImageType > ImageReaderType ;
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

    imageReader->SetFileName(inputFilename.c_str());
    imageReader->Update() ;
    RGBImageType::Pointer image = imageReader->GetOutput();


    typedef itk::X2UMapRotationMatrixImageFilter< RGBImageType > X2UMapRotationMatrixImageFilterType;
    typename X2UMapRotationMatrixImageFilterType::Pointer x2urotationfilter = X2UMapRotationMatrixImageFilterType::New();
    x2urotationfilter->SetInput(image);
    x2urotationfilter->Update();
    typedef X2UMapRotationMatrixImageFilterType::OutputImageType OutputImageType;
    OutputImageType::Pointer rotationmatriximage = x2urotationfilter->GetOutput();


    typedef itk::ImageRegionIteratorWithIndex< RGBImageType > InputRegionIteratorType;
    InputRegionIteratorType imageit(image, image->GetLargestPossibleRegion());
    imageit.GoToBegin();


    typedef itk::ImageRegionIteratorWithIndex< OutputImageType > OutputRegionIteratorType;

    OutputRegionIteratorType outit( rotationmatriximage, rotationmatriximage->GetLargestPossibleRegion());
    outit.GoToBegin();

    int neighborhoodsize = 8;
    int m_StepSize = 8;

    OutputImageType::IndexType xstartindex = outit.GetIndex();
    xstartindex[0] += neighborhoodsize;
    xstartindex[1] += neighborhoodsize;

    OutputImageType::IndexType xendindex = outit.GetIndex();
    xendindex[0] += rotationmatriximage->GetLargestPossibleRegion().GetSize()[0] - neighborhoodsize;
    xendindex[1] += rotationmatriximage->GetLargestPossibleRegion().GetSize()[1] - neighborhoodsize;
    xendindex[2] += rotationmatriximage->GetLargestPossibleRegion().GetSize()[2];

    OutputImageType::IndexType ystartindex = outit.GetIndex();
    ystartindex[0] += neighborhoodsize;
    ystartindex[2] += neighborhoodsize;

    OutputImageType::IndexType yendindex = outit.GetIndex();
    yendindex[0] += rotationmatriximage->GetLargestPossibleRegion().GetSize()[0] - neighborhoodsize;
    yendindex[1] += rotationmatriximage->GetLargestPossibleRegion().GetSize()[1];
    yendindex[2] += rotationmatriximage->GetLargestPossibleRegion().GetSize()[2] - neighborhoodsize;

    OutputImageType::IndexType zstartindex = outit.GetIndex();
    zstartindex[1] += neighborhoodsize;
    zstartindex[2] += neighborhoodsize;

    OutputImageType::IndexType zendindex = outit.GetIndex();
    zendindex[0] += rotationmatriximage->GetLargestPossibleRegion().GetSize()[0];
    zendindex[1] += rotationmatriximage->GetLargestPossibleRegion().GetSize()[1] - neighborhoodsize;
    zendindex[2] += rotationmatriximage->GetLargestPossibleRegion().GetSize()[2] - neighborhoodsize;




    while(!outit.IsAtEnd()){
        int dim = 2;
        if(imageit.Get()[dim] != 0){

            OutputImageType::IndexType outindex = outit.GetIndex();
            bool ztexel = false, ytexel = false, xtexel = false;

            if(         xstartindex[0] <= outindex[0] && outindex[0] <= xendindex[0]
                    &&  xstartindex[1] <= outindex[1] && outindex[1] <= xendindex[1]
                    &&  (outindex[0] - xstartindex[0]) % m_StepSize == 0
                    &&  (outindex[1] - xstartindex[1]) % m_StepSize == 0
                    ){
                ztexel = true;
            }
            if(         ystartindex[0] <= outindex[0] && outindex[0] <= yendindex[0]
                    &&  ystartindex[2] <= outindex[2] && outindex[2] <= yendindex[2]
                    &&  (outindex[0] - ystartindex[0]) % m_StepSize == 0
                    &&  (outindex[2] - ystartindex[2]) % m_StepSize == 0
                    ){
                ytexel = true;
            }
            if(         zstartindex[1] <= outindex[1] && outindex[1] <= zendindex[1]
                    &&  zstartindex[2] <= outindex[2] && outindex[2] <= zendindex[2]
                    &&  (outindex[1] - zstartindex[1]) % m_StepSize == 0
                    &&  (outindex[2] - zstartindex[2]) % m_StepSize == 0
                    ){
                xtexel = true;
            }

            OutputImageType::PixelType rotationmatrix = outit.Get();

            if(xtexel && ytexel && ztexel && 150 <= imageit.Get()[dim]  && imageit.Get()[dim] <= 180){



                
                //for(unsigned i = 0; i < 3; i++)
                unsigned i = 0;
                {
		    vnl_vector<double> offsetvnl(3, 0);
                    //offsetvnl[1] = 1;
                    offsetvnl[2] = 1;

                    //offsetvnl = rotationmatrix.GetTranspose  () * offsetvnl;
                    offsetvnl = rotationmatrix * offsetvnl;
                    //offsetvnl.normalize();
                    double weightednorm[3] = {0,0,0};

                    weightednorm[0] = offsetvnl[0];
                    weightednorm[1] = offsetvnl[1];
                    weightednorm[2] = offsetvnl[2];

                    vtkSmartPointer<vtkPolyDataMapper> actormapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                    vtkSmartPointer<vtkActor>  vtkactor = vtkActor::New();
                    vtkactor->SetMapper(actormapper);

                    vtkSmartPointer<vtkArrowSource> vtkarrow		= vtkSmartPointer<vtkArrowSource>::New();
                    vtkarrow->SetTipResolution(30);
                    vtkarrow->SetShaftResolution( 30  );
                    actormapper->SetInput( vtkarrow->GetOutput() );

                    double scale[3];
                    scale[0] = 0.3;
                    scale[1] = 0.3;
                    scale[2] = 0.3;
                    vtkactor->SetScale(scale);

                    double orientation[3] = {0,0,0};
                    vtkactor->SetOrientation(orientation);

                    double valxy = 0, valxz = 0, valxyz = 0;
                    double angle0 = atan2(weightednorm[2], weightednorm[0]) * 180.0 / PI;

                    valxz = sqrt(pow(weightednorm[0],2) + pow(weightednorm[2],2));
                    valxyz = sqrt(pow(weightednorm[0],2) + pow(weightednorm[1],2) + pow(weightednorm[2],2));
                    valxy = sqrt(pow(valxyz,2) - pow(valxz,2));
                    double angle1 = atan2(valxy, valxz) * 180.0 / PI;

                    vtkactor->RotateY(-angle0);
                    if(weightednorm[1] < 0){
                            vtkactor->RotateZ(-angle1);
                    }else{
                            vtkactor->RotateZ(angle1);
                    }

                    OutputImageType::IndexType currentpos = outit.GetIndex();
                    double color[3] = {currentpos[0]/255.0,currentpos[1]/255.0, currentpos[2]/255.0};
                    //color[i] = 1;
                    vtkactor->GetProperty()->SetColor(color);

                    itk::Vector<double, 3> spacing = rotationmatriximage->GetSpacing();
                    vtkactor->SetPosition(currentpos[0] * spacing[0], currentpos[1]* spacing[1], currentpos[2]* spacing[2]);
                   renderer->AddActor(vtkactor);
                
                
		  OutputImageType::PixelType rotationmatrixmodif;
		  rotationmatrixmodif[0][0] = 1 - 2 * (offsetvnl[1]*offsetvnl[1] + offsetvnl[2] * offsetvnl[2] );
		  rotationmatrixmodif[1][0] = 2 * (offsetvnl[0]*offsetvnl[1] ); 
		  rotationmatrixmodif[2][0] = 2 * (offsetvnl[0] * offsetvnl[2]);
		  
		  rotationmatrixmodif[0][1] = 2 * (offsetvnl[0] * offsetvnl[1]);
		  rotationmatrixmodif[1][1] = 1 - 2 * (offsetvnl[0] * offsetvnl[0] + offsetvnl[2] * offsetvnl[2]);
		  rotationmatrixmodif[2][1] = 2 * (offsetvnl[1] * offsetvnl[2]);
		  
		  rotationmatrixmodif[0][2] = 2 * (offsetvnl[0] * offsetvnl[2]);
		  rotationmatrixmodif[1][2] = 2 * (offsetvnl[1] * offsetvnl[2]);
		  rotationmatrixmodif[2][2] = 1 - 2 * (offsetvnl[0] * offsetvnl[0] + offsetvnl[1] * offsetvnl[1]);
		  
		  rotationmatrixmodif = rotationmatrix;
		  
                  /*for(int j = -4; j <= 4; j++ ){
		      for(int k = -4; k <= 4; k++ ){
			  
			      vnl_vector<double> offsetvnl(3, 0);
			      offsetvnl[i] = 0;
			      unsigned next = i + 1;
			      if(next == 3){
				next = 0;
			      }
			      offsetvnl[next] = j;
			      
			      next = next + 1;
			      if(next == 3){
				next = 0;
			      }
                              offsetvnl[next] = k;

                              //vnl_vector<double> offsetvnl0 = offsetvnl;

			      offsetvnl = rotationmatrixmodif * offsetvnl;

			      vtkSmartPointer<vtkSphereSource> sphereSource =
			      vtkSmartPointer<vtkSphereSource>::New();
			      sphereSource->SetCenter(0.0, 0.0, 0.0);
			      sphereSource->SetRadius(0.5);

			      vtkSmartPointer<vtkPolyDataMapper> mapper =
			      vtkSmartPointer<vtkPolyDataMapper>::New();
			      mapper->SetInputConnection(sphereSource->GetOutputPort());

			      vtkSmartPointer<vtkActor> actor =
			      vtkSmartPointer<vtkActor>::New();
			      actor->SetMapper(mapper);

			      itk::Vector<double, 3> spacing = rotationmatriximage->GetSpacing();
                              RGBUCImageType::IndexType currentpos = outit.GetIndex();

			      double color[3] = {currentpos[0]/255.0,currentpos[1]/255.0, currentpos[2]/255.0};
			      //color[i] = 1;
			      actor->GetProperty()->SetColor(color);


                              //offsetvnl = offsetvnl0;
			      actor->SetPosition( (currentpos[0] +  offsetvnl[0] )  * spacing[0]
						,(currentpos[1] +  offsetvnl[1] )  * spacing[1]
						,(currentpos[2] +  offsetvnl[2] )  * spacing[2]
						);
			      renderer->AddActor(actor);

                              VectorInterpolateImageType::PointType pointinter ;
                              pointinter[0] = (currentpos[0] +  offsetvnl[0] )  * spacing[0];
                              pointinter[1] = (currentpos[1] +  offsetvnl[1] )  * spacing[1];
                              pointinter[2] = (currentpos[2] +  offsetvnl[2] )  * spacing[2];

                              if(interpolate->IsInsideBuffer(pointinter)){

                                  RGBUCImageType::PixelType outpix = interpolate->Evaluate(pointinter);

                                  RGBUCImageType::IndexType indexinter;
                                  interpolate->ConvertPointToNearestIndex(pointinter, indexinter);

                                  imagergboutit.SetIndex(indexinter);
                                  imagergboutit.Set(outpix);
                              }
                      }*/

		}
            }
        }

        ++imageit;
        ++outit;
    }






    typedef itk::ImageFileWriter< RGBUCImageType > ImageFileWriterType;
    ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
    writer->SetFileName("testImageRotation.mhd");
    writer->SetInput(outimage);
    writer->Update();






    renderWindow->Render();

    renderWindowInteractor->Start();

    return 0;

}
