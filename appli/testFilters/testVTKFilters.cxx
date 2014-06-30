#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyData.h>
#include <vtkMetaImageReader.h>
#include <vtkMetaImageWriter.h>
#include <vtkPLYWriter.h>
#include "vtkImageData.h"

using namespace std;

int main(int argc, char *argv[])
{


  /*vtkSmartPointer<vtkMetaImageReader> reader =
    vtkSmartPointer<vtkMetaImageReader>::New();

  reader->SetFileName(argv[1]);
  reader->Update();
  vtkImageData* image = reader->GetOutput();

  vtkSmartPointer<vtkMarchingCubes> surface =
    vtkSmartPointer<vtkMarchingCubes>::New();

  surface->SetInput(image);
  surface->ComputeNormalsOn();

  surface->GenerateValues(3, 254, 255);

  vtkSmartPointer<vtkPLYWriter> writer = vtkSmartPointer<vtkPLYWriter>::New();
  string filenameout(argv[1]);
  filenameout = filenameout.substr(0, filenameout.size() - 4);
  filenameout.append(".ply");
  writer->SetFileName(filenameout.c_str());
  writer->SetInputConnection(surface->GetOutputPort());
  writer->Update();
  return EXIT_SUCCESS;*/







    vtkSmartPointer<vtkMetaImageReader> reader =
        vtkSmartPointer<vtkMetaImageReader>::New();

      reader->SetFileName(argv[1]);
      reader->Update();
      vtkImageData* image = reader->GetOutput();
      int* extent = image->GetExtent();

      vtkSmartPointer<vtkImageData> imageout =  vtkSmartPointer<vtkImageData>::New();
      imageout->SetSpacing(image->GetSpacing());
      imageout->SetOrigin(image->GetOrigin());
      imageout->SetScalarType(image->GetScalarType());
      imageout->SetExtent(extent[0], extent[1], 0,69,extent[4],extent[5]);
      imageout->AllocateScalars();


      for(unsigned i = 0; i <= 69; i++){

          cout<<i<<endl;

          vtkSmartPointer<vtkMetaImageReader> reader =
              vtkSmartPointer<vtkMetaImageReader>::New();

          char buf[50];
          sprintf(buf, "./out/%dout.mhd", i);

            reader->SetFileName(buf);
            reader->Update();
            vtkImageData* image1 = reader->GetOutput();

            for(int x = extent[0]; x < extent[1]; x++){
                //for(int y = 0; y < extent[3]; y++){
                int y = 153;{
                    for(int z = extent[4]; z < extent[5]; z++){
                        short *p0 = (short*)image->GetScalarPointer(x, y, z);
                        short *p1 = 0;
                        unsigned char *p11 = 0;
                        if(i == 0){
                            p11 = (unsigned char*)image1->GetScalarPointer(x, y, z);
                        }else{
                            p1 = (short*)image1->GetScalarPointer(x, y, z);
                        }
                        short *pout = (short*)imageout->GetScalarPointer(x, i, z);
                        if(i == 0){
                            if(*p11 == 3){
                                *pout = 2012;
                            }else{
                                *pout = *p0;
                            }
                        }else if(*p1 != 0){
                            *pout = 2012;
                        }else{
                            *pout = *p0;
                        }
                    }
                }
            }

      }

      vtkSmartPointer< vtkMetaImageWriter > writer = vtkSmartPointer< vtkMetaImageWriter >::New();
      writer->SetInput(imageout);
      writer->SetFileName("movout.mhd");
      writer->Write();


}
