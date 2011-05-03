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
#include <fstream>
#include "itkByteSwapper.h"
#include "itkNiftiImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkImageRegionConstIterator.h"

namespace
{
//
// Analyze 7.5 header -- this describes the data below,
// as an 6 x 6 x 8 image of float pixels
const unsigned char LittleEndian_hdr[] = {
  92, 1, 0, 0, 70, 76, 79, 65, 84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 114, 0,
  4, 0, 6, 0, 6, 0, 8, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 32, 0, 0, 0, 0, 0, 128, 63,
  0, 0, 128, 63, 0, 0, 128, 63, 0, 0, 128, 63, 0, 0, 128, 63, 0, 0, 128, 63,
  0, 0, 128, 63, 0, 0, 128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};
//
// float data, represented as a char stream, in little-endian
// order
const unsigned char LittleEndian_img[] = {
  0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 128, 65, 0, 0, 128, 65,
  0, 0, 128, 65, 0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 128, 65,
  0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 16, 67,
  0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 80, 67, 0, 0, 80, 67,
  0, 0, 80, 67, 0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 80, 67,
  0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 160, 66,
  0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 160, 66, 0, 0, 160, 66,
  0, 0, 160, 66, 0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 128, 65,
  0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 16, 67,
  0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 16, 67, 0, 0, 16, 67,
  0, 0, 16, 67, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 80, 67,
  0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 160, 66,
  0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 160, 66, 0, 0, 160, 66,
  0, 0, 160, 66, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 160, 66,
  0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 16, 67,
  0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 16, 67, 0, 0, 16, 67,
  0, 0, 16, 67, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 16, 67,
  0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 128, 65,
  0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 160, 66, 0, 0, 160, 66,
  0, 0, 160, 66, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 160, 66,
  0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 80, 67,
  0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 16, 67, 0, 0, 16, 67,
  0, 0, 16, 67, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 16, 67,
  0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 128, 65, 0, 0, 128, 65, 0, 0, 128, 65,
  0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 16, 67, 0, 0, 128, 65, 0, 0, 128, 65,
  0, 0, 128, 65, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 160, 66,
  0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 80, 67, 0, 0, 80, 67, 0, 0, 80, 67,
  0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 80, 67, 0, 0, 80, 67,
  0, 0, 80, 67, 0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 160, 66, 0, 0, 48, 67,
  0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 64, 66,
  0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 64, 66, 0, 0, 64, 66,
  0, 0, 64, 66, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 64, 66,
  0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 112, 67,
  0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 112, 67, 0, 0, 112, 67,
  0, 0, 112, 67, 0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 112, 67,
  0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 224, 66,
  0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 64, 66, 0, 0, 64, 66,
  0, 0, 64, 66, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 64, 66,
  0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 48, 67,
  0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 112, 67, 0, 0, 112, 67,
  0, 0, 112, 67, 0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 112, 67,
  0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 224, 66,
  0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 224, 66, 0, 0, 224, 66,
  0, 0, 224, 66, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 64, 66,
  0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 48, 67,
  0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 48, 67, 0, 0, 48, 67,
  0, 0, 48, 67, 0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 112, 67,
  0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 224, 66,
  0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 224, 66, 0, 0, 224, 66,
  0, 0, 224, 66, 0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 224, 66,
  0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 48, 67,
  0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 48, 67, 0, 0, 48, 67,
  0, 0, 48, 67, 0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 48, 67,
  0, 0, 48, 67, 0, 0, 48, 67, 0, 0, 64, 66, 0, 0, 64, 66, 0, 0, 64, 66,
  0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 224, 66, 0, 0, 224, 66,
  0, 0, 224, 66, 0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 224, 66,
  0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 112, 67, 0, 0, 112, 67, 0, 0, 112, 67,
  0, 0, 224, 66, 0, 0, 224, 66, 0, 0, 224, 66
};

/** WriteFile
 * Write out a char array as binary
 */
int WriteFile(const std::string &name, const unsigned char *buf, size_t buflen)
{
  std::ofstream f(name.c_str(),std::ios::binary|std::ios::out);
  if(!f.is_open())
    {
    return EXIT_FAILURE;
    }
  f.write(reinterpret_cast<const char *>(buf),buflen);
  f.close();
  return EXIT_SUCCESS;
}

/** ReadFile
 * read an image from disk
 */
template <class TImage>
typename TImage::Pointer
ReadImage( const std::string &fileName )
{
  typedef itk::ImageFileReader<TImage> ReaderType;

  typename ReaderType::Pointer reader = ReaderType::New();
  {
  reader->SetFileName( fileName.c_str() );
  reader->SetImageIO(itk::NiftiImageIO::New());
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cout << "Caught an exception: " << std::endl;
    std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
    throw err;
    }
  catch(...)
    {
    std::cout << "Error while reading in image  " << fileName << std::endl;
    throw;
    }
  }
  typename TImage::Pointer image = reader->GetOutput();
  return image;
}

}

int itkNiftiReadAnalyzeTest(int ac, char *av[])
{
  if(ac < 2)
    {
    std::cerr << "itkNiftiReadAnalyzeTest: Missing test directory argument"
              << std::endl;
    return EXIT_FAILURE;
    }
  //
  // since ITK4 doesn't support writing Analyze images, we write out a
  // 'canned' image which is in little-endian byte order.
  std::string hdrName(av[1]);
  hdrName += "/littleEndian.hdr";
  std::string imgName(av[1]);
  imgName += "/littleEndian.img";
  if(WriteFile(hdrName,LittleEndian_hdr,sizeof(LittleEndian_hdr)) != EXIT_SUCCESS)
    {
    std::cerr << "itkNiftiReadAnalyzeTest: failed to write "
              << hdrName
              << std::endl;
    return EXIT_FAILURE;
    }
  if(WriteFile(imgName,LittleEndian_img,sizeof(LittleEndian_img)) != EXIT_SUCCESS)
    {
    std::cerr << "itkNiftiReadAnalyzeTest: failed to write "
              << imgName
              << std::endl;
    return EXIT_FAILURE;
    }
  //
  // read the image just written back in.
  typedef itk::Image<float,3> ImageType;
  ImageType::Pointer img;
  try
    {
    img = ReadImage<ImageType>(hdrName);
    }
  catch(...)
    {
    return EXIT_FAILURE;
    }
  //
  // compare read pixels with pixels in the array we wrote out.
  const float *fPtr = reinterpret_cast<const float *>(LittleEndian_img);
  itk::ImageRegionConstIterator<ImageType> it(img,img->GetLargestPossibleRegion());
  it.GoToBegin();
  for(; !it.IsAtEnd(); ++it,++fPtr)
    {
    //
    // in the unlikely event we're testing on a big-endian machine, do
    // byte swapping on floats pulled from the little-endian array.
    float cur = *fPtr;
    itk::ByteSwapper<float>::SwapFromSystemToLittleEndian(&cur);
    if(it.Get() != cur)
      {
      std::cerr << "expected pixel value "
                << cur
                << " but found "
                << it.Get()
                << std::endl;
      return EXIT_FAILURE;
      }
    }
  return EXIT_SUCCESS;
}
