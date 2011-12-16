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
#include "itkImage.h"
#include "vnl/vnl_math.h"
#include "itkIntTypes.h"
#include "itkTestingMacros.h"
#include "itkCentralDifferenceImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkAffineTransform.h"
#include "itkCenteredRigid2DTransform.h"
#include "itkContinuousIndex.h"
#include <iomanip>
#include "itkLinearInterpolateImageFunction.h"
#include "itkEuler3DTransform.h"

template <typename TVector>
bool PreWarpTestTestArray( const TVector & v1, const TVector & v2 )
{
  bool pass=true;
  for ( unsigned int i = 0; i < v1.Size(); i++ )
    {
    const double epsilon = 1e-10;
    if( vcl_fabs( v1[i] - v2[i] ) > epsilon )
      pass=false;
    }
  return pass;
}

////////////////////////////////////////////////////////////
template<unsigned int ImageDimensionality, typename TTransform>
int itkPreWarpTestRunTest( unsigned int imageSize, double doPixelValueSquares, typename TTransform::Pointer transform )
{
  typedef itk::Image< double, ImageDimensionality > ImageType;

  typename ImageType::SizeType size;
  size.Fill( imageSize );
  typename ImageType::IndexType      index;
  index.Fill( 0 );
  typename ImageType::RegionType     region;
  region.SetSize( size );
  region.SetIndex( index );
  typename ImageType::SpacingType    spacing;
  spacing.Fill(1.0);
  typename ImageType::PointType      origin;
  origin.Fill( 0.0 );
  typename ImageType::DirectionType  direction;
  direction.SetIdentity();

  // Create simple test images.
  typename ImageType::Pointer image = ImageType::New();
  image->SetRegions( region );
  image->SetSpacing( spacing );
  image->SetOrigin( origin );
  image->SetDirection( direction );
  image->Allocate();

  // Fill images
  itk::ImageRegionIteratorWithIndex<ImageType> it( image, region );
  it.GoToBegin();
  int count = 1;
  while( !it.IsAtEnd() )
    {
    if( doPixelValueSquares )
      {
      it.Set( count * count );
      }
    else
      {
      it.Set( count );
      }
    count++;
    ++it;
    }

  // Create a "moving" image
  typedef itk::ResampleImageFilter< ImageType, ImageType > ResampleFilterType;
  typename ResampleFilterType::Pointer resample = ResampleFilterType::New();
  resample->SetTransform( transform );
  resample->SetInput( image );
  resample->SetOutputParametersFromImage( image );
  resample->SetDefaultPixelValue( 0 );
  resample->Update();
  typename ImageType::Pointer movingImage = resample->GetOutput();

  std::cout << "movingImage:\n";
  itk::ImageRegionIteratorWithIndex<ImageType> itm( movingImage, region );
  itm.GoToBegin();
  count = 0;
  while( !itm.IsAtEnd() )
    {
    std::cout << itm.Get() << " ";
    count++;
    if( count % imageSize == 0 )
      std::cout << std::endl;
    ++itm;
    }

  // The inverse of the transform is what we'd be estimating
  // as the moving transform during registration
  typename TTransform::InverseTransformBasePointer
      movingTransform = transform->GetInverseTransform();

  // "Pre-warp" the moving image using the 'moving' transform.
  // Will be the same as original image, except for border effects
  // or rounding errors.
  typename ResampleFilterType::Pointer resample2 = ResampleFilterType::New();
  resample2->SetTransform( movingTransform );
  resample2->SetInput( movingImage );
  resample2->SetOutputParametersFromImage( movingImage );
  resample2->SetDefaultPixelValue( 0 );
  resample2->Update();
  typename ImageType::Pointer preWarpImage = resample2->GetOutput();

  // print out pre-warp image
  std::cout << "preWarpImage:\n";
  itk::ImageRegionIteratorWithIndex<ImageType> itp( preWarpImage, region );
  itp.GoToBegin();
  count = 0;
  while( !itp.IsAtEnd() )
    {
    std::cout << itp.Get() << " ";
    count++;
    if( count % imageSize == 0 )
      std::cout << std::endl;
    ++itp;
    }


  // Gradient Calculators
  typedef itk::CentralDifferenceImageFunction<ImageType, double>
                                              CentralDifferenceCalculatorType;
  typename CentralDifferenceCalculatorType::Pointer
    movingCalculator = CentralDifferenceCalculatorType::New();
  movingCalculator->UseImageDirectionOn();
  movingCalculator->SetInputImage( movingImage );

  typename CentralDifferenceCalculatorType::Pointer
                  preWarpCalculator = CentralDifferenceCalculatorType::New();
  preWarpCalculator->UseImageDirectionOn();
  preWarpCalculator->SetInputImage( preWarpImage );

  // An interpolator for verification
  typedef itk::LinearInterpolateImageFunction<ImageType, double>
                                              InterpolatorType;
  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
  interpolator->SetInputImage( movingImage );

  typedef typename TTransform::InputCovariantVectorType CovariantImageGradientType;

  // Iterate over the original image, since this is the space into which
  // we're transforming the moving image.
  it.GoToBegin();
  count = 1;
  while( !it.IsAtEnd() )
    {
    index = it.GetIndex();

    //From pre-warped image
    CovariantImageGradientType preWarpGradient =
      preWarpCalculator->EvaluateAtIndex( index );

    //From moving image
    typename ImageType::PointType point;
    image->TransformIndexToPhysicalPoint( index, point );
    typename ImageType::PointType mappedPoint = movingTransform->TransformPoint( point );
    typename InterpolatorType::OutputType mappedPixel = interpolator->Evaluate( mappedPoint );
    CovariantImageGradientType movingGradient = movingCalculator->Evaluate( mappedPoint );
    CovariantImageGradientType mappedMovingGradient =
      movingTransform->TransformCovariantVector( movingGradient );

    //Transform as regular vector to check if different
    typename TTransform::OutputVectorType nonCovariantMovingGradient;
    for(unsigned int i = 0; i < ImageDimensionality; i++ )
      {
      nonCovariantMovingGradient[i] = movingGradient[i];
      }
    typename TTransform::OutputVectorType nonCovariantMappedMovingGradient =
      movingTransform->TransformVector( nonCovariantMovingGradient );

    // Print out results only from center of image
    bool print = true;
    for( unsigned int i=0; i<ImageDimensionality; i++ )
      {
      if( vcl_fabs( index[i]-( (imageSize-1) / 2 ) ) > 1 )
        {
        print = false;
        }
      }
    if( print )
      {
      std::cout << "**" << std::endl
                << "ind: " << index
                << " pnt: " << point
                << " mapPnt: " << mappedPoint
                << " pwPx: " << preWarpImage->GetPixel(index)
                << " mapPx: " << mappedPixel
                << " pwGrd: " << preWarpGradient
                << " mapMovGrd: " << mappedMovingGradient
                //<< " novCovMapGrd: " << nonCovariantMappedMovingGradient
                << std::endl;
      }
    ++it;
    // Test with with tensors. Requires 3D image.
    if( ImageDimensionality == 3 )
      {
      // Create tensors from outer-product of image gradient with itself
      typename TTransform::OutputDiffusionTensor3DType preWarpTensor;
      typename TTransform::OutputDiffusionTensor3DType movingTensor;
      unsigned int ind = 0;
      for( unsigned int i=0; i<3; i++ )
        {
        for( unsigned int j=0; j<3; j++ )
          {
          //Symmetric matrix - upper-triangualar
          if( (j == 1 && i == 0) || (j == 2 && i < 2) )
            {
            continue;
            }
          //Tensor from the gradient in fixed space, from pre-warped image
          preWarpTensor[ind] = preWarpGradient[j] * preWarpGradient[i];
          //Tensor from the gradient in moving space
          movingTensor[ind] = movingGradient[j] * movingGradient[i];
          ind++;
          }
        }

      //Transform the moving-space tensor into fixed space
      typename TTransform::OutputDiffusionTensor3DType mappedMovingTensor;
      mappedMovingTensor = movingTransform->TransformDiffusionTensor3D( movingTensor);

      if( print )
        {
        std::cout << "tensors: pre-warp     : " << preWarpTensor << std::endl
                  << "moving mapped to fixed: " << mappedMovingTensor << std::endl
                  << "un-mapped moving-space: " << movingTensor << std::endl;
        }
      }
    }

  return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////
int itkPreWarpTest(int argc, char *argv[] )
{
  typedef unsigned int    DimensionSizeType;
  DimensionSizeType imageSize = 7;
  double rotationDegrees = 90;
  double doPixelValueSquares = 1;
  unsigned int dimensionality = 3;

  if( argc >= 2 )
    {
    imageSize = atoi( argv[1] );
    }
  if( argc >= 3 )
    {
    dimensionality = atoi( argv[2] );
    }
  if( argc >= 4 )
    {
    rotationDegrees = atof( argv[3] );
    }
  if( argc >= 5 )
    {
    doPixelValueSquares = atof( argv[4] );
    }

  std::cout << std::setw(3);
  //std::cout << std::setprecision(2);
  std::cout << "dimensionality: " << dimensionality << std::endl;
  std::cout << "imageSize: " << imageSize << std::endl;
  std::cout << "rotation: " << rotationDegrees << std::endl;

  if( dimensionality == 2 )
    {
    // Transform
    typedef itk::Image< double, 2 >               ImageType;
    typedef itk::CenteredRigid2DTransform<double> TransformType;
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();
    transform->SetAngle( itk::Math::pi  * rotationDegrees / 180 );
    std::cout << "GetAngle: " << transform->GetAngle() << std::endl;
    ImageType::PointType center;
    center.Fill( (imageSize-1) / 2.0 );
    transform->SetCenter( center );

    return itkPreWarpTestRunTest<2, TransformType>( imageSize, doPixelValueSquares, transform );
    }

  if( dimensionality == 3 )
    {
    // Transform
    typedef itk::Euler3DTransform<double> TransformType;
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();
    double angleRad = itk::Math::pi * rotationDegrees / 180;
    transform->SetRotation( angleRad, angleRad, angleRad );
    TransformType::ParametersType center(3);
    center.Fill( (imageSize-1) / 2.0 );
    transform->SetFixedParameters( center );
    std::cerr << "3D: calling RunTest..." << std::endl;
    return itkPreWarpTestRunTest<3, TransformType>( imageSize, doPixelValueSquares, transform );
    }

  std::cerr << "Unsupported image dimensionality: " << dimensionality << std::endl;
  return EXIT_FAILURE;
}
