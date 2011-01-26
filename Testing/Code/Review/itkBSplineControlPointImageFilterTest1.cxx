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
#include "itkBSplineControlPointImageFilter.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkPointSet.h"
#include "itkVector.h"

int itkBSplineControlPointImageFilterTest1( int argc, char *argv[] )
{


//   We construct a B-spline parametric curve equal to f(u) = 0.5 * u^2 - 0.5 * u
//   + 1/6.  This is done using a cubic order spline with controls points
//   (1, 0, 0, 1)

  const unsigned int ParametricDimension = 1;
  const unsigned int DataDimension = 1;

  typedef float                                         RealType;
  typedef itk::Vector<RealType, DataDimension>          VectorType;
  typedef itk::Image<VectorType, ParametricDimension>   VectorImageType;
  typedef itk::PointSet
    <VectorImageType::PixelType, ParametricDimension>   PointSetType;

  VectorImageType::Pointer phiLattice = VectorImageType::New();

  VectorImageType::SizeType size;
  VectorImageType::SpacingType spacing;
  VectorImageType::PointType origin;

  size.Fill( 4 );
  spacing.Fill( 1.0 );
  origin.Fill( 0.0 );
  phiLattice->SetOrigin( origin );
  phiLattice->SetSpacing( spacing );
  phiLattice->SetRegions( size );
  phiLattice->Allocate();
  phiLattice->FillBuffer( 0.0 );

  // To create the specified function, the first and last control points have
  // a value of 1.0;

  VectorImageType::IndexType index;
  VectorImageType::PixelType value;
  index.Fill( 0 );
  value.Fill( 1.0 );
  phiLattice->SetPixel( index, value );
  index.Fill( 3 );
  value.Fill( 1.0 );
  phiLattice->SetPixel( index, value );

  typedef itk::BSplineControlPointImageFilter<VectorImageType, VectorImageType>
    BSplinerType;
  BSplinerType::Pointer bspliner = BSplinerType::New();

  // Define the parametric domain [0, 1).
  origin.Fill( 0 );
  spacing.Fill( 0.01 );
  size.Fill( 101 );

  bspliner->SetOrigin( origin );
  bspliner->SetSpacing( spacing );
  bspliner->SetSize( size );
  bspliner->SetSplineOrder( 3 );
  bspliner->SetInput( phiLattice );

  BSplinerType::PointType point;
  BSplinerType::GradientType gradient;
  BSplinerType::GradientType hessianComponent;
  BSplinerType::PointDataType data;

  // f(0) = 1/6;
  // f'(u) = u - 0.5 so f'(0) should be -0.5.
  // f"(u) = 1
  try
    {
    point[0] = 0.0;

    bspliner->EvaluateAtPoint( point, data );
    if( vnl_math_abs( data[0] - 0.166666666667 ) > 1e-5 )
      {
      std::cerr << "Evaluate1: data is further away from the expected value."
        << std::endl;
      return EXIT_FAILURE;
      }

    bspliner->EvaluateGradientAtPoint( point, gradient );
    if( vnl_math_abs( gradient(0, 0) + 0.5 ) > 1e-5 )
      {
      std::cerr << "Evaluate1: gradient is further away from the expected value."
        << std::endl;
      return EXIT_FAILURE;
      }

    bspliner->EvaluateHessianAtPoint( point, hessianComponent, 0 );

    if( vnl_math_abs( hessianComponent(0, 0) - 1.0 ) > 1e-5 )
      {
      std::cerr << "Evaluate1: hessian is further away from the expected value."
        << std::endl;
      return EXIT_FAILURE;
      }
    }
  catch(...)
    {
    std::cerr << "Error in evaluate functions" << std::endl;
    return EXIT_FAILURE;
    }

  // f(0.351) = 0.05276717;
  // f'(0.351) = -0.149
  try
    {
    point[0] = 0.351;

    bspliner->EvaluateAtPoint( point, data );
    if( vnl_math_abs( data[0] - 0.05276717 ) > 1e-5 )
      {
      std::cerr << "Evaluate2: data is further away from the expected value."
        << std::endl;
      return EXIT_FAILURE;
      }

    bspliner->EvaluateGradientAtPoint( point, gradient );
    if( vnl_math_abs( gradient(0, 0) + 0.149 ) > 1e-5 )
      {
      std::cerr << "Evaluate2: gradient is further away from the expected value."
        << std::endl;
      return EXIT_FAILURE;
      }

    bspliner->EvaluateHessianAtPoint( point, hessianComponent, 0 );
    if( vnl_math_abs( hessianComponent(0, 0) - 1.0 ) > 1e-5 )
      {
      std::cerr << "Evaluate2: hessian is further away from the expected value."
        << std::endl;
      return EXIT_FAILURE;
      }
    }
  catch(...)
    {
    std::cerr << "Error in evaluate functions" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
