/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
// Disable warning for long symbol names in this file only

// Software Guide : BeginLatex
//
// \index{itk::ImageMaskSpatialObject}
// \index{itk::ImageMaskSpatialObject!GetValue()}
//
// This example illustrates how to compute the bounding box around a binary
// contained in an ImageMaskSpatialObject. This is typically useful for
// extracting the region of interest of a segmented object and ignoring the
// larger region of the image that is not occupied by the segmentation.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "itkImageMaskSpatialObject.h"
#include "itkImage.h"
// Software Guide : EndCodeSnippet

#include "itkImageFileReader.h"

namespace
{
int
ExampleMain(int argc, const char * const argv[])
{

  if (argc < 2)
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputBinaryImageFile  ";
    return EXIT_FAILURE;
  }

  using ImageMaskSpatialObject = itk::ImageMaskSpatialObject<3>;

  using ImageType = ImageMaskSpatialObject::ImageType;
  ImageType::Pointer input;
  input = itk::ReadImage<ImageType>(argv[1]);


  auto maskSO = ImageMaskSpatialObject::New();

  maskSO->SetImage(input);
  maskSO->Update();

  std::cout << "Bounding Box Region: "
            << maskSO->GetMyBoundingBoxInWorldSpace()->GetBounds()
            << std::endl;

  return EXIT_SUCCESS;
}
} // namespace

int
main(int argc, char * argv[])
{
  try
  {
    return ExampleMain(argc, argv);
  }
  catch (const itk::ExceptionObject & exceptionObject)
  {
    std::cerr << "ITK exception caught:\n" << exceptionObject << '\n';
  }
  catch (const std::exception & stdException)
  {
    std::cerr << "std exception caught:\n" << stdException.what() << '\n';
  }
  catch (...)
  {
    std::cerr << "Unhandled exception!\n";
  }
  return EXIT_FAILURE;
}
