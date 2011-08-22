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
//  Software Guide : BeginLatex
//
//  This example illustrates how to use the deprecated \code{AnalyzeImageIO}
//  to convert Analyze files written with ITK prior to version 4. It
//  exists as a utility to convert any Analyze files that are not
//  readable using \code{NiftiImageIO}.
//
//
//  Let's start by including the relevant header files.
//
//  \index{ImageFileRead!Convert Analyze Files}
//
//  Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAnalyzeImageIO.h"
#include "itkImage.h"
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// Use a function templated over the desired
// image pixel type.   You still have to
// decide which version of ReadAnalyzeWriteNIfTI to
// call at runtime, but at least the read/write code
// only happens once in the source code.
//
// Software Guide : EndLatex
// Software Guid : BeginCodeSnippet
template <typename TImage>
int
ReadAnalyzeWriteNIfTI(const char *inputName, const char *outputName)
{
  typedef itk::ImageFileReader<TImage> ReaderType;
  typedef itk::ImageFileWriter<TImage> WriterType;

  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  // To force the use of a particular ImageIO class, you
  // create an instance of that class, and use ImageIO::SetImageIO
  // to force the image file reader to exclusively use that
  // ImageIO.
  typename itk::AnalyzeImageIO::Pointer analyzeIO = itk::AnalyzeImageIO::New();

  reader->SetImageIO(analyzeIO);
  reader->SetFileName(inputName);
  writer->SetFileName(outputName);
  writer->SetInput(reader->GetOutput());
  try
    {
    writer->Update();
    }
  catch( itk::ImageFileReaderException& readErr )
    {
    std::cerr << "Failed to read file " << inputName
              << " " << readErr.what()
              << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ImageFileWriterException& writeErr )
    {
    std::cerr << "Failed to write file " << outputName
              << writeErr.what()
              << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << " Failure reading " << inputName
              << " or writing " << outputName
              << " " << err.what()
              << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
// Software Guide : EndCodeSnippet

int
main( int argc, char **argv )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl
              << argv[0] << " dim pixelType analyzeHdrFile  outputFileName" << std::endl
              << "pixeltype is one of CHAR, UCHAR, SHORT, USHORT, INT, UINT, FLOAT, DOUBLE, RGB"
              << std::endl;
    return EXIT_FAILURE;
    }
  // determine desired image dimension
  int dim;
  switch(argv[1][0])
    {
    case '2':
      dim = 2;
      break;
    case '3':
      dim = 3;
      break;
    default:
      std::cerr << "Unknown dimension " << argv[1] << std::endl;
      return EXIT_FAILURE;
    }
   // Software Guide : BeginLatex
   // Create an instance of AnalyzeImageIO, this will be used to read
   // in the Analyze file.  Then choose between which template
   // function to call based on a command line parameter. This kind of
   // runtime template selection is rather brute force and ugly, but
   // sometimes it's unavoidable.
   // Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
  itk::AnalyzeImageIO::Pointer analyzeIO = itk::AnalyzeImageIO::New();
  if(!analyzeIO->CanReadFile(argv[3]))
    {
    std::cerr << argv[0] << ": AnalyzeImageIO cannot read "
              << argv[3] << std::endl;
    return EXIT_FAILURE;
    }

  // base reading/writing type on command line parameter
  std::string pixelType(argv[2]);

  if(pixelType == "CHAR")
    {
    if(dim == 2)
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<char,2> >(argv[3],argv[4]);
      }
    else
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<char,3> >(argv[3],argv[4]);
      }
    }
  else if(pixelType == "UCHAR")
    {
    if(dim == 2)
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<unsigned char,2> >( argv[3],argv[4]);
      }
    else
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<unsigned char,3> >( argv[3],argv[4]);
      }
    }
// Software Guide : EndCodeSnippet
  if(pixelType == "SHORT")
    {
    if(dim == 2)
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<short,2> >( argv[3],argv[4]);
      }
    else
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<short,3> >( argv[3],argv[4]);
      }
    }
  else if(pixelType == "USHORT")
    {
    if(dim == 2)
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<unsigned short,2> >( argv[3],argv[4]);
      }
    else
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<unsigned short,3> >( argv[3],argv[4]);
      }
    }
  if(pixelType == "INT")
    {
    if(dim == 2)
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<int,2> >( argv[3],argv[4]);
      }
    else
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<int,3> >( argv[3],argv[4]);
      }
    }
  else if(pixelType == "UINT")
    {
    if(dim == 2)
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<unsigned int,2> >( argv[3],argv[4]);
      }
    else
      {
      return ReadAnalyzeWriteNIfTI<itk::Image<unsigned int,3> >( argv[3],argv[4]);
      }
    }
  if(pixelType == "FLOAT")
    {
    return ReadAnalyzeWriteNIfTI<itk::Image<float,3> >( argv[3],argv[4]);
    }
  else if(pixelType == "DOUBLE")
    {
    return ReadAnalyzeWriteNIfTI<itk::Image<double,3> >( argv[3],argv[4]);
    }
  std::cerr << "Unsupported pixel type " << pixelType << " unable to read or write file"
            << std::endl;
  return EXIT_FAILURE;
}
