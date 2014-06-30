#ifndef ITKSAMPLEPCAREDUCTION_TXX
#define ITKSAMPLEPCAREDUCTION_TXX


#ifndef ITKSAMPLEPCAREDUCTION_H
#include "itksamplepcareduction.h"
#endif


#include <itkSymmetricEigenAnalysis.h>
#include "itkCovarianceImageFunction.h"

#include <itkMeanSampleFilter.h>
#include <itkCovarianceSampleFilter.h>

using namespace std;

namespace itk {
namespace Statistics {


template< class TSample, class TOutSample >
SamplePCAReduction< TSample, TOutSample >::SamplePCAReduction()
{
    m_Output = 0;
    m_KeepValues = 1;
    m_FeatureVector.Fill(0);
    m_MeanArray = 0;
}

template< class TSample, class TOutSample >
SamplePCAReduction< TSample, TOutSample >::~SamplePCAReduction(){

}


template< class TSample, class TOutSample >
void SamplePCAReduction< TSample, TOutSample >::PrintSelf(std::ostream& os, Indent indent) const{
    Superclass::PrintSelf(os,indent);

    os << indent << "Output: " << m_Output << std::endl;

}

template< class TSample, class TOutSample >
void SamplePCAReduction< TSample, TOutSample >::Update(){
    GenerateData();
}

/** Calculates the covariance and save it. This method calls
 * ComputeCovarianceWithGivenMean, if the user provides mean vector
 * using SetMean method. Otherwise, it calls
 * ComputeCovarianceWithoutGivenMethod depending on */
template< class TSample, class TOutSample >
void SamplePCAReduction< TSample, TOutSample >::GenerateData( void ){

    const MeasurementVectorSizeType measurementVectorSize = this->GetInputSample()->GetMeasurementVectorSize();
    if( measurementVectorSize == 0 )
      {
      itkExceptionMacro( <<
          "Measurement vector size must be set. Use SetMeasurementVectorSize( .. )");
      }

    InputSamplePointerType inputsample = this->GetInputSample();


    typedef itk::Statistics::MeanSampleFilter< InputSampleType > MeanCalculator;
    typename MeanCalculator::Pointer meancalc = MeanCalculator::New();
    meancalc->SetInput(inputsample);
    meancalc->Update();
    typedef typename MeanCalculator::OutputType MeanOutputType;
    const MeanOutputType* originalmean = meancalc->GetOutput();

    m_MeanArray.clear();
    m_MeanArray.set_size ((originalmean->Get()).GetSize());
    m_MeanArray.fill(-1);
    for(unsigned i = 0; i < m_MeanArray.size(); i++){
        m_MeanArray[i] =  (originalmean->Get())[i];
    }

    typename OutputSampleType::Pointer inputtempsample = OutputSampleType::New();
    int size = inputsample->GetMeasurementVectorSize();
    inputtempsample->SetMeasurementVectorSize(size);    

    for(unsigned i = 0; i < inputsample->Size(); i++){
        OutMeasurementVectorType currentvect;
        currentvect.SetSize(inputsample->GetMeasurementVectorSize());
        currentvect.Fill(-1);
        for(unsigned j = 0; j < inputsample->GetMeasurementVectorSize(); j++){
            double mean = (originalmean->Get())[j];
            currentvect[j] = inputsample->GetMeasurementVector(i)[j] - mean;
        }
        inputtempsample->PushBack(currentvect);
    }



    typedef itk::Statistics::CovarianceSampleFilter< OutputSampleType > CovarianceCalculatorType;
    typename CovarianceCalculatorType::Pointer covariancecalc = CovarianceCalculatorType::New();
    covariancecalc->SetInput( inputtempsample );

    covariancecalc->Update();
    MatrixType covmatrix = covariancecalc->GetCovarianceMatrix ();
    MatrixType eigenvectors;
    eigenvectors.SetSize(covmatrix.Rows(), covmatrix.Cols());
    OutMeasurementVectorType eigenvalues;
    eigenvalues.SetSize(covmatrix.Rows());

    typedef itk::SymmetricEigenAnalysis< MatrixType, OutMeasurementVectorType, MatrixType > SymmetricEigenType;
    SymmetricEigenType symmetriceigen;
    symmetriceigen.SetDimension(covmatrix.Cols());
    symmetriceigen.SetOrderEigenValues(true);
    symmetriceigen.ComputeEigenValuesAndVectors(covmatrix, eigenvalues, eigenvectors);


    double total = 0;
    for(unsigned i = 0; i < eigenvalues.Size(); i++){
        total+=eigenvalues[i];
    }
#ifdef WIN32
	if(!(total!=total)){
#else
    if(!isnan(total)){
#endif
        double keepvalues = 0;
        double leastvalue = 0;
        int numberofvectors = 0;
        //eigenvalues are assumed to be organized
        for(int i = eigenvalues.Size()-1; i >= 0; i--){            
            keepvalues+= (eigenvalues[i]/total);
            cout<<"eigen = "<<eigenvalues[i]/total<<endl;
            if(keepvalues >= this->m_KeepValues){
                //numberofvectors to keep that are greater than the value set by the percentage set by the user between [0, 1]
                numberofvectors = eigenvalues.Size() - i;
                leastvalue = eigenvalues[i];
                break;
            }
        }

        /*
         * Forming the feature vector.
         * Taking the eigenvectors that you want to keep from the list of eigenvectors,
         * form a matrix with these eigenvectors in the columns
         *
         */
        //typename InputSampleType::Pointer m_FeatureVector = InputSampleType::New();

        if(numberofvectors == 0){
            itkExceptionMacro( << "The number of vectors is 0, the sample is empty."<<endl);
        }

        m_FeatureVector.SetSize(numberofvectors, eigenvectors.Rows());
        m_FeatureVector.Fill(-1);

        int currentnumbervector = 0;
        //the most important vector is on the last position
        for(int i = eigenvalues.Size()-1; i >= 0; i--){
            if(eigenvalues[i] >= leastvalue){
                for(unsigned j = 0; j < eigenvectors.Rows();j++){
                    m_FeatureVector[currentnumbervector][j] = eigenvectors[i][j];
                }
                currentnumbervector++;
            }else{
                //since the eigenvalues are organized there is no need to continue when the value of the eigenvalue drops from the limit
                break;
            }
        }


        /*
         * mean adjusted data transposed
         * data items are in each column (vector in column) with each row holding a separete dimension
         */
        MatrixType meandata;
        meandata.SetSize(inputsample->Size(), inputsample->GetMeasurementVectorSize());
        meandata.Fill(-1);

        for(unsigned i = 0; i < inputtempsample->Size(); i++){
            for(unsigned j = 0; j < inputtempsample->GetMeasurementVectorSize(); j++){
                meandata[i][j] = inputtempsample->GetMeasurementVector(i)[j];
            }
        }

        //mean data is transposed (last step), with the feature vector transposed, here is where the dimension reduction happens
        m_finaldata = meandata * m_FeatureVector.GetTranspose();

        m_Output = OutputSampleType::New();
        m_Output->SetMeasurementVectorSize(m_finaldata.cols());

        for(unsigned i = 0; i < m_finaldata.rows(); i++){
            OutMeasurementVectorType currentvect;
            currentvect.SetSize(m_finaldata.cols());
            currentvect.Fill(-1);
            for(unsigned j = 0; j < m_finaldata.cols(); j++){
                currentvect[j] = m_finaldata[i][j];
            }
            m_Output->PushBack(currentvect);
        }


        if(this->GetDebug()){

            cout<<"covmatrix "<<endl;
            for(unsigned i = 0; i < covmatrix.Rows(); i++){
                for(unsigned j = 0; j < covmatrix.Cols(); j++){
                    cout<<covmatrix[i][j]<<"\t";
                }
                cout<<endl;
            }


            for(unsigned i = 0; i < eigenvectors.Rows(); i++){
                cout<<"evector "<<i<<": ";
                for(unsigned j = 0; j < eigenvectors.Cols(); j++){
                    cout<<eigenvectors[i][j]<<", ";
                }
                cout<<endl;
            }
            cout<<"eigenvalues ";
            for(unsigned i = 0; i < eigenvalues.Size(); i++){
                cout<<eigenvalues[i]<<", ";
            }

            cout<<"Least value "<<leastvalue<<endl;

            for(unsigned i = 0; i < m_FeatureVector.Rows(); i++){
                cout<<"feature vector "<<i<<": ";
                for(unsigned j = 0; j < m_FeatureVector.Cols(); j++){
                    cout<<m_FeatureVector[i][j]<<", ";
                }
                cout<<endl;
            }

            for(unsigned i = 0; i < m_finaldata.rows(); i++){
                cout<<"final vector "<<i<<": ";
                for(unsigned j = 0; j < m_finaldata.cols(); j++){
                    cout<<m_finaldata[i][j]<<", ";

                }
                cout<<endl;
            }
        }


    }else{

        cout<<"dimensionality reduction failed... outputsample = inputsample"<<endl;

        m_Output = OutputSampleType::New();
        m_Output->SetMeasurementVectorSize(inputsample->GetMeasurementVectorSize());

        m_finaldata.set_size(inputsample->Size(), inputsample->GetMeasurementVectorSize());

        for(unsigned i = 0; i < inputsample->Size(); i++){
            OutMeasurementVectorType currentvect;
            currentvect.SetSize(inputsample->GetMeasurementVectorSize());
            currentvect.Fill(-1);
            for(unsigned j = 0; j < inputsample->GetMeasurementVectorSize(); j++){
                currentvect[j] = inputsample->GetMeasurementVector(i)[j];
                m_finaldata[i][j] = inputsample->GetMeasurementVector(i)[j];
            }

            m_Output->PushBack(currentvect);


        }
    }

}

template< class TSample, class TOutSample >
vnl_vector<double>
SamplePCAReduction< TSample, TOutSample >::GetMeasurementVector(const int index){

    return m_finaldata.get_row(index);
}

/**
 *    Multiplies samplevector with the current m_FeatureVector if it exists
 *    reduces the dimensionality of the vector
 */
template< class TSample, class TOutSample >
vnl_vector<double>
SamplePCAReduction< TSample, TOutSample >::GetPCAReducedVector(const MeasurementVectorType vector){

    if(m_FeatureVector.Cols() == 0){
        vnl_vector<double> vnlvect(vector.Size());
        vnlvect.fill(0);
        for(unsigned i = 0; i <vnlvect.size(); i++){
             vnlvect[i] = vector[i];
        }
        return vnlvect;
    }

    if(m_MeanArray.size() > 0){

        /*
         * mean adjusted data transposed
         * data items are in each column (vector in column) with each row holding a separete dimension
         */
        vnl_vector<double> vnlvect(vector.Size());
        vnlvect.fill(0);

        //the original vector minus the mean at each dim
        for(unsigned i = 0; i <vnlvect.size(); i++){
             vnlvect[i] = vector[i] - m_MeanArray[i];
        }

        //the mean vector multiplied by the feature matrix
        vnl_vector<double> finalvect = vnlvect * this->m_FeatureVector.GetTranspose();

        vnlvect.clear();

        return finalvect;

        /*MeasurementVectorType finalmeasurvect;
        finalmeasurvect.SetSize(finalvect.size());
        finalmeasurvect.Fill(-1);
        for(unsigned i = 0; i <finalvect.size(); i++){
             finalmeasurvect[i] = finalvect[i];
        }
        return finalmeasurvect;*/

    }else{
        itkExceptionMacro(<<"The mean array from the covariance matrix is not set");
    }
}

/**
 *    Given an index converts the pca vector in m_Output into the original vector
 *    OriginalData =  (m_FeatureVector ^(-1) x FinalData(index)) + OriginalMean
 */
template< class TSample, class TOutSample >
vnl_vector<double>
SamplePCAReduction< TSample, TOutSample >::GetOriginalMeasurementVector(const int index){


    if(!m_Output){
        itkExceptionMacro(<<"Execute the update method first"<<endl);
    }

    //MeasurementVectorType finaldata = m_Output->GetMeasurementVector(index);

    return GetOriginalMeasurementVector( GetMeasurementVector(index) );
}

/**
 *    Given a vector converts the pca vector in m_Output into the original vector
 *    OriginalData =  (m_FeatureVector ^(-1) x FinalData) + OriginalMean
 */
template< class TSample, class TOutSample >
vnl_vector<double>
SamplePCAReduction< TSample, TOutSample >::GetOriginalMeasurementVector(const vnl_vector<double> vector){
    if(!m_Output){
        itkExceptionMacro(<<"Execute the update method first"<<endl);
    }


    return (this->m_FeatureVector.GetTranspose() * vector) + m_MeanArray;

    /*MeasurementVectorType finaldata = vector;

    vnl_vector<double> vnlfinaldata(finaldata.Size());
    for(unsigned i = 0; i <vnlfinaldata.size(); i++){
         vnlfinaldata[i] = finaldata[i];
    }

    vnl_vector<double> originaldata = (this->m_FeatureVector.GetTranspose() * vnlfinaldata) + m_MeanArray;

    MeasurementVectorType finaloriginaldata;
    finaloriginaldata.SetSize(originaldata.size());
    finaloriginaldata.Fill(-1);
    for(unsigned i = 0; i < originaldata.size(); i++){
         finaloriginaldata[i] = originaldata[i];
    }
    return finaloriginaldata;*/

}


}
}

#endif
