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
#include "itkPixelValueSpatialTransformationTransduceFunctionBase.h"

/* Test the base class, which implements identity function */

int itkPixelValueSpatialTransformationTransduceFunctionBaseTest(int ,char *[] )
{
  typedef double  ScalarType;
  typedef double  TInput;
  typedef double  TOutput;
  const unsigned int NInputDimensions = 2;
  const unsigned int NOutputDimensions = 2;
  typedef itk::Point<ScalarType, NOutputDimensions>  TPoint;

  typedef itk::PixelValueSpatialTransformationTransduceFunctionBase
          < TInput, TPoint, NInputDimensions, NOutputDimensions>
          TransducerType;

  /* Instantiate */
  TransducerType::Pointer transducer = TransducerType::New();

  /* Add an Affine transform */
  typedef itk::AffineTransform<TPoint::ValueType, NOutputDimensions> AffineType;
  AffineType::Pointer affine = AffineType::New();

  transducer->SetTransform( affine );
  TransducerType::TransformType::ConstPointer
    affineReturn = transducer->GetTransform();
  if( affineReturn.IsNull() )
    {
    std::cout << "Failed getting transform." << std::endl;
    return EXIT_FAILURE;
    }

  /* Test do-nothing identity operator */
  TInput value = 319;
  TPoint point;
  TInput result = (*transducer)( point, value );
  if( result != value )
    {
    std::cout << "Failed Transduce()." << std::endl;
    return EXIT_FAILURE;
    }

  /* PrintSelf */
  std::cout << "Transducer object: " << std::endl << transducer;

  return EXIT_SUCCESS;
}
