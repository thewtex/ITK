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

#include "itkBSplineDerivativeKernelFunction.h"
#include "itkTestingMacros.h"


int
itkBSplineKernelFunctionTest(int, char *[])
{

  // Externally generated results
  constexpr unsigned int npoints = 49;

  constexpr double x[npoints] = { -3,    -2.875, -2.75, -2.625, -2.5,  -2.375, -2.25, -2.125, -2,    -1.875,
                                  -1.75, -1.625, -1.5,  -1.375, -1.25, -1.125, -1,    -0.875, -0.75, -0.625,
                                  -0.5,  -0.375, -0.25, -0.125, 0,     0.125,  0.25,  0.375,  0.5,   0.625,
                                  0.75,  0.875,  1,     1.125,  1.25,  1.375,  1.5,   1.625,  1.75,  1.875,
                                  2,     2.125,  2.25,  2.375,  2.5,   2.625,  2.75,  2.875,  3 };

  constexpr double b0[npoints] = { 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 1, 1, 1, 1,
                                   1, 1, 1, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0 };

  constexpr double b1[npoints] = { 0,    0,     0,   0,     0,     0,     0,     0,   0,     0,    0,     0, 0,
                                   0,    0,     0,   0,     0.125, 0.25,  0.375, 0.5, 0.625, 0.75, 0.875, 1, 0.875,
                                   0.75, 0.625, 0.5, 0.375, 0.25,  0.125, 0,     0,   0,     0,    0,     0, 0,
                                   0,    0,     0,   0,     0,     0,     0,     0,   0,     0 };

  constexpr double b2[npoints] = { 0,         0,         0,        0,        0,        0,         0,
                                   0,         0,         0,        0,        0,        0,         0.0078125,
                                   0.03125,   0.0703125, 0.125,    0.195313, 0.28125,  0.382813,  0.5,
                                   0.609375,  0.6875,    0.734375, 0.75,     0.734375, 0.6875,    0.609375,
                                   0.5,       0.382813,  0.28125,  0.195313, 0.125,    0.0703125, 0.03125,
                                   0.0078125, 0,         0,        0,        0,        0,         0,
                                   0,         0,         0,        0,        0,        0,         0 };

  constexpr double b3[npoints] = { 0,         0,         0,           0,          0,           0,         0,
                                   0,         0,         0.000325521, 0.00260417, 0.00878906,  0.0208333, 0.0406901,
                                   0.0703125, 0.111654,  0.166667,    0.236003,   0.315104,    0.398112,  0.479167,
                                   0.552409,  0.611979,  0.652018,    0.666667,   0.652018,    0.611979,  0.552409,
                                   0.479167,  0.398112,  0.315104,    0.236003,   0.166667,    0.111654,  0.0703125,
                                   0.0406901, 0.0208333, 0.00878906,  0.00260417, 0.000325521, 0,         0,
                                   0,         0,         0,           0,          0,           0,         0 };


  // Testing the output of BSplineKernelFunction
#define TEST_BSPLINE_KERNEL(ORDERNUM)                                               \
  {                                                                                 \
    using FunctionType = itk::BSplineKernelFunction<ORDERNUM>;                      \
    auto function = FunctionType::New();                                            \
                                                                                    \
    function->Print(std::cout);                                                     \
    constexpr double epsilon = 1e-6;                                                \
    for (unsigned int j = 0; j < npoints; ++j)                                      \
    {                                                                               \
      double results = function->Evaluate(x[j]);                                    \
      /* compare with external results */                                           \
      if (itk::Math::abs(results - b##ORDERNUM[j]) > epsilon)                       \
      {                                                                             \
        std::cerr.precision(static_cast<int>(itk::Math::abs(std::log10(epsilon)))); \
        std::cerr << "Test failed!" << std::endl;                                   \
        std::cerr << "Error with " << ORDERNUM << " order BSplineKernelFunction ";  \
        std::cerr << "at index [" << j << "] " << std::endl;                        \
        std::cerr << "Expected value " << b##ORDERNUM[j] << std::endl;              \
        std::cerr << " differs from " << results;                                   \
        std::cerr << " by more than " << epsilon << std::endl;                      \
        return EXIT_FAILURE;                                                        \
      }                                                                             \
    }                                                                               \
  }                                                                                 \
  ITK_MACROEND_NOOP_STATEMENT

  TEST_BSPLINE_KERNEL(0);
  TEST_BSPLINE_KERNEL(1);
  TEST_BSPLINE_KERNEL(2);
  TEST_BSPLINE_KERNEL(3);

  // Testing derivative spline order = 0
  {
    constexpr unsigned int SplineOrder = 0;
    using DerivativeFunctionType = itk::BSplineDerivativeKernelFunction<SplineOrder>;
    auto derivFunction = DerivativeFunctionType::New();
    derivFunction->Print(std::cout);

    constexpr double xx = -0.25;
    constexpr double expectedValue = 0.0;
    const double     results = derivFunction->Evaluate(xx);

    constexpr double epsilon = 1e-6;
    if (itk::Math::abs(results - expectedValue) > epsilon)
    {
      std::cerr.precision(static_cast<int>(itk::Math::abs(std::log10(epsilon))));
      std::cerr << "Test failed!" << std::endl;
      std::cerr << "Error with " << SplineOrder << " order BSplineDerivativeKernelFunction at " << xx << std::endl;
      std::cerr << "Expected value " << expectedValue << std::endl;
      std::cerr << " differs from " << results;
      std::cerr << " by more than " << epsilon << std::endl;
      return EXIT_FAILURE;
    }
  }

  // Testing derivative spline order = 1
  {
    constexpr unsigned int SplineOrder = 1;
    using DerivativeFunctionType = itk::BSplineDerivativeKernelFunction<SplineOrder>;
    auto derivFunction = DerivativeFunctionType::New();

    using FunctionType = itk::BSplineKernelFunction<SplineOrder - 1>;
    auto function = FunctionType::New();

    for (double xx = -3.0; xx <= 3.0; xx += 0.1)
    {
      const double expectedValue = function->Evaluate(xx + 0.5) - function->Evaluate(xx - 0.5);
      const double results = derivFunction->Evaluate(xx);

      constexpr double epsilon = 1e-6;
      if (itk::Math::abs(results - expectedValue) > epsilon)
      {
        std::cerr.precision(static_cast<int>(itk::Math::abs(std::log10(epsilon))));
        std::cerr << "Test failed!" << std::endl;
        std::cerr << "Error with " << SplineOrder << " order BSplineDerivativeKernelFunction at " << xx << std::endl;
        std::cerr << "Expected value " << expectedValue << std::endl;
        std::cerr << " differs from " << results;
        std::cerr << " by more than " << epsilon << std::endl;
        return EXIT_FAILURE;
      }
    }
  }

  // Testing derivative spline order = 2
  {
    constexpr unsigned int SplineOrder = 2;
    using DerivativeFunctionType = itk::BSplineDerivativeKernelFunction<SplineOrder>;
    auto derivFunction = DerivativeFunctionType::New();
    derivFunction->Print(std::cout);

    using FunctionType = itk::BSplineKernelFunction<SplineOrder - 1>;
    auto function = FunctionType::New();

    for (double xx = -3.0; xx <= 3.0; xx += 0.1)
    {
      const double expectedValue = function->Evaluate(xx + 0.5) - function->Evaluate(xx - 0.5);
      const double results = derivFunction->Evaluate(xx);

      constexpr double epsilon = 1e-6;
      if (itk::Math::abs(results - expectedValue) > epsilon)
      {
        std::cerr.precision(static_cast<int>(itk::Math::abs(std::log10(epsilon))));
        std::cerr << "Test failed!" << std::endl;
        std::cerr << "Error with " << SplineOrder << " order BSplineDerivativeKernelFunction at " << xx << std::endl;
        std::cerr << "Expected value " << expectedValue << std::endl;
        std::cerr << " differs from " << results;
        std::cerr << " by more than " << epsilon << std::endl;
        return EXIT_FAILURE;
      }
    }
  }

  // Testing derivative spline order = 3
  {
    constexpr unsigned int SplineOrder = 3;
    using DerivativeFunctionType = itk::BSplineDerivativeKernelFunction<SplineOrder>;
    auto derivFunction = DerivativeFunctionType::New();
    derivFunction->Print(std::cout);

    using FunctionType = itk::BSplineKernelFunction<SplineOrder - 1>;
    auto function = FunctionType::New();

    for (double xx = -3.0; xx <= 3.0; xx += 0.1)
    {
      const double expectedValue = function->Evaluate(xx + 0.5) - function->Evaluate(xx - 0.5);
      const double results = derivFunction->Evaluate(xx);

      constexpr double epsilon = 1e-6;
      if (itk::Math::abs(results - expectedValue) > epsilon)
      {
        std::cerr.precision(static_cast<int>(itk::Math::abs(std::log10(epsilon))));
        std::cerr << "Test failed!" << std::endl;
        std::cerr << "Error with " << SplineOrder << " order BSplineDerivativeKernelFunction at " << xx << std::endl;
        std::cerr << "Expected value " << expectedValue << std::endl;
        std::cerr << " differs from " << results;
        std::cerr << " by more than " << epsilon << std::endl;
        return EXIT_FAILURE;
      }
    }
  }

  // Testing case of unimplemented spline order
  {
    using FunctionType = itk::BSplineKernelFunction<7>;
    auto function = FunctionType::New();

    ITK_TRY_EXPECT_EXCEPTION(function->Evaluate(0.0));
  }

  // Testing case of unimplemented spline order
  {
    using FunctionType = itk::BSplineDerivativeKernelFunction<5>;
    auto function = FunctionType::New();

    ITK_TRY_EXPECT_EXCEPTION(function->Evaluate(0.0));
  }


  std::cout << "Test finished. " << std::endl;
  return EXIT_SUCCESS;
}
