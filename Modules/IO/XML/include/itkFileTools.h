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

#ifndef __itkFileTools_h
#define __itkFileTools_h

namespace itk
{

/**
 * \class FileTools
 * \brief This is a helper class to provide file/directory manipulations such as file creation, directory creation, etc.
 * \ingroup ITKIOXML
 */
class FileTools
{
public:
  /** Function to create the directory for a file, if it does not exist. */
  static void CreateDirectory( const char* fn );

  /** Function to create a file, if it does not exist. */
  static void CreateFile( const char* fn );
};

} // namespace itk

// here comes the implementation

#include "itkFancyString.h"
#include "itksys/SystemTools.hxx"
#include "itkExceptionObject.h"

namespace itk
{

/** Helper function to create the directory for a file, if it does not exist. */
inline void
FileTools::CreateDirectory( const char* fn )
{
  FancyString dir = itksys::SystemTools::GetFilenamePath( fn );
  // if dir is a file, directory cannot be created
  if ( itksys::SystemTools::FileExists( dir, true ) )
    {
    ExceptionObject eo( __FILE__, __LINE__, "directory cannot be created" );
    throw eo;
    }
  // if dir does exist, create it
  if ( !itksys::SystemTools::FileExists( dir, false ) )
    {
    itksys::SystemTools::MakeDirectory( dir );
    }
}

/** Helper function to create a file, if it does not exist. */
inline void
FileTools::CreateFile( const char* fn )
{
  if ( itksys::SystemTools::FileExists( fn, true ) )
    {
    return;
    }
  // create the directory, if necessary
  FileTools::CreateDirectory( fn );
  // create the file
  itksys::SystemTools::Touch( fn, true );
}

} // namespace itk

#endif // __itkFileTools_h
