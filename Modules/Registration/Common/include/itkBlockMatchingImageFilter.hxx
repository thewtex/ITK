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
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
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
  this->SetNumberOfRequiredInputs( 3 );
  this->RemoveRequiredInputName( "Primary" );
  this->AddRequiredInputName( "FixedImage" );
  this->AddRequiredInputName( "MovingImage" );
  this->AddRequiredInputName( "FeaturePoints" );
}

template<
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::~BlockMatchingImageFilter()
{
}

template<
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
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
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
std::vector< SmartPointer< DataObject > >::size_type
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
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
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
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
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
DataObject::Pointer
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::MakeOutput( ProcessObject::DataObjectPointerArraySizeType idx )
{
  DataObject::Pointer output;

  switch ( idx )
    {
      case 0:
        {
          typename DisplacementsType::Pointer displacements = DisplacementsType::New();
          output = displacements;
        }
        break;

      case 1:
        {
          typename SimilaritiesType::Pointer similarities = SimilaritiesType::New();
          output = similarities;
        }
        break;
    }
  return output;
}

template<
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::AllocateOutputs()
{
  this->SetNthOutput( 0, this->MakeOutput(0) );
  this->SetNthOutput( 1, this->MakeOutput(1) );
}

template<
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::BeforeThreadedGenerateData()
{
  m_PointsCount = itk::NumericTraits< SizeValueType >::Zero;
  FeaturePointsConstPointer featurePoints = this->GetFeaturePoints();
  if ( featurePoints )
    {
      m_PointsCount = featurePoints->GetNumberOfPoints();
    }

  if ( m_PointsCount < 1 )
  {
    itkExceptionMacro( "Invalid number of feature points: " << m_PointsCount << "." );
  }

  m_DisplacementsVectorsArray = new DisplacementsVector[ m_PointsCount ];
  m_SimilaritiesValuesArray = new SimilaritiesValue[ m_PointsCount ];
}

template<
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::AfterThreadedGenerateData()
{
  m_PointsCount = itk::NumericTraits< SizeValueType >::Zero;

  FeaturePointsConstPointer featurePoints = this->GetFeaturePoints();
  const typename FeaturePointsType::PointsContainer *points;
  if ( featurePoints )
    {
      points = featurePoints->GetPoints();
    }

  DisplacementsPointer displacements = this->GetDisplacements();

  typedef typename DisplacementsType::PointsContainerPointer  DisplacementsPointsContainerPointerType;
  typedef typename DisplacementsType::PointsContainer         DisplacementsPointsContainerType;
  DisplacementsPointsContainerPointerType displacementsPoints = DisplacementsPointsContainerType::New();

  typedef typename DisplacementsType::PointDataContainerPointer  DisplacementsPointDataContainerPointerType;
  typedef typename DisplacementsType::PointDataContainer         DisplacementsPointDataContainerType;
  DisplacementsPointDataContainerPointerType displacementsData = DisplacementsPointDataContainerType::New();

  SimilaritiesPointer similarities = this->GetSimilarities();

  typedef typename SimilaritiesType::PointsContainerPointer  SimilaritiesPointsContainerPointerType;
  typedef typename SimilaritiesType::PointsContainer         SimilaritiesPointsContainerType;
  SimilaritiesPointsContainerPointerType similaritiesPoints = SimilaritiesPointsContainerType::New();

  typedef typename SimilaritiesType::PointDataContainerPointer  SimilaritiesPointDataContainerPointerType;
  typedef typename SimilaritiesType::PointDataContainer         SimilaritiesPointDataContainerType;
  SimilaritiesPointDataContainerPointerType similaritiesData = SimilaritiesPointDataContainerType::New();

  // insert displacements and similarities
  for ( SizeValueType i = 0; i < m_PointsCount; i++ )
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
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
ITK_THREAD_RETURN_TYPE
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
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
  class TFixedImagePixel,
  class TMovingImagePixel,
  class TFeaturesPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedImagePixel, VImageDimension >,
  Image< TMovingImagePixel, VImageDimension >,
  PointSet< TFeaturesPixel, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >
::ThreadedGenerateData( ThreadIdType threadId ) throw ( ExceptionObject )
{
  FixedImageConstPointer fixedImage = this->GetFixedImage();
  MovingImageConstPointer floatingImage = this->GetMovingImage();
  FeaturePointsConstPointer featurePoints = this->GetFeaturePoints();

  SizeValueType threadCount = this->GetNumberOfThreads();

  // compute first point and number of points (count) for this thread
  SizeValueType count = m_PointsCount / threadCount;
  SizeValueType first = threadId * count;
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
      numberOfVoxelInBlock *= m_BlockRadius[ i ] + 1 + m_BlockRadius[ i ];
    }

  // loop thru feature points
  for ( SizeValueType id = first, last = first + count; id < last; id++ )
    {
      FeaturePointsPhysicalCoordinates originalLocation = featurePoints->GetPoint( id );
      ImageIndexType index;
      fixedImage->TransformPhysicalPointToIndex( originalLocation, index );

      // the block is selected for a minimum similarity metric
      SimilaritiesValue  similarity = NumericTraits< SimilaritiesValue >::Zero;

      // New point location
      DisplacementsVector displacement;

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
      for ( windowIterator.GoToBegin(); !windowIterator.IsAtEnd(); ++windowIterator )
        {
          SimilaritiesValue  valRef = NumericTraits< SimilaritiesValue >::Zero;
          SimilaritiesValue  refMean = NumericTraits< SimilaritiesValue >::Zero;
          SimilaritiesValue  refVariance = NumericTraits< SimilaritiesValue >::Zero;
          SimilaritiesValue  valFlo = NumericTraits< SimilaritiesValue >::Zero;
          SimilaritiesValue  floMean = NumericTraits< SimilaritiesValue >::Zero;
          SimilaritiesValue  floVariance = NumericTraits< SimilaritiesValue >::Zero;
          SimilaritiesValue  covariance = NumericTraits< SimilaritiesValue >::Zero;

          // iterate over voxels in blockRadius
          for ( SizeValueType i = 0; i < numberOfVoxelInBlock; i++ ) // windowIterator.Size() == numberOfVoxelInBlock
            {
              valRef = windowIterator.GetPixel( i );
              valFlo = centerIterator.GetPixel( i );
              floMean += valFlo;
              refMean += valRef;
              floVariance += valFlo * valFlo;
              refVariance += valRef * valRef;
              covariance += valRef * valFlo;
            }
          refMean /= numberOfVoxelInBlock;
          floMean /= numberOfVoxelInBlock;
          refVariance -= numberOfVoxelInBlock * refMean * refMean;
          floVariance -= numberOfVoxelInBlock * floMean * floMean;
          covariance -= numberOfVoxelInBlock *  refMean * floMean;

          SimilaritiesValue sim = NumericTraits< SimilaritiesValue >::Zero;
          if ( refVariance * floVariance )
            {
              sim = covariance * covariance / refVariance / floVariance;
            }

          if ( sim >= similarity )
            {
              FeaturePointsPhysicalCoordinates newLocation;
              fixedImage->TransformIndexToPhysicalPoint( windowIterator.GetIndex(), newLocation );
              displacement = newLocation - originalLocation;
              similarity = sim;
            }
        }
      m_DisplacementsVectorsArray[ id ] = displacement;
      m_SimilaritiesValuesArray[ id ] = similarity;
    }
}

} // end namespace itk

#endif
