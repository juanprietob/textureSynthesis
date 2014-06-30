/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkSampleMeanShiftClusteringFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSampleMeanShiftClusteringFilter_txx
#define __itkSampleMeanShiftClusteringFilter_txx

#include "vnl/vnl_math.h"

#include "itkKdTreeGenerator.h"


namespace itk { 
namespace Statistics {

template< class TSample >
SampleMeanShiftClusteringFilter< TSample >
::SampleMeanShiftClusteringFilter()
{
  m_Threshold = 0.5;
  m_MinimumClusterSize = 16;
}

template< class TSample >
SampleMeanShiftClusteringFilter< TSample >
::~SampleMeanShiftClusteringFilter()
{
}

template< class TSample >
void
SampleMeanShiftClusteringFilter< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Threshold: " << m_Threshold << std::endl;
  os << indent << "Minimum cluster size: " << m_MinimumClusterSize
     << std::endl;
}

template< class TSample >
void
SampleMeanShiftClusteringFilter< TSample >
::GenerateData()
{
  const TSample* inputSample = this->GetInputSample();
  m_Output.clear();
  m_Output.resize( inputSample->Size(), 0 );
  int currentLabel = 1;
  unsigned long currentClusterSize;
  SearchResultVectorType queryPoints;
  SearchResultVectorType queryPoints2;
  MeasurementVectorType tempQueryPoint;
  MeasurementVectorType tempPreviousQueryPoint;
  bool searchLoopBegin;
  SearchResultVectorType tempSearchResult;
  SearchResultVectorType* currentQueryPoints;
  SearchResultVectorType* nextQueryPoints;
  SearchResultVectorType* tempQueryPoints;
  typename SearchResultVectorType::iterator cp_iter;
  typename SearchResultVectorType::iterator cp_end; 
  typename SearchResultVectorType::iterator sr_iter;
  typename SearchResultVectorType::iterator sr_end;
  typename TSample::ConstIterator iter = inputSample->Begin();
  typename TSample::ConstIterator end = inputSample->End();

  typedef typename itk::Statistics::KdTreeGenerator< TSample > TreeGeneratorType;
    typename TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();
    treeGenerator->SetSample( const_cast<TSample*>(inputSample) );
    treeGenerator->SetBucketSize( 16 );
    treeGenerator->Update();

    typedef typename TreeGeneratorType::KdTreeType TreeType;
    typedef typename TreeType::NearestNeighbors NeighborsType;
    typedef typename TreeType::KdTreeNodeType NodeType;

    typename TreeType::Pointer tree = treeGenerator->GetOutput();



  while ( iter != end )
    {
    if ( m_Output[iter.GetInstanceIdentifier()] == 0 )
      {
      currentClusterSize = 0;
      currentQueryPoints = &queryPoints;
      nextQueryPoints = &queryPoints2;
      currentQueryPoints->clear();
      nextQueryPoints->clear();
      currentQueryPoints->push_back( iter.GetInstanceIdentifier() );
      searchLoopBegin = true;
      tempPreviousQueryPoint.Fill(0);
      while ( currentQueryPoints->size() > 0 )
        {
        cp_iter = currentQueryPoints->begin();
        cp_end = currentQueryPoints->end();
        while ( cp_iter != cp_end )
          {
          tempQueryPoint = inputSample->GetMeasurementVector( ( *cp_iter) );

          if ( !searchLoopBegin && tempQueryPoint == tempPreviousQueryPoint )
            {
            break;
            }

          tempSearchResult.clear();

          tree->Search( tempQueryPoint, m_Threshold, tempSearchResult ) ;
          //inputSample->Search( tempQueryPoint, m_Threshold, tempSearchResult );

          tempPreviousQueryPoint = tempQueryPoint;
          if ( searchLoopBegin )
            {
            searchLoopBegin = false;
            }

          sr_iter = tempSearchResult.begin();
          sr_end = tempSearchResult.end();
          while ( sr_iter != sr_end )
            {
            if ( m_Output[*sr_iter] == 0 ) 
              {
              m_Output[*sr_iter] = currentLabel;
              nextQueryPoints->push_back( *sr_iter );
              ++currentClusterSize;
              }
            ++sr_iter;
            } // end of while sr_iter
          ++cp_iter;
          } // end of cp_iter
        tempQueryPoints = currentQueryPoints;
        currentQueryPoints = nextQueryPoints;
        nextQueryPoints = tempQueryPoints;
        nextQueryPoints->clear();
        } // end of while (cp->size())

      if ( currentClusterSize < m_MinimumClusterSize )
        {
        }
      else
        {
        itkDebugMacro(<< "cluster label = " << currentLabel
                      << " cluster size = " << currentClusterSize);
        ++currentLabel;
        }
      } // end of if
    ++iter;
    }
}


} // end of namespace Statistics 
} // end of namespace itk

#endif
