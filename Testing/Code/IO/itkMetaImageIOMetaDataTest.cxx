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
#include <iostream>
#include <itkImage.h>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaDataDictionary.h"
#include "itkRandomImageSource.h"
#include "itkNumericTraits.h"
#include "itkMetaDataObject.h"

/** read an image using ITK -- image-based template */
template <typename TImage>
typename TImage::Pointer ReadImage( const std::string &fileName,
                                    const bool zeroOrigin = false )
{
  typename TImage::Pointer image;
  typedef itk::ImageFileReader<TImage> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( fileName.c_str() );
  try
    {
    reader->Update();
    }
  catch ( itk::ExceptionObject &err )
    {
    std::cout << "Caught an exception: " << std::endl;
    std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
    throw err;
    }
  catch ( ... )
    {
    std::cout << "Error while reading in image " << fileName << std::endl;
    throw;
    }
  image = reader->GetOutput();
  return image;
}

/** write an image using ITK */
template <class ImageType>
void
WriteImage(typename ImageType::Pointer &image,
           const std::string &fileName)
{
  typedef itk::ImageFileWriter<ImageType> WriterType;
  typename  WriterType::Pointer writer = WriterType::New();

  writer->SetFileName( fileName.c_str() );

  writer->SetInput(image);

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject &err )
    {
    std::cout << "Exception Object caught: " << std::endl;
    std::cout << err << std::endl;
    throw;
    }
  catch ( ... )
    {
    std::cout << "Error while writing in image " << fileName << std::endl;
    throw;
    }
}


int
itkMetaImageIOMetaDataTest(int argc, char **argv)
{
  if(argc < 2)
    {
    std::cerr << "Usage: metadatatest outputimage" << std::endl;
    return 1;
    }
  // write out an image -- using a random image source, but
  // the image data is irrelevant
  const int Dim(2);

  typedef unsigned char                     PixelType;
  typedef itk::Image<PixelType,Dim>         ImageType;
  typedef itk::RandomImageSource<ImageType> SourceType;

  SourceType::Pointer source = SourceType::New();
  ImageType::SizeValueType size[Dim] = { 32,32 };
  source->SetSize(size);
  source->SetMin(itk::NumericTraits<PixelType>::min());
  source->SetMax(itk::NumericTraits<PixelType>::max());
  source->Update();

  ImageType::Pointer randImage(source->GetOutput());
  //
  // add an arbitrary key to check whether it persists with the image
  std::string key("hello"); std::string value("world");
  itk::EncapsulateMetaData<std::string>(randImage->GetMetaDataDictionary(),
                                        key,value);
  WriteImage<ImageType>(randImage,argv[1]);

  //
  // Read the image just written and check if the key we added
  // persisted with the file.
  ImageType::Pointer randImage2 = ReadImage<ImageType>(argv[1]);
  std::string readValue;
  bool foundKey = itk::ExposeMetaData<std::string>(randImage2->GetMetaDataDictionary(),key,readValue);
  if(foundKey)
    {
    std::cout << "Key " << key << " found " << readValue << std::endl;
    }
  else
    {
    std::cerr << "Key " << key << " not found " << std::endl;
    return 1;
    }
  return 0;
}
