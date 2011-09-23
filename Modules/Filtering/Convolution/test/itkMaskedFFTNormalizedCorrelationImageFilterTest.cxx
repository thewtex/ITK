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

#include "itkImage.h"
#include "itkMaskedFFTNormalizedCorrelationImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMinimumMaximumImageCalculator.h"

int itkMaskedFFTNormalizedCorrelationImageFilterTest(int argc, char * argv[] )
{
  if( argc < 6 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " fixedImageName movingImageName fixedMaskName movingMaskName outputImageName [numberOfOverlapPixelsImageName]" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image<unsigned short, 2> ImageType;
  // We need to enforce the output to be either float or double since
  // the correlation image contains values between -1 and 1.
  typedef itk::Image<double, 2> OutputImageType;

  char * fixedImageFileName = argv[1];
  char * movingImageFileName = argv[2];
  char * fixedMaskFileName = argv[3];
  char * movingMaskFileName = argv[4];
  const char * outputImageFileName = argv[5];

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer fixedImageReader = ReaderType::New();
  fixedImageReader->SetFileName( fixedImageFileName );

  ReaderType::Pointer movingImageReader = ReaderType::New();
  movingImageReader->SetFileName( movingImageFileName );

  ReaderType::Pointer fixedMaskReader = ReaderType::New();
  fixedMaskReader->SetFileName( fixedMaskFileName );

  ReaderType::Pointer movingMaskReader = ReaderType::New();
  movingMaskReader->SetFileName( movingMaskFileName );

  typedef itk::MaskedFFTNormalizedCorrelationImageFilter< ImageType, OutputImageType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetFixedImageInput( fixedImageReader->GetOutput() );
  filter->SetMovingImageInput( movingImageReader->GetOutput() );
  filter->SetFixedMaskInput( fixedMaskReader->GetOutput() );
  filter->SetMovingMaskInput( movingMaskReader->GetOutput() );
  filter->Update();

  // Zero-out the correlation values that arise from too few pixels since they are statistically unreliable.
  // Here we threshold based on a percentage of the max number of overlap pixels since this will be different for
  // different image sizes and masked regions.
  // The numberOfOverlapPixels image tells how many pixels are overlapping at each location of the correlation image.
  typedef itk::MinimumMaximumImageCalculator<FilterType::ULongImageType> CalculatorType;
  CalculatorType::Pointer calculator = CalculatorType::New();
  calculator->SetImage( filter->GetNumberOfOverlapPixels() );
  calculator->ComputeMaximum();
  // Here we ignore pixels that have less than 5% of the maximum number of overlap pixels.
  double overlapThreshold = 0.05 * calculator->GetMaximum();

  OutputImageType::Pointer outputImage = filter->GetOutput();
  itk::ImageRegionIterator<OutputImageType> imageIt( outputImage, outputImage->GetLargestPossibleRegion() );
  itk::ImageRegionConstIterator<FilterType::ULongImageType> countIt( filter->GetNumberOfOverlapPixels(), filter->GetNumberOfOverlapPixels()->GetLargestPossibleRegion());
  for( imageIt.GoToBegin(), countIt.GoToBegin(); !imageIt.IsAtEnd(); ++imageIt, ++countIt )
  {
    if( countIt.Get() < overlapThreshold )
    {
      imageIt.Set( 0.0 );
    }
  }

  typedef itk::ImageFileWriter< OutputImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImageFileName );
  writer->SetInput( outputImage );
  writer->Update();

  if( argc > 6 )
    {
    typedef itk::ImageFileWriter< FilterType::ULongImageType > WriterType2;
    WriterType2::Pointer writer2 = WriterType2::New();
    const char * numberOfOverlapPixelsImageName = argv[6];
    writer2->SetFileName( numberOfOverlapPixelsImageName );
    writer2->SetInput( filter->GetNumberOfOverlapPixels() );
    writer2->Update();
    }

  return EXIT_SUCCESS;
}
