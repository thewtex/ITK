/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

// Ensure we do not get NaN's with a constant image

#include "itkScalarImageToCooccurrenceMatrixFilter.h"
#include "itkHistogramToTextureFeaturesFilter.h"
#include "itkMath.h"

int
itkHistogramToTextureFeaturesFilterNaNTest(int, char *[])
{
  constexpr unsigned int Dimension = 2;
  using PixelType = unsigned char;
  using ImageType = itk::Image<PixelType, Dimension>;

  // Build a constant image
  auto                  image = ImageType::New();
  ImageType::RegionType region;
  auto                  size = ImageType::SizeType::Filled(256);
  region.SetSize(size);
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(128);

  // Generate co-occurence matrix
  using MatrixGeneratorType = itk::Statistics::ScalarImageToCooccurrenceMatrixFilter<ImageType>;
  auto generator = MatrixGeneratorType::New();
  auto offset = itk::MakeFilled<MatrixGeneratorType::OffsetType>(1);
  generator->SetOffset(offset);
  generator->SetInput(image);
  generator->Update();

  using TextureFilterType = itk::Statistics::HistogramToTextureFeaturesFilter<MatrixGeneratorType::HistogramType>;
  auto filter = TextureFilterType::New();
  filter->SetInput(generator->GetOutput());
  filter->Update();

  const TextureFilterType::MeasurementType correlation = filter->GetCorrelation();
  std::cout << "Correlation: " << correlation << std::endl;
  if (itk::Math::isnan(correlation))
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
