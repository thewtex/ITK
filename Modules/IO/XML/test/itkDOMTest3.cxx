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

/*
This program tests the functions of itk::DOMReader and itk::DOMWriter by implementing the
reader and writer for a simple testing object.
*/

#include "itkTestObjectDOMReader.h"
#include "itkTestObjectDOMReader.cxx"

#include "itkTestObjectDOMWriter.h"
#include "itkTestObjectDOMWriter.cxx"

#include <iostream>
#include "itkMacro.h"

int itkDOMTest3( int argc, char* argv[] )
{
  if ( argc < 2 )
  {
    std::cerr << "arguments expected: output.objtype.xml" << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    // create a testing object for writing
    itk::TestObject::Pointer testobj1 = itk::TestObject::New();
    testobj1->GetNotes() = "this is a testing object";
    testobj1->GetActive() = true;
    testobj1->GetCount() = 100;
    testobj1->GetFparams() = std::vector<float>( 5, 0.8f );
    //
    testobj1->GetData1().SetSize( 7 );
    testobj1->GetData1().Fill( 7 );
    //
    testobj1->GetData2().SetSize( 3 );
    testobj1->GetData2().Fill( 3 );

    // write it to an XML file using the DOM writer
    itk::TestObjectDOMWriter::Pointer writer = itk::TestObjectDOMWriter::New();
    writer->SetInputObject( testobj1 );
    writer->SetFilename( argv[1] );
    writer->Update();
    //
    std::cout << "TestObject created: " << std::endl;
    testobj1->Print( std::cout );
    std::cout << std::endl;

    // create a new testing object for reading
    itk::TestObject::Pointer testobj2 = itk::TestObject::New();

    // read the object back from an XML file using the DOM reader
    itk::TestObjectDOMReader::Pointer reader = itk::TestObjectDOMReader::New();
    reader->SetOutputObject( testobj2 );
    reader->SetFilename( argv[1] );
    reader->Update();
    //
    std::cout << "TestObject read back from saved XML file: " << std::endl;
    testobj2->Print( std::cout );
    std::cout << std::endl;
  }
  catch ( const itk::ExceptionObject& eo )
  {
    eo.Print( std::cerr );
    return EXIT_FAILURE;
  }
  catch ( ... )
  {
    std::cerr << "Unknown exception caught!" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
