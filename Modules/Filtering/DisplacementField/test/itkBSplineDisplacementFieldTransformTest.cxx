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

#include "itkBSplineDisplacementFieldTransform.h"
#include "itkImageFileWriter.h"
#include "itkVectorInterpolateImageFunction.h"
#include "itkVectorLinearInterpolateImageFunction.h"

#define NDIMENSIONS 2
typedef itk::BSplineDisplacementFieldTransform<double, NDIMENSIONS>
                                               DisplacementTransformType;
typedef DisplacementTransformType::ScalarType  ScalarType;

const ScalarType epsilon = 1e-10;

int itkBSplineDisplacementFieldTransformTest( int ,char *[] )
{

  /* NOTE
   * Requires updating when DisplacementFieldTransform is
   * fully implemented with operations for transforming
   * vectors as well. Currently this just tests transforming
   * points. */

  typedef  itk::Matrix<ScalarType, NDIMENSIONS, NDIMENSIONS>  Matrix2Type;
  typedef  itk::Vector<ScalarType, NDIMENSIONS>               Vector2Type;

  /* Create a displacement field transform */
  DisplacementTransformType::Pointer displacementTransform =
      DisplacementTransformType::New();
  typedef DisplacementTransformType::DisplacementFieldType FieldType;
  FieldType::Pointer field = FieldType::New(); //This is based on itk::Image

  FieldType::SizeType size;
  FieldType::IndexType start;
  FieldType::RegionType region;
  size.Fill( 30 );
  start.Fill( 0 );
  region.SetSize( size );
  region.SetIndex( start );
  field->SetRegions( region );
  field->Allocate();

  DisplacementTransformType::OutputVectorType zeroVector;
  zeroVector.Fill( 0 );
  field->FillBuffer( zeroVector );

  /*
   * Set a displacement field that has only one point with
   * a non-zero vector. */
  FieldType::IndexType nonZeroFieldIndex;
  nonZeroFieldIndex[0] = 3;
  nonZeroFieldIndex[1] = 4;
  ScalarType data1[] = {4,-2.5};
  DisplacementTransformType::OutputVectorType nonZeroFieldVector(data1);
  field->SetPixel( nonZeroFieldIndex, nonZeroFieldVector );

  DisplacementTransformType::ArrayType meshSize;
  meshSize.Fill( 1 );

  DisplacementTransformType::ArrayType numberOfFittingLevels;
  numberOfFittingLevels.Fill( 5 );

  displacementTransform->SetSplineOrder( 3 );
  displacementTransform->SetMeshSize( meshSize );
  displacementTransform->SetNumberOfFittingLevels( numberOfFittingLevels );
  displacementTransform->SetCalculateApproximateInverseDisplacementField( true );

  displacementTransform->SetDisplacementField( field );

  std::cout << "displacementTransform: " << std::endl
            << displacementTransform << std::endl;
  std::cout << "Spline order: " << displacementTransform->GetSplineOrder()
    << std::endl;
  std::cout << "Number of control points: "
    << displacementTransform->GetNumberOfControlPoints() << std::endl;
  std::cout << "Number of fitting levels: "
    << displacementTransform->GetNumberOfFittingLevels() << std::endl;
  std::cout << "Inverse?: " << ( displacementTransform->
    GetCalculateApproximateInverseDisplacementField() ? '1' : '0' )
    << std::endl;

  displacementTransform->Print( std::cout, 4 );

  /* Test transforming some points. */

  DisplacementTransformType::InputPointType testPoint;
  DisplacementTransformType::OutputPointType deformOutput, deformTruth;

  /* Test a point with non-zero displacement */
  testPoint[0] = nonZeroFieldIndex[0];
  testPoint[1] = nonZeroFieldIndex[1];
  deformOutput = displacementTransform->TransformPoint( testPoint );
  std::cout << "point 1 transformed: " << deformOutput;

  /* Test a non-integer point using the linear interpolator.
   * The default interpolator thus far is linear, but set it
   * just in case, and to test the set function and test TransforPoint's
   * handling of changed displacement field and interpolator objects. */
  typedef itk::VectorLinearInterpolateImageFunction<FieldType, ScalarType>
    LinearInterpolatorType;
  LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
  std::cout << "Interpolator:" << std::endl << interpolator;
  displacementTransform->SetInterpolator( interpolator );
  std::cout << "\n***Transform after add interpolator: \n"
    << displacementTransform << std::endl;

  ScalarType xoffset = 0.4;
  testPoint[0] = nonZeroFieldIndex[0] + xoffset;
  testPoint[1] = nonZeroFieldIndex[1];
  deformOutput = displacementTransform->TransformPoint( testPoint );
  std::cout << "Transform point with offset: " << std::endl
            << "  Test point: " << testPoint << std::endl
            << "  Output: " << deformOutput << std::endl;

  /* Test IsLinear()
   * Should always return false */
  if( displacementTransform->IsLinear() )
    {
    std::cout << "DisplacementFieldTransform returned 'true' for IsLinear()."
      " Expected 'false'." << std::endl;
    return EXIT_FAILURE;
    }

  /* Test inverse transform */

  /* Retrieve the inverse transform */
  DisplacementTransformType::Pointer inverseTransform
    = DisplacementTransformType::New();
  if( !displacementTransform->GetInverse( inverseTransform ) )
    {
    std::cout << "Expected GetInverse() to succeed." << std::endl;
    return EXIT_FAILURE;
    }

  /* Transform a point using inverse. Not much of a different test
   * than for forwards transform. */
  FieldType::IndexType inverseFieldIndex;
  inverseFieldIndex[0] = 7;
  inverseFieldIndex[1] = 11;
  DisplacementTransformType::OutputVectorType inverseFieldVector;
  DisplacementTransformType::OutputPointType inverseTruth, inverseOutput;
  testPoint[0] = inverseFieldIndex[0];
  testPoint[1] = inverseFieldIndex[1];
  inverseOutput = inverseTransform->TransformPoint( testPoint );

  /* Test GetJacobian - Since there are no parameters for this transform,
   * the Jacobian shouldn't be requested and will throw an exception. */
  DisplacementTransformType::JacobianType jacobian;
  DisplacementTransformType::InputPointType inputPoint;
  inputPoint[0]=1;
  inputPoint[1]=2;
  try
    {
    displacementTransform->ComputeJacobianWithRespectToParameters(
      inputPoint, jacobian );
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << e.GetDescription() << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
