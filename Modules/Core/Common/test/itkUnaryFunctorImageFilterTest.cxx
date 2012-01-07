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

#include "itkUnaryFunctorImageFilter.h"
#include "itkImageRegionIterator.h"

namespace itk
{
  namespace Functor
  {
  template< class TInput, class TOutput >
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

    inline TOutput operator()(const TInput & value) const
    {
      return static_cast< TOutput >( 2.0f * value );
    }
  };
  }
}

int itkUnaryFunctorImageFilterTest(int, char*[])
{
  typedef float                                             PixelType;
  typedef itk::Image< PixelType, 3 >                        ImageType;
  typedef itk::Functor::TestFunctor< PixelType, PixelType > FunctorType;
  typedef itk::UnaryFunctorImageFilter< ImageType,
                                        ImageType,
                                        FunctorType >       FilterType;

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
    inIt.Set( static_cast< float >( i++ ) );
    ++inIt;
    }

  // Set up the filter
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( input );
  filter->UpdateLargestPossibleRegion();

  i = 0;
  ImageType::Pointer output = filter->GetOutput();
  itk::ImageRegionIterator< ImageType > outIt( output, region );
  while ( !outIt.IsAtEnd() )
    {
    ImageType::PixelType expectedValue = static_cast< ImageType::PixelType >( 2.0f * i );
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

  std::cout << filter->Superclass::GetNameOfClass() << std::endl;

  // Invoke with an output image that is higher dimensional than the
  // input image
  typedef itk::Image< PixelType, 2 >                        Image2Type;
  typedef itk::UnaryFunctorImageFilter< Image2Type,
                                        ImageType,
                                        FunctorType >       Filter2Type;

  Image2Type::SizeType size2 = {{ 16, 16 }};
  Image2Type::RegionType region2( size2 );

  Image2Type::Pointer input2 = Image2Type::New();
  input2->SetRegions( region2 );
  input2->Allocate();
  input2->FillBuffer( 2.0f );

  Filter2Type::Pointer filter2 = Filter2Type::New();
  filter2->SetInput( input2 );
  filter2->UpdateLargestPossibleRegion();

  return EXIT_SUCCESS;
}
