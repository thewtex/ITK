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

#include "itkMaskFeaturePointSelectionFilter.h"
#include "itkImageRegionIterator.h"
#include <vector>
#include <algorithm>
#include <cmath>

namespace itk
{
template< class TImagePixel, class TMaskPixel >
MaskFeaturePointSelectionFilter<
  Image< TImagePixel, 3 >, Image< TMaskPixel, 3 >, PointSet< Index< 3 >, 3 > >
::MaskFeaturePointSelectionFilter()
{
  // defaults
  m_BlockNonConnexity = 26;
  m_BlockRejectedFraction = 0.9;
  m_BlockHalfSize.Fill( 2 );
  m_BlockHalfWindow.Fill( 3 );
}

template< class TImagePixel, class TMaskPixel >
MaskFeaturePointSelectionFilter<
  Image< TImagePixel, 3 >, Image< TMaskPixel, 3 >, PointSet< Index< 3 >, 3 > >
::~MaskFeaturePointSelectionFilter()
{
}

template< class TImagePixel, class TMaskPixel >
void
MaskFeaturePointSelectionFilter<
  Image< TImagePixel, 3 >, Image< TMaskPixel, 3 >, PointSet< Index< 3 >, 3 > >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "m_BlockNonConnexity: " << static_cast< unsigned >( m_BlockNonConnexity ) << std::endl
     << indent << "m_BlockHalfSize: " << m_BlockHalfSize << std::endl
     << indent << "m_BlockRejectedFraction: " << m_BlockRejectedFraction << std::endl;
}

template< class TImagePixel, class TMaskPixel >
void
MaskFeaturePointSelectionFilter<
  Image< TImagePixel, 3 >, Image< TMaskPixel, 3 >, PointSet< Index< 3 >, 3 > >
::SetBlockNonConnexity( int connex ) throw ( ExceptionObject )
{
  if ( connex != 6 && connex != 18 && connex != 26 )
    {
      itkExceptionMacro( "Cannot set connexity to " << connex << ", allowed 6, 18 and 26." );
    }

  this->m_BlockNonConnexity = connex;
}

template< class TImagePixel, class TMaskPixel >
void
MaskFeaturePointSelectionFilter<
  Image< TImagePixel, 3 >, Image< TMaskPixel, 3 >, PointSet< Index< 3 >, 3 > >
::GenerateData()
{
  std::vector< FeaturePoint > pointList;

  OutputPointSetPointer pointSet = this->GetOutput();
  InputImageConstPointer image = this->GetInput();
  MaskImageConstPointer inputMask = this->GetMaskImage();
  typename MaskImageType::Pointer mask;

  //create all 1s mask if no mask was provided
  if ( inputMask.IsNull() )
    {
      mask = MaskImageType::New();
      mask->SetRegions( image->GetRequestedRegion() );
      mask->Allocate();
      mask->FillBuffer( 1 );
    }
  else
    {
      mask = SmartPointer< MaskImageType >( const_cast< MaskImageType * >( inputMask.GetPointer() ) );
    }

  PointsContainerPointer points = PointsContainer::New();
  PointDataContainerPointer pointData = PointDataContainer::New();

  FeaturePoint feature;

  // initialize selectionBlockMap
  typedef Image<unsigned char, 3>             MapImageType;
  typedef typename MapImageType::PixelType    MapImagePixelType;
  typedef typename MapImageType::IndexType    MapImageIndexType;
  typedef ImageRegionIterator<MaskImageType>  MapImageIterator;

  MapImageType::Pointer selectionBlockMap = MapImageType::New();
  MapImageType::RegionType region = image->GetRequestedRegion();
  selectionBlockMap->SetRegions( region );
  selectionBlockMap->Allocate();

  // modify region such that it wont include points too close to the boundary
  IndexType safeIndex;
  safeIndex.Fill( 0 );
  safeIndex += m_BlockHalfSize + m_BlockHalfWindow;
  InputImageSizeType safeSize = region.GetSize();
  safeSize -= m_BlockHalfSize + m_BlockHalfWindow + m_BlockHalfSize + m_BlockHalfWindow;
  region.SetIndex( safeIndex );
  region.SetSize( safeSize );

  // Neighborhood iterator with window size m_BlockHalfSize + 1 + m_BlockHalfSize
  InputImageIterator itImage( m_BlockHalfSize, image, region );
  MapImageIterator itMap( selectionBlockMap, region );
  MaskImageIterator itMask( mask, region );

  typedef typename InputImageIterator::NeighborIndexType NeighborIndexType;
  NeighborIndexType numPixelsInNeighborhood = itImage.Size();

  // compute mean and variance for eligible points
  for ( itImage.GoToBegin(), itMask.GoToBegin(), itMap.GoToBegin(); !itImage.IsAtEnd();
        ++itImage, ++itMask, ++itMap )
    {
      if ( itMask.Get() )
        {
          feature.idx = itImage.GetIndex();
          image->TransformIndexToPhysicalPoint( feature.idx, feature.coor );

          double m = 0.0, v = 0.0;
          for ( NeighborIndexType i = 0; i < numPixelsInNeighborhood; i++ )
            {
              InputImagePixelType pixel = itImage.GetPixel( i );
              m += pixel;
              v += pixel * pixel;
            }
          feature.variance = v / ( (double) numPixelsInNeighborhood )
                           - m / numPixelsInNeighborhood * m / numPixelsInNeighborhood;

          pointList.push_back( feature );
          itMap.Set( 1 );
        }
      else
        {
          itMap.Set( 0 );
        }
    }

  // sort wrt variance
  std::sort( pointList.begin(), pointList.end() );

  unsigned pointsLeft = floor( 0.5 + pointList.size() * ( 1.0 - m_BlockRejectedFraction ) );

  // set/compute connexity offset
  int connexOffSix[6][3] =
  { { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 },
    { -1, 0, 0 },
    { 0, -1, 0 },
    { 0, 0, -1 } };
  int connexOffEighteen[18][3] =
  { { 1, 0, 0 },
    { 1, 1, 0 },
    { 1, 0, 1 },
    { 1, -1, 0 },
    { 1, 0, -1 },
    { 0, 1, 0 },
    { 0, 1, 1 },
    { -1, 1, 0 },
    { 0, 1, -1 },
    { 0, 0, 1 },
    { -1, 0, 1 },
    { 0, -1, 1 },
    { -1, 0, 0 },
    { -1, -1, 0 },
    { -1, 0, -1 },
    { 0, -1, 0 },
    { 0, -1, -1 },
    { 0, 0, -1 } };
  int connexOffTwentysix[27][3];
  typedef int Int3[3];
  Int3 *connexOff;
  if ( m_BlockNonConnexity == 6 )
    {
      connexOff = connexOffSix;
    }
  else if ( m_BlockNonConnexity == 18 )
    {
      connexOff = connexOffEighteen;
    }
  else
    {
      connexOff = connexOffTwentysix;
      for ( int j = -1, m = 0; j < 2; j++ )
        {
          for ( int k = -1; k < 2; k++ )
            {
              for ( int l = -1; l < 2; l++, m++ )
                {
                  connexOffTwentysix[m][0] = j;
                  connexOffTwentysix[m][1] = k;
                  connexOffTwentysix[m][2] = l;
               }
            }
        }
    }

  // iterate backwards: from higher to lower variance
  for ( typename std::vector< FeaturePoint >::reverse_iterator rit = pointList.rbegin();
        rit < pointList.rend(); ++rit )
    {
      feature = *rit;
      if ( selectionBlockMap->GetPixel( feature.idx ) && pointsLeft )
        {
          pointsLeft--;

          // add point to pointset
          points->InsertElement( pointsLeft, feature.coor );
          pointData->InsertElement( pointsLeft, feature.idx );

          // mark off connexed points
          for ( int j = 0; j < m_BlockNonConnexity; j++ )
          {
            MapImageIndexType idx = feature.idx;
            idx[0] += connexOff[j][0];
            idx[1] += connexOff[j][1];
            idx[2] += connexOff[j][2];
            if ( region.IsInside( idx ) ) selectionBlockMap->SetPixel( idx, 0 );
          }
        }
    }

  // set points
  pointSet->SetPoints( points );
  pointSet->SetPointData( pointData );
}
} // end namespace itk

#endif
