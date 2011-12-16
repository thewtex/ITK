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

/**
 *
 */
#include "itkJointHistogramMutualInformationImageToImageMetricv4.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkRegistrationParameterScalesFromShift.h"
#include "itkCenteredRigid2DTransform.h"

#include "itkCastImageFilter.h"

#include "itkCommand.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkNumericTraits.h"

#include <iomanip>

int itkPreWarpRegistrationTest(int argc, char *argv[])
{

  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile ";
    std::cerr << " outputImageFile ";
    std::cerr << " [movingImageRotationDegrees = 20] ";
    std::cerr << " [numberOfIterations = 10]";
    std::cerr << " [doPreWarp = 0] [useGaussianFilterForImageGradients = 1] ";
    std::cerr << " [useNewTxfCovVec = 1] ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << argc << std::endl;

  double movingImageRotationDegrees = 20;
  unsigned int numberOfIterations = 10;
  bool         doPreWarp = false;
  bool         useGaussianFilterForImageGradients = true;
  bool         useNewTxfCovVec = true;
  if( argc >=4 )
    {
    movingImageRotationDegrees = atof( argv[3] );
    }
  if( argc >= 5 )
    {
    numberOfIterations = atoi( argv[4] );
    }
  if( argc >= 6 )
    {
    doPreWarp = atoi( argv[5] );
    }
  if( argc >= 7 )
    {
    useGaussianFilterForImageGradients = atoi( argv[6] );
    }
  if( argc >= 8 )
    {
    useNewTxfCovVec = atoi( argv[7] );
    }

  std::cout
    << "movingImageRotationDegrees " << movingImageRotationDegrees << std::endl
    << "iterations "<< numberOfIterations << std::endl
    << "doPreWarp " << doPreWarp << std::endl
    << "useGaussianFilterForImageGradients " << useGaussianFilterForImageGradients << std::endl
    << "useNewTxfCovVec " <<  useNewTxfCovVec << std::endl
    << std::endl;


  const unsigned int Dimension = 2;
  typedef double PixelType; //I assume png is unsigned short

  typedef itk::Image< PixelType, Dimension >  ImageType;

  typedef itk::ImageFileReader< ImageType  > FixedImageReaderType;

  FixedImageReaderType::Pointer fixedImageReader = FixedImageReaderType::New();

  fixedImageReader->SetFileName( argv[1] );

  //get the images
  fixedImageReader->Update();
  ImageType::Pointer  fixedImage = fixedImageReader->GetOutput();

  // create a rotation transform
  typedef itk::CenteredRigid2DTransform<double> MovingTransformType;
  MovingTransformType::Pointer rotationTransform = MovingTransformType::New();
  rotationTransform->SetIdentity();
  rotationTransform->SetAngle( itk::Math::pi  * movingImageRotationDegrees / 180 );
  std::cout << "GetAngle: " << rotationTransform->GetAngle() << std::endl;
  ImageType::PointType center;
  ImageType::SizeType size = fixedImage->GetLargestPossibleRegion().GetSize();
  center[0] = (size[0] - 1) / 2.0;
  center[1] = (size[1] - 1) / 2.0;
  rotationTransform->SetCenter( center );

  // create a moving image by applying the rotation transform
  typedef itk::ResampleImageFilter< ImageType, ImageType > MovingResampleFilterType;
  MovingResampleFilterType::Pointer movingResample = MovingResampleFilterType::New();
  movingResample->SetTransform( rotationTransform );
  movingResample->SetInput( fixedImage );
  movingResample->SetOutputParametersFromImage( fixedImage );
  movingResample->SetDefaultPixelValue( 0 );
  movingResample->Update();
  ImageType::Pointer movingImage = movingResample->GetOutput();

  // identity transform for fixed image
  typedef itk::IdentityTransform<double, Dimension> IdentityTransformType;
  IdentityTransformType::Pointer identityTransform = IdentityTransformType::New();
  identityTransform->SetIdentity();

  // moving transform
  MovingTransformType::Pointer movingTransform = MovingTransformType::New();
  movingTransform->SetIdentity();
  // set it's center to the same as the one used for creating the moving image
  // to simplify the registration
  rotationTransform->SetCenter( center );
  // hacked-in member to test new implemetnation of TransformCovariantVector
  movingTransform->m_UseNewTxfCovVec = useNewTxfCovVec;

  // The metric
  typedef itk::JointHistogramMutualInformationImageToImageMetricv4 < ImageType, ImageType >  MetricType;
  MetricType::Pointer metric = MetricType::New();
  metric->SetNumberOfHistogramBins(20);

  // Assign images and transforms.
  // By not setting a virtual domain image or virtual domain settings,
  // the metric will use the fixed image for the virtual domain.
  metric->SetFixedImage( fixedImage );
  metric->SetMovingImage( movingImage );
  metric->SetFixedTransform( identityTransform );
  metric->SetMovingTransform( movingTransform );
  metric->SetDoFixedImagePreWarp( doPreWarp );
  metric->SetDoMovingImagePreWarp( doPreWarp );
  metric->SetUseMovingImageGradientFilter( useGaussianFilterForImageGradients );
  metric->SetUseFixedImageGradientFilter( useGaussianFilterForImageGradients );
  metric->Initialize();

  typedef itk::RegistrationParameterScalesFromShift< MetricType >
    RegistrationParameterScalesFromShiftType;
  RegistrationParameterScalesFromShiftType::Pointer shiftScaleEstimator
    = RegistrationParameterScalesFromShiftType::New();
  shiftScaleEstimator->SetMetric(metric);

  // perform registration
  typedef itk::GradientDescentOptimizerv4  OptimizerType;
  OptimizerType::Pointer  optimizer = OptimizerType::New();
  optimizer->SetMetric( metric );
  optimizer->SetNumberOfIterations( numberOfIterations );
  if( 1 )
    {
    optimizer->SetScalesEstimator( shiftScaleEstimator );
    std::cout << "Using ScalesEstimator. " << std::endl;
    }
  else
    {
    //NOTE this doesn't work well at all, neither with scales 3 & 4
    //set to 1, nor to max. Seems to step estimation is important.
    std::cout << "Using manual scales values. " << std::endl;
    OptimizerType::ScalesType scales(5);
    typedef OptimizerType::ScalesType::ValueType ScalesValueType;
    scales[0] = 130049.0;
    scales[1] = 1.0;
    scales[2] = 1.0;
    scales[3] = 1.0;
    scales[4] = 1.0;
    //scales[3] = itk::NumericTraits<ScalesValueType>::max();
    //scales[4] = itk::NumericTraits<ScalesValueType>::max();
    optimizer->SetScales( scales );
    }
  optimizer->StartOptimization();

  std::cout << "scales: " << optimizer->GetScales() << std::endl;

  //warp the image with the moving transform
  typedef itk::ResampleImageFilter< ImageType, ImageType >    ResampleFilterType;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();
  resample->SetTransform( movingTransform );
  resample->SetInput( movingImage );
  resample->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection() );
  resample->SetDefaultPixelValue( 0 );
  resample->Update();

  //write the warped image into a file
  typedef double                                    OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension >  OutputImageType;
  typedef itk::CastImageFilter<
                        ImageType,
                        OutputImageType >           CastFilterType;
  typedef itk::ImageFileWriter< OutputImageType >   WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();
  writer->SetFileName( argv[2] );
  caster->SetInput( resample->GetOutput() );
  writer->SetInput( caster->GetOutput() );
  writer->Update();

  std::cout << "After optimization, moving txf params are: " <<  movingTransform->GetParameters() << std::endl;
  std::cout << "Test PASSED." << std::endl;
  return EXIT_SUCCESS;

}
