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
#ifndef __itkImageAlgorithm_hxx
#define __itkImageAlgorithm_hxx

#include "itkImageAlgorithm.h"

#include "itkImageRegionIterator.h"

namespace itk
{

template<typename InputImageType, typename OutputImageType >
void ImageAlgorithm::Copy( const InputImageType *inImage, OutputImageType *outImage,
                           const typename InputImageType::RegionType &inRegion,
                           const typename OutputImageType::RegionType &outRegion )
{
  itk::ImageRegionConstIterator<InputImageType> it( inImage, inRegion );
  itk::ImageRegionIterator<OutputImageType> ot( outImage, outRegion );

  while( !it.IsAtEnd() )
    {
    ot.Set( static_cast< typename InputImageType::PixelType >( it.Get() ) );
    ++ot;
    ++it;
    }
}


template<typename ImageType >
void ImageAlgorithm::MemCopy( const ImageType *inImage, ImageType *outImage,
                              const typename ImageType::RegionType &inRegion,
                              const typename ImageType::RegionType &outRegion )
{
  typedef typename ImageType::RegionType RegionType;
  typedef typename ImageType::IndexType  IndexType;

  typedef typename ImageType::PixelType PixelType;


  size_t PixelSize= 0;
  if ( strcmp(inImage->GetNameOfClass(), "VectorImage") == 0 )
    {
    PixelSize = ImageType::AccessorFunctorType::GetVectorLength(inImage) * sizeof(typename NumericTraits<PixelType>::ValueType);
    }
  else
      {
      PixelSize = sizeof( PixelType );
      }

  const void *in = inImage->GetBufferPointer();
  void *out = outImage->GetBufferPointer();

  const RegionType &inBufferedRegion = inImage->GetBufferedRegion();
  const RegionType &outBufferedRegion = outImage->GetBufferedRegion();

  ImageRegionConstIterator<ImageType> it( inImage, inRegion );

  // compute the number of continuous pixel which can be copied
  size_t numberOfPixel = 1;
  unsigned int    movingDirection = 0;
  do
    {
    numberOfPixel *=  inRegion.GetSize(movingDirection );
    ++movingDirection;
    }

  // The copy regions must extend to the full buffered region, to
  // ensure continuity of pixel between dimensions
  while ( movingDirection < RegionType::ImageDimension
          && inRegion.GetSize( movingDirection - 1 ) == inBufferedRegion.GetSize( movingDirection - 1 )
          && outRegion.GetSize( movingDirection - 1 ) == outBufferedRegion.GetSize( movingDirection - 1 )
          && inBufferedRegion.GetSize(movingDirection - 1) == outBufferedRegion.GetSize(movingDirection - 1) );

  size_t sizeOfChunk = numberOfPixel*PixelSize;

  IndexType inCurrentIndex = inRegion.GetIndex();
  IndexType outCurrentIndex = outRegion.GetIndex();

  while ( inRegion.IsInside( inCurrentIndex ) )
    {
    size_t inOffset = 0; // in bytes
    size_t outOffset = 0;

    size_t inSubDimensionQuantity = 1; // in pixels
    size_t outSubDimensionQuantity = 1;

    for (unsigned int i = 0; i < RegionType::ImageDimension; ++i )
      {
      inOffset += inSubDimensionQuantity*PixelSize*static_cast<size_t>( inCurrentIndex[i] - inBufferedRegion.GetIndex(i) );
      inSubDimensionQuantity *= inBufferedRegion.GetSize(i);

      outOffset += outSubDimensionQuantity*PixelSize*static_cast<size_t>( outCurrentIndex[i] - outBufferedRegion.GetIndex(i) );
      outSubDimensionQuantity *= outBufferedRegion.GetSize(i);
      }

    const char *inBuffer = static_cast<const char*>(in) + inOffset;
    char* outBuffer = static_cast<char*>(out) + outOffset;

    memcpy( outBuffer, inBuffer, sizeOfChunk );


    if ( movingDirection == RegionType::ImageDimension )
      {
      break;
      }

    // increment index to next chunk
    ++inCurrentIndex[movingDirection];
    for ( unsigned int i = movingDirection; i < RegionType::ImageDimension - 1; ++i )
      {
      // when reaching the end of the moving index in the copy region
      // dimension carry to higher dimensions
      if ( static_cast<SizeValueType>(inCurrentIndex[i]  - inRegion.GetIndex(i)) >= inRegion.GetSize(i) )
        {
        inCurrentIndex[i] = inRegion.GetIndex(i);
        ++inCurrentIndex[i + 1];
        }
      }

    // increment index to next chunk
    ++outCurrentIndex[movingDirection];
    for ( unsigned int i = movingDirection; i < RegionType::ImageDimension - 1; ++i )
      {
      if ( static_cast<SizeValueType>(outCurrentIndex[i]  - outRegion.GetIndex(i)) >= outRegion.GetSize(i) )
        {
        outCurrentIndex[i] = outRegion.GetIndex(i);
        ++outCurrentIndex[i + 1];
        }
      }

    }
}


} // end namespace itk

#endif
