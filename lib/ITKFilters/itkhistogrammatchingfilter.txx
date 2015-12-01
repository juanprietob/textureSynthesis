
#ifndef ITKHISTOGRAMMATCHINGFILTER_TXX
#define ITKHISTOGRAMMATCHINGFILTER_TXX


namespace itk{

template<class TInputImage,class TOutputImage >
HistogramMatchingFilter< TInputImage, TOutputImage>::HistogramMatchingFilter()
{
    m_TexelDimension = 0;
    m_InputImageObject = 0;
    m_InputImageSource = 0;
    m_ImageMask = 0;
    m_CalculateOutputImage = true;
    m_NBins = 1024;

}

template<class TInputImage,class TOutputImage >
HistogramMatchingFilter< TInputImage, TOutputImage>::~HistogramMatchingFilter()
{
    m_LutObjectVect.clear();
    m_LutSampleVect.clear();
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
template<class TInputImage,class TOutputImage >
void HistogramMatchingFilter< TInputImage, TOutputImage>::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId ){

    if(m_CalculateOutputImage){

        OutputImagePointer inputmageobject = this->GetInputImageObject();

        OutputImageRegionIteratorType inputobjectit(inputmageobject, outputRegionForThread);
        //OutputImageRegionIteratorType inputobjectit(inputmageobject, inputmageobject->GetLargestPossibleRegion());

        inputobjectit.GoToBegin();

        OutputImagePointer matchimageobject = this->GetOutput();

        OutputImageRegionIteratorType matchobjectit(matchimageobject, outputRegionForThread);
        //OutputImageRegionIteratorType matchobjectit(matchimageobject, matchimageobject->GetLargestPossibleRegion());


        inputobjectit.GoToBegin();
        matchobjectit.GoToBegin();

        InputImageMaskRegionIteratorType itmask;
        if(m_ImageMask){
            itmask = InputImageMaskRegionIteratorType(m_ImageMask, outputRegionForThread);
        }

        //if(threadId == 0)
        {
            while(!inputobjectit.IsAtEnd()){

                OutputPixelType pixobj = inputobjectit.Get();
                OutputPixelType matchpix;
                matchpix.Fill(0);

                bool object = true;

                if(m_ImageMask && !itmask.Get()){
                    object = false;
                }

                if(object){
                    for(unsigned i = 0; i < pixobj.Size(); i++){

                        double cdf[3];

                        m_LutObjectVect[i]->GetColor((double)pixobj[i], cdf);

                        double color[3];
                        m_LutSampleVect[i]->GetColor(cdf[0], color);


                        matchpix[i] = color[0];

                        //cout<<(double)pixobj[i]<<",\t"<<cdf[0]<<",\t "<<color[0]<<endl;
                    }
                }

                matchobjectit.Set(matchpix);

                ++inputobjectit;
                ++matchobjectit;
                if(m_ImageMask){
                    ++itmask;
                }


            }
        }
    }


}


/** The GenerateData method normally allocates the buffers for all of the
 * outputs of a filter. Some filters may want to override this default
 * behavior. For example, a filter may have multiple outputs with
 * varying resolution. Or a filter may want to process data in place by
 * grafting its input to its output. */
template<class TInputImage,class TOutputImage >
void HistogramMatchingFilter< TInputImage, TOutputImage>::AllocateOutputs(){

    if(m_InputImageObject && m_CalculateOutputImage){

        OutputImagePointer imageobject = this->GetOutput();

        imageobject->SetRegions(m_InputImageObject->GetLargestPossibleRegion());
        imageobject->SetSpacing(m_InputImageObject->GetSpacing());
        imageobject->SetOrigin(m_InputImageObject->GetOrigin());
        imageobject->Allocate();

        OutputPixelType texel;
        texel.Fill(0);
        imageobject->FillBuffer(texel);

    }

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
template<class TInputImage,class TOutputImage >
void HistogramMatchingFilter< TInputImage, TOutputImage>::BeforeThreadedGenerateData(){



    m_HistogramObjectVect.clear();
    m_LutObjectVect.clear();

    m_HistogramSampleVect.clear();
    m_LutSampleVect.clear();

    for(unsigned j = 0; j< m_TexelDimension; j++){

        //typename HistogramGeneratorObjectType::Pointer histogramobject = HistogramGeneratorObjectType::New();
        if(m_ImageMask){

            MaskedHistogramGeneratorObjectPointerType maskedhisto = MaskedHistogramGeneratorObjectType::New();
            maskedhisto->SetMaskImage(m_ImageMask);
            m_HistogramObjectVect.push_back((HistogramObjectPointerType)maskedhisto);
        }else{
            m_HistogramObjectVect.push_back(HistogramGeneratorObjectType::New());
        }

        m_HistogramSampleVect.push_back(HistogramGeneratorSampleType::New());

        typedef typename HistogramGeneratorObjectType::HistogramSizeType   HistogramGeneratorObjectSizeType;
        HistogramGeneratorObjectSizeType sizeobj(m_TexelDimension);


        typedef typename HistogramGeneratorSampleType::HistogramSizeType   HistogramGeneratorSampleSizeType;
        HistogramGeneratorSampleSizeType sizesamp(m_TexelDimension);

        int nbins = m_NBins;

        for(unsigned k = 0; k < m_TexelDimension; k++){
            if(k != j){
                sizeobj[k] = 1;
                sizesamp[k] = 1;
            }else{
                sizeobj[k] = nbins;
                sizesamp[k] = nbins;
            }
        }

        m_HistogramObjectVect[j]->SetHistogramSize( sizeobj );
        m_HistogramObjectVect[j]->SetAutoMinimumMaximum(true);        
        m_HistogramObjectVect[j]->SetInput(  this->GetInputImageObject()  );
        m_HistogramObjectVect[j]->Update();

        m_LutObjectVect.push_back(vtkSmartPointer<vtkColorTransferFunction>::New());

        double cdfobject = 0;
        double totalfreq = m_HistogramObjectVect[j]->GetOutput()->GetTotalFrequency();
        //cout<<"total freq: "<<totalfreq<<endl;
        typedef typename HistogramGeneratorObjectType::HistogramType HistogramType;
        HistogramType* histogram = m_HistogramObjectVect[j]->GetOutput();

        for(unsigned i = 0; i < histogram->Size(); i++){

            //cout<<m_HistogramObjectVect[j]->GetOutput()->GetFrequency(i)<<endl;
            if(histogram->GetFrequency(i) != 0){

                double freq = histogram->GetFrequency(i);

                //cout<<"freq:"<<freq<<endl;

                cdfobject += freq/totalfreq;
                //cout<<cdfobject<<endl;


                //cout<<color[j]<<endl;
                m_LutObjectVect[j]->AddRGBPoint(histogram->GetMeasurement(i, j), cdfobject, 0, 0);
                //m_LutObjectVect[j]->MapValue(color[j]);
                //m_LutObjectVect[j]->SetTableValue(m_LutObjectVect[j]->GetIndex(color[j]), cdfobject, 0, 0);
            }
        }
        //m_LutObjectVect[j]->Build();



        //for(unsigned k = 0; k < m_LutObjectVect[j]->GetSize(); k++){
        //    double d[6];
        //    m_LutObjectVect[j]->GetNodeValue(k, d);
        //    cout<<d[1]<<endl;
        //}

        //cout<<"cdf "<<cdfobject<<endl;


        m_HistogramSampleVect[j]->SetHistogramSize( sizesamp );
        m_HistogramSampleVect[j]->SetAutoMinimumMaximum(true);
        m_HistogramSampleVect[j]->SetInput(  this->GetInputImageSource()  );
        m_HistogramSampleVect[j]->Update();

        m_LutSampleVect.push_back(vtkSmartPointer<vtkColorTransferFunction>::New());

        double cdfsample = 0;
        typedef typename HistogramGeneratorSampleType::HistogramType HistogramSampleType;
        HistogramSampleType* histogramsample = m_HistogramSampleVect[j]->GetOutput();
        double totalfreqsample = histogramsample->GetTotalFrequency();

        for(unsigned i = 0; i < histogramsample->Size(); i++){

            if(histogramsample-> GetFrequency(i) != 0){

                double freq = histogramsample->GetFrequency(i);
                cdfsample += (freq/totalfreqsample);                

                m_LutSampleVect[j]->AddRGBPoint(cdfsample, histogramsample->GetMeasurement(i, j), 0, 0);
            }
        }

        if(totalfreq == 0 && totalfreqsample == 0){
            m_CalculateOutputImage = false;
        }

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
template<class TInputImage,class TOutputImage >
void HistogramMatchingFilter< TInputImage, TOutputImage>::AfterThreadedGenerateData(){

}


template<class TInputImage,class TOutputImage >
double HistogramMatchingFilter< TInputImage, TOutputImage>::GetFrequencySource(int n, HistogramMeasurementVectorType measurement, bool normalize){

    typedef typename HistogramGeneratorSampleType::HistogramType HistogramSampleType;
    HistogramSampleType* histogramsample = m_HistogramSampleVect[n]->GetOutput();

    HistogramIndexType index;
    if(histogramsample->GetIndex(measurement, index)){
        if(normalize){
            double freq = histogramsample->GetFrequency(index);
            double totalfreq = histogramsample->GetTotalFrequency();
            return freq/totalfreq;
        }else{
            return histogramsample->GetFrequency(index);
        }
    }
    return 0;
}


template<class TInputImage,class TOutputImage >
double HistogramMatchingFilter< TInputImage, TOutputImage>::GetFrequencyObject(int n, HistogramMeasurementVectorObjectType measurement, bool normalize){
    typedef typename MaskedHistogramGeneratorObjectType::HistogramType HistogramSampleType;
    HistogramSampleType* histogramobject = m_HistogramObjectVect[n]->GetOutput();

    HistogramIndexType index;
    if(histogramobject->GetIndex(measurement, index)){
        if(normalize){
            double freq = histogramobject->GetFrequency(index);
            double totalfreq = histogramobject->GetTotalFrequency();
            return freq/totalfreq;
        }else{
            return histogramobject->GetFrequency(index);
        }
    }
    return 0;

}

template<class TInputImage,class TOutputImage >
void HistogramMatchingFilter< TInputImage, TOutputImage>::UpdateFrequencyObject(HistogramMeasurementVectorObjectType measuresample, HistogramMeasurementVectorObjectType measuresample1){

    typedef typename MaskedHistogramGeneratorObjectType::HistogramType HistogramSampleType;

    for(unsigned i = 0; i < m_HistogramObjectVect.size(); i++){
        HistogramSampleType* histogramobject = m_HistogramObjectVect[i]->GetOutput();

        HistogramIndexType index;
        if(histogramobject->GetIndex(measuresample, index)){
            histogramobject->SetFrequency(histogramobject->GetFrequency(index) - 1);
        }

        if(histogramobject->GetIndex(measuresample1, index)){
            histogramobject->SetFrequency(histogramobject->GetFrequency(index) + 1);
        }
    }
}


}

#endif
