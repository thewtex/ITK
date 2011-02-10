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

#include <iostream>
#include "itkBioFormatsImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"

#if defined(ITK_USE_MODULAR_BUILD)
  #define SPECIFIC_IMAGEIO_MODULE_TEST
#endif

int itkBioFormatsImageIOTest( int argc, char * argv [] )
{
  if( argc < 3)
    {
    std::cerr << "Usage: " << argv[0] << " input output\n";
    return EXIT_FAILURE;
    }

  typedef unsigned char       PixelType;
  const unsigned int          Dimension = 2;

  typedef itk::Image< PixelType, Dimension >   ImageType;

  typedef itk::ImageFileReader<ImageType> ReaderType;

  itk::BioFormatsImageIO::Pointer io = itk::BioFormatsImageIO::New();

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName(argv[1]);
  reader->SetImageIO(io);

  itk::ImageFileWriter<ImageType>::Pointer writer;

  writer = itk::ImageFileWriter<ImageType>::New();
  writer->SetInput(reader->GetOutput());
  writer->SetFileName(argv[2]);
  writer->SetImageIO(io);

  try
    {
    writer->Update();
    }
  catch (itk::ExceptionObject &e)
    {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
