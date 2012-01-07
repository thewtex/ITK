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

#include "itkBinaryFunctorImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkTestingMacros.h"

namespace itk
{
  namespace Functor
  {
  template< class TInput1, class TInput2, class TOutput >
  class MultHelper
  {
  public:
    MultHelper() {}
    ~MultHelper() {}

    bool operator!=(const MultHelper &) const
    {
      return false;
    }

    bool operator==(const MultHelper & other) const
    {
      return !( *this != other );
    }

    inline TOutput operator()(const TInput1 & value1, const TInput2 & value2) const
    {
      return static_cast< TOutput >( value1 * value2 );
    }
  };
  }

}

int itkBinaryFunctorImageFilterTest(int, char*[])
{
  typedef float                                                       PixelType;
  typedef itk::Image< PixelType, 3 >                                  ImageType;
  typedef itk::Functor::MultHelper< PixelType, PixelType, PixelType > MultFunctorType;
  typedef itk::BinaryFunctorImageFilter< ImageType,
                                         ImageType,
                                         ImageType,
                                         MultFunctorType >            FilterType;

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

  // Set up the filter
  FilterType::Pointer filter = FilterType::New();

  // Test the filter with two input images
  filter->SetInput1( input1 );
  filter->SetInput2( input2 );
  filter->UpdateLargestPossibleRegion();

  ImageType::Pointer output = filter->GetOutput();
  itk::ImageRegionConstIterator< ImageType > outIt( output, region );
  ImageType::PixelType expectedValue = 3.0f * 4.0f;
  while ( !outIt.IsAtEnd() )
    {
    if ( outIt.Get() != expectedValue )
      {
      std::cerr << "Unexpected value found in test case 1. Expected " << expectedValue
                << ", got " << outIt.Get() << " instead at index " << outIt.GetIndex() << "."
                << std::endl;
      return EXIT_FAILURE;
      }
    ++outIt;
    }

  // Test the filter with the first input set and a constant set in
  // the second input
  filter->SetConstant2( 5.0f );
  filter->UpdateLargestPossibleRegion();

  output = filter->GetOutput();
  outIt = itk::ImageRegionConstIterator< ImageType >( output, region );
  expectedValue = 3.0f * 5.0f;
  while ( !outIt.IsAtEnd() )
    {
    if ( outIt.Get() != expectedValue )
      {
      std::cerr << "Unexpected value found in test case 2. Expected " << expectedValue
                << ", got " << outIt.Get() << " instead at index " << outIt.GetIndex() << "."
                << std::endl;
      return EXIT_FAILURE;
      }
    ++outIt;
    }

  // Test the filter with the first input set to a constant and the
  // second input set to an image
  filter->SetConstant1( 5.0f );
  filter->SetInput2( input2 );
  filter->UpdateLargestPossibleRegion();

  output = filter->GetOutput();
  outIt = itk::ImageRegionConstIterator< ImageType >( output, region );
  expectedValue = 5.0f * 4.0f;
  while ( !outIt.IsAtEnd() )
    {
    if ( outIt.Get() != expectedValue )
      {
      std::cerr << "Unexpected value found in test case 3. Expected " << expectedValue
                << ", got " << outIt.Get() << " instead at index " << outIt.GetIndex() << "."
                << std::endl;
      return EXIT_FAILURE;
      }
    ++outIt;
    }

  // Test the first with both inputs being a constant
  filter->SetInput1( (const ImageType *) NULL );
  filter->SetInput2( (const ImageType *) NULL );
  filter->SetConstant1( 3.0f );
  filter->SetConstant2( 4.0f );
  try
    {
    filter->UpdateLargestPossibleRegion();
    std::cerr << "Failed to catch expected exception when both inputs are set to a constant."
              << std::endl;
    }
  catch ( itk::ExceptionObject & exception )
    {
    std::cout << "Caught EXPECTED exception when both inputs are set to a constant:"
              << exception << std::endl;
    }

  // Now test the various input methods
  filter->SetInput1( input1 );
  TEST_SET_GET_VALUE( input1, filter->GetInput( 0 ) );

  filter->SetInput2( input2 );
  TEST_SET_GET_VALUE( input2, filter->GetInput( 1 ) );

  FilterType::DecoratedInput1ImagePixelType::Pointer constant1 =
    FilterType::DecoratedInput1ImagePixelType::New();
  constant1->Set( 1.0f );
  filter->SetInput1( constant1 );
  TEST_SET_GET_VALUE( constant1->Get(), filter->GetConstant1() );

  FilterType::DecoratedInput1ImagePixelType::Pointer constant2 =
    FilterType::DecoratedInput1ImagePixelType::New();
  constant2->Set( 2.0f );
  filter->SetInput2( constant2 );
  TEST_SET_GET_VALUE( constant2->Get(), filter->GetConstant2() );
  TEST_SET_GET_VALUE( constant2->Get(), filter->GetConstant() );

  filter->SetInput1( 1.0f );
  TEST_SET_GET_VALUE( 1.0f, filter->GetConstant1() );

  filter->SetInput1( 2.0f );
  TEST_SET_GET_VALUE( 2.0f, filter->GetConstant2() );

  filter->Print( std::cout );

  filter = FilterType::New();
  try
    {
    filter->GetConstant1();
    std::cerr << "Failed to catch expected exception when constant 1 value is queried "
               << "and no constant value was set." << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & exception )
    {
    std::cout << "Caught EXPECTED exception when constant 1 value is queried and no "
              << "constant value was set:" << exception << std::endl;
    }

  try
    {
    filter->GetConstant2();
    std::cerr << "Failed to catch expected exception when constant 2 value is queried "
               << "and no constant value was set." << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & exception )
    {
    std::cout << "Caught EXPECTED exception when constant 2 value is queried and no "
              << "constant value was set:" << exception << std::endl;
    }

  MultFunctorType functor = filter->GetFunctor();
  filter->SetFunctor( functor );

  return EXIT_SUCCESS;
}
