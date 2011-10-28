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

#include "itkVideoDummyCamera.h"
#include "itkImportImageFilter.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

///////////////////////////////////////////////////////////////////////////////
// This tests all of the functionality of the VideoDummyCamera
//
// Usage: [Width] [Height] [Num Frames] [Frames Per Second]

int test_VideoDummyCamera ( unsigned int inWidth,
                            unsigned int inHeight,
                            unsigned long inNumFrames,
                            double inFpS )
{

  // ITK typedefs
  typedef itk::RGBPixel<unsigned char>         PixelType;
  typedef itk::ImportImageFilter<PixelType, 2> ImportFilterType;
  typedef itk::Image<PixelType, 2>             ImageType;
  typedef itk::ImageFileReader<ImageType>      ReaderType;
  typedef itk::ImageFileWriter<ImageType>      WriterType;

  int ret = EXIT_SUCCESS;

  // Create the VideoIO
  itk::VideoDummyCamera::Pointer videoDummyIO = itk::VideoDummyCamera::New();

  std::stringstream DummyCameraURLStream;
  DummyCameraURLStream << "camera://dummy/solidcolor?color=00c020"
                       << "&fps="       << inFpS
                       << "&maxframes=" << inNumFrames
                       << "&width=" << inWidth
                       << "&height=" << inHeight;
  std::string DummyCameraURL = DummyCameraURLStream.str();


  //////
  // CanReadFile
  //////
  std::cout << "VideoDummyCamera::CanReadFile..." << std::endl;


  // Test CanReadFile on good camera://dummy/... URL
  if (!videoDummyIO->CanReadFile(DummyCameraURL.c_str()))
    {
    std::cerr << "Could not read " << DummyCameraURL << std::endl;
    ret = EXIT_FAILURE;
    }

  // Test CanReadFile on non-existant file
  std::string nonExistantFile = "Bad/Path/To/Nothing";
  if (videoDummyIO->CanReadFile(nonExistantFile.c_str()))
    {
    std::cerr << "Should have failed to open \"" << nonExistantFile << "\"" << std::endl;
    ret = EXIT_FAILURE;
    }

  // Test CanReadFile on non-supported camera:// URL
  std::string badCameraURL = "camera://opencv/0";
  if (videoDummyIO->CanReadFile(badCameraURL.c_str()))
    {
    std::cerr << "Should have failed to open \"" << badCameraURL << "\"" << std::endl;
    ret = EXIT_FAILURE;
    }


  //////
  // SetFileName
  //////
  std::cout << "VideoDummyCamera::SetFileName..." << std::endl;

  videoDummyIO->SetFileName(DummyCameraURL.c_str());


  //////
  // ReadImageInformation
  //////
  std::cout << "VideoDummyCamera::ReadImageInformation..." << std::endl;

  videoDummyIO->ReadImageInformation();

  bool infoSet = true;
  std::stringstream paramMessage;
  if (videoDummyIO->GetDimensions(0) != inWidth)
    {
    infoSet = false;
    paramMessage << "Width mismatch: (expected) " << inWidth << " != (got) "
      << videoDummyIO->GetDimensions(0) << std::endl;
    }
  if (videoDummyIO->GetDimensions(1) != inHeight)
    {
    infoSet = false;
    paramMessage << "Height mismatch: (expected) " << inHeight << " != (got) "
      << videoDummyIO->GetDimensions(1) << std::endl;
    }
  double epsilon = 0.0001;
  if (videoDummyIO->GetFramesPerSecond() < inFpS - epsilon || videoDummyIO->GetFramesPerSecond() > inFpS + epsilon)
    {
    infoSet = false;
    paramMessage << "FpS mismatch: (expected) " << inFpS << " != (got) " << videoDummyIO->GetFramesPerSecond()
      << std::endl;
    }
  if (videoDummyIO->GetFrameTotal() != inNumFrames)
    {
    infoSet = false;
    paramMessage << "FrameTotal mismatch: (expected) " << inNumFrames << " != (got) "
      << videoDummyIO->GetFrameTotal() << std::endl;
    }

  if (!infoSet)
    {
    std::cerr << paramMessage.str();
    ret = EXIT_FAILURE;
    }

  //////
  // Print out PrintSelf Info
  //////
  std::cout << "VideoDummyCamera::print()..." << std::endl;
  videoDummyIO->Print(std::cout);

  //////
  // Read
  //////
  size_t bufferSize = videoDummyIO->GetImageSizeInBytes();
  PixelType* buffer = new PixelType[bufferSize];
  videoDummyIO->Read(static_cast<void*>(buffer));

  std::cout << "VideoDummyCamera::Read..." << std::endl;

  // Check buffer contents for each generated frame
  // For speed reasons, don't check more than 10000 frames
  itk::intmax_t framesToCheck = (inNumFrames>10000)?10000:inNumFrames;
  itk::intmax_t pixelCount = videoDummyIO->GetImageSizeInPixels();
  //itk::intmax_t pixelSize = videoDummyIO->GetPixelSize();
  PixelType correctPixelValue;
  correctPixelValue.Set(0x00, 0xc0, 0x20);
  for(itk::intmax_t frame=0; frame<framesToCheck; frame++)
    {
    // Read the next frame using VideoDummyCamera
    videoDummyIO->Read(static_cast<void*>(buffer));
    // Test each pixel in the resulting frame
    for(int i=0; i<pixelCount; i++)
      {
      if (buffer[i] != correctPixelValue)
        {
        std::cerr << "Frame buffer["<<frame<<"] incorrect for pixel["<<i<<"] = "
                  << buffer[i] << std::endl;
        ret = false;
        }
      }
    }
  delete[] buffer;


  std::cout<<"Status = "<<ret<<" ("<<((EXIT_SUCCESS==ret)?"pass":"FAIL")<<")"<<std::endl;

  std::cout<<"Done !"<<std::endl;
  return ret;
}

int itkVideoDummyCameraTest ( int argc, char *argv[] )
{
  if (argc != 5)
    {
    std::cerr << "Usage: [Width] [Height] [Num Frames] [FpS]" << std::endl;
    return EXIT_FAILURE;
    }
  return test_VideoDummyCamera(atoi(argv[1]), atoi(argv[2]),
                               atoi(argv[3]), atof(argv[4]));
}
