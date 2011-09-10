/*=I========================================================================
 * Modified by
 * Author: Raghuram Onti Srinivasan
 * email: onti@cse.ohio-state.edu
=========================================================================*/


#include "itkConfocal.h"

int main(int argc, char* argv[])
{

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  InputImageType3D::Pointer image;
  InputImageType3D::Pointer image2;

  reader->SetFileName( argv[1] );
  writer->SetFileName( argv[5] );
  reader->UpdateLargestPossibleRegion();


  MedianFilterType::Pointer median_filter = MedianFilterType::New();

  InputImageType3D::SizeType indexRadius;

  unsigned int rx,ry,rz;
  if(argc == 6)
      rx = ry = rz = atoi(argv[2]);
  else
  {
    //Usage to be changed later.
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile   outputImageFile" << std::endl;
    return EXIT_FAILURE;
  }
  indexRadius[0] = rx; // radius along x
  indexRadius[1] = ry; // radius along y
  indexRadius[2] = rz; // radius along y

  median_filter->SetRadius( indexRadius );

  InputImageType3D::ConstPointer inputImage = reader->GetOutput();
  median_filter->SetInput( reader->GetOutput() );
  std::cout<<"Median filtering done"<<std::endl;
  InternalImageType3D::Pointer internalImage = InternalImageType3D::New();
  internalImage = median_filter->GetOutput();
  median_filter->Update();

  // Median filter ends

  IntensityFilterType::Pointer intensityWindowing =
      IntensityFilterType::New();
  intensityWindowing->SetWindowMinimum( 0 );
  intensityWindowing->SetWindowMaximum( 4095 );
  intensityWindowing->SetOutputMinimum(  0.0 );
  intensityWindowing->SetOutputMaximum( 255.0 );
  intensityWindowing->SetInput( internalImage);

  GaussianFilterType::Pointer smootherX = GaussianFilterType::New();
  GaussianFilterType::Pointer smootherY = GaussianFilterType::New();
  GaussianFilterType::Pointer smootherZ = GaussianFilterType::New();
  smootherX->SetInput( intensityWindowing->GetOutput( ));
  smootherY->SetInput( smootherX->GetOutput( ));
  smootherZ->SetInput( smootherY->GetOutput() );

  const InternalImageType3D::SpacingType& inputSpacing =
      internalImage->GetSpacing();

  const double isoSpacing = vcl_sqrt( inputSpacing[0] * inputSpacing[0] );
  smootherX->SetSigma( isoSpacing );
  smootherY->SetSigma( isoSpacing );
  smootherZ->SetSigma( isoSpacing );

  smootherX->SetDirection( 0 );
  smootherY->SetDirection( 1 );
  smootherZ->SetDirection( 2 );

  smootherX->SetNormalizeAcrossScale( true );
  smootherY->SetNormalizeAcrossScale( true );
  smootherZ->SetNormalizeAcrossScale( true );

  ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();
  resampler->SetTransform( transform );

  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  resampler->SetInterpolator( interpolator );
  resampler->SetDefaultPixelValue( 0 );

  InternalImageType3D::SpacingType spacing;
  spacing[0] = isoSpacing;
  spacing[1] = isoSpacing;
  spacing[2] = isoSpacing;
  resampler->SetOutputSpacing( spacing );

  resampler->SetOutputOrigin( internalImage->GetOrigin( ));
  resampler->SetOutputDirection( internalImage->GetDirection( ));

  InternalImageType3D::SizeType inputSize =
      internalImage->GetLargestPossibleRegion().GetSize();
  const double dx = inputSize[0] * inputSpacing[0] / isoSpacing;
  const double dy = inputSize[1] * inputSpacing[1] / isoSpacing;
  const double dz = (inputSize[2] - 1 ) * inputSpacing[2] / isoSpacing;

  InternalImageType3D::SizeType   size;
  size[0] = static_cast<SizeValueType>( dx );
  size[1] = static_cast<SizeValueType>( dy );
  size[2] = static_cast<SizeValueType>( dz );
  resampler->SetSize( size );

  resampler->SetInput( smootherZ->GetOutput( ));
  RescaleType::Pointer rescale = RescaleType::New();
  resampler->Update();
  rescale->SetInput(resampler->GetOutput());
  rescale->SetOutputMinimum(0);
  rescale->SetOutputMaximum(4095);
  //internalImage = rescale->GetOutput();

  //Otsu starts here.

  OtsuFilterType::Pointer otsu_filter = OtsuFilterType::New();
  const OutputPixelType outsideValue = atoi( argv[3] );
  const OutputPixelType insideValue  = atoi( argv[4] );
  std::cout<<"Otsu filter"<<std::endl<<
      "Outside value: "<<argv[3]<<" "<<"Inside value: "
      <<argv[4]<<std::endl;
  otsu_filter->SetOutsideValue( outsideValue );
  otsu_filter->SetInsideValue(  insideValue  );
  otsu_filter->SetNumberOfHistogramBins( 128 );
  otsu_filter->SetInput( rescale->GetOutput());
  //otsu_filter->SetInput( internalImage );

  InternalPixelType threshold_low, threshold_hi;
  threshold_low = 1;
  threshold_hi = 255;
  ThresholdFilterType::Pointer threshold =
      ThresholdFilterType::New();

  threshold->SetInput (otsu_filter->GetOutput());
  threshold->SetInsideValue(itk::NumericTraits
          <InternalPixelType>::One);
  threshold->SetOutsideValue(itk::NumericTraits
          <InternalPixelType>::Zero);
  threshold->SetLowerThreshold(threshold_low);
  threshold->SetUpperThreshold(threshold_hi);

  // Label the components in the image and relabel them so that object
  // numbers increase as the size of the objects decrease.
  ConnectedComponentType::Pointer connected =
      ConnectedComponentType::New();
  RelabelComponentType::Pointer relabel =
      RelabelComponentType::New();
  StatisticsFilterType::Pointer statistics =
      StatisticsFilterType::New();
  connected->SetInput (threshold->GetOutput());
  relabel->SetInput( connected->GetOutput() );
  //relabel->SetNumberOfObjectsToPrint( 5 );
  relabel->Update();
  std::cout << "NumberOfObjects: " <<
      relabel->GetNumberOfObjects() <<
      " OriginalNumberOfObjects: " <<
    relabel->GetOriginalNumberOfObjects() <<
    " MinimumObjectSize: " <<
    relabel->GetMinimumObjectSize() << std::endl;

  writer->SetInput( relabel->GetOutput() );
  //writer->SetInput( median_filter->GetOutput() );
  //writer->SetInput( internalImage );
  //writer->SetInput( rescale->GetOutput());
  writer->SetUseCompression(true);
  writer->Update();
  std::cout << "Finished execution"<<std::endl;

  return EXIT_SUCCESS;
}
