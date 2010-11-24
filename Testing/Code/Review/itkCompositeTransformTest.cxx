/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkCompositeTransformTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

#include "itkAffineTransform.h"
#include "itkScaleTransform.h"
#include "itkCompositeTransform.h"

const double epsilon = 1e-10;

template <typename TPoint>
bool testPoint( const TPoint & p1, const TPoint & p2 )
  {
  bool pass=true;
  for ( unsigned int i = 0; i < TPoint::PointDimension; i++ )
    {
    if( vcl_fabs( p1[i] - p2[i] ) > epsilon )
      pass=false;
    }
  return pass;
  }

template <typename TMatrix>
bool testMatrix( const TMatrix & m1, const TMatrix & m2 )
  {
  unsigned int i, j;
  bool pass=true;
  for ( i = 0; i < TMatrix::RowDimensions; i++ )
    {
    for ( j = 0; j < TMatrix::ColumnDimensions; j++ )
      {
      if( vcl_fabs( m1[i][j] - m2[i][j] ) > epsilon )
        pass=false;
      }
  }
  return pass;
  }

template <typename TVector>
bool testVector( const TVector & v1, const TVector & v2 )
  {
  bool pass=true;
  for ( unsigned int i = 0; i < TVector::Dimension; i++ )
    {
    if( vcl_fabs( v1[i] - v2[i] ) > epsilon )
      pass=false;
    }
  return pass;
  }

/******/

int itkCompositeTransformTest(int ,char *[] )
{
  #define NDIMENSIONS 2

  /* Create composite transform */
  typedef itk::CompositeTransform<double, NDIMENSIONS>  CompositeType;
  typedef CompositeType::ScalarType                     ScalarType;

  CompositeType::Pointer compositeTransform = CompositeType::New();

  /* Test obects */
  typedef  itk::Matrix<ScalarType,2,2>   Matrix2Type;
  typedef  itk::Vector<ScalarType,2>     Vector2Type;

  /* Add an affine transform */
  typedef itk::AffineTransform<ScalarType, NDIMENSIONS> AffineType;
  AffineType::Pointer affine = AffineType::New();
  Matrix2Type matrix2;
  Vector2Type vector2;
  matrix2[0][0] = 1;
  matrix2[0][1] = 2;
  matrix2[1][0] = 3;
  matrix2[1][1] = 4;
  vector2[0] = 5;
  vector2[1] = 6;
  affine->SetMatrix(matrix2);
  affine->SetOffset(vector2);

  compositeTransform->PushFrontTransform( affine );

  /* Test that we have one tranform in the queue */
  if( compositeTransform->GetNumberOfComposingTransforms() != 1 )
    {
    std::cout << "Failed adding transform to queue." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Composite Transform:" << std::endl << compositeTransform;

  /* Retrieve the transform and check that it's the same */
  AffineType::Pointer affineGet;
  affineGet = dynamic_cast<AffineType *>
    ( compositeTransform->GetFrontTransform().GetPointer() );
  if( affineGet.IsNull() )
    {
    std::cout << "Failed retrieving transform from queue." << std::endl;
    return EXIT_FAILURE;
    }

  Matrix2Type matrix2Get = affineGet->GetMatrix();
  Vector2Type vector2Get = affineGet->GetOffset();
  if( !testMatrix(matrix2, matrix2Get) || !testVector(vector2, vector2Get ) )
    {
    std::cout << "Failed retrieving correct transform data." << std::endl;
    return EXIT_FAILURE;
    }

  /* Setup test point and truth value for tests */
  CompositeType::InputPointType  inputPoint;
  CompositeType::OutputPointType outputPoint, affineTruth;
  inputPoint[0] = 2;
  inputPoint[1] = 3;
  affineTruth[0] = 13;
  affineTruth[1] = 24;

  /* Test transforming the point with just a single affine transform */
  outputPoint = compositeTransform->TransformPoint( inputPoint );
  if( !testPoint( outputPoint, affineTruth) )
      {
      std::cout << "Failed transforming point with single transform."
                << std::endl;
      return EXIT_FAILURE;
      }

  /* Test inverse */
  CompositeType::Pointer inverseTransform = CompositeType::New();
  CompositeType::OutputPointType inverseTruth, inverseOutput;
  if( !compositeTransform->GetInverse( inverseTransform ) )
    {
    std::cout << "Expected GetInverse() to succeed." << std::endl;
    return EXIT_FAILURE;
    }
  inverseTruth = inputPoint;
  inverseOutput = inverseTransform->TransformPoint( affineTruth );
  std::cout << "Transform point with inverse composite transform: "
            << std::endl
            << "  Test point: " << affineTruth << std::endl
            << "  Truth: " << inverseTruth << std::endl
            << "  Output: " << inverseOutput << std::endl;
  if( !testPoint( inverseOutput, inverseTruth ) )
    {
    std::cout << "Failed transform point with inverse composite transform (1)."
              << std::endl;
    return EXIT_FAILURE;
    }

  /*
   * Create and add 2nd transform .
   */
  typedef itk::ScaleTransform<double, NDIMENSIONS>  ScaleTransformType;
  ScaleTransformType::Pointer scaleTransform = ScaleTransformType::New();

  ScaleTransformType::ScaleType::ValueType iscaleInit[2] = {2,-0.5};
  ScaleTransformType::ScaleType            iscale = iscaleInit;
  scaleTransform->SetScale( iscale );

  compositeTransform->PushBackTransform( scaleTransform );

  std::cout << std::endl << "Two-component Composite Transform:"
            << std::endl << compositeTransform;

  /* Test that we have two tranforms in the queue */
  if( compositeTransform->GetNumberOfComposingTransforms() != 2 )
    {
    std::cout << "Failed adding 2nd transform to queue." << std::endl;
    return EXIT_FAILURE;
    }

  /* Transform a point with both transforms */
  CompositeType::OutputPointType compositeTruth;
  compositeTruth[0] = affineTruth[0] * iscale[0];
  compositeTruth[1] = affineTruth[1] * iscale[1];

  outputPoint = compositeTransform->TransformPoint( inputPoint );
  std::cout << "Transform point with two-component composite transform: "
            << std::endl
            << "  Test point: " << inputPoint << std::endl
            << "  Truth: " << compositeTruth << std::endl
            << "  Output: " << outputPoint << std::endl;

  if( !testPoint( outputPoint, compositeTruth) )
    {
    std::cout << "Failed transforming point with two transforms."
              << std::endl;
    return EXIT_FAILURE;
    }

  /* Test inverse with two transforms */
  if( !compositeTransform->GetInverse( inverseTransform ) )
    {
    std::cout << "Expected GetInverse() to succeed." << std::endl;
    return EXIT_FAILURE;
    }
  inverseTruth = inputPoint;
  inverseOutput = inverseTransform->TransformPoint( compositeTruth );
  std::cout << "Transform point with two-component inverse composite transform: "
            << std::endl
            << "  Test point: " << compositeTruth << std::endl
            << "  Truth: " << inverseTruth << std::endl
            << "  Output: " << inverseOutput << std::endl;
  if( !testPoint( inverseOutput, inverseTruth ) )
    {
    std::cout << "Failed transform point with two-component inverse composite transform."
              << std::endl;
    return EXIT_FAILURE;
    }

  /* Get inverse transform again, but using other accessor. */
  CompositeType::ConstPointer inverseTransform2;
  inverseTransform2 = dynamic_cast<const CompositeType *>
    ( compositeTransform->GetInverseTransform().GetPointer() );
  if( ! inverseTransform2 )
    {
    std::cout << "Failed calling GetInverseTransform()." << std::endl;
    return EXIT_FAILURE;
    }
  inverseOutput = inverseTransform2->TransformPoint( compositeTruth );
  if( !testPoint( inverseOutput, inverseTruth ) )
    {
    std::cout << "Failed transform point with two-component inverse composite transform (2)."
              << std::endl;
    return EXIT_FAILURE;
    }

  /* Test IsLinear() by calling on each component transform */
  bool allAreLinear = true;
  for( unsigned int n=0;
        n < compositeTransform->GetNumberOfComposingTransforms(); n ++)
    {
    if( ! compositeTransform->GetNthTransform( n )->IsLinear() )
      {
      allAreLinear = false;
      }
    }
  if( compositeTransform->IsLinear() != allAreLinear )
    {
    std::cout << "compositeTransform returned unexpected value for IsLinear()."
      " Expected " << allAreLinear << std::endl;
    return EXIT_FAILURE;
    }

  /* Test GetJacobian - simplistic testing for now */
  CompositeType::JacobianType jacobian;
  CompositeType::InputPointType jacPoint;
  jacPoint[0]=1;
  jacPoint[1]=2;
  jacobian = compositeTransform->GetJacobian( jacPoint );

  /* Test parameter get/set - minimal exercising for now */
  CompositeType::ParametersType parameters, fixedParameters;

  parameters = compositeTransform->GetParameters();
  compositeTransform->SetParameters( parameters );

  fixedParameters = compositeTransform->GetFixedParameters();
  /* unimplemented */
  //compositeTransform->SetFixedParameters( fixedParameters );

  unsigned int nParameters = compositeTransform->GetNumberOfParameters();
  std::cout << "Number of parameters: " << nParameters << std::endl;

  return EXIT_SUCCESS;
}
