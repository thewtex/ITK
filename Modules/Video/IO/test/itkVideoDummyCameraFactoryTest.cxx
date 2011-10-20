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
#include <sstream>

#include "itkVideoIOFactory.h"
#include "itkVideoDummyCameraFactory.h"

///////////////////////////////////////////////////////////////////////////////
// This tests all of the functionality of the VideoDummyCameraFactory
//
// Usage: [Video Input, some form of camera://dummy/...]

int test_VideoDummyCameraFactory( const char* input )
{

  int ret = EXIT_SUCCESS;

  //////
  // Register the VideoDummyCameraFactory.
  //
  // There's something strange going on here that makes the factories not be
  // registered by default because of the order in which the includes happen.
  // The real strangeness seems to be in ITK's system with the modularized
  // framework since none of the factories get reigstered by default.
  // -->The next line is now moved to VideoIOFactory::RegisterBuiltInFactories()
  //itk::ObjectFactoryBase::RegisterFactory( itk::VideoDummyCameraFactory::New() );

  //////
  // Create the VideoIOBase for reading from a file
  //////
  std::cout << "Trying to create IO for reading from specified input..." << std::endl;
  itk::VideoIOBase::Pointer ioReadFile = itk::VideoIOFactory::CreateVideoIO(
      itk::VideoIOFactory::ReadMode, input);
  if (!ioReadFile)
    {
    std::cerr << "Did not create valid VideoIO for reading from "
              << input << std::endl;
    ret = EXIT_FAILURE;
    }

  std::cout<<"Done !"<<std::endl;
  return ret;
}

int itkVideoDummyCameraFactoryTest( int argc, char *argv[] )
{
  if (argc != 2)
    {
    std::cerr << "Usage: [Video Input, some form of camera://dummy/...]" << std::endl;
    return EXIT_FAILURE;
    }

  return test_VideoDummyCameraFactory(argv[1]);
}
