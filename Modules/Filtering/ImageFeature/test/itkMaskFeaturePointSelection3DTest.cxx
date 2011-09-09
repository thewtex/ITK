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

#include <iostream>
#include "itkIndex.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "itkMaskFeaturePointSelection3D.h"

bool compareIndices ( itk::Index< 3 > a, itk::Index< 3 > b )
{
  // a < b
  if ( a[2] == b[2] )
    {
      if ( a[1] == b[1] )
        {
        return ( a[0] < b[0] );
        }
      else
        {
        return ( a[1] < b[1] );
        }
    }
  else
    {
      return ( a[2] < b[2] );
    }
}

int itkMaskFeaturePointSelection3DTest( int, char *[] )
{
  try
    {
      typedef unsigned char                                                  PixelType;
      typedef itk::MaskFeaturePointSelection3D<float, PixelType, PixelType>  FilterType;
      typedef FilterType::PointType                                          PointType;
      typedef FilterType::InputImageType                                     ImageType;
      typedef FilterType::OutputPointSetType                                 PointSetType;
      typedef PointSetType::PixelType                                        PointSetPixelType;

      // Set up filter
      FilterType::Pointer filter = FilterType::New();

      // Generate input image and mask
      ImageType::Pointer image = ImageType::New();
      ImageType::Pointer mask = ImageType::New();

      ImageType::IndexType start;
      start[0] = 0;
      start[1] = 0;
      start[2] = 0;

      ImageType::SizeType size;
      size[0] = 10;
      size[1] = 10;
      size[2] = 10;

      ImageType::RegionType region;
      region.SetSize( size );
      region.SetIndex( start );

      image->SetRegions( region );
      image->Allocate();

      mask->SetRegions( region );
      mask->Allocate();

      typedef itk::ImageRegionIterator<ImageType> IteratorType;
      ImageType::IndexType idx;

      // Set pixels with all same indices to 100 while set the rest to 1
      IteratorType itImage( image, image->GetRequestedRegion() );

      for ( itImage.GoToBegin(); !itImage.IsAtEnd(); ++itImage )
        {
        idx = itImage.GetIndex();
        itImage.Set( idx[0] == idx[1] && idx[1] == idx[2] ? 100 : 1 );
      }

      filter->SetInputImage( image );

      // Fill half of the mask with 0s
      IteratorType itMask( mask, mask->GetRequestedRegion() );

      for ( itMask.GoToBegin(); !itMask.IsAtEnd(); ++itMask )
        {
        idx = itMask.GetIndex();
        itMask.Set( idx[1] < 5 );
      }

      filter->SetMaskImage( mask );

      std::cout << "filter: " << filter;

      filter->Update();

      // put points selected by the filter into a vector
      std::vector< PointSetPixelType > selectedPoints;

      PointSetType::PointDataContainerIterator itPointData =
              filter->GetOutput()->GetPointData()->Begin();
      PointSetType::PointDataContainerIterator end =
              filter->GetOutput()->GetPointData()->End();
      while( itPointData != end )
        {
          selectedPoints.push_back( itPointData.Value() );
          ++itPointData;
        }

      // sort vector to enable simple comparison with preselected points
      std::sort( selectedPoints.begin(), selectedPoints.end(), compareIndices );

      // points selected for that same image/mask combination by our tested implementation
      PointSetPixelType preselectedPoints[50] =
      { {{ 0, 0, 0 }},
        {{ 2, 0, 0 }},
        {{ 4, 0, 0 }},
        {{ 9, 1, 0 }},
        {{ 0, 2, 0 }},
        {{ 2, 2, 0 }},
        {{ 4, 3, 0 }},
        {{ 9, 3, 0 }},
        {{ 0, 4, 0 }},
        {{ 2, 4, 0 }},
        {{ 7, 1, 1 }},
        {{ 7, 3, 1 }},
        {{ 0, 0, 2 }},
        {{ 9, 0, 2 }},
        {{ 2, 1, 2 }},
        {{ 0, 2, 2 }},
        {{ 5, 2, 2 }},
        {{ 9, 2, 2 }},
        {{ 3, 3, 2 }},
        {{ 1, 4, 2 }},
        {{ 5, 4, 2 }},
        {{ 4, 0, 3 }},
        {{ 7, 1, 3 }},
        {{ 8, 4, 3 }},
        {{ 0, 0, 4 }},
        {{ 2, 0, 4 }},
        {{ 0, 2, 4 }},
        {{ 2, 2, 4 }},
        {{ 4, 2, 4 }},
        {{ 0, 4, 4 }},
        {{ 2, 4, 4 }},
        {{ 4, 4, 4 }},
        {{ 6, 4, 4 }},
        {{ 6, 2, 5 }},
        {{ 8, 4, 5 }},
        {{ 2, 2, 6 }},
        {{ 4, 2, 6 }},
        {{ 2, 4, 6 }},
        {{ 4, 4, 6 }},
        {{ 6, 4, 6 }},
        {{ 8, 4, 7 }},
        {{ 0, 0, 8 }},
        {{ 8, 1, 8 }},
        {{ 0, 2, 8 }},
        {{ 2, 2, 8 }},
        {{ 4, 2, 8 }},
        {{ 6, 2, 8 }},
        {{ 0, 4, 8 }},
        {{ 2, 4, 8 }},
        {{ 5, 4, 8 }} };

      // checking correctness
      if ( selectedPoints.size() != 50 )
        {
          std::cout << "ERROR: Should have produced 50 points instead of "
                    << selectedPoints.size() << std::endl;
          return EXIT_FAILURE;
        }

      for ( int i = 0; i < 50; i++)
        {
          if ( preselectedPoints[i] != selectedPoints[i] )
            {
              std::cout << "ERROR: One (or more) of the selected points does not match any of "
                        << "the preselected points" << std::endl;
              return EXIT_FAILURE;
            }
        }
    }
  catch ( itk::ExceptionObject &err )
    {
      ( &err )->Print( std::cerr );
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
