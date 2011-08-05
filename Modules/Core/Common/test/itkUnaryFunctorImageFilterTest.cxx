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
#include "itkImage.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkRandomImageSource.h"
#include "itkTestingComparisonImageFilter.h"

namespace itk
{
  namespace Functor
  {
  template< class TInput, class TOutput >
  class NoOp
  {
  public:
    typedef TInput  InputType;
    typedef TOutput OutputType;

    NoOp() {}
    ~NoOp() {}
    bool operator!=(const NoOp &) const
    {
      return false;
    }

    bool operator==(const NoOp & other) const
    {
      return !( *this != other );
    }

    inline TOutput operator()(const TInput & A) const
    {
      return A;
    }
  };
  }
}

int itkUnaryFunctorImageFilterTest (int, char*[])
{
  typedef itk::Image<float,3>                           ImageType;
  typedef itk::Image<float,3>                           FunctorImageType;
  typedef itk::RandomImageSource< ImageType >           SourceType;
  typedef itk::Functor::NoOp<FunctorImageType::PixelType,
    FunctorImageType::PixelType>
                                                        FunctorType;
  typedef itk::UnaryFunctorImageFilter<ImageType, ImageType,
    FunctorType >                                       FilterType;
  typedef itk::Testing::ComparisonImageFilter< ImageType, ImageType >
                                                        DiffType;

  SourceType::Pointer source = SourceType::New();

  ImageType::SizeValueType randomSize[3] = {11, 5, 3};
  source->SetSize( randomSize );

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( source->GetOutput() );

  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput( source->GetOutput() );
  diff->SetTestInput( filter->GetOutput() );
  diff->Update();

  itk::SizeValueType status =
    itk::NumericTraits< itk::SizeValueType >::Zero;
  status = diff->GetNumberOfPixelsWithDifferences();
  if (status != 0)
    {
    std::cout << "FAILED: number of different pixels is: "
              << status
              << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
