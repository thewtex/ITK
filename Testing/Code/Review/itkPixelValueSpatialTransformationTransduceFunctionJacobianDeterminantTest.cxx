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
#include "itkPixelValueSpatialTransformationTransduceFunctionJacobianDeterminant.h"
#include "vnl/algo/vnl_determinant.h"

using namespace itk;

/******/

int itkPixelValueSpatialTransformationTransduceFunctionJacobianDeterminantTest(int ,char *[] )
{
  typedef double  ScalarType;
  const unsigned int vectorLength = 3;
  typedef itk::Vector<ScalarType, vectorLength>  TInput;
  typedef itk::Vector<ScalarType, vectorLength>  TOutput;
  const unsigned int NInputDimensions = 2;
  const unsigned int NOutputDimensions = 2;
  typedef itk::Point<ScalarType, NOutputDimensions>  TPoint;

  typedef
  PixelValueSpatialTransformationTransduceFunctionJacobianDeterminant
                                       < TInput,
                                         TOutput,
                                         TPoint,
                                         NInputDimensions,
                                         NOutputDimensions> TransducerType;

  /* Test obects */
  typedef  itk::Matrix<ScalarType,NOutputDimensions,NOutputDimensions>
                                                      Matrix2Type;
  typedef  itk::Vector<ScalarType,NOutputDimensions>  Vector2Type;

  /* Instantiate */
  TransducerType::Pointer transducer = TransducerType::New();

  /* Check for default transform */
  typedef itk::AffineTransform<TPoint::ValueType, NOutputDimensions> AffineType;
  const AffineType *aff =
    dynamic_cast<const AffineType*>( transducer->GetTransform() );
  if( aff == NULL )
    {
    std::cout << "Failed getting default transform." << std::endl;
    return EXIT_FAILURE;
    }

  /* Add an Affine transform */
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

  transducer->SetTransform( affine );
  transducer->Prepare();

  /* Test function operator */
  ScalarType r[vectorLength] = {1,2,3};
  TInput value(r);
  TPoint point;
  point[0] = 2;
  point[1] = 2;
  /* Truth value */
  //See notes in .h for ...JacobianDeterminant::()
  AffineType::MatrixValueType det =
    vnl_determinant( affine->GetMatrix().GetVnlMatrix() );
  TInput truth = det * value;
  /* Call the transducer */
  TOutput result = (*transducer)( point, value );
  std::cout << "result: " << result << "  truth: " << truth << std::endl;
  for( unsigned int ii=0; ii<vectorLength; ii++)
    {
    if( result[ii] != truth[ii] )
      {
      std::cout << "Failed calling transducer()." << std::endl;
      return EXIT_FAILURE;
      }
    }

  /* PrintSelf */
  std::cout << "Transducer object: " << std::endl << transducer;

  return EXIT_SUCCESS;
}
