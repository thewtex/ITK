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

#include "itkIOCommon.h"
#include <cstring>
#include "itksys/SystemTools.hxx"

bool CheckFileNameParsing(const std::string & fileName,
                          const std::string & correctNameOnly,
                          const std::string & correctExtension,
                          const std::string & correctPath)
{
  // the current kwsys way...
  std::cout << "(kwsys) Extracting...file name...";
  std::string nameOnly =
    itksys::SystemTools::GetFilenameWithoutLastExtension(itksys::SystemTools::GetFilenameName(fileName));

  std::cout << "extension...";
  std::string extension = itksys::SystemTools::GetFilenameLastExtension(fileName);
  if(extension.size() >= 1 && extension[0] == '.')
    {
    extension = extension.substr(1);
    }

  std::cout << "path...";
  std::string path = itksys::SystemTools::GetFilenamePath(fileName);
  path = itksys::SystemTools::ConvertToOutputPath(path);
  if(path.size() > 1)
    {
  #if defined(_WIN32)
      path += "\\";
  #else
      path += "/";
  #endif
    }
  std::cout << "DONE" << std::endl;

  std::cout << "Comparing...file name...";
  bool nameMatches;
  if ( nameOnly.empty() )
    {
    nameMatches = correctNameOnly.size() == 0;
    }
  else
    {
    nameMatches = correctNameOnly.compare(nameOnly) == 0;
    }

  std::cout << "extension...";
  bool extensionMatches;
  if ( extension.empty() )
    {
    extensionMatches = correctExtension.size() == 0;
    }
  else
    {
    extensionMatches = correctExtension.compare(extension) == 0;
    }

  std::cout << "path...";
  bool pathMatches;
  if ( path.empty() )
    {
    pathMatches = correctPath.size() == 0;
    }
  else
    {
    pathMatches = correctPath.compare(path) == 0;
    }
  std::cout << "DONE" << std::endl;

  std::cout << "FullFileName: \"" << fileName << "\"" << std::endl;
  std::cout << "FileName: (expected) \"" << correctNameOnly
            << "\" (actual) \"" << nameOnly
            << "\""
            << " (correct) " << nameMatches << std::endl;
  std::cout << "Extension: (expected) \"" << correctExtension
            << "\" (actual) \"" << extension
            << "\""
            << " (correct) " << extensionMatches << std::endl;
  std::cout << "Path: (expected) \"" << correctPath
            << "\" (actual) \"" << path
            << "\""
            << " (correct) " << pathMatches << std::endl;

  bool correctParse = nameMatches && extensionMatches && pathMatches;
  std::cout << "Parsing is " << (correctParse ? "correct" : "incorrect")
            << std::endl;

  // clean up
  std::cout << "DONE" << std::endl;

  return correctParse;
}

int itkIOCommonTest(int , char* [])
{
  bool success = true;

  //
  // reasonable cases
  //

#if defined(_WIN32)
  success = success &&
    CheckFileNameParsing("c:\\dir1\\dir2\\myfile.tar.gz",
                         "myfile.tar",
                         "gz",
                         "c:\\dir1\\dir2\\");
  success = success &&
    CheckFileNameParsing("\\\\sambaserver\\dir1\\dir2\\myfile.tar.gz",
                         "myfile.tar",
                         "gz",
                         "\\\\sambaserver\\dir1\\dir2\\");
#else
  success = success &&
    CheckFileNameParsing("/dir1/dir2/myfile.tar.gz",
                         "myfile.tar",
                         "gz",
                         "/dir1/dir2/");
#endif

  //
  // less reasonable cases
  //
  success = success &&
    CheckFileNameParsing(".", "", "", "");

#if defined(_WIN32)
  success = success &&
    CheckFileNameParsing("\\", "", "", "\\");
  success = success &&
    CheckFileNameParsing("\\.tar.gz", ".tar", "gz", "\\");
#else
  success = success &&
    CheckFileNameParsing("/", "", "", "/");
  success = success &&
    CheckFileNameParsing("/.tar.gz", ".tar", "gz", "/");
#endif

#if defined(_WIN32)
  success = success &&
    CheckFileNameParsing("\\.tar.gz", ".tar", "gz", "\\");
#endif

  success = success &&
    CheckFileNameParsing(".tar.gz", ".tar", "gz", "");

  success = success &&
    CheckFileNameParsing("myfile", "myfile", "", "");

#if defined(_WIN32)
  success = success &&
    CheckFileNameParsing("\\myfile", "myfile", "", "\\");
#else
  success = success &&
    CheckFileNameParsing("/myfile", "myfile", "", "/");
#endif

  success = success &&
    CheckFileNameParsing("", "", "", "");

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
