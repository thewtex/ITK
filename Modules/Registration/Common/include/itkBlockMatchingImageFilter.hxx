/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkBlockMatchingImageFilter_hxx
#define __itkBlockMatchingImageFilter_hxx

#include "itkBlockMatchingImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include <limits>


namespace itk
{
template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::BlockMatchingImageFilter()
{
  // defaults
  m_BlockRadius.Fill( 2 );
  m_SearchRadius.Fill( 3 );
  m_BlockStep.Fill( 1 );

  // all inputs are required
  this->RemoveRequiredInputName("Primary");
  this->AddRequiredInputName("FixedImage");
  this->AddRequiredInputName("MovingImage");
  this->AddRequiredInputName("FeaturePoints");
}

template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::~BlockMatchingImageFilter()
{
}

template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Number of threads: " << this->GetNumberOfThreads() << std::endl
     << indent << "m_BlockRadius: " << m_BlockRadius << std::endl
     << indent << "m_SearchRadius: " << m_SearchRadius << std::endl
     << indent << "m_BlockStep: " << m_BlockStep << std::endl;
}

template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
std::vector< SmartPointer< DataObject > >::size_type
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::GetNumberOfValidRequiredInputs() const
{
  typename std::vector< SmartPointer< DataObject > >::size_type num = 0;

  if ( this->GetFixedImage() )
  {
    num++;
  }

  if ( this->GetMovingImage() )
  {
    num++;
  }

  if ( this->GetFeaturePoints() )
  {
    num++;
  }

  return num;
}

template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::GenerateData()
{
  // Call a method that can be overriden by a subclass to allocate
  // memory for the filter's outputs
  this->AllocateOutputs();

  // Call a method that can be overridden by a subclass to perform
  // some calculations prior to splitting the main computations into
  // separate threads
  this->BeforeThreadedGenerateData();

  // Set up the multithreaded processing
  ThreadStruct str;
  str.Filter = this;

  this->GetMultiThreader()->SetNumberOfThreads( this->GetNumberOfThreads() );
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallback, &str);

  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Call a method that can be overridden by a subclass to perform
  // some calculations after all the threads have completed
  this->AfterThreadedGenerateData();
}

template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
DataObject::Pointer
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::MakeOutput( ProcessObject::DataObjectPointerArraySizeType idx )
{
  DataObject::Pointer output;

  switch ( idx )
    {
    case 0:
      output = ( DisplacementsType::New() ).GetPointer();
      break;
    case 1:
      output = ( SimilaritiesType::New() ).GetPointer();
      break;
    }
  return output.GetPointer();
}

template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::AllocateOutputs()
{
  this->SetNthOutput( 0, this->MakeOutput(0) );
  this->SetNthOutput( 1, this->MakeOutput(1) );
}

template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::BeforeThreadedGenerateData()
{
  m_PointsCount = this->GetFeaturePoints()->GetNumberOfPoints();
  if ( m_PointsCount < 1 )
  {
    itkExceptionMacro( "Invalid number of feature points: " << m_PointsCount << "." );
  }

  m_DisplacementsVectorsArray = new DisplacementsVector[ m_PointsCount ];
  m_SimilaritiesValuesArray = new SimilaritiesValue[ m_PointsCount ];
}

template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::AfterThreadedGenerateData()
{
  const typename FeaturePointsType::PointsContainer *points = this->GetFeaturePoints()->GetPoints();

  DisplacementsPointer displacements = this->GetDisplacements();
  typename DisplacementsType::PointsContainerPointer
    displacementsPoints = DisplacementsType::PointsContainer::New();
  typename DisplacementsType::PointDataContainerPointer
    displacementsData = DisplacementsType::PointDataContainer::New();

  SimilaritiesPointer similarities = this->GetSimilarities();
  typename SimilaritiesType::PointsContainerPointer
    similaritiesPoints = SimilaritiesType::PointsContainer::New();
  typename SimilaritiesType::PointDataContainerPointer
    similaritiesData = SimilaritiesType::PointDataContainer::New();

  // insert displacements and similarities
  for ( unsigned i = 0; i < m_PointsCount; i++ )
    {
      displacementsPoints->InsertElement( i, points->GetElement( i ) );
      similaritiesPoints->InsertElement( i, points->GetElement( i ) );
      displacementsData->InsertElement( i, m_DisplacementsVectorsArray[ i ] );
      similaritiesData->InsertElement( i, m_SimilaritiesValuesArray[ i ] );
    }

  displacements->SetPoints( displacementsPoints );
  displacements->SetPointData( displacementsData );
  similarities->SetPoints( similaritiesPoints );
  similarities->SetPointData( similaritiesData );

  // clean up
  delete[] m_DisplacementsVectorsArray;
  delete[] m_SimilaritiesValuesArray;
}

// Callback routine used by the threading library. This routine just calls
// the ThreadedGenerateData method after setting the correct region for this
// thread.
template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
ITK_THREAD_RETURN_TYPE
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::ThreaderCallback(void *arg)
{
  ThreadStruct *str = (ThreadStruct *)( ( (MultiThreader::ThreadInfoStruct *)( arg ) )->UserData );
  ThreadIdType threadId = ( (MultiThreader::ThreadInfoStruct *)( arg ) )->ThreadID;

  str->Filter->ThreadedGenerateData( threadId );

  return ITK_THREAD_RETURN_VALUE;
}

template<
  class TFixedPixel,
  class TMovingPixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TMovingPixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::ThreadedGenerateData( ThreadIdType threadId ) throw ( ExceptionObject )
{
  FixedImageConstPointer fixedImage = this->GetFixedImage();
  MovingImageConstPointer floatingImage = this->GetMovingImage();
  FeaturePointsConstPointer featurePoints = this->GetFeaturePoints();

  unsigned threadCount = this->GetNumberOfThreads();

  // compute first point and number of points (count) for this thread
  unsigned count = m_PointsCount / threadCount;
  unsigned first = threadId * count;
  if ( threadId + 1 == threadCount ) // last thread
    {
      count += m_PointsCount % threadCount;
    }

  // start constructing window region and center region (single voxel)
  ImageRegionType window;
  ImageRegionType center;
  ImageSizeType windowSize;
  windowSize.Fill( 1 );
  center.SetSize( windowSize ); // size of center region is 1
  windowSize += m_SearchRadius + m_SearchRadius;
  window.SetSize( windowSize ); // size of window region is 1+2*m_BlockHalfWindow

  // start constructing block iterator
  SizeValueType numberOfVoxelInBlock = 1;
  for ( unsigned i = 0; i < ImageSizeType::Dimension; i++ )
    {
      numberOfVoxelInBlock *= 1 + m_BlockRadius[ i ] + m_BlockRadius[ i ];
    }

  // loop thru feature points
  for ( unsigned id = first, last = first + count; id < last; id++ )
    {
      FeaturePointsPhysicalCoordinates originalLocation = featurePoints->GetPoint( id );
      ImageIndexType index;
      fixedImage->TransformPhysicalPointToIndex( originalLocation, index );

      // the block is selected for a minimum similarity metric
      SimilaritiesValue  similarity = std::numeric_limits< TSimilarity >::max();
      SimilaritiesValue  crit = 0.0;

      // New point location
      DisplacementsVector displacement;
      displacement.Fill( 0.0 ); // is this necessary?

      // set centers of window and center regions to current location
      ImageIndexType start = index - m_SearchRadius;
      window.SetIndex( start );
      center.SetIndex( index );

      // iterate over neighbourhoods in region window, for each neighbourhood: iterate over voxels in blockRadius
      ConstNeighborhoodIterator< FixedImageType > windowIterator( m_BlockRadius, fixedImage, window );

      // iterate over voxels in neighbourhood of current feature point
      ConstNeighborhoodIterator< MovingImageType > centerIterator( m_BlockRadius, floatingImage, center );
      centerIterator.GoToBegin();

      // iterate over neighbourhoods in region window
      for ( windowIterator.GoToBegin(); !windowIterator.IsAtEnd(); ++windowIterator ) // TODO: += m_BlockStep
        {
          SimilaritiesValue  valRef = 0.0;
          SimilaritiesValue  refMean = 0.0;
          SimilaritiesValue  refVariance = 0.0;
          SimilaritiesValue  valFlo = 0.0;
          SimilaritiesValue  floMean = 0.0;
          SimilaritiesValue  floVariance = 0.0;
          SimilaritiesValue  cov = 0.0;

          // iterate over voxels in blockRadius
          for ( unsigned i = 0; i < numberOfVoxelInBlock; i++ ) // windowIterator.Size() == numberOfVoxelInBlock
            {
              valRef = windowIterator.GetPixel( i );
              valFlo = centerIterator.GetPixel( i );
              floMean += valFlo;
              refMean += valRef;
              floVariance += valFlo * valFlo;
              refVariance += valRef * valRef;
              cov += valRef * valFlo;
            }
          refMean /= numberOfVoxelInBlock;
          floMean /= numberOfVoxelInBlock;
          refVariance -= numberOfVoxelInBlock * refMean * refMean;
          floVariance -= numberOfVoxelInBlock * floMean * floMean;
          cov -= numberOfVoxelInBlock *  refMean * floMean;
          crit = ( refVariance * floVariance ) ? 1.0 - ( cov * cov / ( refVariance * floVariance ) ) : 0.0;

          if ( crit < similarity )
          {
            FeaturePointsPhysicalCoordinates newLocation;
            fixedImage->TransformIndexToPhysicalPoint( windowIterator.GetIndex(), newLocation );
            displacement = newLocation - originalLocation;
            similarity = crit;
          }
        }
      m_DisplacementsVectorsArray[ id ] = displacement;
      m_SimilaritiesValuesArray[ id ] = 1.0 - crit;
    }
}

} // end namespace itk

#endif
