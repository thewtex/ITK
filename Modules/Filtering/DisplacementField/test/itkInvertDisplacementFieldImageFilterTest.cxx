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

#include "itkInvertDisplacementFieldImageFilter.h"
#include "itkImageRegionIterator.h"

int itkInvertDisplacementFieldImageFilterTest( int, char * [] )
{
  const unsigned int   ImageDimension = 2;

  typedef itk::Vector<float, ImageDimension>       VectorType;
  typedef itk::Image<VectorType, ImageDimension>   DisplacementFieldType;

  // Create a displacement field
  DisplacementFieldType::PointType     origin;
  DisplacementFieldType::SpacingType   spacing;
  DisplacementFieldType::SizeType      size;
  DisplacementFieldType::DirectionType direction;

  direction.SetIdentity();
  origin.Fill( 0.0 );
  spacing.Fill( 0.5 );
  size.Fill( 100 );

  VectorType ones( 1 );

  DisplacementFieldType::Pointer field = DisplacementFieldType::New();
  field->SetOrigin( origin );
  field->SetSpacing( spacing );
  field->SetRegions( size );
  field->SetDirection( direction );
  field->Allocate();
  field->FillBuffer( ones );

  unsigned int numberOfIterations = 50;
  float        maxTolerance = 0.1;
  float        meanTolerance = 0.001;

  typedef itk::InvertDisplacementFieldImageFilter<DisplacementFieldType> InverterType;
  InverterType::Pointer inverter = InverterType::New();
  inverter->SetInput( field );
  inverter->SetMaximumNumberOfIterations( numberOfIterations );
  inverter->SetMeanErrorToleranceThreshold( meanTolerance );
  inverter->SetMaxErrorToleranceThreshold( maxTolerance );

  std::cout << "number of iterations: " << inverter->GetMaximumNumberOfIterations() << std::endl;
  std::cout << "mean error tolerance: " << inverter->GetMeanErrorToleranceThreshold() << std::endl;
  std::cout << "max error tolerance: " << inverter->GetMaxErrorToleranceThreshold() << std::endl;

  try
    {
    inverter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    }

  DisplacementFieldType::IndexType index;
  index[0] = 30;
  index[1] = 30;

  VectorType v = inverter->GetOutput()->GetPixel( index );
  VectorType delta = v + ones;
  if( delta.GetNorm() > 0.05 )
    {
    std::cerr << "Failed to find proper inverse." << std::endl;
    return EXIT_FAILURE;
    }

  if( inverter->GetMeanErrorNorm() >= inverter->GetMeanErrorToleranceThreshold() &&
    inverter->GetMaxErrorNorm() >= inverter->GetMaxErrorToleranceThreshold() )
    {
    std::cerr << "Failed to converge properly." << std::endl;
    return EXIT_FAILURE;
    }

  inverter->Print( std::cout, 3 );

  return EXIT_SUCCESS;
}
