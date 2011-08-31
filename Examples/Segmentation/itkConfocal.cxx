/*=I========================================================================
 * Modified by
 * Author: Raghuram Onti Srinivasan
 * email: onti@cse.ohio-state.edu
=========================================================================*/
//  Software Guide : BeginLatex
//
//  The header file corresponding to this filter should be included first.
//
//  \index{itk::MedianImageFilter!header}
//
//  Software Guide : EndLatex


// Software Guide : BeginCodeSnippet
#include "itkConfocal.h"
// Software Guide : EndCodeSnippet
//
//

int main(int argc, char* argv[])
{
  //if( argc < 3 )
    //{
    //std::cerr << "Usage: " << std::endl;
    //std::cerr << argv[0] << "  inputImageFile   outputImageFile int_radius" << std::endl;
    //return EXIT_FAILURE;
    //}


  //  Software Guide : BeginLatex
  //
  //  Then the pixel and image types of the input and output must be defined.
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  InputImageType3D::Pointer image;
  InputImageType3D::Pointer image2;

  reader->SetFileName( argv[1] );
  writer->SetFileName( argv[7] );
  reader->UpdateLargestPossibleRegion();

  //  Software Guide : BeginLatex
  //
  //  Using the image types, it is now possible to define the filter type
  //  and create the filter object.
  //
  //  \index{itk::MedianImageFilter!instantiation}
  //  \index{itk::MedianImageFilter!New()}
  //  \index{itk::MedianImageFilter!Pointer}
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet

  MedianFilterType::Pointer median_filter = MedianFilterType::New();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The size of the neighborhood is defined along every dimension by
  //  passing a \code{SizeType} object with the corresponding values. The
  //  value on each dimension is used as the semi-size of a rectangular
  //  box. For example, in $2D$ a size of \(1,2\) will result in a $3 \times
  //  5$ neighborhood.
  //
  //  \index{itk::MedianImageFilter!Radius}
  //  \index{itk::MedianImageFilter!Neighborhood}
  //
  //  Software Guide : EndLatex


  // Software Guide : BeginCodeSnippet
  InputImageType3D::SizeType indexRadius;

  unsigned int rx,ry,rz;
  if(argc == 8)
      rx = ry = rz = atoi(argv[2]);
  else if (argc == 10)
  {
      rx = atoi(argv[2]);
      ry = atoi(argv[3]);
      rz = atoi(argv[4]);
  }
  else
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile   outputImageFile" << std::endl;
    return EXIT_FAILURE;
  }
  indexRadius[0] = rx; // radius along x
  indexRadius[1] = ry; // radius along y
  indexRadius[2] = rz; // radius along y

  median_filter->SetRadius( indexRadius );
  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  //
  //  The input to the filter can be taken from any other filter, for example
  //  a reader. The output can be passed down the pipeline to other filters,
  //  for example, a writer. An update call on any downstream filter will
  //  trigger the execution of the median filter.
  //
  //  \index{itk::MedianImageFilter!SetInput()}
  //  \index{itk::MedianImageFilter!GetOutput()}
  //
  //  Software Guide : EndLatex


  // Software Guide : BeginCodeSnippet
  InputImageType3D::ConstPointer inputImage = reader->GetOutput();
  reader->Update();
  median_filter->SetInput( inputImage );
  cout<<"finished median";
 // echo "finished median" > "../data/test.txt";
//  fflush(stdout);

  IntensityFilterType::Pointer intensityWindowing = IntensityFilterType::New();
  intensityWindowing->SetWindowMinimum( atoi( argv[3] ) );
  intensityWindowing->SetWindowMaximum( atoi( argv[4] ) );
  intensityWindowing->SetOutputMinimum(  0.0 );
  intensityWindowing->SetOutputMaximum( 255.0 ); // floats but in the range of char
  //intensityWindowing->SetInput( reader->GetOutput( ));
  intensityWindowing->SetInput( median_filter->GetOutput( ));

  GaussianFilterType::Pointer smootherX = GaussianFilterType::New();
  GaussianFilterType::Pointer smootherY = GaussianFilterType::New();
  GaussianFilterType::Pointer smootherZ = GaussianFilterType::New();
  smootherX->SetInput( intensityWindowing->GetOutput( ));
  smootherY->SetInput( smootherX->GetOutput( ));
  smootherZ->SetInput( smootherY->GetOutput() );
//  update(inputImage);

  const InputImageType3D::SpacingType& inputSpacing = inputImage->GetSpacing();

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
  resampler->SetDefaultPixelValue( 0 ); // highlight regions without source

  OutputImageType3D::SpacingType spacing;
  spacing[0] = isoSpacing;
  spacing[1] = isoSpacing;
  spacing[2] = isoSpacing;
  resampler->SetOutputSpacing( spacing );

  resampler->SetOutputOrigin( inputImage->GetOrigin( ));
  resampler->SetOutputDirection( inputImage->GetDirection( ));

  InputImageType3D::SizeType   inputSize = inputImage->GetLargestPossibleRegion().GetSize();
  const double dx = inputSize[0] * inputSpacing[0] / isoSpacing;
  const double dy = inputSize[1] * inputSpacing[1] / isoSpacing;
  const double dz = (inputSize[2] - 1 ) * inputSpacing[2] / isoSpacing;

  InputImageType3D::SizeType   size;
  size[0] = static_cast<SizeValueType>( dx );
  size[1] = static_cast<SizeValueType>( dy );
  size[2] = static_cast<SizeValueType>( dz );
  resampler->SetSize( size );

  resampler->SetInput( smootherZ->GetOutput( ));
  RescaleType::Pointer rescale = RescaleType::New();
  rescale->SetInput(resampler->GetOutput());
  rescale->SetOutputMinimum(0);
  rescale->SetOutputMaximum(4095);
  //resampler->Update();
//cout << "works till here";

  OtsuFilterType::Pointer otsu_filter = OtsuFilterType::New();
  const OutputPixelType outsideValue = atoi( argv[5] );
  const OutputPixelType insideValue  = atoi( argv[6] );
  cout<<outsideValue<<" "<<insideValue<<endl;
  otsu_filter->SetOutsideValue( outsideValue );
  otsu_filter->SetInsideValue(  insideValue  );
  otsu_filter->SetNumberOfHistogramBins( 128 );
  //image = median_filter->GetOutput();
  //otsu_filter->SetInput( image );
  otsu_filter->SetInput( rescale->GetOutput());
  otsu_filter->Update();
  cout << "works till here";

  InternalPixelType threshold_low, threshold_hi;
  threshold_low = 0;
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
  threshold->Update();

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

  // pull out the largest object
  //fflush(stdout);
  //image2 = otsu_filter->GetOutput();
  //writer->SetInput( image2 );
  writer->SetInput( relabel->GetOutput() );
  writer->SetUseCompression(true);
  //cout << "exec here";
  writer->Update();
  cout << "exec here";
  // Software Guide : EndCodeSnippet
  //


  //  Software Guide : BeginLatex
  //
  // \begin{figure}
  // \center
  // \includegraphics[width=0.44\textwidth]{BrainProtonDensitySlice.eps}
  // \includegraphics[wid=0.44\textwidth]{MedianImageFilterOutput.eps}
  // \itkcaption[Effect of the Median filter.]{Effect of the MedianImageFilter on a
  // slice from a MRI proton density brain image.}
  // \label{fig:MedianImageFilterOutput}
  // \end{figure}
  //
  //  Figure \ref{fig:MedianImageFilterOutput} illustrates the effect of the MedianImageFilter
  //  filter on a slice of MRI brain image using a neighborhood radius of
  //  \(1,1\), which corresponds to a $ 3 \times 3 $ classical neighborhood.
  //  The filtered image demonstrates the moderate tendency of the median
  //  filter to preserve edges.
  //
  //  Software Guide : EndLatex


  return EXIT_SUCCESS;
}
