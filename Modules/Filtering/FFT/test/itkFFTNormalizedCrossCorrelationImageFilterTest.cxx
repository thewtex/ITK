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
#include "itkFFTNormalizedCrossCorrelationImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageFileWriter.h"

int itkFFTNormalizedCrossCorrelationImageFilterTest(int argc, char * argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " fixedImageName movingImageName outputImageName" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image<unsigned short, 2>     FixedImageType;
  typedef itk::Image<unsigned short, 2 >    MovingImageType;
  // We need to enforce the output to be either float or double since
  // the correlation image contains values between -1 and 1.
  typedef itk::Image<float, 2> OutputImageType;

  char * fixedImageFileName = argv[1];
  char * movingImageFileName = argv[2];
  const char * outputImageFileName = argv[3];

  typedef itk::ImageFileReader< FixedImageType > FixedReaderType;
  FixedReaderType::Pointer fixedImageReader = FixedReaderType::New();
  fixedImageReader->SetFileName( fixedImageFileName );

  typedef itk::ImageFileReader< MovingImageType > MovingReaderType;
  MovingReaderType::Pointer movingImageReader = MovingReaderType::New();
  movingImageReader->SetFileName( movingImageFileName );

  typedef itk::FFTNormalizedCrossCorrelationImageFilter< FixedImageType, OutputImageType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetFixedImageInput( fixedImageReader->GetOutput() );
  filter->SetMovingImageInput( movingImageReader->GetOutput() );
  //filter->SetVerbose( true );

  typedef itk::ImageFileWriter< OutputImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImageFileName );
  writer->SetInput( filter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected: "  << e.GetDescription();
    return -1;
    }

  return EXIT_SUCCESS;
}
