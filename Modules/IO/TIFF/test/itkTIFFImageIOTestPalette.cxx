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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"
#include "itkMetaDataObject.h"
#include "itkTestingMacros.h"
#include "itkTIFFImageIO.h"

template <typename T>
void
printPalette(const std::vector<T> & palette)
{
  for (unsigned int i = 0; i < palette.size(); ++i)
  {
    std::cout << '[' << i << "]:" << palette[i] << std::endl;
  }
}


int
itkTIFFImageIOTestPalette(int argc, char * argv[])
{

  if (argc != 5)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << itkNameOfTestExecutableMacro(argv);
    std::cerr << " input"
              << " output"
              << " expandRGBPalette: request palette expanding"
              << " isPaletteImage: is image palette type" << std::endl;
    return EXIT_FAILURE;
  }

  const auto expandRGBPalette = static_cast<bool>(std::stoi(argv[3]));
  const auto isPaletteImage = static_cast<bool>(std::stoi(argv[4]));
  std::cout << "Expanding palette: " << ((expandRGBPalette) ? "True" : "False") << std::endl;
  std::cout << "Is palette: " << ((isPaletteImage) ? "True" : "False") << std::endl;

  constexpr unsigned int Dimension = 2;
  using ScalarPixelType = unsigned char;

  using ScalarImageType = itk::Image<ScalarPixelType, Dimension>;
  using ReaderType = itk::ImageFileReader<ScalarImageType>;
  using WriterType = itk::ImageFileWriter<ScalarImageType>;
  using IOType = itk::TIFFImageIO;

  auto io = IOType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(io, TIFFImageIO, ImageIOBase);

  auto reader = ReaderType::New();
  auto writer = WriterType::New();

  ITK_TEST_SET_GET_BOOLEAN(io, ExpandRGBPalette, expandRGBPalette);

  io->SetFileName(argv[1]);
  reader->SetFileName(argv[1]);
  reader->SetImageIO(io);

  if (io->CanReadFile(""))
  {
    std::cerr << "Test failed!" << std::endl;
    std::cout << "No filename specified." << std::endl;
    std::cout << "CanReadFile: "
              << "Expected false but got true" << std::endl;
    return EXIT_FAILURE;
  }

  if (!io->SupportsDimension(Dimension))
  {
    std::cerr << "Test failed!" << std::endl;
    std::cerr << "itk::TIFFImageIO does not support dimension: " << Dimension << std::endl;
    return EXIT_FAILURE;
  }

  if (io->CanStreamRead())
  {
    std::cout << "itk::TIFFImageIO can stream read" << std::endl;
  }
  else
  {
    std::cout << "itk::TIFFImageIO cannot stream read" << std::endl;
  }

  if (!io->CanReadFile(argv[1]))
  {
    std::cerr << "Test failed!" << std::endl;
    std::cout << "itk::TIFFImageIO cannot read file " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }

  ITK_TRY_EXPECT_NO_EXCEPTION(reader->Update());

  io->Print(std::cout);

  // Try Palette reading and scalar image reading
  IOType::PaletteType palette_read = io->GetColorPalette();
  if (!io->GetExpandRGBPalette() && isPaletteImage)
  {
    std::cout << "Expect to read palette image as scalar+palette." << std::endl;
    if (io->GetIsReadAsScalarPlusPalette())
    {
      std::cout << "Image read as scalar+palette." << std::endl;
    }
    else
    {
      std::cerr << "Test failed!" << std::endl;
      std::cerr << "Cannot read data of this palette image as scalar." << std::endl;
      return EXIT_FAILURE;
    }
  }
  else if (io->GetExpandRGBPalette())
  {
    if (isPaletteImage)
    {
      std::cout << "Expect to expand palette image." << std::endl;
    }
    else
    {
      std::cout << "Expect to read non palette image with expanding palette requested." << std::endl;
    }
    if (io->GetIsReadAsScalarPlusPalette())
    {
      std::cerr << std::endl << "Test failed!" << std::endl;
      std::cerr << std::endl << "Palette image read as scalar plus palette, but expanding is requested." << std::endl;
      return EXIT_FAILURE;
    }
    if (!palette_read.empty())
    {
      std::cerr << std::endl << "Test failed!" << std::endl;
      std::cerr << std::endl << "Non empty palette but expanding palette to rgb requested." << std::endl;
      return EXIT_FAILURE;
    }
    std::cout << "Image not read as scalar and palette is empty." << std::endl;
  }
  else if (!isPaletteImage)
  {
    std::cout << "Expect to read non palette image with not expanding palette requested." << std::endl;
    if (io->GetIsReadAsScalarPlusPalette())
    {
      std::cerr << std::endl << "Test failed!" << std::endl;
      std::cerr << std::endl << "Image indicated as non palette read as scalar plus palette." << std::endl;
      return EXIT_FAILURE;
    }
    if (!palette_read.empty())
    {
      std::cerr << std::endl << "Test failed!" << std::endl;
      std::cerr << std::endl << "Non empty palette but image indicated as RGB." << std::endl;
      return EXIT_FAILURE;
    }
    std::cout << "Image not read as scalar and palette is empty." << std::endl;
  }
  else
  {
    // case not possible here
  }

  const ScalarImageType::Pointer inputImage = reader->GetOutput();

  // test writing palette
  const bool writePalette = !expandRGBPalette && isPaletteImage;
  std::cerr << "Trying to write the image as " << ((writePalette) ? "palette" : "expanded palette") << std::endl;
  ITK_TEST_SET_GET_BOOLEAN(io, WritePalette, writePalette);

  io->Print(std::cout);

  writer->SetInput(inputImage);
  writer->SetImageIO(io);
  writer->SetFileName(argv[2]);

  ITK_TRY_EXPECT_NO_EXCEPTION(writer->Update());

  // try reading written image
  std::cerr << "Reading the written image, expecting " << ((writePalette) ? "palette" : "RGB") << std::endl;
  reader->SetImageIO(io);
  reader->SetFileName(argv[2]);

  ITK_TRY_EXPECT_NO_EXCEPTION(reader->Update());

  std::cerr << "Image read as " << ((io->GetIsReadAsScalarPlusPalette()) ? "scalar + palette" : "RGB") << std::endl;

  if (io->GetIsReadAsScalarPlusPalette() != writePalette)
  {
    std::cerr << "Test failed!" << std::endl;
    std::cerr << "Written image is expected to be a palette image, but could not be read such as." << std::endl;
    return EXIT_FAILURE;
  }
  if (io->GetIsReadAsScalarPlusPalette())
  {
    IOType::PaletteType palette_written = io->GetColorPalette();

    if (palette_written.size() != palette_read.size())
    {
      std::cerr << "Test failed!" << std::endl;
      std::cerr << "Read and written palette don't have the same size ( " << palette_read.size() << " vs "
                << palette_written.size() << ')' << std::endl;
      return EXIT_FAILURE;
    }
    bool   palette_equal = true;
    size_t i = 0;
    for (; i < palette_written.size(); ++i)
    {
      if (palette_written[i] != palette_read[i])
      {
        palette_equal = false;
        break;
      }
    }
    if (!palette_equal)
    {
      std::cerr << "Test failed!" << std::endl;
      std::cerr << "Palette not written as it was read at position [" << i << "]." << std::endl;
      return EXIT_FAILURE;
    }

    std::cout << "Read and written palette are equal" << std::endl;
  }

  // Exercise other methods
  const itk::ImageIOBase::SizeType pixelStride = io->GetPixelStride();
  std::cout << "PixelStride: " << static_cast<itk::NumericTraits<itk::ImageIOBase::SizeType>::PrintType>(pixelStride)
            << std::endl;

  // ToDo
  // When the palette has made into the Metadata Dictionary (as opposed to the ImageIO):
  // Use the MetaDataDictionary
  // io->SetMetaDataDictionary();
  // //

  std::cout << "Test finished" << std::endl;
  return EXIT_SUCCESS;
}
