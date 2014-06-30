//----------------------------------------------------------------------
//		File:                   freeSurferLabels.cxx
//      Author:                 Juan Carlos Prieto Bernal
//----------------------------------------------------------------------

//      Given a subject segmented using freesurfer
//      all the labels are extracted and put into new image files
//      not that freesurfer must be accessible by the path as an image convertion is performed
//      using mri_convert


#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <itkBinaryThresholdImageFilter.h>
#include <itkMaskImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>
#include <itkBinaryMedianImageFilter.h>

#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

using namespace std;					// make std:: accessible


void help(char* execname){
    cout<<"Given a subject segmented using freesurfer all the labels are extracted and put into new image files"<<endl;
    cout<<"note that freesurfer must be accessible by the path as an image convertion is performed using mri_convert"<<endl;
    cout<<"Usage: "<<execname<<" -p <path freesurfer subject> [options] -i <image1> -i <image2>"<<endl;
    cout<<"options:"<<endl;
    cout<<"--h --help show help menu"<<endl;
    cout<<"-p <path> path to the freesurfer subject"<<endl;
    cout<<"options:"<<endl;
    cout<<"-i <image filename> to perform the AND operation with the aseg.auto image file for each label intensity."<<endl;
    cout<<"-op <output path> output path for the labels"<<endl;
    cout<<"-native <bool> native coordiantes = 1 or freesurfer coordinates = 0"<<endl;
    cout<<"-smooth <double> set to value > 0 if you want to smooth the segmentation with a median filter."<<endl;
    cout<<"-dilate <int> set to value > 0 if you want to dilate the segmentation."<<endl;
}

int main(int argc, char **argv)
{


    if (argc == 1){
        help(argv[0]);
        return 0;
    }

    string path = "";
    string outputpath = "output/";
    vector< string > vectimagefilename;
    bool native = 0;
    double smoothradius = 0;
    double dilateradius = 0;


    for(int i = 1; i < argc; i++){

        if(string(argv[i]) == "--h" || string(argv[i]) == "--help"){
            help(argv[0]);
            return 0;
        }else if (string(argv[i]) == "-p"){
            if(i + 1 >= argc){
                std::cout << "ERROR: freesurfer subject's path missing."
                          << std::endl ;
                return EXIT_FAILURE;
            }
            path  = argv[i + 1];
            //path.replace(path.find(" "), 1, "\\ ");
        }else if (string(argv[i]) == "-i"){
            if(i + 1 >= argc){
                std::cout << "ERROR: image filename missing."
                          <<std::endl
                          <<"-i <filename>"
                          << std::endl ;
                return EXIT_FAILURE;
            }

            string infilename = argv[i + 1];
            vectimagefilename.push_back(infilename);

        }else if (string(argv[i]) == "-op"){
            outputpath = string(argv[i + 1]);
        }else if (string(argv[i]) == "-native"){
            native = argv[i + 1];
        }else if (string(argv[i]) == "-smooth"){
            smoothradius = atof(argv[i + 1]);
        }else if (string(argv[i]) == "-dilate"){
            dilateradius = atof(argv[i + 1]);
        }
    }

    std::string aseglabels = path;
    aseglabels.append("/mri/aseg.auto_noCCseg.label_intensities.txt");

    string line;
    ifstream myfile (aseglabels.c_str());

    vector<int> vectNumlabels;
    vector<string> vectStringLabels;

    if (myfile.is_open())
    {
        while ( myfile.good() ){
            getline (myfile,line);
            int pos = 0, pos1 = 0;
            pos1 = line.find(" ");

            string num = line.substr(pos, pos1);


            vectNumlabels.push_back(atoi(num.c_str()));

            pos = pos1 + 1;
            pos1 = line.find(" ", pos);

            vectStringLabels.push_back(line.substr(pos, pos1-pos));

        }
        myfile.close();
    }else{
         cout << "Unable to label intensities file from freesurfer. <subject>/mri/aseg.auto_noCCseg.label_intensities.txt";
         return 0;
    }

    char cmd[500];

    cout<<"Output path = "<<outputpath<<endl;
    cout<<"native = "<<native<<endl;
    cout<<"smooth = "<<smoothradius<<endl;
    cout<<"dilate = "<<dilateradius<<endl;

    string inputFilename = "";
    string outputFilename = "";


    string inputFilenameaseg = "";
    string outputFilenameaseg = "";

    if(native){

        sprintf(cmd, "mkdir %s", outputpath.c_str() );
        cout<<cmd<<endl;
        system(cmd);

        inputFilename = path;
        inputFilename.append("/mri/aseg.auto_noCCseg.mgz");


        string inputRawFilename = path;
        inputRawFilename.append("/mri/rawavg.mgz");

        string outputFilenameRaw = path;
        outputFilenameRaw.append("/mri/aseg.auto_noCCseg-in-rawavg.mgz");

        sprintf(cmd, "mri_label2vol --seg %s --temp %s --o %s --regheader %s",
                      inputFilename.c_str(), inputRawFilename.c_str(), outputFilenameRaw.c_str(), inputFilename.c_str() );
        cout<<cmd<<endl;
        system(cmd);








        outputFilename = outputpath;
        outputFilename.append("/aseg.auto_noCCseg-in-rawavg.nii.gz");
        inputFilenameaseg = path;
        inputFilenameaseg.append("/mri/aseg.auto.mgz");

        string outputFilenameRawaseg = path;
        outputFilenameRawaseg.append("/mri/aseg.auto-in-rawavg.mgz");

        sprintf(cmd, "mri_label2vol --seg %s --temp %s --o %s --regheader %s",
                      inputFilenameaseg.c_str(), inputRawFilename.c_str(), outputFilenameRawaseg.c_str(), inputFilename.c_str() );
        cout<<cmd<<endl;
        system(cmd);






        inputFilename = outputFilenameRaw;
        inputFilenameaseg = outputFilenameRawaseg;

    }else{

        inputFilename = path;
        inputFilename.append("/mri/aseg.auto_noCCseg.mgz");

        sprintf(cmd, "mkdir %s", outputpath.c_str() );
        cout<<cmd<<endl;
        system(cmd);

        outputFilename = outputpath;
        outputFilename.append("/aseg.auto_noCCseg.nii.gz");

        inputFilenameaseg = path;
        inputFilename.append("/mri/aseg.auto.mgz");


    }

    typedef unsigned char PixelType;
    typedef itk::Image< PixelType, 3 > InputImageType ;
    typedef itk::ImageFileReader< InputImageType > ImageReaderType ;

    typedef itk::ImageFileWriter< InputImageType > ImageWriterType;

    sprintf(cmd, "mri_convert %s -ot nii %s", inputFilename.c_str(), outputFilename.c_str() );
    cout<<cmd<<endl;
    system(cmd);

    string outputFilenameCorpus = outputpath;
    outputFilenameCorpus.append("/");
    outputFilenameCorpus.append("Corpus_Callosum");

    sprintf(cmd, "mkdir %s", outputFilenameCorpus.c_str() );
    cout<<cmd<<endl;
    system(cmd);

    outputFilenameaseg = outputFilenameCorpus;
    outputFilenameaseg.append("/Corpus_Callosum.nii.gz");

    sprintf(cmd, "mri_extract_label %s 251 252 253 254 255 %s", inputFilenameaseg.c_str(), outputFilenameaseg.c_str() );
    cout<<cmd<<endl;
    system(cmd);

    /*ImageReaderType::Pointer readercc = ImageReaderType::New();
    readercc->SetFileName( outputFilenameaseg.c_str() );
    readercc->Update();

    outputFilenameaseg = outputFilenameCorpus;
    outputFilenameaseg.append("/Corpus_Callosum.mhd");

    ImageWriterType::Pointer writercc = ImageWriterType::New();
    writercc->SetFileName(outputFilenameaseg.c_str());
    writercc->SetInput(readercc->GetOutput());
    writercc->Update();*/

    ImageReaderType::Pointer imageReader = ImageReaderType::New();


    imageReader->SetFileName(outputFilename.c_str());
    imageReader->Update() ;
    InputImageType::Pointer image = imageReader->GetOutput();

    typedef itk::BinaryThresholdImageFilter<InputImageType, InputImageType> ThresholdImageFilterType;
    ThresholdImageFilterType::Pointer thresholdfilter = ThresholdImageFilterType::New();
    thresholdfilter->SetInput(image);


    ImageWriterType::Pointer imagewriter = ImageWriterType::New();

    for(unsigned i = 0; i < vectNumlabels.size()-1; i++){

        if(vectStringLabels[i].compare("") != 0){            
            thresholdfilter->SetInput(image);
            thresholdfilter->SetLowerThreshold(vectNumlabels[i]);
            thresholdfilter->SetUpperThreshold(vectNumlabels[i]);
            thresholdfilter->SetInsideValue(255);
            thresholdfilter->SetOutsideValue(0);
            thresholdfilter->Update();
            InputImageType::Pointer threshimage = thresholdfilter->GetOutput();

              /*typedef itk::NearestNeighborInterpolateImageFunction<InputImageType, double> T_Interpolator;

              // The resampler type itself.
              typedef itk::ResampleImageFilter<InputImageType, InputImageType>  T_ResampleFilter;

              // Prepare the resampler.

              // Instantiate the transform and specify it should be the id transform.

              typedef itk::IdentityTransform<double, 3> T_Transform;
              T_Transform::Pointer _pTransform = T_Transform::New();
              _pTransform->SetIdentity();

              // Instantiate the b-spline interpolator and set it as the third order
              // for bicubic.
              T_Interpolator::Pointer _pInterpolator = T_Interpolator::New();
              //_pInterpolator->SetSplineOrder(3);

              // Instantiate the resampler. Wire in the transform and the interpolator.
              T_ResampleFilter::Pointer _pResizeFilter = T_ResampleFilter::New();
              _pResizeFilter->SetTransform(_pTransform);
              _pResizeFilter->SetInterpolator(_pInterpolator);

              _pResizeFilter->SetOutputParametersFromImage(threshimage);

              const InputImageType::RegionType& inputRegion = threshimage->GetLargestPossibleRegion();
              const InputImageType::SizeType&   vnInputSize = inputRegion.GetSize();
              const InputImageType::SpacingType& vfInputSpacing = threshimage->GetSpacing();

              double vfOutputSpacing[3];
              vfOutputSpacing[0] = vfInputSpacing[0]/2.0;
              vfOutputSpacing[1] = vfInputSpacing[1]/2.0;
              vfOutputSpacing[2] = vfInputSpacing[2]/2.0;

              _pResizeFilter->SetOutputSpacing(vfOutputSpacing);

              // Set the output size as specified on the command line.
              itk::Size<3> vnOutputSize;
              vnOutputSize[0] = vnInputSize[0]*2.0;
              vnOutputSize[1] = vnInputSize[1]*2.0;
              vnOutputSize[2] = vnInputSize[2]*2.0;
              _pResizeFilter->SetSize(vnOutputSize);

              // Specify the input.

              _pResizeFilter->SetInput(threshimage);
              _pResizeFilter->Update();
              threshimage = _pResizeFilter->GetOutput();*/


            typedef itk::BinaryMedianImageFilter<InputImageType, InputImageType > BinaryMedianType;
            BinaryMedianType::Pointer medianFilter = BinaryMedianType::New();
            BinaryMedianType::InputSizeType radius;
            radius.Fill(smoothradius);
            medianFilter->SetRadius(radius);
            medianFilter->SetInput( threshimage );

            threshimage = medianFilter->GetOutput();

            typedef itk::BinaryBallStructuringElement<InputImageType::PixelType, 3> StructuringElementType;
            StructuringElementType structuringElement;
            structuringElement.SetRadius(dilateradius);
            structuringElement.CreateStructuringElement();

            typedef itk::BinaryDilateImageFilter <InputImageType, InputImageType, StructuringElementType> BinaryDilateImageFilterType;
            BinaryDilateImageFilterType::Pointer dilateFilter = BinaryDilateImageFilterType::New();
            dilateFilter->SetInput(threshimage);
            dilateFilter->SetKernel(structuringElement);

            threshimage = dilateFilter->GetOutput();


            string outputFilenameThreshold = outputpath;
            outputFilenameThreshold.append("/");
            outputFilenameThreshold.append(vectStringLabels[i]);

            sprintf(cmd, "mkdir %s", outputFilenameThreshold.c_str() );
            cout<<cmd<<endl;
            system(cmd);

            char fn[50];
            sprintf(fn, "/%s.mhd", vectStringLabels[i].c_str());
            outputFilenameThreshold.append(fn);

            imagewriter->SetFileName(outputFilenameThreshold.c_str());
            imagewriter->SetInput(threshimage);
            imagewriter->Update();


            for(unsigned j=0; j < vectimagefilename.size();j++){
                typedef float RelaxoPixelType;
                typedef itk::Image< RelaxoPixelType, 3 > RelaxoImageType ;
                typedef itk::ImageFileReader< RelaxoImageType > RelaxoImageReaderType ;
                RelaxoImageReaderType::Pointer relaxoImageReader = RelaxoImageReaderType::New();

                relaxoImageReader->SetFileName(vectimagefilename[j]);
                relaxoImageReader->Update();

                typedef itk::MaskImageFilter<RelaxoImageType, InputImageType, RelaxoImageType> MaskImageFilterType;
                MaskImageFilterType::Pointer maskImageFilter = MaskImageFilterType::New();
                maskImageFilter->SetInput1( relaxoImageReader->GetOutput() );
                maskImageFilter->SetInput2( thresholdfilter->GetOutput() );
                maskImageFilter->SetOutsideValue(0);
                maskImageFilter->Update();

                string relaxoOutput = vectimagefilename[j];
                if(relaxoOutput.find_last_of("/") != string::npos){
                    relaxoOutput = relaxoOutput.substr(relaxoOutput.find_last_of("/"));
                }

                string relaxoOutputFilename = outputpath;
                relaxoOutputFilename.append(vectStringLabels[i]);
                relaxoOutputFilename.append("/");
                relaxoOutputFilename.append(relaxoOutput);


                typedef itk::ImageFileWriter< RelaxoImageType > RelaxoImageWriterType;
                RelaxoImageWriterType::Pointer relaxoImagewriter = RelaxoImageWriterType::New();
                relaxoImagewriter->SetInput(maskImageFilter->GetOutput());
                relaxoImagewriter->SetFileName(relaxoOutputFilename.c_str());
                relaxoImagewriter->Update();
            }
        }
    }






    return 0;
}

