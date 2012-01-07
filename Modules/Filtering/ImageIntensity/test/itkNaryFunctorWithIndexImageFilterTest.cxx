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

#include "itkNaryFunctorWithIndexImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include <vector>

namespace itk
{
  namespace Functor
  {
  template< class TIndex, class TInput, class TOutput >
  class TestFunctor
  {
  public:
    TestFunctor() {}
    ~TestFunctor() {}

    bool operator!=(const TestFunctor &) const
    {
      return false;
    }

    bool operator==(const TestFunctor & other) const
    {
      return !( *this != other );
    }

    inline TOutput operator()(const TIndex & index, const TInput & values) const
    {
      return static_cast< TOutput >( ( index[0] + index[1] + index[2] ) *
                                     values[0] * values[1] * values[2] * values[3] );
    }
  };
  }

}

int itkNaryFunctorWithIndexImageFilterTest(int, char*[])
{
  typedef float                                               PixelType;
  typedef itk::Image< PixelType, 3 >                          ImageType;
  typedef ImageType::IndexType                                IndexType;
  typedef std::vector< PixelType >                            ValueArrayType;
  typedef itk::Functor::TestFunctor< IndexType,
                                     ValueArrayType,
                                     PixelType >              FunctorType;
  typedef itk::NaryFunctorWithIndexImageFilter< ImageType,
                                                ImageType,
                                                FunctorType > FilterType;

  // Set up the test images
  ImageType::SizeType size = {{ 16, 16, 16 }};
  ImageType::RegionType region( size );

  ImageType::Pointer input1 = ImageType::New();
  input1->SetRegions( region );
  input1->Allocate();
  input1->FillBuffer( 3.0f );

  ImageType::Pointer input2 = ImageType::New();
  input2->SetRegions( region );
  input2->Allocate();
  input2->FillBuffer( 4.0f );

  ImageType::Pointer input3 = ImageType::New();
  input3->SetRegions( region );
  input3->Allocate();
  input3->FillBuffer( 5.0f );

  ImageType::Pointer input4 = ImageType::New();
  input4->SetRegions( region );
  input4->Allocate();
  input4->FillBuffer( 6.0f );

  // Set up the filter
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( 0, input1 );
  filter->SetInput( 1, input2 );
  filter->SetInput( 2, input3 );
  filter->SetInput( 3, input4 );

  // Test the filter with four input images
  filter->UpdateLargestPossibleRegion();

  ImageType::Pointer output = filter->GetOutput();
  itk::ImageRegionConstIteratorWithIndex< ImageType > outIt( output, region );
  while ( !outIt.IsAtEnd() )
    {
    IndexType index = outIt.GetIndex();
    PixelType expectedValue = static_cast< PixelType >( ( index[0] + index[1] + index[2] )
                                                        * 3.0f * 4.0f * 5.0f * 6.0f );
    if ( outIt.Get() != expectedValue )
      {
      std::cerr << "Unexpected value found in test case. Expected " << expectedValue
                << ", got " << outIt.Get() << " instead at index " << index << "."
                << std::endl;
      return EXIT_FAILURE;
      }
    ++outIt;
    }

  filter->Print( std::cout );

  filter->SetFunctor( filter->GetFunctor() );

  return EXIT_SUCCESS;
}
