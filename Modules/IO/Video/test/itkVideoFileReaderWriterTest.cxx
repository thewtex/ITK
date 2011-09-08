#include <iostream>
#include <sstream>

#include "itkVideoFileReader.h"
#include "itkVideoFileWriter.h"
#include "itkRGBPixel.h"
#include "itkFileListVideoIOFactory.h"

int itkVideoFileReaderWriterTest( int argc, char *argv[] )
{
  // Check parameters
  if (argc != 7)
    {
    std::cerr << "Usage: [Video Input] [Image Output]" << std::endl;
    return EXIT_FAILURE;
    }

  // Instantiate a new reader
  typedef itk::RGBPixel<unsigned char> PixelType;
  const unsigned int NumberOfDimensions =             2;
  typedef itk::Image< PixelType, NumberOfDimensions > FrameType;
  typedef itk::VideoStream< FrameType >               VideoType;
  typedef itk::VideoFileReader< VideoType >           VideoReaderType;
  typedef itk::VideoFileWriter< VideoType >           VideoWriterType;

  std::string inFile = "";
  for( int i = 1; i <= 5; ++i )
    {
    inFile = inFile + std::string(argv[i]);
    if( i != 5 )
      {
      inFile = inFile + std::string(",");
      }
    }
  VideoReaderType::Pointer reader = VideoReaderType::New();
  reader->SetFileName(inFile.c_str() );

  // I'm still not sure how to handle this right, but for now, just manually
  // register an FileListVideoIO
  itk::ObjectFactoryBase::RegisterFactory( itk::FileListVideoIOFactory::New() );

  // Set up the writer
  VideoWriterType::Pointer writer = VideoWriterType::New();
  writer->SetInput(reader->GetOutput() );
  writer->SetFileName(argv[6]);

  // Call Update on the writer to process the entire video
  writer->Update();

  return EXIT_SUCCESS;
}
