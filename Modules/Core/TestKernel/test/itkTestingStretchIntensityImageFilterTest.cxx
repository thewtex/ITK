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

#include "itkTestingMacros.h"
#include "itkTestingStretchIntensityImageFilter.h"
#include "itkRandomImageSource.h"
#include "itkStatisticsImageFilter.h"

int itkTestingStretchIntensityImageFilterTest(int, char* [] )
{
  typedef signed short                                         PixelType;
  typedef itk::Image<PixelType,2>                              ImageType;
  typedef itk::RandomImageSource<ImageType>                    RandomSourceType;
  typedef itk::Testing::StretchIntensityImageFilter<ImageType> StretchFilterType;
  typedef itk::StatisticsImageFilter<ImageType>                StatsFilterType;

  ImageType::SizeType       imageSize = {{ 32,32 }};
  RandomSourceType::Pointer randSrc = RandomSourceType::New();
  randSrc->SetSize(imageSize);

  const PixelType minPix(-255);
  const PixelType maxPix(255);
  randSrc->SetMin(minPix);
  randSrc->SetMax(maxPix);

  StretchFilterType::Pointer stretchFilter = StretchFilterType::New();
  const PixelType outputMinPix(-5000);
  const PixelType outputMaxPix(16384);
  stretchFilter->SetOutputMinimum(outputMinPix);
  stretchFilter->SetOutputMaximum(outputMaxPix);
  TEST_SET_GET_VALUE(outputMinPix, stretchFilter->GetOutputMinimum());
  TEST_SET_GET_VALUE(outputMaxPix, stretchFilter->GetOutputMaximum());

  stretchFilter->SetInput(randSrc->GetOutput());
  std::cout << stretchFilter;

  TRY_EXPECT_NO_EXCEPTION(stretchFilter->Update());

  const StretchFilterType::RealType scale = stretchFilter->GetScale();
  const StretchFilterType::RealType shift = stretchFilter->GetShift();
  const PixelType                   inputMinimum = stretchFilter->GetInputMinimum();
  const PixelType                   inputMaximum = stretchFilter->GetInputMaximum();
  std::cout << "Scale: " << scale << std::endl
            << "Shift: " << shift << std::endl
            << "InputMinimum: " << inputMinimum << std::endl
            << "InputMaximum: " << inputMaximum << std::endl;

  StatsFilterType::Pointer statsFilter = StatsFilterType::New();
  statsFilter->SetInput(stretchFilter->GetOutput());
  TRY_EXPECT_NO_EXCEPTION(statsFilter->Update());
  std::cout << "Output Minimum: " << statsFilter->GetMinimum() << std::endl
            << "Output Maximum: " << statsFilter->GetMaximum() << std::endl
            << "Output Mean: " << statsFilter->GetMean() << std::endl
            << "Output Variance: " << statsFilter->GetVariance() << std::endl
            << "Output Sigma: " << statsFilter->GetSigma() << std::endl;
  return EXIT_SUCCESS;
}
