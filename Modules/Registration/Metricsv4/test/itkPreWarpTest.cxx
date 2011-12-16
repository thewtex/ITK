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
#include "itkContinuousIndex.h"
#include <iomanip>
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageFileWriter.h"

/*
 * This test verifies the implementation of
 * itkAffineTransform::TransformCovariantVector by simulating the
 * process of image gradient calculation used in image metrics.
 * Gradients are calculated from a moving image and transformed into
 * the fixed space, and also from a "pre-warped" image that is mapped
 * into fixed space. The gradients from each method are compared and
 * tested to match within tolerance.
 */

template<unsigned int ImageDimensionality, typename TTransform>
double itkPreWarpTestRunTest( unsigned int imageSize, typename TTransform::Pointer transform , double rotation, bool verbose, std::string & outputPath )
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

  // Fill images, with a border
  itk::ImageRegionIteratorWithIndex<ImageType> it( image, region );
  it.GoToBegin();
  unsigned int imageBorder = 20;
  while( !it.IsAtEnd() )
    {
    it.Set(0);
    bool awayfromborder=true;
    for (unsigned int j=0; j<ImageDimensionality; j++)
      {
      if ( it.GetIndex()[j] < imageBorder ||
           fabs( it.GetIndex()[j] - size[j] ) < imageBorder )
        {
        awayfromborder=false;
        }
      }
    if( awayfromborder )
      {
      it.Set(1);
      }
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

  // Write out the images if requested
  if( verbose )
    {
    typedef double                                             OutputPixelType;
    typedef itk::Image< OutputPixelType, ImageDimensionality > OutputImageType;
    typedef itk::ImageFileWriter< OutputImageType >            WriterType;
    typename WriterType::Pointer      writer =  WriterType::New();
    // moving
    writer->SetFileName( outputPath + "_moving.nii.gz" );
    writer->SetInput( movingImage );
    writer->Update();
    // pre-warp
    writer->SetFileName( outputPath + "_prewarp.nii.gz" );
    writer->SetInput( preWarpImage );
    writer->Update();
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

  // An interpolator
  typedef itk::LinearInterpolateImageFunction<ImageType, double>
                                              InterpolatorType;
  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
  interpolator->SetInputImage( movingImage );

  typedef typename TTransform::InputCovariantVectorType CovariantImageGradientType;

  // Iterate over the original image, since this is the space into which
  // we're transforming the moving image.
  it.GoToBegin();
  double innerProductSum = itk::NumericTraits<double>::Zero;
  unsigned long ct = itk::NumericTraits<unsigned long>::Zero;
  //while( !it.IsAtEnd() )
  //  {
  //  index = it.GetIndex();
    // Test only points at the border
  index.Fill( imageBorder );
    if ( index[0] == imageBorder &&
         index[1] == imageBorder &&
         index[2] == imageBorder )
      {
      //Image gradient from from pre-warped image
      CovariantImageGradientType preWarpGradient =
        preWarpCalculator->EvaluateAtIndex( index );

      //Image gradient from moving image
      typename ImageType::PointType point;
      image->TransformIndexToPhysicalPoint( index, point );
      typename ImageType::PointType mappedPoint = movingTransform->TransformPoint( point );
      if( interpolator->IsInsideBuffer( mappedPoint ) )
        {
        CovariantImageGradientType movingGradient = movingCalculator->Evaluate( mappedPoint );
        CovariantImageGradientType mappedMovingGradient =
          movingTransform->TransformCovariantVector( movingGradient );

        // Calculate inner product between the gradients using the
        // two methods.
        double norm1 = itk::NumericTraits<double>::Zero;
        double norm2 = itk::NumericTraits<double>::Zero;
        for (  unsigned int i=0; i<ImageDimensionality; i++ )
          {
          norm1 += preWarpGradient[i] * preWarpGradient[i];
          norm2 += mappedMovingGradient[i] * mappedMovingGradient[i];
          }
        norm1 = vcl_sqrt( norm1 );
        norm2 = vcl_sqrt( norm2 );
        double innerProduct = itk::NumericTraits<double>::Zero;
        if ( norm1 > 0 && norm2 > 0 )
          {
          for (  unsigned int i=0; i<ImageDimensionality; i++ )
            {
            innerProduct += preWarpGradient[i] / norm1 * mappedMovingGradient[i] / norm2;
            }
          innerProduct = vcl_sqrt( vcl_fabs( innerProduct ) );
          }
        if( innerProduct > 0 )
          {
          innerProductSum += innerProduct;
          ++ct;
          }

        if( verbose )
          {
          std::cout << "rotation: " << rotation << std::endl
                    << "index: " << index << std::endl
                    << "movingGradient: " << movingGradient << std::endl
                    << "mappedMovingGradient: " << mappedMovingGradient << std::endl
                    << "preWarpGradient: " << preWarpGradient << std::endl;
          }

        // Test with with tensors. Requires 3D image.
        // TODO: compare principal directions
        if( ImageDimensionality == 3 )
          {
          // Create tensors from outer-product of image gradient with itself
          typename TTransform::OutputDiffusionTensor3DType preWarpTensor;
          typename TTransform::OutputDiffusionTensor3DType movingTensor;
          unsigned int ind = 0;
          for( unsigned int j=0; j<3; j++ )
            {
            for( unsigned int i=0; i<3; i++ )
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

          //if( print )
            {
        /*        std::cerr << "tensors: pre-warp     : " << preWarpTensor << std::endl
                      << "moving mapped to fixed: " << mappedMovingTensor << std::endl
                      << "un-mapped moving-space: " << movingTensor << std::endl;*/
            }
          }
        }
      }
  //    ++it;
  //  }

  double average = innerProductSum / ct;

  return average;
}

//////////////////////////////////////////////////////
int itkPreWarpTest(int argc, char *argv[] )
{
  typedef unsigned int    DimensionSizeType;
  DimensionSizeType imageSize = 60;
  unsigned int dimensionality = 3;
  double minimumAverage = itk::NumericTraits<double>::max();
  double rotationDegrees = static_cast<double> (3.0);
  double maxDegrees = static_cast<double> (90.0);
  double degreeStep = static_cast<double> (3.0);

  std::string outputPath("");
  if( argc >= 2 )
    {
    std::string path( argv[1] );
    outputPath = path;
    }
  std::string commandName( argv[0] );
  outputPath += commandName;
  std::cout << outputPath << std::endl;

  for (; rotationDegrees < maxDegrees; rotationDegrees += degreeStep )
  {

  std::cerr << std::setw(3);
  double average;

  if( dimensionality == 2 )
    {
    // Transform
    typedef itk::Image< double, 2 >             ImageType;
    typedef itk::AffineTransform<double,2>      TransformType;
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();
    transform->Rotate2D( itk::Math::pi  * rotationDegrees / 180 );
    ImageType::PointType center;
    center.Fill( (imageSize-1) / 2.0 );
    transform->SetCenter( center );

    average = itkPreWarpTestRunTest<2, TransformType>( imageSize, transform,rotationDegrees, false, outputPath );
    }

  if( dimensionality == 3 )
    {
    // Transform
    typedef itk::AffineTransform<double,3> TransformType;
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();
    double angleRad = itk::Math::pi * rotationDegrees / 180;
    //    transform->SetRotation( angleRad, angleRad, angleRad );
    TransformType::OutputVectorType axis1;
    axis1[0]=1;
    axis1[1]=0;
    axis1[2]=0;
    TransformType::OutputVectorType axis2;
    axis2[0]=0;
    axis2[1]=1;
    axis2[2]=0;
    transform->Rotate3D( axis1 , angleRad );
    transform->Scale( 1.2 );
    transform->Shear( 0, 1 , 0.05 );
    TransformType::ParametersType center(3);
    center.Fill( (imageSize-1) / 2.0 );
    transform->SetFixedParameters( center );
    average = itkPreWarpTestRunTest<3, TransformType>( imageSize, transform, rotationDegrees, rotationDegrees == 45.0, outputPath );
    }

  if( average < minimumAverage )
    {
    minimumAverage = average;
    }
    std::cout << average <<  ", " << rotationDegrees << std::endl;
  }

  std::cout << "minimumAverage: " << minimumAverage << std::endl;
  double threshold = static_cast<double> (0.98);
  if( minimumAverage < threshold )
    {
    std::cerr << "Minimum average of all runs is below threshold of "
              << threshold << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
