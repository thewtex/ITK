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
#ifndef __itkPThreadJob_h__
#define __itkPThreadJob_h__
#ifdef __unix__
#include <pthread.h>
#include <iostream>
#include "itkThreadPoolDebug.h"

namespace itk
{
/** \class PThreadJob
 * TODO:  This needs to be filled out
*/

class ThreadJob
{
public:
  struct ThreadArgs
    {
    void *otherArgs;
    };
  int       Id;
  bool      Assigned;
  bool      Executed;
  pthread_t Ptid;
  void *    UserData; // any data user wants the function to use

  void *     (*ThreadFunction)(void *ptr);
  pthread_t  ThreadHandle;
  ThreadArgs ThreadArgs;

  ThreadJob() :
    Id(-1),
    Assigned(false),
    Executed(false),
    Ptid(-1),
    UserData(NULL),
    ThreadHandle(-1)
  {
    ThreadArgs.otherArgs = NULL;
    THREAD_DIAGNOSTIC_PRINT(  "Starting thread \t address=" << this << std::endl );
  }

  ~ThreadJob()
  {
    THREAD_DIAGNOSTIC_PRINT( std::endl << "Thread finished. pid is  " << Ptid << "\t address=" << this << std::endl );
  }

};

} // End namespace itk

#endif
#endif // __itkPThreadJob_h__

