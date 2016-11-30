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
#include "itkTimeStamp.h"
#include "itksys/SystemTools.hxx"
#include "itkDynamicLoader.h"
#include "itkConfigure.h"

// Additional headers required for finding the module path
#if !defined(ITK_BUILD_SHARED_LIBS) && defined(ITK_WRAP_PYTHON)

#if defined(__linux__)
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <inttypes.h>
#if defined(__ANDROID__)
#include <fcntl.h>
#include <sys/mman.h>
#endif // __ANDROID__

#elif defined(__APPLE__)
#define _DARWIN_BETTER_REALPATH
#include <mach-o/dyld.h>
#include <dlfcn.h>
#endif

#endif

namespace itk
{

/**
 * Instance creation.
 */
TimeStamp *
TimeStamp
::New()
{
  return new Self;
}

/**
 * Make this timestamp to be the same as another one.
 */
const TimeStamp &
TimeStamp::operator=( const Self & other )
{
  this->m_ModifiedTime = other.m_ModifiedTime;
  return *this;
}

/**
 * Make sure the new time stamp is greater than all others so far.
 */
void
TimeStamp
::Modified()
{
  // static assignment inside the function is required to avoid the
  // static initialization order fiasco (SIOF)
  static GlobalTimeStampType * globalTimeStamp = Self::GetGlobalTimeStamp();
  this->m_ModifiedTime = ++(*globalTimeStamp);
}


TimeStamp::GlobalTimeStampType *
TimeStamp
::GetGlobalTimeStamp()
{
  if( m_GlobalTimeStamp == ITK_NULLPTR )
    {
    GlobalTimeStampInitializer::InitializeGlobalTimeStamp();
    }
  return m_GlobalTimeStamp;
}


void
TimeStamp
::TimeStampInitialize( GlobalTimeStampType * existingGlobalTimeStamp )
{
  if( existingGlobalTimeStamp == ITK_NULLPTR )
    {
    std::cout << "creating new timestamp" << std::endl;
    Self::m_UsedExistingGlobalTimeStamp = false;
    Self::m_GlobalTimeStamp = new GlobalTimeStampType;
    }
  else
    {
    std::cerr << "Initializing with " << existingGlobalTimeStamp << std::endl;
    Self::m_UsedExistingGlobalTimeStamp = true;
    Self::m_GlobalTimeStamp = existingGlobalTimeStamp;
    }
}


void
TimeStamp
::TimeStampFinalize()
{
  if( !Self::m_UsedExistingGlobalTimeStamp )
    {
    delete Self::m_GlobalTimeStamp;
    }
}


GlobalTimeStampInitializer
::GlobalTimeStampInitializer()
{
  if(++Self::m_Count == 1)
    {
    Self::InitializeGlobalTimeStamp();
    }
}


void
GlobalTimeStampInitializer
::InitializeGlobalTimeStamp()
{
#if !defined(ITK_BUILD_SHARED_LIBS) && defined(ITK_WRAP_PYTHON)

  // Each plaform implementation should define these two variables
  std::string itkCommonPythonModule;
  std::string modulePath;
#if defined(__linux__)
  itkCommonPythonModule = "/_ITKCommonPython.so";
  int length = -1;
  const unsigned int numberOfRetries = 3;
  for( unsigned int ii = 0; ii < numberOfRetries; ++ii )
    {
    std::ifstream maps( "/proc/self/maps" );
    if( !maps.is_open() )
      {
      break;
      }

    std::string line;
    while( std::getline( maps, line ) )
      {
      const unsigned int pathMax = 2048;
      char buffer[pathMax];
      uint64_t low;
      uint64_t high;
      char perms[5];
      uint64_t offset;
      uint32_t major, minor;
      char path[pathMax];
      uint32_t inode;

      if( sscanf(line.c_str(), "%" PRIx64 "-%" PRIx64 " %s %" PRIx64 " %x:%x %u %s\n", &low, &high, perms, &offset, &major, &minor, &inode, path) == 8 )
        {
        uint64_t addr = (uint64_t)(uintptr_t)__builtin_extract_return_addr(__builtin_return_address(0));
        if( low <= addr && addr <= high )
          {
          char * resolved = realpath( path, buffer );
          if( !resolved )
            {
            break;
            }

          length = static_cast< int >( strlen( buffer ) );
#if defined(__ANDROID__)
          if (length > 4
              && path[length - 1] == 'k'
              && path[length - 2] == 'p'
              && path[length - 3] == 'a'
              && path[length - 4] == '.')
            {
            int fd = open(path, O_RDONLY);
            char* begin;
            char* p;

            begin = (char*)mmap(0, offset, PROT_READ, MAP_SHARED, fd, 0);
            p = begin + offset;

            while (p >= begin) // scan backwards
              {
              if (*((uint32_t*)p) == 0x04034b50UL) // local file header found
                {
                uint16_t length_ = *((uint16_t*)(p + 26));

                if (length + 2 + length_ < (int)sizeof(buffer))
                  {
                  memcpy(&buffer[length], "!/", 2);
                  memcpy(&buffer[length + 2], p + 30, length_);
                  length += 2 + length_;
                  }

                break;
                }

              p -= 4;
              }

            munmap(begin, offset);
            close(fd);
            }
#endif // __ANDROID__

          modulePath = resolved;
          break;
          }
        }
      }

    maps.close();

    if( length != -1 )
      {
      break;
      }
    }

#elif defined(__APPLE__)
  itkCommonPythonModule = "/_ITKCommonPython.so";

  Dl_info info;
  if (dladdr(__builtin_extract_return_addr(__builtin_return_address(0)), &info))
    {
    modulePath = itksys::SystemTools::GetRealPath( info.dli_fname );
    }

#else
    // Not yet supported
#   error "Platform not yet supported for ITK_WRAP_PYTHON=ON && BUILD_SHARED_LIBS=OFF"
#endif // Supported platforms

    if( !modulePath.empty() )
      {
      const std::string pythonExtensionPath = itksys::SystemTools::GetParentDirectory( modulePath );
      const std::string itkCommonPythonPath = pythonExtensionPath + itkCommonPythonModule;
      if( itkCommonPythonPath != modulePath &&
          itksys::SystemTools::FileExists( itkCommonPythonPath, true ) )
        {
        LibHandle lib = DynamicLoader::OpenLibrary( itkCommonPythonPath.c_str() );
        if( lib )
          {
          typedef itk::TimeStamp::GlobalTimeStampType * ( *ITK_GET_GLOBAL_TIMESTAMP_FUNCTION )();
          ITK_GET_GLOBAL_TIMESTAMP_FUNCTION getGlobalTimeStamp = ( ITK_GET_GLOBAL_TIMESTAMP_FUNCTION ) DynamicLoader::GetSymbolAddress( lib, "itkGetGlobalTimeStamp" );
          if( getGlobalTimeStamp )
            {
            itk::TimeStamp::GlobalTimeStampType * globalTimeStamp = ( *getGlobalTimeStamp )();
      std::cerr << "modulePath: " << modulePath << std::endl;
            TimeStamp::TimeStampInitialize( globalTimeStamp );
            return;
            }
          }
        }
      }

#endif // !ITK_BUILD_SHARED_LIBS && ITK_WRAP_PYTHON
  TimeStamp::TimeStampInitialize();
}


GlobalTimeStampInitializer
::~GlobalTimeStampInitializer()
{
  if(--Self::m_Count == 0)
    {
    TimeStamp::TimeStampFinalize();
    }
}

static GlobalTimeStampInitializer GlobalTimeStampInitializerInstance;
unsigned int GlobalTimeStampInitializer::m_Count;
AtomicInt< ModifiedTimeType > * TimeStamp::m_GlobalTimeStamp;
bool TimeStamp::m_UsedExistingGlobalTimeStamp;

} // end namespace itk
