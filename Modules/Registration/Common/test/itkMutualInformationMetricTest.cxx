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

#include "itkMutualInformationImageToImageMetric.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkTimeProbesCollectorBase.h"

#include <iostream>

/**
 *  This test uses two 2D-Gaussians (standard deviation RegionSize/2)
 *  One is shifted by 5 pixels from the other.
 *
 *  This test computes the mutual information value and derivatives
 *  for various shift values in (-10,10).
 *
 */

int
itkMutualInformationMetricTest(int, char *[])
{

  //------------------------------------------------------------
  // Create two simple images
  //------------------------------------------------------------

  // Allocate Images
  using MovingImageType = itk::Image<unsigned char, 2>;
  using FixedImageType = itk::Image<unsigned char, 2>;
  enum
  {
    ImageDimension = MovingImageType::ImageDimension
  };

  constexpr MovingImageType::SizeType  size = { { 100, 100 } };
  constexpr MovingImageType::IndexType index = { { 0, 0 } };
  const MovingImageType::RegionType    region{ index, size };

  auto imgMoving = MovingImageType::New();
  imgMoving->SetRegions(region);
  imgMoving->Allocate();

  auto imgFixed = FixedImageType::New();
  imgFixed->SetRegions(region);
  imgFixed->Allocate();

  // Fill images with a 2D gaussian
  using ReferenceIteratorType = itk::ImageRegionIterator<MovingImageType>;
  using TargetIteratorType = itk::ImageRegionIterator<FixedImageType>;

  itk::Point<double, 2> center;
  center[0] = static_cast<double>(region.GetSize()[0]) / 2.0;
  center[1] = static_cast<double>(region.GetSize()[1]) / 2.0;

  const double s = static_cast<double>(region.GetSize()[0]) / 2.0;

  itk::Point<double, 2>  p;
  itk::Vector<double, 2> d;

  // Set the displacement
  itk::Vector<double, 2> displacement;
  displacement[0] = 5;
  displacement[1] = 0;

  ReferenceIteratorType ri(imgMoving, region);
  TargetIteratorType    ti(imgFixed, region);
  ri.GoToBegin();
  while (!ri.IsAtEnd())
  {
    p[0] = ri.GetIndex()[0];
    p[1] = ri.GetIndex()[1];
    d = p - center;
    d += displacement;
    const double x = d[0];
    const double y = d[1];
    ri.Set(static_cast<unsigned char>(200.0 * std::exp(-(x * x + y * y) / (s * s))));
    ++ri;
  }


  ti.GoToBegin();
  while (!ti.IsAtEnd())
  {
    p[0] = ti.GetIndex()[0];
    p[1] = ti.GetIndex()[1];
    d = p - center;
    const double x = d[0];
    const double y = d[1];
    ti.Set(static_cast<unsigned char>(200.0 * std::exp(-(x * x + y * y) / (s * s))));
    ++ti;
  }

  //-----------------------------------------------------------
  // Set up a transformer
  //-----------------------------------------------------------
  using TransformType = itk::AffineTransform<double, ImageDimension>;
  using ParametersType = TransformType::ParametersType;

  auto transformer = TransformType::New();

  //------------------------------------------------------------
  // Set up an interpolator
  //------------------------------------------------------------
  using InterpolatorType = itk::LinearInterpolateImageFunction<MovingImageType, double>;

  auto interpolator = InterpolatorType::New();

  //------------------------------------------------------------
  // Set up the metric
  //------------------------------------------------------------
  using MetricType = itk::MutualInformationImageToImageMetric<FixedImageType, MovingImageType>;

  auto metric = MetricType::New();

  // connect the interpolator
  metric->SetInterpolator(interpolator);

  // connect the transform
  metric->SetTransform(transformer);

  // connect the images to the metric
  metric->SetFixedImage(imgFixed);
  metric->SetMovingImage(imgMoving);

  // set the standard deviations
  metric->SetFixedImageStandardDeviation(5.0);
  metric->SetMovingImageStandardDeviation(5.0);

  // set the number of samples to use
  metric->SetNumberOfSpatialSamples(100);

  // set the region over which to compute metric
  metric->SetFixedImageRegion(imgFixed->GetBufferedRegion());

  // initialize the metric before use
  metric->Initialize();

  //------------------------------------------------------------
  // Set up an affine transform parameters
  //------------------------------------------------------------
  const unsigned int numberOfParameters = transformer->GetNumberOfParameters();
  ParametersType     parameters(numberOfParameters);

  // set the parameters to the identity
  unsigned long count = 0;

  // initialize the linear/matrix part
  for (unsigned int row = 0; row < ImageDimension; ++row)
  {
    for (unsigned int col = 0; col < ImageDimension; ++col)
    {
      parameters[count] = 0;
      if (row == col)
      {
        parameters[count] = 1;
      }
      ++count;
    }
  }

  // initialize the offset/vector part
  for (unsigned int k = 0; k < ImageDimension; ++k)
  {
    parameters[count] = 0;
    ++count;
  }


  //---------------------------------------------------------
  // Print out mutual information values
  // for parameters[4] = {-10,10}
  //---------------------------------------------------------

  MetricType::MeasureType    measure = NAN;
  MetricType::DerivativeType derivative(numberOfParameters);

  itk::TimeProbesCollectorBase collector;
  collector.Start("Loop");

  std::cout << "param[4]\tMI\tdMI/dparam[4]" << std::endl;

  for (double trans = -10; trans <= 5; trans += 0.5)
  {
    parameters[4] = trans;
    metric->GetValueAndDerivative(parameters, measure, derivative);

    std::cout << trans << '\t' << measure << '\t' << derivative[4] << std::endl;

    // exercise the other functions
    metric->GetValue(parameters);
    metric->GetDerivative(parameters, derivative);
  }
  collector.Stop("Loop");
  collector.Report();

  //-------------------------------------------------------
  // exercise misc member functions
  //-------------------------------------------------------
  std::cout << "Name of class: " << metric->GetNameOfClass() << std::endl;
  std::cout << "No. of samples used = " << metric->GetNumberOfSpatialSamples() << std::endl;
  std::cout << "Fixed image std dev = " << metric->GetFixedImageStandardDeviation() << std::endl;
  std::cout << "Moving image std dev = " << metric->GetMovingImageStandardDeviation() << std::endl;

  metric->Print(std::cout);

  const itk::KernelFunctionBase<double>::Pointer theKernel = metric->GetModifiableKernelFunction();
  metric->SetKernelFunction(theKernel);
  theKernel->Print(std::cout);

  std::cout << "Try causing an exception by making std dev too small";
  std::cout << std::endl;
  metric->SetFixedImageStandardDeviation(0.001);
  try
  {
    metric->Initialize();
    std::cout << "Value = " << metric->GetValue(parameters);
    std::cout << std::endl;
  }
  catch (const itk::ExceptionObject & err)
  {
    std::cout << "Caught the exception." << std::endl;
    std::cout << err << std::endl;
  }

  // reset standard deviation
  metric->SetFixedImageStandardDeviation(5.0);

  std::cout << "Try causing an exception by making fixed image nullptr";
  std::cout << std::endl;
  metric->SetFixedImage(nullptr);
  try
  {
    metric->Initialize();
    std::cout << "Value = " << metric->GetValue(parameters);
    std::cout << std::endl;
  }
  catch (const itk::ExceptionObject & err)
  {
    std::cout << "Caught the exception." << std::endl;
    std::cout << err << std::endl;
  }


  return EXIT_SUCCESS;
}
