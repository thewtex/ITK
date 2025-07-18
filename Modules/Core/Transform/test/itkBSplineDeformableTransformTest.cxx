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
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/

#include "itkBSplineDeformableTransform.h"

#include "itkVersorRigid3DTransform.h"

#include "itkTextOutput.h"
#include "itkTestingMacros.h"

/**
 * This module test the functionality of the BSplineDeformableTransform class.
 *
 */
int
itkBSplineDeformableTransformTest1()
{

  // Comment the following if you want to use the itk text output window
  itk::OutputWindow::SetInstance(itk::TextOutput::New());

  // Uncomment the following if you want to see each message independently
  // itk::OutputWindow::GetInstance()->PromptUserOn();

  constexpr unsigned int SpaceDimension = 3;
  constexpr unsigned int SplineOrder = 3;
  using CoordinateRepType = double;
  using TransformType = itk::BSplineDeformableTransform<CoordinateRepType, SpaceDimension, SplineOrder>;

  using ParametersType = TransformType::ParametersType;

  /**
   * Define the deformable grid region, spacing and origin
   */

  using OriginType = TransformType::OriginType;
  constexpr OriginType origin{};

  using RegionType = TransformType::RegionType;
  RegionType region;
  auto       size = RegionType::SizeType::Filled(10);
  region.SetSize(size);
  std::cout << region << std::endl;

  using SpacingType = TransformType::SpacingType;
  auto spacing = itk::MakeFilled<SpacingType>(2.0);

  /**
   * Instantiate a transform
   */
  auto transform = TransformType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(transform, BSplineDeformableTransform, BSplineBaseTransform);

  transform->SetGridSpacing(spacing);
  ITK_TEST_SET_GET_VALUE(spacing, transform->GetGridSpacing());

  transform->SetGridOrigin(origin);
  ITK_TEST_SET_GET_VALUE(origin, transform->GetGridOrigin());

  transform->SetGridRegion(region);
  ITK_TEST_SET_GET_VALUE(region, transform->GetGridRegion());

  using DirectionType = TransformType::DirectionType;
  const DirectionType direction = transform->GetCoefficientImages()[0]->GetDirection();
  transform->SetGridDirection(direction);
  ITK_TEST_SET_GET_VALUE(direction, transform->GetGridDirection());

  std::cout << transform->GetValidRegion() << std::endl;

  /**
   * Allocate memory for the parameters
   */
  const unsigned long numberOfParameters = transform->GetNumberOfParameters();
  ParametersType      parameters(numberOfParameters);
  parameters.Fill(ParametersType::ValueType{});

  /**
   * Define N * N-D grid of spline coefficients by wrapping the
   * flat array into N images.
   * Initialize by setting all elements to zero
   */
  using CoefficientType = ParametersType::ValueType;
  using CoefficientImageType = itk::Image<CoefficientType, SpaceDimension>;

  CoefficientImageType::Pointer coeffImages[SpaceDimension];
  const unsigned int            numberOfControlPoints = region.GetNumberOfPixels();
  CoefficientType *             dataPointer = parameters.data_block();
  for (auto & it : coeffImages)
  {
    it = CoefficientImageType::New();
    it->SetRegions(region);
    it->GetPixelContainer()->SetImportPointer(dataPointer, numberOfControlPoints);
    dataPointer += numberOfControlPoints;
    it->FillBuffer(0.0);
  }

  /**
   * Populate the spline coefficients with some values.
   */
  auto index = CoefficientImageType::IndexType::Filled(5);

  coeffImages[1]->SetPixel(index, 1.0);

  const unsigned long n = coeffImages[1]->ComputeOffset(index) + numberOfControlPoints;

  /**
   * Set the parameters in the transform
   */
  transform->SetParameters(parameters);

  // outParametersCopy should make a copy of the parameters
  const ParametersType outParametersCopy = transform->GetParameters();

  /**
   * Set a bulk transform
   */
  using BulkTransformType = itk::VersorRigid3DTransform<CoordinateRepType>;
  auto bulkTransform = BulkTransformType::New();

  // optional: set bulk transform parameters

  transform->SetBulkTransform(bulkTransform);
  ITK_TEST_SET_GET_VALUE(bulkTransform, transform->GetBulkTransform());

  /**
   * Transform some points
   */
  using PointType = TransformType::InputPointType;

  PointType inputPoint;
  PointType outputPoint;

  // point within the grid support region
  inputPoint.Fill(9.0);
  outputPoint = transform->TransformPoint(inputPoint);

  std::cout << "Input Point: " << inputPoint << std::endl;
  std::cout << "Output Point: " << outputPoint << std::endl;
  std::cout << std::endl;

  // point outside the grid support region
  inputPoint.Fill(40.0);
  outputPoint = transform->TransformPoint(inputPoint);

  std::cout << "Input Point: " << inputPoint << std::endl;
  std::cout << "Output Point: " << outputPoint << std::endl;
  std::cout << std::endl;

  // point inside the grid support region
  inputPoint.Fill(2.0);
  outputPoint = transform->TransformPoint(inputPoint);

  std::cout << "Input Point: " << inputPoint << std::endl;
  std::cout << "Output Point: " << outputPoint << std::endl;
  std::cout << std::endl;

  // point inside the grid support region
  inputPoint.Fill(15.9);
  outputPoint = transform->TransformPoint(inputPoint);

  std::cout << "Input Point: " << inputPoint << std::endl;
  std::cout << "Output Point: " << outputPoint << std::endl;
  std::cout << std::endl;

  // point outside the grid support region
  inputPoint.Fill(1.9);
  outputPoint = transform->TransformPoint(inputPoint);

  std::cout << "Input Point: " << inputPoint << std::endl;
  std::cout << "Output Point: " << outputPoint << std::endl;
  std::cout << std::endl;

  // point outside the grid support region
  inputPoint.Fill(16.0);
  outputPoint = transform->TransformPoint(inputPoint);

  std::cout << "Input Point: " << inputPoint << std::endl;
  std::cout << "Output Point: " << outputPoint << std::endl;
  std::cout << std::endl;

  // set bulk transform to nullptr
  transform->SetBulkTransform(nullptr);

  // use the other version of TransformPoint
  using WeightsType = TransformType::WeightsType;
  using IndexArrayType = TransformType::ParameterIndexArrayType;

  WeightsType    weights;
  IndexArrayType indices;
  bool           inside = false;

  inputPoint.Fill(8.3);
  transform->TransformPoint(inputPoint, outputPoint, weights, indices, inside);

  std::cout << "Number of Parameters: " << transform->GetNumberOfParameters() << std::endl;
  std::cout << "Number of Parameters per dimension: " << transform->GetNumberOfParametersPerDimension() << std::endl;
  std::cout << "Input Point: " << inputPoint << std::endl;
  std::cout << "Output Point: " << outputPoint << std::endl;
  std::cout << "Indices: " << indices << std::endl;
  std::cout << "Weights: " << weights << std::endl;
  std::cout << "Inside: " << inside << std::endl;
  std::cout << std::endl;

  // cycling through all the parameters and weights used in the previous
  // transformation
  constexpr unsigned int numberOfCoefficientInSupportRegion = TransformType::NumberOfWeights;
  const unsigned int     numberOfParametersPerDimension = transform->GetNumberOfParametersPerDimension();

  std::cout << "Index" << '\t' << "Value" << '\t' << "Weight" << std::endl;
  for (unsigned int j = 0; j < SpaceDimension; ++j)
  {
    const auto baseIndex = j * numberOfParametersPerDimension;
    for (unsigned int k = 0; k < numberOfCoefficientInSupportRegion; ++k)
    {
      const auto linearIndex = indices[k] + baseIndex;
      std::cout << linearIndex << '\t';
      std::cout << parameters[linearIndex] << '\t';
      std::cout << weights[k] << '\t';
      std::cout << std::endl;
    }
  }

  /**
   * TODO: add test to check the numerical accuracy of the transform
   */

  /**
   * Compute the Jacobian for various points
   */
  using JacobianType = TransformType::JacobianType;

#define PRINT_VALUE(R, C)                    \
  std::cout << "Jacobian[" #R "," #C "] = "; \
  std::cout << jacobian[R][C] << std::endl;  \
  ITK_MACROEND_NOOP_STATEMENT

  {
    // point inside the grid support region
    inputPoint.Fill(10.0);
    JacobianType jacobian;
    transform->ComputeJacobianWithRespectToParameters(inputPoint, jacobian);
    PRINT_VALUE(0, n);
    PRINT_VALUE(1, n);
    PRINT_VALUE(2, n);
    std::cout << std::endl;
  }

  {
    // point outside the grid support region
    inputPoint.Fill(-10.0);
    JacobianType jacobian;
    transform->ComputeJacobianWithRespectToParameters(inputPoint, jacobian);
    PRINT_VALUE(0, n);
    PRINT_VALUE(1, n);
    PRINT_VALUE(2, n);
    std::cout << std::endl;
  }

  /**
   * TODO: add test to check the numerical accuracy of the jacobian output
   */

  /**
   * TransformVector and TransformCovariant are not applicable for this
   * transform and should throw exceptions
   */
  {
    using VectorType = TransformType::InputVectorType;
    auto vector = itk::MakeFilled<VectorType>(1.0);

    bool pass = false;
    try
    {
      transform->TransformVector(vector);
    }
    catch (const itk::ExceptionObject & err)
    {
      std::cout << "Caught expected exception." << std::endl;
      std::cout << err << std::endl;
      pass = true;
    }
    if (!pass)
    {
      std::cout << "Did not catch expected exception." << std::endl;
      std::cout << "Test failed. " << std::endl;
      return EXIT_FAILURE;
    }
  }

  {
    using VectorType = TransformType::InputCovariantVectorType;
    auto vector = itk::MakeFilled<VectorType>(1.0);

    bool pass = false;
    try
    {
      transform->TransformCovariantVector(vector);
    }
    catch (const itk::ExceptionObject & err)
    {
      std::cout << "Caught expected exception." << std::endl;
      std::cout << err << std::endl;
      pass = true;
    }
    if (!pass)
    {
      std::cout << "Did not catch expected exception." << std::endl;
      std::cout << "Test failed. " << std::endl;
      return EXIT_FAILURE;
    }
  }

  {
    using VectorType = TransformType::InputVnlVectorType;
    VectorType vector;
    vector.fill(1.0);

    bool pass = false;
    try
    {
      transform->TransformVector(vector);
    }
    catch (const itk::ExceptionObject & err)
    {
      std::cout << "Caught expected exception." << std::endl;
      std::cout << err << std::endl;
      pass = true;
    }
    if (!pass)
    {
      std::cout << "Did not catch expected exception." << std::endl;
      std::cout << "Test failed. " << std::endl;
      return EXIT_FAILURE;
    }
  }

  {
    bool pass = false;
    try
    {
      ParametersType temp(transform->GetNumberOfParameters() - 1);
      temp.Fill(4.0);
      transform->SetParameters(temp);
    }
    catch (const itk::ExceptionObject & err)
    {
      std::cout << "Caught expected exception." << std::endl;
      std::cout << err << std::endl;
      pass = true;
    }
    if (!pass)
    {
      std::cout << "Did not catch expected exception." << std::endl;
      std::cout << "Test failed. " << std::endl;
      return EXIT_FAILURE;
    }
  }

  using EvenOrderTransformType = itk::BSplineDeformableTransform<CoordinateRepType, SpaceDimension, 2>;
  auto evenOrderTransform = EvenOrderTransformType::New();
  if (evenOrderTransform.IsNull())
  {
    return EXIT_FAILURE;
  }

  /**
   * Parameters should remain even when the transform has been destroyed
   */
  transform = nullptr;

  if (outParametersCopy != parameters)
  {
    std::cout << "parameters should remain intact after transform is destroyed";
    std::cout << std::endl;
    std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
  }

  /**
   * Exercise the SetIdentity() Method
   */
  {
    std::cout << "Exercising SetIdentity() " << std::endl;
    auto transform2 = TransformType::New();
    transform2->SetGridSpacing(spacing);
    transform2->SetGridOrigin(origin);
    transform2->SetGridRegion(region);
    transform2->SetParameters(parameters);
    transform2->SetIdentity();
    TransformType::ParametersType parameters2 = transform2->GetParameters();
    const unsigned int            numberOfParameters2 = transform2->GetNumberOfParameters();
    std::cout << "numberOfParameters =  " << numberOfParameters2 << std::endl;
    for (unsigned int i = 0; i < numberOfParameters2; ++i)
    {
      if (itk::Math::abs(parameters2[i]) > 1e-10)
      {
        std::cerr << "SetIdentity failed, parameters are not null "
                  << "after invoking SetIdentity() " << std::endl;
        return EXIT_FAILURE;
      }
    }
  } // end of SetIdentity() test

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}

int
itkBSplineDeformableTransformTest2()
{
  /**
   * This function tests the Set/GetCoefficientImages interface
   */
  itk::OutputWindow::SetInstance(itk::TextOutput::New());

  /**
   * Define a vector field as Dimension number of images
   */
  constexpr unsigned int Dimension = 2;

  // Set up the transform
  constexpr unsigned int SplineOrder = 3;
  using CoordRep = double;
  using TransformType = itk::BSplineDeformableTransform<CoordRep, Dimension, SplineOrder>;
  using ImageType = TransformType::ImageType;

  auto transform = TransformType::New();

  // Set up field spacing, origin, region
  double spacing[Dimension];
  double origin[Dimension];

  for (unsigned int j = 0; j < Dimension; ++j)
  {
    spacing[j] = 10.0;
    origin[j] = -10.0;
  }
  ImageType::SizeType size;
  size[0] = 5;
  size[1] = 7;

  ImageType::RegionType region;
  region.SetSize(size);

  TransformType::CoefficientImageArray field;
  for (unsigned int j = 0; j < Dimension; ++j)
  {
    field[j] = ImageType::New();
    field[j]->SetSpacing(spacing);
    field[j]->SetOrigin(origin);
    field[j]->SetRegions(region);
    field[j]->Allocate();
  }

  // fill the field with a constant displacement
  itk::Vector<double, Dimension> v;
  v[0] = 5;
  v[1] = 7;
  for (unsigned int j = 0; j < Dimension; ++j)
  {
    field[j]->FillBuffer(v[j]);
  }

  // This should generate an exception because parameters have not yet
  // been set.
  auto                           inputPoint = itk::MakeFilled<TransformType::InputPointType>(0.0);
  TransformType::OutputPointType outputPoint;
  {
    bool exceptionCaught(false);
    try
    {
      outputPoint = transform->TransformPoint(inputPoint);
    }
    catch (const itk::ExceptionObject & err)
    {
      std::cout << "Expected exception:" << std::endl;
      std::cout << err << std::endl;
      exceptionCaught = true;
    }
    if (!exceptionCaught)
    {
      std::cerr << "Expected exception not caught" << std::endl;
      return EXIT_FAILURE;
    }
  }
  // Set the coefficient images
  transform->SetCoefficientImages(field);

  // Exercise get and print methods
  transform->Print(std::cout);
  std::cout << "CoefficientImage[0]: " << transform->GetCoefficientImages()[0].GetPointer() << std::endl;

  /**
   * Transform some points
   */
  try
  {

    // try a point inside the valid region
    inputPoint.Fill(10.0);
    outputPoint = transform->TransformPoint(inputPoint);
    std::cout << " InputPoint: " << inputPoint;
    std::cout << " OutputPoint: " << outputPoint;
    std::cout << std::endl;

    // try a point on the valid region boundary
    inputPoint.Fill(0.0);
    outputPoint = transform->TransformPoint(inputPoint);
    std::cout << " InputPoint: " << inputPoint;
    std::cout << " OutputPoint: " << outputPoint;
    std::cout << std::endl;

    // try a point on the valid region boundary
    inputPoint[0] = 19.9;
    inputPoint[1] = 30.0;
    outputPoint = transform->TransformPoint(inputPoint);
    std::cout << " InputPoint: " << inputPoint;
    std::cout << " OutputPoint: " << outputPoint;
    std::cout << std::endl;

    // try a point outside the valid region
    inputPoint[0] = 20.0;
    inputPoint[1] = 30.0;
    outputPoint = transform->TransformPoint(inputPoint);
    std::cout << " InputPoint: " << inputPoint;
    std::cout << " OutputPoint: " << outputPoint;
    std::cout << std::endl;
  }
  catch (const itk::ExceptionObject & err)
  {
    std::cout << err << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}

int
itkBSplineDeformableTransformTest3()
{

  // This function tests the SetParametersByValue interface

  // Comment the following if you want to use the itk text output window
  itk::OutputWindow::SetInstance(itk::TextOutput::New());

  constexpr unsigned int SpaceDimension = 3;
  constexpr unsigned int SplineOrder = 3;
  using CoordinateRepType = double;
  using TransformType = itk::BSplineDeformableTransform<CoordinateRepType, SpaceDimension, SplineOrder>;

  using ParametersType = TransformType::ParametersType;

  /**
   * Define the deformable grid region, spacing and origin
   */

  using OriginType = TransformType::OriginType;
  constexpr OriginType origin{};

  using RegionType = TransformType::RegionType;
  RegionType region;
  auto       size = RegionType::SizeType::Filled(10);
  region.SetSize(size);
  std::cout << region << std::endl;

  using SpacingType = TransformType::SpacingType;
  auto spacing = itk::MakeFilled<SpacingType>(2.0);
  /**
   * Instantiate a transform
   */
  auto transform = TransformType::New();

  transform->SetGridSpacing(spacing);
  transform->SetGridOrigin(origin);
  transform->SetGridRegion(region);
  transform->Print(std::cout);

  /**
   * Allocate memory for the parameters
   */
  const unsigned long numberOfParameters = transform->GetNumberOfParameters();
  ParametersType      parameters(numberOfParameters);

  /**
   * Define N * N-D grid of spline coefficients by wrapping the
   * flat array into N images.
   * Initialize by setting all elements to zero
   */
  using CoefficientType = ParametersType::ValueType;
  using CoefficientImageType = itk::Image<CoefficientType, SpaceDimension>;

  CoefficientImageType::Pointer coeffImages[SpaceDimension];
  const unsigned int            numberOfControlPoints = region.GetNumberOfPixels();
  CoefficientType *             dataPointer = parameters.data_block();
  for (auto & it : coeffImages)
  {
    it = CoefficientImageType::New();
    it->SetRegions(region);
    it->GetPixelContainer()->SetImportPointer(dataPointer, numberOfControlPoints);
    dataPointer += numberOfControlPoints;
    it->FillBuffer(0.0);
  }

  /**
   * Populate the spline coefficients with some values.
   */
  auto index = CoefficientImageType::IndexType::Filled(5);

  coeffImages[1]->SetPixel(index, 1.0);

  /**
   * Set the parameters in the transform
   */
  transform->SetParametersByValue(parameters);

  /**
   * Transform some points
   */
  using PointType = TransformType::InputPointType;


  // point within the grid support region
  auto      inputPoint = itk::MakeFilled<PointType>(9.0);
  PointType outputPoint = transform->TransformPoint(inputPoint);

  std::cout << "Input Point: " << inputPoint << std::endl;
  std::cout << "Output Point: " << outputPoint << std::endl;
  std::cout << std::endl;

  /**
   * Internal parameters should remain even when the external parameters
   *  has been destroyed
   */
  parameters = ParametersType(0);

  // point within the grid support region
  inputPoint.Fill(9.0);
  outputPoint = transform->TransformPoint(inputPoint);

  std::cout << "Input Point: " << inputPoint << std::endl;
  std::cout << "Output Point: " << outputPoint << std::endl;
  std::cout << std::endl;

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}

int
itkBSplineDeformableTransformTest(int, char *[])
{
  bool failed = itkBSplineDeformableTransformTest1();
  if (failed)
  {
    return EXIT_FAILURE;
  }

  failed = itkBSplineDeformableTransformTest2();
  if (failed)
  {
    return EXIT_FAILURE;
  }

  failed = itkBSplineDeformableTransformTest3();
  if (failed)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
