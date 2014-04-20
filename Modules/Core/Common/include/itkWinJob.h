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

#ifndef __itkWinJob_h
#define __itkWinJob_h
#if defined(_WIN32) || defined(_WIN64)

namespace itk
{

class WinJob
{
public:
  struct ThreadArgs
    {

    void *otherArgs;
    };
  int    Id;
  bool   Assigned;
  bool   Executed;
  int    Ptid;
  void * UserData;    // any data user wants the function to use

  unsigned int ( __stdcall *ThreadFunction )( void * ptr );

  ThreadArgs ThreadArgs;
  WinJob() : Assigned(false),
    Id(-1),
    Executed(false),
    Ptid(-1),
    UserData(NULL)
  {
    ThreadArgs.otherArgs = NULL;
  }

  ~WinJob()
  {
  }

};
}
#endif
#endif
