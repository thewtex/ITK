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

#include "itkUnaryFunctorWithIndexImageFilter.h"
#include "itkImageRegionIterator.h"

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

    inline TOutput operator()(const TIndex & index, const TInput & value) const
    {
      return static_cast< TOutput >( ( index[0] + index[1] + index[2] ) * value );
    }
  };
  }

}

int itkUnaryFunctorWithIndexImageFilterTest(int, char*[])
{
  typedef float                                                PixelType;
  typedef itk::Image< PixelType, 3 >                           ImageType;
  typedef ImageType::IndexType                                 IndexType;
  typedef itk::Functor::TestFunctor< IndexType,
                                     PixelType,
                                     PixelType >               FunctorType;
  typedef itk::UnaryFunctorWithIndexImageFilter< ImageType,
                                                 ImageType,
                                                 FunctorType > FilterType;

  // Set up a test image
  ImageType::SizeType size = {{ 16, 16, 16 }};
  ImageType::RegionType region( size );

  ImageType::Pointer input = ImageType::New();
  input->SetRegions( region );
  input->Allocate();

  unsigned int i = 0;
  itk::ImageRegionIterator< ImageType > inIt( input, region );
  while ( !inIt.IsAtEnd() )
    {
    inIt.Set( static_cast< PixelType >( i++ ) );
    ++inIt;
    }

  // Set up the filter
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( input );
  filter->UpdateLargestPossibleRegion();

  i = 0;
  ImageType::Pointer output = filter->GetOutput();
  itk::ImageRegionIteratorWithIndex< ImageType > outIt( output, region );
  while ( !outIt.IsAtEnd() )
    {
    IndexType index = outIt.GetIndex();
    PixelType expectedValue =
      static_cast< PixelType >( ( index[0] + index[1] + index[2] ) * i);
    if ( outIt.Get() != expectedValue )
      {
      std::cerr << "Unexpected value found. Expected " << expectedValue << ", got "
                << outIt.Get() << " instead at index " << outIt.GetIndex() << "."
                << std::endl;
      return EXIT_FAILURE;
      }
    i++;
    ++outIt;
    }

  filter->Print( std::cout );

  return EXIT_SUCCESS;
}
