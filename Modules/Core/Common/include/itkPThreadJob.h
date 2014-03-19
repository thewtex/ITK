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
/*
 * This class is used to submit jobs to the thread pool.
 * The thread job maintains important information of the submitted job
 * such as Job Id, information to identify if the job has finished executing.
 * It holds the function pointer that the user sets to the function the
 * user wants to be executed in parallel by the thread pool.
 * Also holds the args pointer - it is passed to the executing function by
 * the thread pool.
 */
#ifndef __itkPThreadJob_h__
#define __itkPThreadJob_h__
//#if defined(__unix__) || defined(__APPLE__)
#include <pthread.h>

namespace itk
{

class ThreadJob
{
public:
  struct ThreadArgs
    {
    void *otherArgs;
    };
  int    Id;
  bool   Assigned;
  bool   Executed;
  void * UserData;    // any data user wants the function to use

  void *     (*ThreadFunction)(void *ptr);
  ThreadArgs ThreadArgs;

  ThreadJob() :
    Id(-1),
    Assigned(false),
    Executed(false),
    UserData(NULL)
  {
    ThreadArgs.otherArgs = NULL;
  }

  ~ThreadJob()
  {
  }

};

} // End namespace itk

#endif // __itkPThreadJob_h__
