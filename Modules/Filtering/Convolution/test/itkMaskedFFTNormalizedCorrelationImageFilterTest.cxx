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
#include "itkShiftScaleImageFilter.h"

int itkMaskedFFTNormalizedCorrelationImageFilterTest(int argc, char * argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " fixedImageName movingImageName outputImageName [fixedMaskName] [movingMaskName]" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image<unsigned short, 2> InputImageType;
  typedef itk::Image<unsigned char, 2 > OutputImageType;
  // We need to enforce the output to be either float or double since
  // the correlation image contains values between -1 and 1.
  typedef itk::Image<float, 2> RealImageType;

  char * fixedImageFileName = argv[1];
  char * movingImageFileName = argv[2];
  const char * outputImageFileName = argv[3];
  char * fixedMaskFileName;
  char * movingMaskFileName;

  typedef itk::ImageFileReader< InputImageType > ReaderType;
  ReaderType::Pointer fixedImageReader = ReaderType::New();
  fixedImageReader->SetFileName( fixedImageFileName );

  ReaderType::Pointer movingImageReader = ReaderType::New();
  movingImageReader->SetFileName( movingImageFileName );

  typedef itk::MaskedFFTNormalizedCorrelationImageFilter< InputImageType, RealImageType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetFixedImage( fixedImageReader->GetOutput() );
  filter->SetMovingImage( movingImageReader->GetOutput() );
  // Larger values zero-out pixels on a larger border around the correlation image.
  // Thus, larger values remove less stable computations but also limit the capture range.
  filter->SetRequiredNumberOfOverlappingVoxels(20);

  if( argc > 4 )
  {
    fixedMaskFileName = argv[4];
    ReaderType::Pointer fixedMaskReader = ReaderType::New();
    fixedMaskReader->SetFileName(fixedMaskFileName);
    fixedMaskReader->Update();
    filter->SetFixedImageMask(fixedMaskReader->GetOutput());
  }

  if( argc > 5 )
  {
    movingMaskFileName = argv[5];
    ReaderType::Pointer movingMaskReader = ReaderType::New();
    movingMaskReader->SetFileName(movingMaskFileName);
    movingMaskReader->Update();
    filter->SetMovingImageMask(movingMaskReader->GetOutput());
  }

  // Shift the correlation values so they can be written out as a png.
  // The original range is [-1,1], and the new range is [0,255].
  // Shift is computed before scale, so we shift by 1 and then scale by 255/2.
  typedef itk::ShiftScaleImageFilter<RealImageType,RealImageType> RescaleType;
  RescaleType::Pointer rescaler = RescaleType::New();
  rescaler->SetInput( filter->GetOutput() );
  rescaler->SetShift(1);
  rescaler->SetScale(255.0/2.0);

  // Round the output of the rescaling step.  If instead the output type of the
  // ShiftScaleImageFilter were OutputImageType, this would truncate the values
  // instead of rounding.
  typedef itk::RoundImageFilter<RealImageType,OutputImageType> RoundType;
  RoundType::Pointer rounder = RoundType::New();
  rounder->SetInput( rescaler->GetOutput() );

  typedef itk::ImageFileWriter< OutputImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImageFileName );
  writer->SetInput( rounder->GetOutput() );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    }

  return EXIT_SUCCESS;
}
