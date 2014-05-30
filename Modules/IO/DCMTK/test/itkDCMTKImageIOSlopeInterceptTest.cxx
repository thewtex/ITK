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

#include "itkImageFileReader.h"
#include "itkDCMTKImageIO.h"
#include "itkImageRegionConstIterator.h"
#include "itkMultiplyImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkStatisticsImageFilter.h"

int itkDCMTKImageIOSlopeInterceptTest(int ac, char * av[])
{
  if(ac < 3)
    {
    std::cerr << "Usage: " << av[0]
              << " <original image> <slope intercept image>"
              << std::endl;
    return EXIT_FAILURE;
    }

  typedef short                                  InputPixelType;
  typedef itk::Image< InputPixelType, 3 >        ImageType;
  typedef itk::ImageFileReader< ImageType >      ReaderType;
  typedef itk::DCMTKImageIO                      ImageIOType;

  const InputPixelType rescaleSlope(2);
  const InputPixelType rescaleIntercept(-99);

  ImageIOType::Pointer dcmImageIO = ImageIOType::New();

  ImageType::Pointer images[2];
  for(unsigned i = 0; i < 2; ++i)
    {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( av[i+1] );
    reader->SetImageIO( dcmImageIO );

    try
      {
      reader->Update();
      }
    catch (itk::ExceptionObject & e)
      {
      std::cerr << "exception in file reader " << std::endl;
      std::cerr << e << std::endl;
      return EXIT_FAILURE;
      }
    images[i] = reader->GetOutput();
    }

  //
  // the two inputs are a DICOM image without slope/intercept tags,
  // and the same image with slpe/intercept tags.  I read the first
  // image and apply the slope/intercept, and then subtract the first
  // from the second, then look for non-zero min/max/mean.  They
  // should be identical.
  typedef itk::MultiplyImageFilter<ImageType,ImageType,ImageType> MultFilterType;
  typedef itk::AddImageFilter<ImageType,ImageType,ImageType>      AddFilterType;
  typedef itk::SubtractImageFilter<ImageType,ImageType,ImageType> SubtractFilterType;
  typedef itk::StatisticsImageFilter<ImageType>                   StatsFilterType;

  MultFilterType::Pointer mult = MultFilterType::New();
  mult->SetInput1(images[1]);
  mult->SetConstant2(rescaleSlope);

  AddFilterType::Pointer add = AddFilterType::New();
  add->SetInput1(mult->GetOutput());
  add->SetConstant2(rescaleIntercept);

  SubtractFilterType::Pointer sub = SubtractFilterType::New();
  sub->SetInput1(add->GetOutput());
  sub->SetInput2(images[0]);

  StatsFilterType::Pointer stats = StatsFilterType::New();
  stats->SetInput(sub->GetOutput());
  try
    {
    stats->Update();
    }
  catch (itk::ExceptionObject & e)
    {
    std::cerr << "exception in image measurement pipeline " << std::endl;
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
    }
  if(stats->GetMinimum() != 0
     || stats->GetMaximum() != 0
     || stats->GetMean() != 0)
    {
    std::cerr << "Images don't match, minimum = "
              << stats->GetMinimum() << "  maximum "
              << stats->GetMaximum() << " mean = "
              << stats->GetMean() << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
