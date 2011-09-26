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
#ifndef __itkMaskFeaturePointSelectionFilter_hxx
#define __itkMaskFeaturePointSelectionFilter_hxx

#include <map>
#include "vnl/vnl_trace.h"
#include "itkMaskFeaturePointSelectionFilter.h"
#include "itkNeighborhood.h"
#include "itkNumericTraits.h"


namespace itk
{
template<
  class TImagePixel,
  class TMaskPixel,
  class TTensorValueType,
  class TFeaturesTraits,
  unsigned VImageDimension >
MaskFeaturePointSelectionFilter<
  Image< TImagePixel, VImageDimension >,
  Image< TMaskPixel, VImageDimension >,
  PointSet< Matrix< TTensorValueType, VImageDimension, VImageDimension>, VImageDimension, TFeaturesTraits > >
::MaskFeaturePointSelectionFilter()
{
  // default parameters
  m_NonConnectivity = Self::VERTEX_CONNECTIVITY;
  m_SelectFraction = 0.1;
  m_BlockRadius.Fill( 2 );
  m_ComputeStructureTensors = true;
}

template<
  class TImagePixel,
  class TMaskPixel,
  class TTensorValueType,
  class TFeaturesTraits,
  unsigned VImageDimension >
MaskFeaturePointSelectionFilter<
  Image< TImagePixel, VImageDimension >,
  Image< TMaskPixel, VImageDimension >,
  PointSet< Matrix< TTensorValueType, VImageDimension, VImageDimension>, VImageDimension, TFeaturesTraits > >
::~MaskFeaturePointSelectionFilter()
{
}

template<
  class TImagePixel,
  class TMaskPixel,
  class TTensorValueType,
  class TFeaturesTraits,
  unsigned VImageDimension >
void
MaskFeaturePointSelectionFilter<
  Image< TImagePixel, VImageDimension >,
  Image< TMaskPixel, VImageDimension >,
  PointSet< Matrix< TTensorValueType, VImageDimension, VImageDimension>, VImageDimension, TFeaturesTraits > >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "m_NonConnectivity: " << static_cast< unsigned >( m_NonConnectivity ) << std::endl
     << indent << "m_BlockRadius: " << m_BlockRadius << std::endl
     << indent << "m_ComputeStructureTensors: " << ( m_ComputeStructureTensors ? "yes" : "no" ) << std::endl
     << indent << "m_SelectFraction: " << m_SelectFraction << std::endl;
}

template<
  class TImagePixel,
  class TMaskPixel,
  class TTensorValueType,
  class TFeaturesTraits,
  unsigned VImageDimension >
void
MaskFeaturePointSelectionFilter<
  Image< TImagePixel, VImageDimension >,
  Image< TMaskPixel, VImageDimension >,
  PointSet< Matrix< TTensorValueType, VImageDimension, VImageDimension>, VImageDimension, TFeaturesTraits > >
::SetNonConnectivity( unsigned connect ) throw ( ExceptionObject )
{
  if ( 0 <= connect && connect < VImageDimension )
    {
      m_NonConnectivityOffsets.clear();
      // use Neighbourhood to compute all offsets in radius 1
      Neighborhood< unsigned, VImageDimension> neighborhood;
      neighborhood.SetRadius( NumericTraits< SizeValueType >::One );
      for ( unsigned i = 0, n = neighborhood.Size(); i < n; i++ )
        {
          OffsetType off = neighborhood.GetOffset( i );

          // count 0s offsets in each dimension
          unsigned zeros = 0;
          for ( unsigned j = 0; j < VImageDimension; j++ )
            {
              if ( off[ j ] == 0 ) zeros++;
            }

          if ( connect <= zeros && zeros < VImageDimension )
            {
              m_NonConnectivityOffsets.push_back( off );
            }
        }
    }
  else
    {
      itkExceptionMacro( "Cannot set non-connectivity to " << connect << ", allowed 0.." << VImageDimension - 1 << "." );
    }

  m_NonConnectivity = connect;
}

template<
  class TImagePixel,
  class TMaskPixel,
  class TTensorValueType,
  class TFeaturesTraits,
  unsigned VImageDimension >
void
MaskFeaturePointSelectionFilter<
  Image< TImagePixel, VImageDimension >,
  Image< TMaskPixel, VImageDimension >,
  PointSet< Matrix< TTensorValueType, VImageDimension, VImageDimension>, VImageDimension, TFeaturesTraits > >
::GenerateData()
{
  // generate non-connectivity offsets
  if ( m_NonConnectivityOffsets.empty() )
    {
      SetNonConnectivity( m_NonConnectivity );
    }

  FeaturePointsPointer pointSet = this->GetOutput();
  ImageConstPointer image = this->GetInput();
  typename ImageType::SpacingType voxelSize = image->GetSpacing();
  MaskConstPointer inputMask = this->GetMaskImage();
  typename MaskType::Pointer mask;

  //create all 1s mask if no mask was provided
  if ( inputMask.IsNull() )
    {
      mask = MaskType::New();
      mask->SetRegions( image->GetRequestedRegion() );
      mask->Allocate();
      mask->FillBuffer( NumericTraits< TMaskPixel >::One );
    }
  else
    {
      mask = SmartPointer< MaskType >( const_cast< MaskType * >( inputMask.GetPointer() ) );
    }

  typename FeaturePointsType::PointsContainer::Pointer points
    = FeaturePointsType::PointsContainer::New();
  typename FeaturePointsType::PointDataContainer::Pointer pointData
    = FeaturePointsType::PointDataContainer::New();

  // initialize selectionMap
  typedef Image<unsigned char, 3>  MapType;
  MapType::Pointer selectionMap = MapType::New();
  RegionType region = image->GetRequestedRegion();
  selectionMap->SetRegions( region );
  selectionMap->Allocate();

  // avoid points closer than m_BlockRadius to the boundary
  // 2 X m_BlockRadius + 1 since tensor is computed over (2 X radius + 1)
  SizeType onesSize;
  onesSize.Fill( 1 );
  IndexType safeIndex;
  safeIndex.Fill( 0 );
  safeIndex += m_BlockRadius + m_BlockRadius + onesSize;//safeIndex += m_BlockRadius;
  SizeType safeSize = region.GetSize();
  safeSize -= m_BlockRadius + m_BlockRadius + onesSize + m_BlockRadius + m_BlockRadius + onesSize;//safeSize -= m_BlockRadius + m_BlockRadius;
  region.SetIndex( safeIndex );
  region.SetSize( safeSize );

  // Neighborhood iterator with window size m_BlockRadius + 1 + m_BlockRadius
  ConstNeighborhoodIterator< ImageType > itImage( m_BlockRadius, image, region );
  ImageRegionConstIterator< MaskType > itMask( mask, region );
  ImageRegionIterator< MapType > itMap( selectionMap, region );

  typedef typename ConstNeighborhoodIterator< ImageType >::NeighborIndexType NeighborIndexType;
  NeighborIndexType numPixelsInNeighborhood = itImage.Size();

  // sorted container for feature points, stores pair(variance, index)
  typedef std::multimap< double, IndexType > MultiMapType;
  MultiMapType pointMap;

  // compute variance for eligible points
  for ( itImage.GoToBegin(), itMask.GoToBegin(), itMap.GoToBegin(); !itImage.IsAtEnd();
        ++itImage, ++itMask, ++itMap )
    {
      if ( itMask.Get() )
        {
          double mean = 0.0, variance = 0.0;
          for ( NeighborIndexType i = 0; i < numPixelsInNeighborhood; i++ )
            {
              InputImagePixelType pixel = itImage.GetPixel( i );
              mean += pixel;
              variance += pixel * pixel;
            }
          variance = variance / ( static_cast< double >( numPixelsInNeighborhood ) )
                   - mean / numPixelsInNeighborhood * mean / numPixelsInNeighborhood;

          pointMap.insert( MultiMapType::value_type( variance, itImage.GetIndex() ) );
          itMap.Set( 1 );
        }
      else
        {
          itMap.Set( 0 );
        }
    }

  // number of points to select
  unsigned pointsLeft = floor( 0.5 + pointMap.size() * m_SelectFraction );

  // pick points with highest variance first
  for ( typename MultiMapType::reverse_iterator rit = pointMap.rbegin(); rit != pointMap.rend(); ++rit )
    {
      // if point is not marked off in selection map and there are still points to be picked
      if ( selectionMap->GetPixel( ( *rit ).second ) && pointsLeft )
        {
          pointsLeft--;

          // add point to points container
          PointType point;
          image->TransformIndexToPhysicalPoint( ( *rit ).second, point );
          points->InsertElement( pointsLeft, point );

          // compute and add structure tensor into pointData
          if ( this->m_ComputeStructureTensors )
            {
              StructureTensorType product;
              StructureTensorType tensor;
              tensor.Fill( 0 );

              Matrix < double, VImageDimension, 1 > gradI; // vector declared as column matrix

              SizeType radius;
              radius.Fill( 1 ); // iterate over neighbourhood of a voxel

              RegionType center;
              center.SetSize( radius );
              center.SetIndex( ( *rit ).second );

              radius = m_BlockRadius + m_BlockRadius; // neighbourhood radius: should it really be double block size???
              ConstNeighborhoodIterator< ImageType > itGrad( radius, image, center );

              itGrad.GoToBegin();
              for ( unsigned i = 0; i < itGrad.Size(); i++ ) // iterate over voxels in the neighbourhood
                {
                  OffsetType off = itGrad.GetOffset( i );
                  for ( unsigned j = 0; j < VImageDimension; j++ )
                  {
                    OffsetType left = off;
                    left[ j ] -= 1;

                    OffsetType right = off;
                    right[ j ] += 1;

                    // using image GetPixel instead of iterator GetPixel since offsets might be outside of neighbourhood
                    gradI( j, 0 ) = ( image->GetPixel( itGrad.GetIndex( left ) ) - image->GetPixel( itGrad.GetIndex( right ) ) )
                                  / voxelSize[ j ];
                  }
                  product = gradI * gradI.GetTranspose(); // outer(tensor) product of gradI with itself
                  tensor += product;
                }
              tensor /= vnl_trace( tensor.GetVnlMatrix() );

              pointData->InsertElement( pointsLeft, tensor );
            }

          // mark off connected points
          for ( int j = 0, n = m_NonConnectivityOffsets.size(); j < n; j++ )
          {
            IndexType idx = ( *rit ).second;
            idx += m_NonConnectivityOffsets[ j ];
            if ( region.IsInside( idx ) ) // is this check necessary?
              {
                selectionMap->SetPixel( idx, 0 );
              }
          }
        }
    }

  // set points
  pointSet->SetPoints( points );
  pointSet->SetPointData( pointData );
}
} // end namespace itk

#endif
