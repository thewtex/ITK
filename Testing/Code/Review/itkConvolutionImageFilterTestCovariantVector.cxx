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

#include "itkCovariantVector.h"
#include "itkConvolutionImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

int itkConvolutionImageFilterTestCovariantVector(int argc, char * argv[])
{

  if ( argc < 4 )
    {
    std::cout << "Usage: " << argv[0]
              << " inputImage kernelImage outputImage [normalizeImage]" << std::endl;
    return EXIT_FAILURE;
    }

  const int ImageDimension = 3;

  typedef long InputValueType;
  typedef long KernelPixelType;
  typedef itk::Image<itk::CovariantVector<InputValueType, ImageDimension>,
                     ImageDimension>                             ImageType;
  typedef itk::CovariantVector<KernelPixelType,
                               ImageDimension>                             ComputationPixelType;

  typedef itk::ConvolutionImageFilter<ImageType,
                                      itk::Image<KernelPixelType,
                                                 ImageDimension>,
                                      itk::CovariantVector<KernelPixelType, ImageDimension> > FilterType;
  typedef itk::Image<KernelPixelType, ImageDimension> KernelImageType;

  typedef itk::ImageFileReader<ImageType>             ReaderType;
  typedef itk::ImageFileReader<KernelImageType>       KernelReaderType;

  ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName( argv[1] );
  reader1->Update();

  KernelReaderType::Pointer reader2 = KernelReaderType::New();
  reader2->SetFileName( argv[2] );
  reader2->Update();

  typedef itk::ConvolutionImageFilter<ImageType,KernelImageType,ComputationPixelType,ImageType> ConvolutionFilterType;
  ConvolutionFilterType::Pointer convoluter
    = ConvolutionFilterType::New();
  convoluter->SetInput( reader1->GetOutput() );
  convoluter->SetImageKernelInput( reader2->GetOutput() );

  itk::SimpleFilterWatcher watcher(convoluter, "filter");

  if( argc >= 5 )
    {
    convoluter->SetNormalize( static_cast<bool>( atoi( argv[4] ) ) );
    }

  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( convoluter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
