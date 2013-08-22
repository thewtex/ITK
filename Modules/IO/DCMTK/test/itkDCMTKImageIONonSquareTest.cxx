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

/** this test exercises a bug whereby row and column dimennsions were
* assigned backwards in the image dimensions.
*/
int itkDCMTKImageIONonSquareTest(int ac, char * av[])
{
  if(ac < 2)
    {
    std::cerr << "Usage: itkDCMTKImageIONonSquareTest <dicom file>" << std::endl;
    return EXIT_FAILURE;
    }
  typedef itk::Image<unsigned short,2>    ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO(itk::DCMTKImageIO::New());
  reader->SetFileName( av[1] );
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
  ImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  if(size[0] != 1152 || size[1] != 864)
    {
    std::cerr << "Incorrect size reported " << size << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
