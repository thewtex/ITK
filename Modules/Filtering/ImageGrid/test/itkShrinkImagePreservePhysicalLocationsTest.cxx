/*=========================================================================
 *
 *  Copyright Bradley Lowekamp
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

#include "itkShrinkImageFilter.h"
#include "itkPhysicalPointImageSource.h"
#include "itkImageRegionConstIterator.h"


// This test verifies the principle that the ShrinkImageFilter should
// not change the physical location of the signal. This is done by
// constructing an image of points, each coresponding to the phycical
// location of the center of the voxel, then verifying that the value
// still matches the physcial locaiton.

namespace
{

// This function checks that all values in an image are equivalent to
// the physical point of the image.
template <typename TImageType>
bool CheckValueIsPhysicalPoint( const TImageType *img )
{

  typedef itk::ImageRegionConstIterator<TImageType> IteratorType;
  IteratorType it(img, img->GetBufferedRegion() );

  bool match = true;

  typename TImageType::PointType pt;
  img->TransformIndexToPhysicalPoint( it.GetIndex(), pt );
  while( !it.IsAtEnd() )
    {
    for ( unsigned int i = 0; i < TImageType::ImageDimension; ++i )
      {
      img->TransformIndexToPhysicalPoint( it.GetIndex(), pt );
      if ( !itk::Math::FloatAlmostEqual(pt[i], it.Get()[i]) )
        {
        typename TImageType::PointType::VectorType diff;
        for( unsigned int j = 0; j <  TImageType::ImageDimension; ++j )
          {
          diff[j] = pt[j] - it.Get()[j];
          }

        std::cout << "Index: " << it.GetIndex() << " Point: " << pt << " Value: " << it.Get() << " Difference:" << diff << std::endl;
        match = false;
        }
      }

    ++it;
    }
  return match;
}

}


int itkShrinkImagePreservePhysicalLocationsTest( int , char *[] )
{

  const unsigned int ImageDimension = 2;
  typedef itk::Vector<double, ImageDimension> PixelType;
  typedef itk::Image<PixelType, ImageDimension> ImageType;

  typedef itk::PhysicalPointImageSource<ImageType> SourceType;
  SourceType::Pointer source = SourceType::New();


  // these size are chossen as a power of tow and a prime number.
  SourceType::SizeValueType size[] = {512,509};
  source->SetSize( size );

  float origin[] = {1.1, 2.22};
  source->SetOrigin( origin );

  unsigned int factors[] = {1,1};
  bool pass = true;

  for( unsigned int xf = 1; xf < 10; ++xf )
    {
    factors[0] = xf;
    for( unsigned int yf = 1; yf < 10; ++yf )
      {
      factors[1] = yf;


      typedef itk::ShrinkImageFilter<ImageType, ImageType> FilterType;
      FilterType::Pointer shrink = FilterType::New();

      shrink->SetInput(source->GetOutput());

      shrink->SetShrinkFactors( factors );
      shrink->UpdateLargestPossibleRegion();
      if ( !CheckValueIsPhysicalPoint( shrink->GetOutput() ) )
        {
        pass = false;
        std::cout << "== Failed with shrink factors " << factors[0] << " " << factors[1] << " == " << std::endl;
        }
      }
    }


  if ( pass )
    {
    return EXIT_SUCCESS;
    }
  else
    {
    return EXIT_FAILURE;
    }

}
