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
#include "itkArray.h"
#include "itkResampleImageFilter.h"
#include "itkTranslationTransform.h"
#include "itkComposeImageFilter.h"
#include "itkRGBPixel.h"
#include "itkSimpleFilterWatcher.h"

int itkMaskedFFTNormalizedCorrelationImageFilterTest2(int argc, char * argv[] )
{
  if( argc < 4 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " fixedImageName movingImageName outputImageName [requiredFractionOfOverlappingPixels] [fixedMaskName] [movingMaskName]" << std::endl;
    return EXIT_FAILURE;
  }

  const unsigned short ImageDimension = 2;
  typedef itk::Image< unsigned char, ImageDimension >   UCharImageType;
  typedef UCharImageType                                InputImageType;
  typedef UCharImageType                                MaskImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::ImageFileReader< MaskImageType >         MaskReaderType;

  // We need the internal type to be either float or double since
  // the correlation image contains values between -1 and 1.
  typedef itk::Image< double, ImageDimension > RealImageType;
  typedef itk::MaskedFFTNormalizedCorrelationImageFilter< InputImageType, RealImageType, MaskImageType > FilterType;

  char * fixedImageFileName = argv[1];
  char * movingImageFileName = argv[2];
  const char * outputImageFileName = argv[3];
  FilterType::SizeValueType requiredNumberOfOverlappingPixels = 0;
  FilterType::RealPixelType requiredFractionOfOverlappingPixels = 0;
  if( argc > 4 )
  {
    requiredFractionOfOverlappingPixels = atof(argv[4]);
  }

  ReaderType::Pointer fixedImageReader = ReaderType::New();
  fixedImageReader->SetFileName( fixedImageFileName );

  ReaderType::Pointer movingImageReader = ReaderType::New();
  movingImageReader->SetFileName( movingImageFileName );

  FilterType::Pointer filter = FilterType::New();
  filter->SetFixedImage( fixedImageReader->GetOutput() );
  filter->SetMovingImage( movingImageReader->GetOutput() );
  // Larger values zero-out pixels on a larger border around the correlation image.
  // Thus, larger values remove less stable computations but also limit the capture range.
  filter->SetRequiredNumberOfOverlappingPixels( requiredNumberOfOverlappingPixels );
  filter->SetRequiredFractionOfOverlappingPixels( requiredFractionOfOverlappingPixels );

  if( argc > 5 )
  {
    char * fixedMaskFileName = argv[5];
    MaskReaderType::Pointer fixedMaskReader = MaskReaderType::New();
    fixedMaskReader->SetFileName(fixedMaskFileName);
    fixedMaskReader->Update();
    filter->SetFixedImageMask(fixedMaskReader->GetOutput());
  }

  if( argc > 6 )
  {
    char * movingMaskFileName = argv[6];
    MaskReaderType::Pointer movingMaskReader = MaskReaderType::New();
    movingMaskReader->SetFileName(movingMaskFileName);
    movingMaskReader->Update();
    filter->SetMovingImageMask(movingMaskReader->GetOutput());
  }

  itk::SimpleFilterWatcher watcher(filter,"MaskedNCCFilter");

  try
  {
    filter->Update();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }

  // From the correlation map, calculate the peak location and transform the moving
  // image to align with the fixed image.
  InputImageType::SizeType movingImageSize = movingImageReader->GetOutput()->GetLargestPossibleRegion().GetSize();
  InputImageType::SpacingType fixedImageSpacing = fixedImageReader->GetOutput()->GetSpacing();

  // Initialize the peak value of the normalized cross correlation
  // image.  We can set to this -1 since all values in the NCC image
  // are between -1 and 1;
  itk::Array<RealImageType::ValueType> transformParameters;
  transformParameters.SetSize(ImageDimension);
  RealImageType::ValueType peakValue = -1;
  RealImageType::IndexType correlationMapCenter;
  float minSquaredDistToCenter = 0;
  for( unsigned int i = 0; i < ImageDimension; i++ )
  {
    // The correlation map size is usually odd.
    correlationMapCenter[i] = (filter->GetOutput()->GetLargestPossibleRegion().GetSize()[i] - 1)/2;
    minSquaredDistToCenter += correlationMapCenter[i]*correlationMapCenter[i];
  }

  float squaredDistToCenter;
  itk::ImageRegionConstIterator<RealImageType> NCCIt( filter->GetOutput(), filter->GetOutput()->GetLargestPossibleRegion() );
  for( NCCIt.GoToBegin(); !NCCIt.IsAtEnd(); ++NCCIt )
  {
    if( NCCIt.Get() < peakValue )
    {
      continue;
    }
    else if( NCCIt.Get() == peakValue )
    {
      // If there are multiple peaks, find the one that is closest to the center of the correlation map since this leads to the smallest transform.
      squaredDistToCenter = 0;
      for( unsigned int i = 0; i < ImageDimension; i++ )
      {
        squaredDistToCenter += (float)(NCCIt.GetIndex()[i]-correlationMapCenter[i]) * (float)(NCCIt.GetIndex()[i]-correlationMapCenter[i]);
      }
      if( squaredDistToCenter >= minSquaredDistToCenter)
      {
        continue;
      }
      else
      {
        minSquaredDistToCenter = squaredDistToCenter;
      }
    }

    // Subtract the offset of the moving image size.
    // This defines the translation transform as the shift between the
    // fixed and moving images with the origin as the top left pixel of
    // both images.  In other words, if the images have different sizes
    // and they are overlayed with their top left pixels, then the
    // transform is how far the moving image should be translated in
    // order for the images to be registered.
    for( int i = 0; i < ImageDimension; i++ )
    {
      transformParameters[i] = ((float)(movingImageSize[i] - 1) - (float)(NCCIt.GetIndex()[i])) * fixedImageSpacing[i];
    }
    peakValue = NCCIt.Get();
  }

  // Resample the moving image.
  typedef itk::TranslationTransform< double, ImageDimension > TransformType;
  TransformType::Pointer translationTransform = TransformType::New();
  translationTransform->SetParameters( transformParameters );

  typedef itk::ResampleImageFilter< InputImageType, UCharImageType > ResampleType;
  ResampleType::Pointer resampler = ResampleType::New();
  resampler->SetTransform( translationTransform );
  resampler->SetInput( movingImageReader->GetOutput() );
  resampler->SetSize( movingImageReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
  resampler->SetOutputOrigin(  fixedImageReader->GetOutput()->GetOrigin() );
  resampler->SetOutputSpacing( fixedImageReader->GetOutput()->GetSpacing() );
  resampler->SetOutputDirection( fixedImageReader->GetOutput()->GetDirection() );
  resampler->SetDefaultPixelValue( 0 );
  resampler->Update();

  // Create a blank image.
  typename UCharImageType::Pointer blankImage = UCharImageType::New();
  blankImage->SetRegions( fixedImageReader->GetOutput()->GetLargestPossibleRegion() );
  blankImage->Allocate();
  blankImage->FillBuffer( 0 );

  // Create an RGB image overlaying the fixed image with the transformed moving image.
  typedef itk::Image<itk::RGBPixel<unsigned char>,ImageDimension> RGBImageType;
  typedef itk::ComposeImageFilter<UCharImageType,RGBImageType>    ComposeType;
  ComposeType::Pointer composer = ComposeType::New();
  composer->SetInput1(fixedImageReader->GetOutput());
  composer->SetInput2(resampler->GetOutput());
  composer->SetInput3(blankImage);

  typedef itk::ImageFileWriter< ComposeType::OutputImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImageFileName );
  writer->SetInput( composer->GetOutput() );
  try
  {
    writer->Update();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }

  std::cout << "Maximum overlapping pixels: " << filter->GetMaximumNumberOfOverlappingPixels() << std::endl;
  std::cout << "Minimum required fraction of overlapping pixels: " << filter->GetRequiredFractionOfOverlappingPixels() << std::endl;
  std::cout << "Minimum required number of overlapping pixels: " << filter->GetRequiredNumberOfOverlappingPixels() << std::endl;
  std::cout << "Computed transform: " << translationTransform << std::endl;

  return EXIT_SUCCESS;
}
