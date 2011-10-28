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
//#include <cmath>
#include <limits>
/*
#include "itkImageRegionIterator.h"
#include <vector>
#include <algorithm>
*/


namespace itk
{
template<
  class TFixedPixel,
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
::BlockMatchingImageFilter()
{
  // defaults
  m_CheckBoundary = false;
  m_BlockHalfSize.Fill( 2 );
  m_BlockHalfWindow.Fill( 3 );
  m_BlockStep.Fill( 1 );

  // all inputs are required
  this->SetNumberOfRequiredInputs( 3 );
}

template<
  class TFixedPixel,
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
::~BlockMatchingImageFilter()
{
}

template<
  class TFixedPixel,
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Number of threads: " << this->GetNumberOfThreads() << std::endl
     << indent << "m_CheckBoundary: " << ( m_CheckBoundary ? "on" : "off" ) << std::endl
     << indent << "m_BlockHalfSize: " << m_BlockHalfSize << std::endl
     << indent << "m_BlockHalfWindow: " << m_BlockHalfWindow << std::endl
     << indent << "m_BlockStep: " << m_BlockStep << std::endl;
}

template<
  class TFixedPixel,
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
std::vector< SmartPointer< DataObject > >::size_type
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
::GetNumberOfValidRequiredInputs() const
{
  typename std::vector< SmartPointer< DataObject > >::size_type num = 0;

  if ( this->GetFixedImage() )
  {
    num++;
  }

  if ( this->GetFloatingImage() )
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
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
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
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
DataObject::Pointer
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
  ::MakeOutput(unsigned idx)
{
  DataObject::Pointer output;

  switch ( idx )
    {
    case 0:
      output = ( DisplacementsType::New() ).GetPointer();
      break;
    case 1:
      break;
    }
  return output.GetPointer();
}

template<
  class TFixedPixel,
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
::AllocateOutputs()
{
  this->SetNthOutput( 0, this->MakeOutput(0) );
}

template<
  class TFixedPixel,
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
::BeforeThreadedGenerateData()
{
  // if boundary check for feature points is enabled
  if ( m_CheckBoundary )
    {
      FeaturePointsConstPointer inputFeaturePoints = this->GetFeaturePoints();
      FeaturePointsPhysicalCoordinates physical;
      FeaturePointsIndexCoordinates index;

      ImageRegionType safeRegion = this->GetFixedImage()->GetLargestPossibleRegion();
      ImageSizeType safeSize
        = safeRegion.GetSize() - m_BlockHalfSize - m_BlockHalfWindow - m_BlockHalfSize - m_BlockHalfWindow;
      safeRegion.SetSize( safeSize );
      ImageIndexType safeIndex;
      safeIndex.Fill( 0 );
      safeIndex += m_BlockHalfSize + m_BlockHalfWindow;
      safeRegion.SetIndex( safeIndex );

      for ( int i = 0, n = inputFeaturePoints->GetNumberOfPoints(); i < n; i++ )
        {
          if ( inputFeaturePoints->GetPoint( i, &physical ) && inputFeaturePoints->GetPointData( i, &index ) )
            {
              // check if feature points lie within boundaries
              if ( !safeRegion.IsInside( index ) )
                {
                  itkExceptionMacro( "Feature point " << i << ":" << index
                    << " is not within the safe region:" << safeRegion << "." );
                }
            }
          else // in the input point set: point, pixel or both do not exist for indentifier i
            {
              itkExceptionMacro( "Missing feature point with identifier " << i << "." );
            }
        }
    }

  m_PointsCount = this->GetFeaturePoints()->GetNumberOfPoints();
  m_DisplacementsNewPhysicalLocationArray = new DisplacementsNewPhysicalLocation[ m_PointsCount ];
  m_DisplacementsSimilarityArray = new DisplacementsSimilarity[ m_PointsCount ];
}

template<
  class TFixedPixel,
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
::AfterThreadedGenerateData()
{
  DisplacementsPointer displacements = this->GetOutput( 0 );
  DisplacementsType::PointsContainerPointer points = DisplacementsType::PointsContainer::New();
  DisplacementsType::PointDataContainerPointer
    pointData = DisplacementsType::PointDataContainer::New();

  // insert displacements and similarities
  for ( int i = 0; i < m_PointsCount; i++ )
    {
      points->InsertElement( i, m_DisplacementsNewPhysicalLocationArray[ i ] );
      pointData->InsertElement( i, m_DisplacementsSimilarityArray[ i ] );
    }

  displacements->SetPoints( points );
  displacements->SetPointData( pointData );

  // clean up
  delete[] m_DisplacementsNewPhysicalLocationArray;
  delete[] m_DisplacementsSimilarityArray;

}

// Callback routine used by the threading library. This routine just calls
// the ThreadedGenerateData method after setting the correct region for this
// thread.
template<
  class TFixedPixel,
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
ITK_THREAD_RETURN_TYPE
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
::ThreaderCallback(void *arg)
{
  ThreadStruct *str = (ThreadStruct *)( ( (MultiThreader::ThreadInfoStruct *)( arg ) )->UserData );
  ThreadIdType threadId = ( (MultiThreader::ThreadInfoStruct *)( arg ) )->ThreadID;

  str->Filter->ThreadedGenerateData( threadId );

  return ITK_THREAD_RETURN_VALUE;
}

template<
  class TFixedPixel,
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacementsTraits,
  class TSimilarity,
  unsigned VImageDimension >
void
BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< TSimilarity, VImageDimension, TDisplacementsTraits > >
::ThreadedGenerateData( ThreadIdType threadId ) throw ( ExceptionObject )
{
  FixedImageConstPointer fixedImage = this->GetFixedImage();
  FloatingImageConstPointer floatingImage = this->GetFloatingImage();
  FeaturePointsConstPointer featurePoints = this->GetFeaturePoints();

  int threadCount = this->GetNumberOfThreads();

  // compute first point and number of points (count) for this thread
  int count = m_PointsCount / threadCount;
  int first = threadId * count;
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
  windowSize += m_BlockHalfWindow + m_BlockHalfWindow;
  window.SetSize( windowSize ); // size of window region is 1+2*m_BlockHalfWindow

  // start constructing block iterator
  ImageSizeType blockRadius;
  blockRadius.Fill ( 1 );
  blockRadius += m_BlockHalfSize + m_BlockHalfSize;

  SizeValueType numberOfVoxelInBlock = 1;
  for ( unsigned i = 0; i < ImageSizeType::Dimension; i++ )
    {
      numberOfVoxelInBlock *= blockRadius[ i ];
    }

  // loop thru feature points
  for ( unsigned id = first, last = first + count; id < last; id++ )
    {
      FeaturePointsIndexCoordinates index;
      featurePoints->GetPointData( id, &index );

      // the block is selected for a minimum similarity metric
      DisplacementsSimilarity  similarity = std::numeric_limits< TSimilarity >::max();
      DisplacementsSimilarity  crit;

      // New point location
      DisplacementsNewPhysicalLocation displacement;

      // set centers of window and center regions to current location
      ImageIndexType start = index - m_BlockHalfWindow;
      window.SetIndex( start );
      center.SetIndex( index );

      // iterate over neighbourhoods in region window, for each neighbourhood: iterate over voxels in blockRadius
      ConstNeighborhoodIterator< FixedImageType > windowIterator( blockRadius, fixedImage, window );

      // iterate over voxels in neighbourhood of current feature point
      ConstNeighborhoodIterator< FloatingImageType > centerIterator( blockRadius, floatingImage, center );
      centerIterator.GoToBegin();

      // iterate over neighbourhoods in region window
      for ( windowIterator.GoToBegin(); !windowIterator.IsAtEnd(); ++windowIterator ) // TODO: += m_BlockStep
        {
          DisplacementsSimilarity  valRef = 0.0;
          DisplacementsSimilarity  refMean = 0.0;
          DisplacementsSimilarity  refVariance = 0.0;
          DisplacementsSimilarity  valFlo = 0.0;
          DisplacementsSimilarity  floMean = 0.0;
          DisplacementsSimilarity  floVariance = 0.0;
          DisplacementsSimilarity  cov = 0.0;

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

          floVariance -= numberOfVoxelInBlock * floMean*floMean;
          cov -= numberOfVoxelInBlock *  refMean * floMean;
          crit = 1.0 - ( cov*cov / (refVariance*floVariance) );

          if ( crit < similarity )
          {
            fixedImage->TransformIndexToPhysicalPoint( windowIterator.GetIndex(), displacement );
            similarity = crit;
          }
        }
      m_DisplacementsNewPhysicalLocationArray[ id ] = displacement;
      m_DisplacementsSimilarityArray[ id ] = crit;
    }
}

} // end namespace itk

#endif
