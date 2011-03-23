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
// define some itksys* things to make ShareForward.h happy
#define itksys_SHARED_FORWARD_DIR_BUILD ""
#define itksys_SHARED_FORWARD_PATH_BUILD ""
#define itksys_SHARED_FORWARD_PATH_INSTALL ""
#define itksys_SHARED_FORWARD_EXE_BUILD ""
#define itksys_SHARED_FORWARD_EXE_INSTALL ""

#include "itkWin32Header.h"
#include <map>
#include <string>
#include <iostream>
#include <fstream>
// include SharedForward to avoid duplicating the code which find the library
// path variable
// name and the path separator
#include "itksys/SharedForward.h"
#include "itksys/Process.h"

#include "itkTestDriverInclude.h"

#include "itkGDCMImageIOFactory.h"
#include "itkMetaImageIOFactory.h"
#include "itkJPEGImageIOFactory.h"
#include "itkPNGImageIOFactory.h"
#include "itkTIFFImageIOFactory.h"
#include "itkBMPImageIOFactory.h"
#include "itkVTKImageIOFactory.h"
#include "itkNrrdImageIOFactory.h"
#include "itkTestDriverInclude.h"
#include "itkObjectFactoryBase.h"

int main(int ac, char *av[])
{


  itk::ObjectFactoryBase::RegisterFactory( itk::MetaImageIOFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::GDCMImageIOFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::JPEGImageIOFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::VTKImageIOFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::PNGImageIOFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::TIFFImageIOFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::BMPImageIOFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::NrrdImageIOFactory::New() );

  itk::FloatingPointExceptions::Enable();

  double intensityTolerance  = 2.0;
  unsigned int numberOfPixelsTolerance = 0;
  unsigned int radiusTolerance = 0;

  std::vector< char * > args;
  typedef std::pair< char *, char * > ComparePairType;
  std::vector< ComparePairType > compareList;

  // parse the command line
  int  i = 1;
  bool skip = false;
  while ( i < ac )
    {
    if ( !skip && strcmp(av[i], "--add-before-libpath") == 0 )
      {
      if ( i + 1 >= ac )
        {
        usage();
        return 1;
        }
      std::string libpath = KWSYS_SHARED_FORWARD_LDPATH;
      libpath += "=";
      libpath += av[i + 1];
      char *oldenv = getenv(KWSYS_SHARED_FORWARD_LDPATH);
      if ( oldenv )
        {
        libpath += KWSYS_SHARED_FORWARD_PATH_SEP;
        libpath += oldenv;
        }
      itksys::SystemTools::PutEnv( libpath.c_str() );
      // on some 64 bit systems, LD_LIBRARY_PATH_64 is used before
      // LD_LIBRARY_PATH if it is set. It can lead the test to load
      // the system library instead of the expected one, so this
      // var must also be set
      if ( std::string(KWSYS_SHARED_FORWARD_LDPATH) == "LD_LIBRARY_PATH" )
        {
        std::string libpath = "LD_LIBRARY_PATH_64";
        libpath += "=";
        libpath += av[i + 1];
        char *oldenv = getenv("LD_LIBRARY_PATH_64");
        if ( oldenv )
          {
          libpath += KWSYS_SHARED_FORWARD_PATH_SEP;
          libpath += oldenv;
          }
        itksys::SystemTools::PutEnv( libpath.c_str() );
        }
      i += 2;
      }
    else if ( !skip && strcmp(av[i], "--add-before-env") == 0 )
      {
      if ( i + 2 >= ac )
        {
        usage();
        return 1;
        }
      std::string env = av[i + 1];
      env += "=";
      env += av[i + 2];
      char *oldenv = getenv(av[i + 1]);
      if ( oldenv )
        {
        env += KWSYS_SHARED_FORWARD_PATH_SEP;
        env += oldenv;
        }
      itksys::SystemTools::PutEnv( env.c_str() );
      i += 3;
      }
    else if ( !skip && strcmp(av[i], "--add-before-env-with-sep") == 0 )
      {
      if ( i + 3 >= ac )
        {
        usage();
        return 1;
        }
      std::string env = av[i + 1];
      env += "=";
      env += av[i + 2];
      char *oldenv = getenv(av[i + 1]);
      if ( oldenv )
        {
        env += av[i + 3];
        env += oldenv;
        }
      itksys::SystemTools::PutEnv( env.c_str() );
      i += 4;
      }
    else if ( !skip && strcmp(av[i], "--compare") == 0 )
      {
      if ( i + 2 >= ac )
        {
        usage();
        return 1;
        }
      compareList.push_back( ComparePairType(av[i + 1], av[i + 2]) );
      i += 3;
      }
    else if ( !skip && strcmp(av[i], "--") == 0 )
      {
      skip = true;
      i += 1;
      }
    else if ( !skip && strcmp(av[i], "--help") == 0 )
      {
      usage();
      return 0;
      }
    else if ( !skip && strcmp(av[i], "--with-threads") == 0 )
      {
      if ( i + 1 >= ac )
        {
        usage();
        return 1;
        }
      // set the environment which will be read by the subprocess
      std::string threadEnv = "ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS=";
      threadEnv += av[i + 1];
      itksys::SystemTools::PutEnv( threadEnv.c_str() );
      // and set the number of threads locally for the comparison
      itk::MultiThreader::SetGlobalDefaultNumberOfThreads(atoi(av[i + 1]));
      i += 2;
      }
    else if ( !skip && strcmp(av[i], "--without-threads") == 0 )
      {
      itksys::SystemTools::PutEnv( "ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS=1" );
      itk::MultiThreader::SetGlobalDefaultNumberOfThreads(1);
      i += 1;
      }
    else if ( !skip && strcmp(av[i], "--compareNumberOfPixelsTolerance") == 0 )
      {
      if ( i + 1 >= ac )
        {
        usage();
        return 1;
        }
      numberOfPixelsTolerance = atoi(av[i + 1]);
      i += 2;
      }
    else if ( !skip && strcmp(av[i], "--compareRadiusTolerance") == 0 )
      {
      if ( i + 1 >= ac )
        {
        usage();
        return 1;
        }
      radiusTolerance = atoi(av[i + 1]);
      av += 2;
      ac -= 2;
      }
    else if ( !skip && strcmp(av[i], "--compareIntensityTolerance") == 0 )
      {
      if ( i + 1 >= ac )
        {
        usage();
        return 1;
        }
      intensityTolerance = atof(av[i + 1]);
      av += 2;
      ac -= 2;
      }
    else
      {
      args.push_back(av[i]);
      i += 1;
      }
    }

  if ( args.empty() )
    {
    usage();
    return 1;
    }

  // a NULL is required at the end of the table
  char **argv = new char *[args.size() + 1];
  for ( i = 0; i < static_cast< int >( args.size() ); i++ )
    {
    argv[i] = args[i];
    }
  argv[args.size()] = NULL;

  itksysProcess *process = itksysProcess_New();
  itksysProcess_SetCommand(process, argv);
  itksysProcess_SetPipeShared(process, itksysProcess_Pipe_STDOUT, true);
  itksysProcess_SetPipeShared(process, itksysProcess_Pipe_STDERR, true);
  itksysProcess_Execute(process);
  itksysProcess_WaitForExit(process, NULL);

  delete[] argv;

  int state = itksysProcess_GetState(process);
  switch( state )
    {
//     case kwsysProcess_State_Starting:
//       {
//       // this is not a possible state after itksysProcess_WaitForExit
//       std::cerr << "itkTestDriver: Internal error: process can't be in Starting State." << std::endl;
//       return 1;
//       break;
//       }
    case itksysProcess_State_Error:
      {
      std::cerr << "itkTestDriver: Process error: " << itksysProcess_GetErrorString(process) << std::endl;
      return 1;
      break;
      }
    case itksysProcess_State_Exception:
      {
      std::cerr << "itkTestDriver: Process exception: " << itksysProcess_GetExceptionString(process) << std::endl;
      return 1;
      break;
      }
    case itksysProcess_State_Executing:
      {
      // this is not a possible state after itksysProcess_WaitForExit
      std::cerr << "itkTestDriver: Internal error: process can't be in Executing State." << std::endl;
      return 1;
      break;
      }
    case itksysProcess_State_Exited:
      {
      // this is the normal case - it is treated later
      break;
      }
    case itksysProcess_State_Expired:
      {
      // this is not a possible state after itksysProcess_WaitForExit
      std::cerr << "itkTestDriver: Internal error: process can't be in Expired State." << std::endl;
      return 1;
      break;
      }
    case itksysProcess_State_Killed:
      {
      std::cerr << "itkTestDriver: The process has been killed." << std::endl;
      return 1;
      break;
      }
    case itksysProcess_State_Disowned:
      {
      std::cerr << "itkTestDriver: Process disowned." << std::endl;
      return 1;
      break;
      }
    default:
      {
      // this is not a possible state after itksysProcess_WaitForExit
      std::cerr << "itkTestDriver: Internal error: unknown State." << std::endl;
      return 1;
      break;
      }
    }

  int retCode = itksysProcess_GetExitValue(process);
  if ( retCode != 0 )
    {
    std::cerr << "itkTestDriver: Process exited with return value: " << retCode << std::endl;
    // no need to compare the images: the test has failed
    return retCode;
    }

  // now compare the images
  try
    {
    for ( i = 0; i < static_cast< int >( compareList.size() ); i++ )
      {
        char *                                 baselineFilename = compareList[i].first;
        char *                                 testFilename = compareList[i].second;
        std::map< std::string, int >           baselines = RegressionTestBaselines(baselineFilename);
        std::map< std::string, int >::iterator baseline = baselines.begin();
        std::string                            bestBaseline;
        int                                    bestBaselineStatus = itk::NumericTraits< int >::max();
        while ( baseline != baselines.end() )
          {
          baseline->second = RegressionTestImage(testFilename,
                                                 ( baseline->first ).c_str(),
                                                 0,
                                                 intensityTolerance,
                                                 numberOfPixelsTolerance,
                                                 radiusTolerance);
          if ( baseline->second < bestBaselineStatus )
            {
            bestBaseline = baseline->first;
            bestBaselineStatus = baseline->second;
            }
          if ( baseline->second == 0 )
            {
            break;
            }
          ++baseline;
          }
        // if the best we can do still has errors, generate the error images
        if ( bestBaselineStatus )
          {
          RegressionTestImage(testFilename,
                              bestBaseline.c_str(),
                              1,
                              intensityTolerance,
                              numberOfPixelsTolerance,
                              radiusTolerance);
          }

        // output the matching baseline
        std::cout << "<DartMeasurement name=\"BaselineImageName\" type=\"text/string\">";
        std::cout << itksys::SystemTools::GetFilenameName(bestBaseline);
        std::cout << "</DartMeasurement>" << std::endl;

        return bestBaselineStatus;
        }
    }
  catch ( const itk::ExceptionObject & e )
    {
    std::cerr << "ITK test driver caught an ITK exception:\n";
    std::cerr << e.GetFile() << ":" << e.GetLine() << ":\n"
              << e.GetDescription() << "\n";
    return -1;
    }
  catch ( const std::exception & e )
    {
    std::cerr << "ITK test driver caught an exception:\n";
    std::cerr << e.what() << "\n";
    return -1;
    }
  catch ( ... )
    {
    std::cerr << "ITK test driver caught an unknown exception!!!\n";
    return -1;
    }


  return 0;
}
