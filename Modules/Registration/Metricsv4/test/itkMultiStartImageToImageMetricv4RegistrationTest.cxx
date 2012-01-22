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
 * Test program for itkMultiStartImageToImageMetricv4RegistrationTest and
 * GradientDescentOptimizerv4 classes.
 *
 * Perform a registration using user-supplied images.
 * No numerical verification is performed. Test passes as long
 * as no exception occurs.
 */
#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkRegistrationParameterScalesFromShift.h"
#include "itkMultiStartOptimizerv4.h"
#include "itkGaussianSmoothingOnUpdateDisplacementFieldTransform.h"

#include "itkCastImageFilter.h"

#include "itkCommand.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <iomanip>

int itkMultiStartImageToImageMetricv4RegistrationTest(int argc, char *argv[])
{

  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile movingImageFile ";
    std::cerr << " outputImageFile ";
    std::cerr << " [numberOfIterations initialAffine ] ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << argc << std::endl;
  unsigned int numberOfIterations = 10;
  if( argc >= 5 )
    {
    numberOfIterations = atoi( argv[4] );
    }
  std::cout << " iterations "<< numberOfIterations << std::endl;

  const unsigned int Dimension = 2;
  typedef double PixelType; //I assume png is unsigned short

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer fixedImageReader   = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName( argv[1] );
  movingImageReader->SetFileName( argv[2] );

  //get the images
  fixedImageReader->Update();
  FixedImageType::Pointer  fixedImage = fixedImageReader->GetOutput();
  movingImageReader->Update();
  MovingImageType::Pointer movingImage = movingImageReader->GetOutput();

  /** define a resample filter that will ultimately be used to deform the image */
  typedef itk::ResampleImageFilter<
                            MovingImageType,
                            FixedImageType >    ResampleFilterType;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();


  /** create a composite transform holder for other transforms  */
  typedef itk::CompositeTransform<double, Dimension>    CompositeType;
  typedef CompositeType::ScalarType                     ScalarType;

  CompositeType::Pointer compositeTransform = CompositeType::New();

  //create an affine transform
  typedef itk::AffineTransform<double, Dimension>
                                                    AffineTransformType;
  AffineTransformType::Pointer affineTransform = AffineTransformType::New();
  affineTransform->SetIdentity();
  std::cout <<" affineTransform params prior to optimization " << affineTransform->GetParameters() << std::endl;

  //identity transform for fixed image
  typedef itk::IdentityTransform<double, Dimension> IdentityTransformType;
  IdentityTransformType::Pointer identityTransform = IdentityTransformType::New();
  identityTransform->SetIdentity();

  // The metric
  typedef itk::MattesMutualInformationImageToImageMetricv4 < FixedImageType, MovingImageType >  MetricType;
  typedef MetricType::FixedSampledPointSetType                                                              PointSetType;
  MetricType::Pointer metric = MetricType::New();
  metric->SetNumberOfHistogramBins(20);

  if( 0 )
    {
    std::cout << "Dense sampling." << std::endl;
    metric->SetUseFixedSampledPointSet( false );
    }
  else
    {
    typedef PointSetType::PointType     PointType;
    PointSetType::Pointer               pset(PointSetType::New());
    unsigned long ind=0,ct=0;
    itk::ImageRegionIteratorWithIndex<FixedImageType> It(fixedImage, fixedImage->GetLargestPossibleRegion() );
    for( It.GoToBegin(); !It.IsAtEnd(); ++It )
      {
      // take every N^th point
      if ( ct % 20 == 0  )
        {
          PointType pt;
          fixedImage->TransformIndexToPhysicalPoint( It.GetIndex(), pt);
          pset->SetPoint(ind, pt);
          ind++;
        }
        ct++;
      }
    std::cout << "Setting point set with " << ind << " points of " << fixedImage->GetLargestPossibleRegion().GetNumberOfPixels() << " total " << std::endl;
    metric->SetFixedSampledPointSet( pset );
    metric->SetUseFixedSampledPointSet( true );
    std::cout << "Testing metric with point set..." << std::endl;
    }

  // Assign images and transforms.
  // By not setting a virtual domain image or virtual domain settings,
  // the metric will use the fixed image for the virtual domain.
//  metric->SetVirtualDomainImage( fixedImage );
  metric->SetFixedImage( fixedImage );
  metric->SetMovingImage( movingImage );
  metric->SetFixedTransform( identityTransform );
  metric->SetMovingTransform( affineTransform );
  metric->SetDoFixedImagePreWarp( false );
  metric->SetDoMovingImagePreWarp( false );
  const bool gaussian = false;
  metric->SetUseMovingImageGradientFilter( gaussian );
  metric->SetUseFixedImageGradientFilter( gaussian );
  metric->Initialize();

  typedef itk::RegistrationParameterScalesFromShift< MetricType >
    RegistrationParameterScalesFromShiftType;
  RegistrationParameterScalesFromShiftType::Pointer shiftScaleEstimator
    = RegistrationParameterScalesFromShiftType::New();
  shiftScaleEstimator->SetMetric(metric);

  std::cout << "First do an affine registration " << std::endl;
  typedef itk::GradientDescentOptimizerv4  OptimizerType;
  OptimizerType::Pointer  optimizer = OptimizerType::New();
  optimizer->SetMetric( metric );
  optimizer->SetNumberOfIterations( numberOfIterations );
  optimizer->SetScalesEstimator( shiftScaleEstimator );
  optimizer->SetMaximumStepSizeInPhysicalUnits( 0.5 );

  typedef  itk::MultiStartOptimizerv4  MOptimizerType;
  MOptimizerType::Pointer  MOptimizer = MOptimizerType::New();
  MOptimizerType::ParametersListType parametersList = MOptimizer->GetParametersList();
  unsigned int rotplus=180;
  for (  unsigned int i = 0; i <= 180; i+=rotplus )
    {
    AffineTransformType::Pointer aff = AffineTransformType::New();
    aff->SetIdentity();
    float rad=(float)i*vnl_math::pi /180.0;
    aff->Rotate2D(rad);
    parametersList.push_back( aff->GetParameters() );
    }
  MOptimizer->SetMetric( metric );
  MOptimizer->SetParametersList( parametersList );
  MOptimizer->SetLocalOptimizer(optimizer);
  MOptimizer->StartOptimization();
  std::cout <<" done " <<MOptimizer->GetBestParameters() << std::endl;
  affineTransform->SetParameters(MOptimizer->GetBestParameters());

  std::cout << "Affine registration complete. GetNumberOfSkippedFixedSampledPoints: " << metric->GetNumberOfSkippedFixedSampledPoints() << std::endl;
  MOptimizerType::MetricValuesListType  metlist=MOptimizer->GetMetricValuesList();
  for (unsigned int i=0; i < metlist.size(); i++) std::cout << " angle " << i*rotplus <<" energy " <<metlist[i] <<  std::endl;

  //warp the image with the displacement field
  resample->SetTransform( affineTransform );
  resample->SetInput( movingImageReader->GetOutput() );
  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection() );
  resample->SetDefaultPixelValue( 0 );
  resample->Update();
  std::cout << "GetNumberOfThreadsUsed: " << metric->GetNumberOfThreadsUsed() << std::endl;

  //write the warped image into a file
  typedef double                                    OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension >  OutputImageType;
  typedef itk::CastImageFilter<
                        MovingImageType,
                        OutputImageType >           CastFilterType;
  typedef itk::ImageFileWriter< OutputImageType >   WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();
  writer->SetFileName( argv[3] );
  caster->SetInput( resample->GetOutput() );
  writer->SetInput( caster->GetOutput() );
  writer->Update();

  std::cout << "After optimization affine params are: " <<  affineTransform->GetParameters() << std::endl;
  std::cout << "Test PASSED." << std::endl;
  return EXIT_SUCCESS;

}
