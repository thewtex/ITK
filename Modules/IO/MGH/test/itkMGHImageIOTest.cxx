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

#include "itkMGHImageIOFactory.h"
#include "itkMGHImageIO.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itksys/SystemTools.hxx"
#include "itkMetaDataObject.h"
#include "itkIOCommon.h"

#include "itkIOTestHelper.h"


#define SPECIFIC_IMAGEIO_MODULE_TEST

template<unsigned int TDimension>
typename itk::ImageBase<TDimension>::DirectionType
PreFillDirection()
{
  typename itk::ImageBase<TDimension>::DirectionType myDirection;
  myDirection.Fill(0.0);
  myDirection.SetIdentity();
  itkGenericExceptionMacro("This template should never be used. Only valid values are given below.");
  return myDirection;
}


template<>
itk::ImageBase<1>::DirectionType
PreFillDirection<1> ()
{
  itk::ImageBase<1>::DirectionType myDirection;
  myDirection.Fill(0.0);
  myDirection[0][0] = -1.0;
  return myDirection;
}

template<>
itk::ImageBase<2>::DirectionType
PreFillDirection<2> ()
{
  itk::ImageBase<2>::DirectionType myDirection;
  myDirection.Fill(0.0);
  myDirection[0][1] = 1.0;
  myDirection[1][0] = -1.0;
  return myDirection;
}

template<>
itk::ImageBase<3>::DirectionType
PreFillDirection<3> ()
{
  itk::ImageBase<3>::DirectionType myDirection;
  myDirection.Fill(0.0);
  myDirection[0][2] = 1.0;
  myDirection[1][0] = -1.0;
  myDirection[2][1] = 1.0;
  return myDirection;
}

template<>
itk::ImageBase<4>::DirectionType
PreFillDirection<4> ()
{
  itk::ImageBase<4>::DirectionType myDirection;
  myDirection.Fill(0.0);
  myDirection[0][2] = 1.0;
  myDirection[1][0] = -1.0;
  myDirection[2][1] = 1.0;
  myDirection[3][3] = 1.0;
  return myDirection;
}

bool Equal(const double a, const double b)
{
  // actual equality
  double diff = a - b;
  if(diff == 0.0)
    {
    return true;
    }
  // signs match?
  if((a < 0.00 && b >= 0.0) ||
     (b < 0.0 && a >= 0.0))
    {
    return false;
    }
  if(diff < 0.0)
    {
    diff = -diff;
    }
  double avg = (a+b)/2.0;
  if(avg < 0.0)
    {
    avg = - avg;
    }
  if(diff > avg/1000.0)
    {
    return false;
    }
  return true;
}

int itkMGHImageIOTest(int ac, char* av[])
{
  if(ac < 3 )
    {
    std::cerr << "ERROR:  Incorrect number of arguments <TestDirectory> <TestMode> [ImageFileNameRead ImageFileNameWrite]" << std::endl;
    std::cerr << "Only " << ac << " arguments given." << std::endl;
    return EXIT_FAILURE;
    }
  for(int i =0; i < ac; ++i)
    {
    std::cout << i << "  av= " << av[i] << std::endl;
    }
  //
  // first argument is passing in the writable directory to do all testing
  itksys::SystemTools::ChangeDirectory(av[1]);

  static bool firstTime = true;
  if(firstTime)
    {
    itk::ObjectFactoryBase::RegisterFactory(itk::MGHImageIOFactory::New() );
    firstTime = false;
    }
  const std::string TestMode(av[2]);

  int returnStatus = EXIT_SUCCESS;
  if( TestMode == std::string("FactoryCreationTest"))
  //Tests added to increase code coverage.
    {
    itk::MGHImageIOFactory::Pointer MyFactoryTest=itk::MGHImageIOFactory::New();
    if(MyFactoryTest.IsNull())
      {
      returnStatus = EXIT_FAILURE;
      }
    //This was made a protected function.  MyFactoryTest->PrintSelf(std::cout,0);
    }
  else if ( TestMode == std::string("TestReadWriteOfSmallImageOfAllTypes"))
    {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
    itk::MGHImageIOFactory::RegisterOneFactory();
    //TODO: Need to write test that exercises this code more.

    //TODO: Need to test reading/writting of supported types:
    //      MGHImageIO supports unsigned char, int, float and short

    //TODO: Need to test with images of non-identity direction cosigns, spacing, origin
    returnStatus = EXIT_FAILURE;
    }
  else if( TestMode == std::string("ReadImagesTest") ) //This is a mechanism for reading unsigned int images for testing.
    {
    typedef itk::Image<int, 3> ImageType;
    ImageType::Pointer input;
    const std::string imageToBeRead(av[3]);
    const std::string imageToBeWritten(av[4]);
    try
      {
      std::cout << "Reading Image: " << imageToBeRead << std::endl;
      input = itk::IOTestHelper::ReadImage<ImageType>(imageToBeRead);
      std::cout << input << std::endl;
      itk::ImageFileWriter<ImageType>::Pointer testFactoryWriter=itk::ImageFileWriter<ImageType>::New();

      testFactoryWriter->SetFileName(imageToBeWritten);
      testFactoryWriter->SetInput(input);
      testFactoryWriter->Update();
      itk::ImageFileReader<ImageType>::Pointer testFactoryReader=itk::ImageFileReader<ImageType>::New();
      testFactoryReader->SetFileName(imageToBeWritten);
      testFactoryReader->Update();
      ImageType::Pointer new_image = testFactoryReader->GetOutput();
      }
    catch (itk::ExceptionObject &e)
      {
      e.Print(std::cerr);
      returnStatus = EXIT_FAILURE;
      }
    }
  else
    {
    std::cerr << "Invalid TestMode : " << TestMode << std::endl;
    returnStatus = EXIT_FAILURE;
    }
  return returnStatus;
}
