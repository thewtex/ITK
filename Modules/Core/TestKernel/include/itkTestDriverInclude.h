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
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#ifndef __itkTestDriverInclude_h
#define __itkTestDriverInclude_h
//
// This file is used by the TestDriver executables generated by CMake's
// create_test_sourcelist. It defines a function, ProcessArguments
// that processes the command line for the test driver prior to
// invoking the test. It also defines the RegressiontestImage function
// that is called after a test has been run by the driver.
// command line options prior to invoking the test.
//

#include "itksys/Process.h"

#include "itkWin32Header.h"
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "itkMultiThreader.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkTestingStretchIntensityImageFilter.h"
#include "itkTestingExtractSliceImageFilter.h"
#include "itkTestingComparisonImageFilter.h"
#include "itkTestingHashImageFilter.h"
#include "itksys/SystemTools.hxx"
#include "itkIntTypes.h"
#ifdef LINUX
#include "itkFloatingPointExceptions.h"
#endif
#include "vnl/vnl_sample.h"

#define ITK_TEST_DIMENSION_MAX 6

int RegressionTestImage(const char *testImageFilename,
                        const char *baselineImageFilename,
                        int reportErrors,
                        double intensityTolerance,
                        ::itk::SizeValueType numberOfPixelsTolerance = 0,
                        unsigned int radiusTolerance = 0);

int HashTestImage( const char *testImageFilename,
                   const std::string md5hash );


std::map< std::string, int > RegressionTestBaselines(char *);

typedef std::pair< char *, char * > ComparePairType;

// A structure to hold regression test parameters
typedef struct
{
  std::vector< ComparePairType > compareList;
  double intensityTolerance;
  unsigned int numberOfPixelsTolerance;
  unsigned int radiusTolerance;
} RegressionTestParameters;

RegressionTestParameters regressionTestParameters;


typedef std::pair< const char *, std::vector<std::string> > HashPairType;

std::vector< HashPairType > hashTestList;

typedef char ** ArgumentStringType;


// Types to hold parameters that should be processed later
typedef std::vector< char * > ArgumentsList;

struct ProcessedOutputType
{
  bool externalProcessMustBeCalled;

  ArgumentsList args;
  ArgumentsList add_before_libpath;
  ArgumentsList add_before_env;
  ArgumentsList add_before_env_with_sep;
};

// A structure to hold redirect output parameters
typedef struct
{
  bool redirect;
  std::string fileName;
} RedirectOutputParameters;

RedirectOutputParameters redirectOutputParameters;

void usage()
{
  std::cerr << "usage: itkTestDriver [options] prg [args]" << std::endl;
  std::cerr << "       itkTestDriver --no-process [options]" << std::endl;
  std::cerr << std::endl;
  std::cerr << "itkTestDriver alter the environment, run a test program and compare the images" << std::endl;
  std::cerr << "produced." << std::endl;
  std::cerr << std::endl;
  std::cerr << "Options:" << std::endl;
  std::cerr << "  --add-before-libpath PATH" << std::endl;
  std::cerr << "      Add a path to the library path environment. This option take care of" << std::endl;
  std::cerr << "      choosing the right environment variable for your system." << std::endl;
  std::cerr << "      This option can be used several times." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --add-before-env NAME VALUE" << std::endl;
  std::cerr << "      Add a VALUE to the variable name in the environment." << std::endl;
  std::cerr << "      The seperator used is the default one on the system." << std::endl;
  std::cerr << "      This option can be used several times." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --add-before-env-with-sep NAME VALUE SEP" << std::endl;
  std::cerr << "      Add a VALUE to the variable name in the environment using the provided separator." << std::endl;
  std::cerr << "      This option can be used several times." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --compare TEST BASELINE" << std::endl;
  std::cerr << "      Compare the TEST image to the BASELINE one." << std::endl;
  std::cerr << "      This option can be used several times." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --compare-MD5 TEST md5hash0 [ md5hash1 ... ]" << std::endl;
  std::cerr << "      Compare the TEST image file's md5 hash to the provided hash." << std::endl;
  std::cerr << "      md5hash0 is required and assumed to be a hash." << std::endl;
  std::cerr << "      Additional arguments are considerd hashes when the string is 32 hexi-decimal characters. " << std::endl;
  std::cerr << "      This option can be used several times for multiple comparisons." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --with-threads THREADS" << std::endl;
  std::cerr << "      Use at most THREADS threads." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --without-threads" << std::endl;
  std::cerr << "      Use at most one thread." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --compareNumberOfPixelsTolerance TOLERANCE" << std::endl;
  std::cerr << "      When comparing images with --compare, allow TOLERANCE pixels to differ." << std::endl;
  std::cerr << "      Default is 0." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --compareRadiusTolerance TOLERANCE" << std::endl;
  std::cerr << "      Default is 0." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --compareIntensityTolerance TOLERANCE" << std::endl;
  std::cerr << "      Default is 2.0." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --no-process" << std::endl;
  std::cerr << "      The test driver will not invoke any process." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --full-output" << std::endl;
  std::cerr << "      Causes the full output of the test to be passed to cdash." << std::endl;
  std::cerr << "  --redirect-output TEST_OUTPUT" << std::endl;
  std::cerr << "      Redirects the test output to the file TEST_OUTPUT." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --" << std::endl;
  std::cerr << "      The options after -- are not interpreted by this program and passed" << std::endl;
  std::cerr << "      directly to the test program." << std::endl;
  std::cerr << std::endl;
  std::cerr << "  --help" << std::endl;
  std::cerr << "      Display this message and exit." << std::endl;
  std::cerr << std::endl;
}


int ProcessArguments(int *ac, ArgumentStringType *av, ProcessedOutputType * processedOutput = NULL )
{
#ifdef LINUX
  itk::FloatingPointExceptions::Enable();
#endif
  regressionTestParameters.intensityTolerance  = 2.0;
  regressionTestParameters.numberOfPixelsTolerance = 0;
  regressionTestParameters.radiusTolerance = 0;

  redirectOutputParameters.redirect = false;

  if( processedOutput )
    {
    processedOutput->externalProcessMustBeCalled = true;
    }

  // parse the command line
  int  i = 1;
  bool skip = false;
  while ( i < *ac )
  {
     if ( !skip && strcmp((*av)[i], "--compare") == 0 )
      {
      if ( i + 2 >= *ac )
        {
        usage();
        return 1;
        }
      regressionTestParameters.compareList.push_back( ComparePairType((*av)[i + 1], (*av)[i + 2]) );
      (*av) += 3;
      *ac -= 3;
      }
     else if ( !skip && strcmp((*av)[i], "--compare-MD5") == 0 )
      {
      if ( i + 2 >= *ac )
        {
        usage();
        return 1;
        }
      const char *filename = (*av)[i + 1];
      std::string md5hash0 =  (*av)[i + 2];

     // convert hash to all lowercase letters
     for( std::string::iterator iter = md5hash0.begin(); iter != md5hash0.end(); ++iter )
       {
       *iter = tolower(*iter);
       }

     // chech that the hash is of expected format
     if ( md5hash0.size() != 32 ||
          md5hash0.find_first_not_of( "0123456789abcdef" ) != std::string::npos )
       {
       std::cerr << "Warning: argument does not appear to be a valid md5 hash \"" << md5hash0 << "\"." << std::endl;
       }

     std::vector< std::string > hashVector;
     hashVector.push_back( md5hash0 );

     (*av) += 3;
     (*ac) -= 3;

     // continue eating hash values
     while ( *ac - i > 0 )
       {
       std::string md5hashN = (*av)[i];

       // convert hash to all lowercase letters
       for( std::string::iterator iter = md5hashN.begin(); iter != md5hashN.end(); ++iter )
         {
         *iter = tolower(*iter);
         }

       // check if the next argument is a hash
       if ( md5hashN.size() != 32 ||
            md5hashN.find_first_not_of( "0123456789abcdef" ) != std::string::npos )
         {
         break;
         }

       // add the hash
       hashVector.push_back( md5hashN );

       // successful hash,
       // move the arguments along
       ++(*av);
       --(*ac);
       }


     hashTestList.push_back( HashPairType( filename, hashVector )  );

      }
    else if ( !skip && strcmp((*av)[i], "--") == 0 )
      {
      skip = true;
      i += 1;
      }
    else if ( !skip && strcmp((*av)[i], "--help") == 0 )
      {
      usage();
      return 1;
      }
    else if ( !skip && strcmp((*av)[i], "--with-threads") == 0 )
      {
      if ( i + 1 >= *ac )
        {
        usage();
        return 1;
        }
      // set the environment which will be read by the subprocess
      std::string threadEnv = "ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS=";
      threadEnv += (*av)[i + 1];
      itksys::SystemTools::PutEnv( threadEnv.c_str() );
      // and set the number of threads locally for the comparison
      itk::MultiThreader::SetGlobalDefaultNumberOfThreads(atoi((*av)[i + 1]));
      *av += 2;
      *ac -= 2;
      }
    else if ( !skip && strcmp((*av)[i], "--without-threads") == 0 )
      {
      itksys::SystemTools::PutEnv( "ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS=1" );
      itk::MultiThreader::SetGlobalDefaultNumberOfThreads(1);
      *av += 1;
      *ac -= 1;
      }
    else if ( !skip && strcmp((*av)[i], "--compareNumberOfPixelsTolerance") == 0 )
      {
      if ( i + 1 >= *ac )
        {
        usage();
        return 1;
        }
      regressionTestParameters.numberOfPixelsTolerance = atoi((*av)[i + 1]);
      *av += 2;
      *ac -= 2;
      }
    else if ( !skip && strcmp((*av)[i], "--compareRadiusTolerance") == 0 )
      {
      if ( i + 1 >= *ac )
        {
        usage();
        return 1;
        }
     regressionTestParameters.radiusTolerance = atoi((*av)[i + 1]);
      (*av) += 2;
      *ac -= 2;
      }
    else if ( !skip && strcmp((*av)[i], "--compareIntensityTolerance") == 0 )
      {
      if ( i + 1 >= *ac )
        {
        usage();
        return 1;
        }
      regressionTestParameters.intensityTolerance = atof((*av)[i + 1]);
      (*av) += 2;
      *ac -= 2;
      }
    else if ( !skip && strcmp((*av)[i], "--add-before-libpath") == 0 )
      {
      if ( i + 1 >= *ac )
        {
        usage();
        return 1;
        }
      if( processedOutput )
        {
        processedOutput->add_before_libpath.push_back( (*av)[i+1] );
        }
      (*av) += 2;
      *ac -= 2;
      }
    else if ( !skip && strcmp((*av)[i], "--add-before-env") == 0 )
      {
      if ( i + 1 >= *ac )
        {
        usage();
        return 1;
        }
      if( processedOutput )
        {
        processedOutput->add_before_env.push_back( (*av)[i+1] );
        processedOutput->add_before_env.push_back( (*av)[i+2] );
        }
      (*av) += 3;
      *ac -= 3;
      }
    else if ( !skip && strcmp((*av)[i], "--add-before-env-with-sep") == 0 )
      {
      if ( i + 1 >= *ac )
        {
        usage();
        return 1;
        }
      if( processedOutput )
        {
        processedOutput->add_before_env_with_sep.push_back( (*av)[i+1] );
        processedOutput->add_before_env_with_sep.push_back( (*av)[i+2] );
        processedOutput->add_before_env_with_sep.push_back( (*av)[i+3] );
        }
      (*av) += 4;
      *ac -= 4;
      }
    else if ( !skip && strcmp((*av)[i], "--full-output") == 0 )
      {
      // emit the string to tell ctest that the full output should be
      // passed to cdash.
      std::cout << "CTEST_FULL_OUTPUT" << std::endl;
      (*av) += 1;
      *ac -= 1;
      }
    else if ( !skip && strcmp((*av)[i], "--no-process") == 0 )
      {
      // The test driver needs to invoke another executable
      // For example, the python interpreter to run Wrapping tests.
      if( processedOutput )
        {
        processedOutput->externalProcessMustBeCalled = false;
        }
      (*av) += 1;
      *ac -= 1;
      }
    else if ( !skip && strcmp((*av)[i], "--redirectOutput") == 0 )
      {
      if ( i + 1 >= *ac )
        {
        usage();
        return 1;
        }
      redirectOutputParameters.redirect = true;
      redirectOutputParameters.fileName = (*av)[i + 1];
      *av += 2;
      *ac -= 2;
      }
    else
      {
      if( processedOutput )
        {
        processedOutput->args.push_back((*av)[i]);
        }
      i += 1;
      }
  }

  return 0;
}


// Regression Testing Code

int RegressionTestImage(const char *testImageFilename,
                        const char *baselineImageFilename,
                        int reportErrors,
                        double intensityTolerance,
                        ::itk::SizeValueType numberOfPixelsTolerance,
                        unsigned int radiusTolerance)
{
  // Use the factory mechanism to read the test and baseline files and convert
  // them to double
  typedef itk::Image< double, ITK_TEST_DIMENSION_MAX >        ImageType;
  typedef itk::Image< unsigned char, ITK_TEST_DIMENSION_MAX > OutputType;
  typedef itk::Image< unsigned char, 2 >                      DiffOutputType;
  typedef itk::ImageFileReader< ImageType >                   ReaderType;

  // Read the baseline file
  ReaderType::Pointer baselineReader = ReaderType::New();
  baselineReader->SetFileName(baselineImageFilename);
  try
    {
    baselineReader->UpdateLargestPossibleRegion();
    }
  catch ( itk::ExceptionObject & e )
    {
    std::cerr << "Exception detected while reading " << baselineImageFilename << " : "  << e.GetDescription();
    return 1000;
    }

  // Read the file generated by the test
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName(testImageFilename);
  try
    {
    testReader->UpdateLargestPossibleRegion();
    }
  catch ( itk::ExceptionObject & e )
    {
    std::cerr << "Exception detected while reading " << testImageFilename << " : "  << e.GetDescription() << std::endl;
    return 1000;
    }

  // The sizes of the baseline and test image must match
  ImageType::SizeType baselineSize;
  baselineSize = baselineReader->GetOutput()->GetLargestPossibleRegion().GetSize();
  ImageType::SizeType testSize;
  testSize = testReader->GetOutput()->GetLargestPossibleRegion().GetSize();

  if ( baselineSize != testSize )
    {
    std::cerr << "The size of the Baseline image and Test image do not match!" << std::endl;
    std::cerr << "Baseline image: " << baselineImageFilename
              << " has size " << baselineSize << std::endl;
    std::cerr << "Test image:     " << testImageFilename
              << " has size " << testSize << std::endl;
    return 1;
    }

  // Now compare the two images
  typedef itk::Testing::ComparisonImageFilter< ImageType, ImageType > DiffType;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput( baselineReader->GetOutput() );
  diff->SetTestInput( testReader->GetOutput() );
  diff->SetDifferenceThreshold(intensityTolerance);
  diff->SetToleranceRadius(radiusTolerance);
  diff->UpdateLargestPossibleRegion();

  itk::SizeValueType status = itk::NumericTraits< itk::SizeValueType >::Zero;
  status = diff->GetNumberOfPixelsWithDifferences();

  //The measurement errors should be reported for both success and errors
  //to facilitate setting tight tolerances of tests.
  std::cout << "<DartMeasurement name=\"ImageError\" type=\"numeric/double\">";
  std::cout << status;
  std::cout <<  "</DartMeasurement>" << std::endl;

  // if there are discrepencies, create an diff image
  if ( ( status > numberOfPixelsTolerance ) && reportErrors )
    {
    typedef itk::Testing::StretchIntensityImageFilter< ImageType, OutputType >     RescaleType;
    typedef itk::Testing::ExtractSliceImageFilter< OutputType, DiffOutputType >    ExtractType;
    typedef itk::ImageFileWriter< DiffOutputType >                        WriterType;
    typedef itk::ImageRegion< ITK_TEST_DIMENSION_MAX >                    RegionType;
    OutputType::SizeType size; size.Fill(0);

    RescaleType::Pointer rescale = RescaleType::New();
    rescale->SetOutputMinimum( itk::NumericTraits< unsigned char >::NonpositiveMin() );
    rescale->SetOutputMaximum( itk::NumericTraits< unsigned char >::max() );
    rescale->SetInput( diff->GetOutput() );
    rescale->UpdateLargestPossibleRegion();
    size = rescale->GetOutput()->GetLargestPossibleRegion().GetSize();

    //Get the center slice of the image,  In 3D, the first slice
    //is often a black slice with little debugging information.
    OutputType::IndexType index; index.Fill(0);
    for ( unsigned int i = 2; i < ITK_TEST_DIMENSION_MAX; i++ )
      {
      index[i] = size[i] / 2; //NOTE: Integer Divide used to get approximately
                              // the center slice
      size[i] = 0;
      }

    RegionType region;
    region.SetIndex(index);

    region.SetSize(size);

    ExtractType::Pointer extract = ExtractType::New();
    extract->SetDirectionCollapseToIdentity();
    extract->SetInput( rescale->GetOutput() );
    extract->SetExtractionRegion(region);

    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( extract->GetOutput() );

    std::ostringstream diffName;
    diffName << testImageFilename << ".diff.png";
    try
      {
      rescale->SetInput( diff->GetOutput() );
      rescale->Update();
      }
    catch ( const std::exception & e )
      {
      std::cerr << "Error during rescale of " << diffName.str() << std::endl;
      std::cerr << e.what() << "\n";
      }
    catch ( ... )
      {
      std::cerr << "Error during rescale of " << diffName.str() << std::endl;
      }
    writer->SetFileName( diffName.str().c_str() );
    try
      {
      writer->Update();
      }
    catch ( const std::exception & e )
      {
      std::cerr << "Error during write of " << diffName.str() << std::endl;
      std::cerr << e.what() << "\n";
      }
    catch ( ... )
      {
      std::cerr << "Error during write of " << diffName.str() << std::endl;
      }

    std::cout << "<DartMeasurementFile name=\"DifferenceImage\" type=\"image/png\">";
    std::cout << diffName.str();
    std::cout << "</DartMeasurementFile>" << std::endl;

    std::ostringstream baseName;
    baseName << testImageFilename << ".base.png";
    try
      {
      rescale->SetInput( baselineReader->GetOutput() );
      rescale->Update();
      }
    catch ( const std::exception & e )
      {
      std::cerr << "Error during rescale of " << baseName.str() << std::endl;
      std::cerr << e.what() << "\n";
      }
    catch ( ... )
      {
      std::cerr << "Error during rescale of " << baseName.str() << std::endl;
      }
    try
      {
      writer->SetFileName( baseName.str().c_str() );
      writer->Update();
      }
    catch ( const std::exception & e )
      {
      std::cerr << "Error during write of " << baseName.str() << std::endl;
      std::cerr << e.what() << "\n";
      }
    catch ( ... )
      {
      std::cerr << "Error during write of " << baseName.str() << std::endl;
      }

    std::cout << "<DartMeasurementFile name=\"BaselineImage\" type=\"image/png\">";
    std::cout << baseName.str();
    std::cout << "</DartMeasurementFile>" << std::endl;

    std::ostringstream testName;
    testName << testImageFilename << ".test.png";
    try
      {
      rescale->SetInput( testReader->GetOutput() );
      rescale->Update();
      }
    catch ( const std::exception & e )
      {
      std::cerr << "Error during rescale of " << testName.str() << std::endl;
      std::cerr << e.what() << "\n";
      }
    catch ( ... )
      {
      std::cerr << "Error during rescale of " << testName.str() << std::endl;
      }
    try
      {
      writer->SetFileName( testName.str().c_str() );
      writer->Update();
      }
    catch ( const std::exception & e )
      {
      std::cerr << "Error during write of " << testName.str() << std::endl;
      std::cerr << e.what() << "\n";
      }
    catch ( ... )
      {
      std::cerr << "Error during write of " << testName.str() << std::endl;
      }

    std::cout << "<DartMeasurementFile name=\"TestImage\" type=\"image/png\">";
    std::cout << testName.str();
    std::cout << "</DartMeasurementFile>" << std::endl;
    }
  return ( status > numberOfPixelsTolerance ) ? 1 : 0;
}

template< typename TImageType >
std::string ComputeHash( const char *testImageFilename )
{
  typedef TImageType                        ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;


  // Read the file generated by the test
  typename ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName(testImageFilename);
  try
    {
    testReader->UpdateLargestPossibleRegion();
    }
  catch ( itk::ExceptionObject & e )
    {
    std::cerr << "Exception detected while reading " << testImageFilename << " : "  << e.GetDescription() << std::endl;
    throw; // re-throw
    }

  typedef itk::Testing::HashImageFilter<ImageType> HashFilterType;

  typename HashFilterType::Pointer hasher = HashFilterType::New();
  hasher->SetInput( testReader->GetOutput() );
  hasher->Update();

  return hasher->GetHash();
}


int HashTestImage( const char *testImageFilename,
                   const std::vector<std::string> &baselineMD5Vector )
{
  itk::ImageIOBase::Pointer iobase =
    itk::ImageIOFactory::CreateImageIO( testImageFilename, itk::ImageIOFactory::ReadMode);

  if ( iobase.IsNull() )
    {
    itkGenericExceptionMacro( "Unable to determine ImageIO reader for \"" << testImageFilename << "\"" );
    }

  // Read the image information
  iobase->SetFileName( testImageFilename );
  iobase->ReadImageInformation();

  // get output information about input image
  itk::ImageIOBase::IOComponentType componentType = iobase->GetComponentType();

  std::string testMD5 = "";
  switch(componentType)
    {
    case itk::ImageIOBase::CHAR:
      testMD5 = ComputeHash< itk::VectorImage<char, ITK_TEST_DIMENSION_MAX> >( testImageFilename );
      break;
    case itk::ImageIOBase::UCHAR:
      testMD5 = ComputeHash< itk::VectorImage<unsigned char, ITK_TEST_DIMENSION_MAX> >( testImageFilename );
      break;
    case itk::ImageIOBase::SHORT:
      testMD5 = ComputeHash< itk::VectorImage<short, ITK_TEST_DIMENSION_MAX> >( testImageFilename );
      break;
    case itk::ImageIOBase::USHORT:
      testMD5 = ComputeHash< itk::VectorImage<unsigned short, ITK_TEST_DIMENSION_MAX> >( testImageFilename );
      break;
    case itk::ImageIOBase::INT:
      testMD5 = ComputeHash< itk::VectorImage<int, ITK_TEST_DIMENSION_MAX> >( testImageFilename );
      break;
    case itk::ImageIOBase::UINT:
      testMD5 = ComputeHash< itk::VectorImage<unsigned int, ITK_TEST_DIMENSION_MAX> >( testImageFilename );
      break;
    case itk::ImageIOBase::LONG:
      testMD5 = ComputeHash< itk::VectorImage<long, ITK_TEST_DIMENSION_MAX> >( testImageFilename );
      break;
    case itk::ImageIOBase::ULONG:
      testMD5 = ComputeHash< itk::VectorImage<unsigned long, ITK_TEST_DIMENSION_MAX> >( testImageFilename );
      break;
    case itk::ImageIOBase::FLOAT:
    case itk::ImageIOBase::DOUBLE:
      std::cerr << "Hashing is not supporting for float and double images." << std::endl;
      itkGenericExceptionMacro( "Hashing is not supported for images of float or doubles." );
      break;
    case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
    default:
      assert( false ); // should never get here unless we forgot a type
      itkGenericExceptionMacro( "Logic error!" );
    }

  std::vector<std::string>::const_iterator iter = baselineMD5Vector.begin();
  assert( baselineMD5Vector.size() );
  do
    {
    if ( *iter == testMD5 )
      {
      // success, let's get out of here
      return 0;
      }
    }
  while (++iter != baselineMD5Vector.end() );

  // failed to match print the different md5s
  std::cout << "<DartMeasurement name=\"TestMD5\" type=\"text/string\">";
  std::cout << testMD5;
  std::cout <<  "</DartMeasurement>" << std::endl;


  // print out all md5 baselines
  for (   iter = baselineMD5Vector.begin(); iter != baselineMD5Vector.end() ; ++iter )
    {

    std::cout << "<DartMeasurement name=\"BaselineMD5\" type=\"text/string\">";
    std::cout << *iter;
    std::cout <<  "</DartMeasurement>" << std::endl;

    }

  typedef itk::Image< double, ITK_TEST_DIMENSION_MAX >                            ImageType;
  typedef itk::Image< double, 2 >                                                 SliceImageType;
  typedef itk::Image< unsigned char, 2 >                                          OutputType;
  typedef itk::ImageFileReader< ImageType >                                       ReaderType;
  typedef itk::Testing::StretchIntensityImageFilter< SliceImageType, OutputType > RescaleType;
  typedef itk::Testing::ExtractSliceImageFilter< ImageType, SliceImageType >      ExtractType;
  typedef itk::ImageFileWriter< OutputType >                                      WriterType;

  // setup reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( testImageFilename );
  reader->UpdateLargestPossibleRegion();

  ImageType::SizeType size;
  size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();

  //Get the center slice of the image,  In 3D, the first slice
  //is often a black slice with little debugging information.
  ImageType::IndexType index; index.Fill(0);
  for ( unsigned int i = 2; i < ITK_TEST_DIMENSION_MAX; i++ )
    {
    index[i] = size[i] / 2; //NOTE: Integer Divide used to get approximately
    // the center slice
    size[i] = 0;
    }


  ImageType::RegionType region;
  region.SetIndex(index);

  region.SetSize(size);

  ExtractType::Pointer extract = ExtractType::New();
  extract->SetDirectionCollapseToIdentity();
  extract->SetInput( reader->GetOutput() );
  extract->SetExtractionRegion(region);

  RescaleType::Pointer rescale = RescaleType::New();
  rescale->SetOutputMinimum( itk::NumericTraits< unsigned char >::NonpositiveMin() );
  rescale->SetOutputMaximum( itk::NumericTraits< unsigned char >::max() );
  rescale->SetInput( extract->GetOutput() );

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( rescale->GetOutput() );


  std::ostringstream testName;
  testName << testImageFilename << ".test.png";

  writer->SetFileName( testName.str().c_str() );

  try
    {
    rescale->UpdateLargestPossibleRegion();
    writer->Update();
    }
  catch ( const std::exception & e )
    {
    std::cerr << "Error during rescale and writing of " <<  testName.str()<< std::endl;
    std::cerr << e.what() << "\n";
    }
  catch ( ... )
    {
    std::cerr << "Unknow error during rescale and writing of " << testName.str() << std::endl;
    }

  std::cout << "<DartMeasurementFile name=\"TestImage\" type=\"image/png\">";
  std::cout << testName.str();
  std::cout << "</DartMeasurementFile>" << std::endl;

  return 1;
}

//
// Generate all of the possible baselines
// The possible baselines are generated fromn the baselineFilename using the
// following algorithm:
// 1) strip the suffix
// 2) append a digit .x
// 3) append the original suffix.
// It the file exists, increment x and continue
//
std::map< std::string, int > RegressionTestBaselines(char *baselineFilename)
{
  std::map< std::string, int > baselines;
  baselines[std::string(baselineFilename)] = 0;

  std::string originalBaseline(baselineFilename);

  int                    x = 0;
  std::string::size_type suffixPos = originalBaseline.rfind(".");
  std::string            suffix;
  if ( suffixPos != std::string::npos )
    {
    suffix = originalBaseline.substr( suffixPos, originalBaseline.length() );
    originalBaseline.erase( suffixPos, originalBaseline.length() );
    }
  while ( ++x )
    {
    std::ostringstream filename;
    filename << originalBaseline << "." << x << suffix;
    std::ifstream filestream( filename.str().c_str() );
    if ( !filestream )
      {
      break;
      }
    baselines[filename.str()] = 0;
    filestream.close();
    }
  return baselines;
}


// Needed for explicit instantiation
#include "itkTestingComparisonImageFilter.hxx"

#endif
