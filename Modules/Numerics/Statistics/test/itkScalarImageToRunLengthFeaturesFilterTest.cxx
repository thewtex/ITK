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

// Insight classes
#include "itkImageRegionIterator.h"

#include "itkScalarImageToRunLengthFeaturesFilter.h"

int
itkScalarImageToRunLengthFeaturesFilterTest(int, char *[])
{

  // Data definitions
  constexpr unsigned int IMGWIDTH = 5;
  constexpr unsigned int IMGHEIGHT = 5;
  constexpr unsigned int NDIMENSION = 2;


  //------------------------------------------------------
  // Create a simple test images
  //------------------------------------------------------
  using InputImageType = itk::Image<unsigned char, NDIMENSION>;

  using InputImageIterator = itk::ImageRegionIterator<InputImageType>;


  auto image = InputImageType::New();

  constexpr InputImageType::SizeType inputImageSize = { { IMGWIDTH, IMGHEIGHT } };

  constexpr InputImageType::IndexType index{};
  InputImageType::RegionType          region;

  region.SetSize(inputImageSize);
  region.SetIndex(index);

  //--------------------------------------------------------------------------
  // Set up the image first. It looks like:
  //  1 2 1 2 1
  //  1 2 1 2 1
  //  1 2 1 2 1
  //  1 2 1 2 1
  //  1 2 1 2 1
  //--------------------------------------------------------------------------

  image->SetRegions(region);
  image->Allocate();

  // setup the iterator
  InputImageIterator imageIt(image, image->GetBufferedRegion());

  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; j++, ++imageIt)
    {
      imageIt.Set(j % 2 + 1);
    }

  //--------------------------------------------------------------------------
  // Set up the mask next. It looks like:
  //  1 1 1 1 1
  //  1 1 1 1 1
  //  1 1 1 1 1
  //  1 1 1 1 1
  //  1 1 1 1 1
  //--------------------------------------------------------------------------

  auto mask = InputImageType::New();
  mask->SetRegions(region);
  mask->Allocate();

  // setup the iterator
  InputImageIterator maskIt(mask, mask->GetBufferedRegion());
  maskIt.GoToBegin();
  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; j++, ++maskIt)
    {
      maskIt.Set(1);
    }

  //--------------------------------------------------------------------------
  // Test the texFilter
  //--------------------------------------------------------------------------
  try
  {
    using RunLengthFilterType = itk::Statistics::ScalarImageToRunLengthFeaturesFilter<InputImageType>;

    // First test: just use the defaults.
    auto texFilter = RunLengthFilterType::New();

    // Invoke update before adding an input. An exception should be
    // thrown.
    bool passed = true;
    try
    {
      texFilter->Update();
      passed = false;
      std::cerr << "Failed to throw expected exception due to nullptr input: " << std::endl;
      return EXIT_FAILURE;
    }
    catch (const itk::ExceptionObject & excp)
    {
      std::cout << "Expected exception caught: " << excp << std::endl;
    }

    texFilter->ResetPipeline();

    if (texFilter->GetInput() != nullptr)
    {
      std::cerr << "GetInput() should return nullptr since the input is "
                << " not set yet " << std::endl;
      passed = false;
    }

    if (texFilter->GetMaskImage() != nullptr)
    {
      std::cerr << "GetMaskImage() should return nullptr since the mask image is "
                << "not set yet " << std::endl;
      passed = false;
    }

    // Invoke update with a nullptr input. An exception should be
    // thrown.
    texFilter->SetInput(nullptr);
    try
    {
      texFilter->Update();
      passed = false;
      std::cerr << "Failed to throw expected exception due to nullptr input: " << std::endl;
      return EXIT_FAILURE;
    }
    catch (const itk::ExceptionObject & excp)
    {
      std::cout << "Expected exception caught: " << excp << std::endl;
    }

    texFilter->ResetPipeline();

    if (texFilter->GetInput() != nullptr)
    {
      passed = false;
    }

    // Test the Use_PixelContainer boolean
    texFilter->SetFastCalculations(false);
    if (texFilter->GetFastCalculations() != false)
    {
      std::cerr << "Error in Set/Get FastCalculations methods" << std::endl;
      return EXIT_FAILURE;
    }

    texFilter->FastCalculationsOn();
    if (texFilter->GetFastCalculations() != true)
    {
      std::cerr << "Error in Set/Get FastCalculationsOn method" << std::endl;
      return EXIT_FAILURE;
    }

    texFilter->FastCalculationsOff();
    texFilter->SetInput(image);
    texFilter->SetPixelValueMinMax(0, 2);
    texFilter->SetDistanceValueMinMax(0, 5.0);
    texFilter->SetNumberOfBinsPerAxis(5);
    texFilter->SetMaskImage(mask);
    texFilter->Update();

    texFilter->Print(std::cout);

    // Test GetInput
    if (texFilter->GetInput() != image)
    {
      std::cerr << "Error in GetInput() method " << std::endl;
      passed = false;
    }

    // Test GetMaskImage
    if (texFilter->GetMaskImage() != mask)
    {
      std::cerr << "Error in GetMaskImage() method " << std::endl;
      passed = false;
    }

    RunLengthFilterType::FeatureValueVectorPointer means = texFilter->GetFeatureMeans();
    RunLengthFilterType::FeatureValueVectorPointer stds = texFilter->GetFeatureStandardDeviations();

    constexpr double expectedMeans[10] = { 0.76, 7, 10.4, 20, 0.0826667, 15.4, 0.0628267, 11.704, 0.578667, 107.8 };

    {
      RunLengthFilterType::FeatureValueVector::ConstIterator mIt = means->Begin();
      for (int counter = 0; mIt != means->End(); ++mIt, counter++)
      {
        if (itk::Math::abs(expectedMeans[counter] - mIt.Value()) > 0.0001)
        {
          std::cerr << "Error. Mean for feature " << counter << " is " << mIt.Value() << ", expected "
                    << expectedMeans[counter] << '.' << std::endl;
          passed = false;
        }
      }
    }
    {
      constexpr double expectedDeviations[10] = { 0.415692, 10.3923,   4.50333, 8.66025,  0,
                                                  0,        0.0343639, 6.40166, 0.859097, 160.041494 };
      RunLengthFilterType::FeatureValueVector::ConstIterator sIt = stds->Begin();
      for (int counter = 0; sIt != stds->End(); ++sIt, counter++)
      {
        if (itk::Math::abs(expectedDeviations[counter] - sIt.Value()) > 0.0001)
        {
          std::cerr << "Error. Deviation for feature " << counter << " is " << sIt.Value() << ", expected "
                    << expectedDeviations[counter] << '.' << std::endl;
          passed = false;
        }
      }
    }
    // Rerun the feature generation with fast calculations on
    texFilter->FastCalculationsOn();
    texFilter->Update();
    means = texFilter->GetFeatureMeans();
    stds = texFilter->GetFeatureStandardDeviations();

    constexpr double expectedMeans2[10] = { 1, 1, 13, 25, 0.0826667, 15.4, 0.0826667, 15.4, 0.0826667, 15.4 };
    constexpr double expectedDeviations2[10] = { 0 };

    {
      RunLengthFilterType::FeatureValueVector::ConstIterator mIt = means->Begin();
      for (int counter = 0; mIt != means->End(); ++mIt, counter++)
      {
        if (itk::Math::abs(expectedMeans2[counter] - mIt.Value()) > 0.0001)
        {
          std::cerr << "Error2. Mean for feature " << counter << " is " << mIt.Value() << ", expected "
                    << expectedMeans2[counter] << '.' << std::endl;
          passed = false;
        }
      }
    }
    {
      RunLengthFilterType::FeatureValueVector::ConstIterator sIt = stds->Begin();
      for (int counter = 0; sIt != stds->End(); ++sIt, counter++)
      {
        if (itk::Math::abs(expectedDeviations2[counter] - sIt.Value()) > 0.0001)
        {
          std::cerr << "Error2. Deviation for feature " << counter << " is " << sIt.Value() << ", expected "
                    << expectedDeviations2[counter] << '.' << std::endl;
          passed = false;
        }
      }
    }
    //    //Rerun the feature generation setting an offset
    RunLengthFilterType::OffsetType offset;
    offset[0] = -1;
    offset[1] = -1;

    RunLengthFilterType::OffsetVectorPointer offsets;

    offsets = RunLengthFilterType::OffsetVector::New();

    offsets->push_back(offset);
    texFilter->SetOffsets(offsets);

    const RunLengthFilterType::OffsetVector * offsets2 = texFilter->GetOffsets();

    for (RunLengthFilterType::OffsetVector::ConstIterator vIt = offsets->Begin(), vIt2 = offsets2->Begin();
         vIt != offsets->End();
         ++vIt, ++vIt2)
    {
      if (vIt.Value() != vIt2.Value())
      {
        std::cerr << "Offsets not properly set" << std::endl;
        passed = false;
      }
    }

    texFilter->Update();

    if (texFilter->GetMaskImage() == nullptr)
    {
      std::cerr << "Error: " << std::endl;
      std::cerr << "Mask should not be null." << std::endl;
      passed = false;
    }

    means = texFilter->GetFeatureMeans();
    stds = texFilter->GetFeatureStandardDeviations();

    constexpr double expectedMeans3[10] = { 1, 1, 13, 25, 0.0826667, 15.4, 0.0826667, 15.4, 0.0826667, 15.4 };
    constexpr double expectedDeviations3[10] = { 0 };
    {
      RunLengthFilterType::FeatureValueVector::ConstIterator mIt = means->Begin();
      for (int counter = 0; mIt != means->End(); ++mIt, counter++)
      {
        if (itk::Math::abs(expectedMeans3[counter] - mIt.Value()) > 0.0001)
        {
          std::cerr << "Error3. Mean for feature " << counter << " is " << mIt.Value() << ", expected "
                    << expectedMeans3[counter] << '.' << std::endl;
          passed = false;
        }
      }
    }

    {
      RunLengthFilterType::FeatureValueVector::ConstIterator sIt = stds->Begin();
      for (int counter = 0; sIt != stds->End(); ++sIt, counter++)
      {
        if (itk::Math::abs(expectedDeviations3[counter] - sIt.Value()) > 0.0001)
        {
          std::cerr << "Error3. Deviation for feature " << counter << " is " << sIt.Value() << ", expected "
                    << expectedDeviations3[counter] << '.' << std::endl;
          passed = false;
        }
      }
    }

    // Test Set/Get Requested features
    const RunLengthFilterType::FeatureNameVectorPointer requestedFeatures =
      RunLengthFilterType::FeatureNameVector::New();

    requestedFeatures->push_back(static_cast<uint8_t>(itk::Statistics::RunLengthFeatureEnum::ShortRunEmphasis));
    requestedFeatures->push_back(static_cast<uint8_t>(itk::Statistics::RunLengthFeatureEnum::GreyLevelNonuniformity));
    texFilter->SetRequestedFeatures(requestedFeatures);

    texFilter->Print(std::cout, 3);

    const RunLengthFilterType::FeatureNameVector * requestedFeatures2 = texFilter->GetRequestedFeatures();

    RunLengthFilterType::FeatureNameVector::ConstIterator fIt = requestedFeatures2->Begin();
    if (fIt.Value() != static_cast<uint8_t>(itk::Statistics::RunLengthFeatureEnum::ShortRunEmphasis))
    {
      std::cerr << "Requested feature name not correctly set" << std::endl;
      passed = false;
    }
    ++fIt;

    if (fIt.Value() != static_cast<uint8_t>(itk::Statistics::RunLengthFeatureEnum::GreyLevelNonuniformity))
    {
      std::cerr << "Requested feature name not correctly set" << std::endl;
      passed = false;
    }

    if (!passed)
    {
      std::cerr << "Test failed" << std::endl;
      return EXIT_FAILURE;
    }

    std::cerr << "Test succeeded" << std::endl;
    return EXIT_SUCCESS;
  }
  catch (const itk::ExceptionObject & err)
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    std::cerr << "Test failed" << std::endl;
    return EXIT_FAILURE;
  }
}
