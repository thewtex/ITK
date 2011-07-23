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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include "itkImageRegistrationMethod.h"
#include "itkDemonsRegistrationFilter.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkImageFileReader.h"
#include "itkCommandIterationUpdate.h"
#include "itkImageFileWriter.h"
#include "itkGradientDescentOptimizer.h"
#include "itkIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkCommand.h"
#include "vnl/vnl_math.h"
#include "itkVectorCastImageFilter.h"
#include "itkAffineTransform.h"
#include "itkResampleImageFilter.h"
namespace{
// The following class is used to support callbacks
// on the filter in the pipeline that follows later

// Template function to fill in an image with a circle.
template <class TImage>
void
FillWithEllipse(
TImage * image,
double * center,
double radius, double c,
typename TImage::PixelType foregnd,
typename TImage::PixelType backgnd)
{

  typedef itk::ImageRegionIteratorWithIndex<TImage> Iterator;
  Iterator it( image, image->GetBufferedRegion() );
  it.Begin();

  typename TImage::IndexType index;
  for( ; !it.IsAtEnd(); ++it )
    {
    index = it.GetIndex();
    double distancea = sqrt( vnl_math_sqr((double) index[0] - (double) center[0] + c )+ vnl_math_sqr((double) index[1]  - center[1]) );
    double distanceb = sqrt( vnl_math_sqr((double) index[0] - (double) center[0] - c )+ vnl_math_sqr((double) index[1]  - center[1]) );
    double distance = distancea + distanceb;
    if( distance <= 2*radius ) it.Set( foregnd );
    else it.Set( backgnd );
    }

}


}

int itkAffineImageTest(int, char* [] )
{

  typedef float PixelType;
  enum {ImageDimension = 2};
  typedef itk::Image<PixelType,ImageDimension> ImageType;
  typedef itk::Vector<float,ImageDimension> VectorType;
  typedef itk::Image<VectorType,ImageDimension> FieldType;
  typedef itk::Image<VectorType::ValueType,ImageDimension> FloatImageType;
  typedef ImageType::IndexType  IndexType;
  typedef ImageType::SizeType   SizeType;
  typedef ImageType::RegionType RegionType;

  //--------------------------------------------------------
  std::cout << "Generate input images and initial deformation field";
  std::cout << std::endl;

  ImageType::SizeValueType sizeArray[ImageDimension] = { 128, 128 };
  SizeType size;
  size.SetSize( sizeArray );

  ImageType::SizeValueType sizeArray2[ImageDimension] = { 64, 64 };
  //  ImageType::SizeValueType sizeArray2[ImageDimension] = { 128, 128 };
  SizeType size2;
  size2.SetSize( sizeArray2 );

  IndexType index;
  index.Fill( 0 );

  RegionType region;
  region.SetSize( size );
  region.SetIndex( index );

  RegionType region2;
  region2.SetSize( size2 );
  region2.SetIndex( index );

  ImageType::Pointer moving = ImageType::New();
  moving->SetLargestPossibleRegion( region2 );
  moving->SetBufferedRegion( region2 );
  moving->Allocate();
  ImageType::SpacingType spacing;
  spacing.Fill(1.5);
  moving->SetSpacing(spacing);

  ImageType::Pointer fixed = ImageType::New();
  fixed->SetLargestPossibleRegion( region );
  fixed->SetBufferedRegion( region );
  fixed->Allocate();
  spacing.Fill(1);
  fixed->SetSpacing(spacing);

  double center[ImageDimension];
  double radius;
  PixelType fgnd = 1;
  PixelType bgnd = 0;

  // fill fixed with circle
  center[0] = 64; center[1] = 64; radius = 30;
  FillWithEllipse<ImageType>( fixed, center, radius, 0, fgnd, bgnd );
  center[0] = 32; center[1] = 32; radius = 15;
  FillWithEllipse<ImageType>( moving, center, radius, 0, fgnd, bgnd );

  /** define a non-identity direction matrix in the fixed image */
  ImageType::DirectionType direction=fixed->GetDirection( );
  direction[0][0]=-1;
  direction[1][0]=0.05;
  fixed->SetDirection(direction);
  double transx=30;
  ImageType::PointType ftranslation;  ftranslation.Fill(0); ftranslation[0]=transx;
  fixed->SetOrigin(ftranslation);

  ImageType::DirectionType mdirection=moving->GetDirection( );
  mdirection[0][0]=-1;
  mdirection[1][0]=0.0;
  moving->SetDirection(mdirection);
  transx=25;
  ftranslation.Fill(0); ftranslation[0]=transx;
  moving->SetOrigin(ftranslation);


  /** Now register moving to fixed */
  // Transform Type
  typedef itk::AffineTransform< double, ImageDimension > TransformType;
  typedef TransformType::ParametersType             ParametersType;

  // Optimizer Type
  typedef itk::GradientDescentOptimizer                  OptimizerType;

  // Metric Type
  typedef itk::MeanSquaresImageToImageMetric<
                                    ImageType,
                                    ImageType >    MetricType;

  // Interpolation technique
  typedef itk:: LinearInterpolateImageFunction<
                                    ImageType,
                                    double >             InterpolatorType;

  // Registration Method
  typedef itk::ImageRegistrationMethod<
                                    ImageType,
                                    ImageType >    RegistrationType;


  MetricType::Pointer         metric        = MetricType::New();
  TransformType::Pointer      rtransform     = TransformType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  TransformType::Pointer      trasform      = TransformType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     rtransform     );
  registration->SetFixedImage(    fixed    );
  registration->SetMovingImage(   moving   );
  registration->SetInterpolator(  interpolator  );
  // Select the Region of Interest over which the Metric will be computed
  // Registration time will be proportional to the number of pixels in this region.
  metric->SetFixedImageRegion( fixed->GetBufferedRegion() );
  metric->SetNumberOfFixedImageSamples(10000);
  typedef itk::CommandIterationUpdate<
                                  OptimizerType >    CommandIterationType;
  // Instantiate an Observer to report the progress of the Optimization
  CommandIterationType::Pointer iterationCommand = CommandIterationType::New();
  iterationCommand->SetOptimizer(  optimizer.GetPointer() );

  // Scale the translation components of the Transform in the Optimizer
  OptimizerType::ScalesType scales( rtransform->GetNumberOfParameters() );
  scales.Fill( 0.01 );

  unsigned long   numberOfIterations =  200;
  double          translationScale   = 1e-5;
  double          learningRate       = 1e-4;

  unsigned int dimension=ImageDimension;
  for( unsigned int i=0; i<dimension; i++)
    {
    scales[ i + dimension * dimension ] = translationScale;
    }

  optimizer->SetScales( scales );
  optimizer->SetLearningRate( learningRate );
  optimizer->SetNumberOfIterations( numberOfIterations );
  optimizer->MinimizeOn();

  // Start from an Identity transform (in a normal case, the user
  // can probably provide a better guess than the identity...
  rtransform->SetIdentity();
  registration->SetInitialTransformParameters( rtransform->GetParameters() );

  // Initialize the internal connections of the registration method.
  // This can potentially throw an exception
  try
    {
    registration->Update();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << e << std::endl;
    }

  ParametersType finalParameters  = registration->GetLastTransformParameters();
  rtransform->SetParameters(finalParameters);
  typedef itk::ResampleImageFilter<ImageType,ImageType> ResamplerType;
  ResamplerType::Pointer resampler2 = ResamplerType::New();
  resampler2->SetInput( moving );
  resampler2->SetTransform( rtransform );
  resampler2->SetSize( fixed->GetLargestPossibleRegion().GetSize() );
  resampler2->SetOutputOrigin(fixed->GetOrigin() );
  resampler2->SetOutputSpacing(fixed->GetSpacing() );
  resampler2->SetOutputDirection(fixed->GetDirection());
  resampler2->SetDefaultPixelValue( 0 );
  resampler2->Update();
  {
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  writer->SetFileName( "zmoving_resam.mhd" );
  writer->SetInput( resampler2->GetOutput() );
  writer->Update();
  }
  {
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  writer->SetFileName( "zmoving_orig.mhd" );
  writer->SetInput( moving );
  writer->Update();
  }


  /* But now, we put the affine transform back into the moving image... */
  /** Currently not correct! */

  TransformType::Pointer inv=TransformType::New();
  ImageType::PointType centerPoint = rtransform->GetCenter();
  inv->SetCenter( centerPoint );
  inv->SetIdentity();
  rtransform->GetInverse(inv);     //Now change the Origin and Direction to make data aligned.
  moving->SetOrigin( inv->GetMatrix() * moving->GetOrigin() + inv->GetTranslation() );
  moving->SetDirection( inv->GetMatrix() * moving->GetDirection() );

  /** open the images written out below in ITK-SNAP to see that they are aligned, but in different physical space*/
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  writer->SetFileName( "zfixed.mhd" );
  writer->SetInput( fixed );
  writer->Update();

  std::cout << "Compare warped moving and fixed in index space and physical space. " << std::endl;

  // compare the warp and fixed images
  itk::ImageRegionIteratorWithIndex<ImageType> fixedIter( fixed,
      fixed->GetBufferedRegion() );
  interpolator->SetInputImage( moving );

  double error_index_space=0;
  double error_physical_space=0;
  unsigned int ct=0;
  fixedIter.GoToBegin();
  while( !fixedIter.IsAtEnd() )
    {
      error_index_space+=fabs(fixedIter.Get()-moving->GetPixel(fixedIter.GetIndex()));
      ImageType::PointType point;
      fixed->TransformIndexToPhysicalPoint(fixedIter.GetIndex(), point);

      // now do physical space
      if ( interpolator->IsInsideBuffer(point) )
      {
        double value = interpolator->Evaluate(point);
        error_physical_space+=fabs(fixedIter.Get()-value);
      }
      // Check boundaries and assign
      if ( fixedIter.Get() > 0 ) ct++;
      ++fixedIter;
    }

  std::cout << "Average difference in index space : " <<error_index_space/(float)ct <<std::endl;
  std::cout << "Average difference in phys space : " <<error_physical_space/(float)ct ;
  std::cout << std::endl;
  double ground_truth=error_physical_space/(float)ct ;
  std::cout <<" write a z*** mhd image for comparison " << std::endl;
  WriterType::Pointer      writer2 =  WriterType::New();
  writer2->SetFileName( "zmoving.mhd" );
  writer2->SetInput( moving );
  writer2->Update();

  std::cout <<" write a z*** nifti image for comparison " << std::endl;
  WriterType::Pointer      writer3 =  WriterType::New();
  writer3->SetFileName( "zmoving.nii.gz" );
  writer3->SetInput( moving );
  writer3->Update();

  std::cout <<" write a z*** nrrd image for comparison " << std::endl;
  WriterType::Pointer      writer4 =  WriterType::New();
  writer4->SetFileName( "zmoving.nrrd" );
  writer4->SetInput( moving );
  writer4->Update();

  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer nrrdReader = ReaderType::New();
  nrrdReader->SetFileName("zmoving.nrrd");
  nrrdReader->Update();
  ReaderType::Pointer niftiReader = ReaderType::New();
  niftiReader->SetFileName("zmoving.nii.gz");
  niftiReader->Update();
  ReaderType::Pointer mhdReader = ReaderType::New();
  mhdReader->SetFileName("zmoving.mhd");
  mhdReader->Update();

  double merror_physical_space=0;
  double nrerror_physical_space=0;
  double nferror_physical_space=0;
  fixedIter.GoToBegin();
  while( !fixedIter.IsAtEnd() )
    {
      ImageType::PointType point;
      fixed->TransformIndexToPhysicalPoint(fixedIter.GetIndex(), point);

      interpolator->SetInputImage( nrrdReader->GetOutput() );
      if ( interpolator->IsInsideBuffer(point) )
      {
        double value = interpolator->Evaluate(point);
        nrerror_physical_space+=fabs(fixedIter.Get()-value);
      }

      interpolator->SetInputImage( niftiReader->GetOutput() );
      if ( interpolator->IsInsideBuffer(point) )
      {
        double value = interpolator->Evaluate(point);
        nferror_physical_space+=fabs(fixedIter.Get()-value);
      }

      interpolator->SetInputImage( mhdReader->GetOutput() );
      if ( interpolator->IsInsideBuffer(point) )
      {
        double value = interpolator->Evaluate(point);
        merror_physical_space+=fabs(fixedIter.Get()-value);
      }
      ++fixedIter;
    }
  bool nrrdfails=false;
  bool mhdfails=false;
  bool niftifails=false;
  if ( fabs(nrerror_physical_space/(float)ct-ground_truth) > 0.001 ) nrrdfails=true;
  if ( fabs(merror_physical_space/(float)ct-ground_truth) > 0.001 ) mhdfails=true;
  if ( fabs(nferror_physical_space/(float)ct-ground_truth) > 0.001 ) niftifails=true;
  std::cout << "Average difference in index space : " <<error_index_space/(float)ct << std::endl;
  std::cout << "Average difference in phys space mhd : " <<merror_physical_space/(float)ct  <<" fails? " <<  mhdfails <<  std::endl;
  std::cout << "Average difference in phys space nrrd : " <<nrerror_physical_space/(float)ct  << " fails? " <<  nrrdfails << std::endl;
  std::cout << "Average difference in phys space nii : " <<nferror_physical_space/(float)ct  << " fails? " <<  niftifails << std::endl;
  std::cout << std::endl;

  std::cout <<" mhd direction " <<  mhdReader->GetOutput()->GetDirection() << std::endl;
  std::cout <<" nrrd direction " <<  nrrdReader->GetOutput()->GetDirection() << std::endl;
  std::cout <<" nii direction " <<  niftiReader->GetOutput()->GetDirection() << std::endl;

  if ( error_physical_space/(float)ct > 0.05 ) return EXIT_FAILURE;
  std::cout << "Test passed" << std::endl;
  return EXIT_SUCCESS;

}
