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

#include "vnl/vnl_math.h"

#include "itkImage.h"
#include "itkLevelSetImageBase.h"
#include "itkImageRegionIteratorWithIndex.h"

int itkLevelSetImageBaseTest( int , char* [] )
{
  const unsigned int Dimension = 2;

  typedef float PixelType;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::LevelSetImageBase< ImageType > LevelSetType;

  ImageType::IndexType index;
  index[0] = 0;
  index[1] = 0;

  ImageType::SizeType size;
  size[0] = 20;
  size[1] = 20;

  ImageType::RegionType region;
  region.SetIndex( index );
  region.SetSize( size );

  PixelType zeroValue = 0.;

  ImageType::SpacingType spacing;
  spacing[0] = 1.;
  spacing[1] = 0.5;

  ImageType::Pointer input = ImageType::New();
  input->SetRegions( region );
  input->SetSpacing( spacing );
  input->Allocate();
  input->FillBuffer( zeroValue );

  itk::ImageRegionIteratorWithIndex< ImageType > it( input,
                                              input->GetLargestPossibleRegion() );

  it.GoToBegin();

  ImageType::IndexType idx;
  ImageType::PointType pt;

  while( !it.IsAtEnd() )
    {
    idx = it.GetIndex();
    input->TransformIndexToPhysicalPoint( idx, pt );

    it.Set( vcl_sqrt(
             static_cast< float> ( ( pt[0] - 5 ) * ( pt[0] - 5 ) +
                                   ( pt[1] - 5 ) * ( pt[1] - 5 ) ) )
           - 3 );
    ++it;
    }

  LevelSetType::Pointer level_set = LevelSetType::New();
  level_set->SetImage( input );

  idx[0] = 9;
  idx[1] = 18;
  input->TransformIndexToPhysicalPoint( idx, pt );
  LevelSetType::OutputType theoreticalValue = vcl_sqrt(
           static_cast< float> ( ( pt[0] - 5 ) * ( pt[0] - 5 ) +
                                ( pt[1] - 5 ) * ( pt[1] - 5 ) ) )
          -3;

  LevelSetType::OutputType value = level_set->Evaluate( idx );

  if( vnl_math_abs( theoreticalValue - value ) > 1e-5 )
    {
    std::cout << idx << " * " << value << " != "
              << theoreticalValue << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "index = " << idx << std::endl;
  std::cout << "point = " << pt << std::endl;
  std::cout << "value = " << value << std::endl;

  LevelSetType::GradientType grad = level_set->EvaluateGradient( idx );
  std::cout << "gradient = " << grad << std::endl;


  if ( vnl_math_abs( vnl_math_abs( grad[0] ) - vnl_math_abs( grad[1] ) ) > 5e-2 )
    {
    std::cout << idx << " *GradientTestFail* " << vnl_math_abs( grad[0] ) << " != "
              << vnl_math_abs( grad[1] ) << std::endl;
    return EXIT_FAILURE;
    }

  LevelSetType::HessianType hessian = level_set->EvaluateHessian( idx );
  std::cout << "hessian = " << std::endl << hessian << std::endl;

  if ( vnl_math_abs( vnl_math_abs( hessian[0][0] ) - vnl_math_abs( hessian[0][1] ) ) > 5e-2 )
    {
    std::cout << idx << " *HessianTestFail* " << vnl_math_abs( hessian[0][0] ) << " != "
              << vnl_math_abs( hessian[0][1] ) << std::endl;
    return EXIT_FAILURE;
    }

  LevelSetType::OutputRealType laplacian = level_set->EvaluateLaplacian( idx );
  std::cout << "laplacian = " << laplacian << std::endl;

  LevelSetType::OutputRealType gradientnorm = level_set->EvaluateGradientNorm( idx );
  std::cout <<"gradient norm = " << gradientnorm << std::endl;

  if( vnl_math_abs( 1 - gradientnorm ) > 5e-2 )
    {
    std::cout << idx << " *GradientNormFail* " << gradientnorm << " != "
              << 1 << std::endl;
    return EXIT_FAILURE;
    }

  LevelSetType::OutputRealType meancurvature = level_set->EvaluateMeanCurvature( idx );
  std::cout <<"mean curvature = " << meancurvature << std::endl;

  return EXIT_SUCCESS;
}
