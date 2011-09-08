#include <iostream>
#include <sstream>

#include "itkVideoIOFactory.h"
#include "itkFileListVideoIOFactory.h"

///////////////////////////////////////////////////////////////////////////////
// This tests all of the functionality of the FileListVideoIO
//
// Usage: [Video Input] [Non-Video Input] [Video Output] [Width] [Height]
//            [Num Frames] [FpS]

int test_FileListVideoIOFactory( const char* input,
                                 char* output,
                                 unsigned long itkNotUsed(cameraNumber) )
{

  int ret = EXIT_SUCCESS;

  //////
  // Register the FileListVideoIOFactory.
  //
  // There's something strange going on here that makes the factories not be
  // registered by default because of the order in which the includes happen.
  // The real strangeness seems to be in ITK's system with the modularized
  // framework since none of the factories get reigstered by default.
  itk::ObjectFactoryBase::RegisterFactory( itk::FileListVideoIOFactory::New() );

  //////
  // Create the VideoIOBase for reading from a file
  //////
  std::cout << "Trying to create IO for reading from file..." << std::endl;
  itk::VideoIOBase::Pointer ioReadFile = itk::VideoIOFactory::CreateVideoIO(
      itk::VideoIOFactory::ReadFileMode, input);
  if (!ioReadFile)
    {
    std::cerr << "Did not create valid VideoIO for reading from file " << std::endl;
    ret = EXIT_FAILURE;
    }

  //////
  // Create the VideoIOBase for writing to a file
  //////
  std::cout << "Trying to create IO for writing to file..." << std::endl;
  itk::VideoIOBase::Pointer ioWrite = itk::VideoIOFactory::CreateVideoIO(
      itk::VideoIOFactory::WriteMode, output);
  if (!ioWrite)
    {
    std::cerr << "Did not create valid VideoIO for writing " << std::endl;
    ret = EXIT_FAILURE;
    }

  std::cout<<"Done !"<<std::endl;
  return ret;
}

int itkFileListVideoIOFactoryTest( int argc, char *argv[] )
{
  if (argc != 8)
    {
    std::cerr << "Usage: [Video Input] [Video Output] [Webcam Number]" << std::endl;
    return EXIT_FAILURE;
    }

  std::string inFile = "";
  for( int i = 1; i <= 5; ++i )
    {
    inFile = inFile + std::string(argv[i]);
    if( i != 5 )
      {
      inFile = inFile + std::string(",");
      }
    }
  return test_FileListVideoIOFactory(inFile.c_str(), argv[6], atoi(argv[7]) );
}
