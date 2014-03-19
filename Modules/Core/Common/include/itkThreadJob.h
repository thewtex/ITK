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
#ifndef __itkThreadJob_h__
#define __itkThreadJob_h__

namespace itk
{

class ThreadJob
{
public:
  /** Stores the user's data that needs to be passed into the function */
  struct ThreadArgs
    {
    void *otherArgs;
    };

  /** This is the Job's id */
  int Id;

  /** Set if the job is assigned to a thread */
  bool Assigned;

  /**  set if job is finished */
  bool Executed;

  /** Declaring function thatwill be called */
#if defined(ITK_USE_WIN32_THREADS)
  unsigned int ( __stdcall *ThreadFunction )( void * ptr );
#else
  void * (*ThreadFunction)(void *ptr);
#endif

 /** declaring the struct */
  ThreadArgs ThreadArgs;

  ThreadJob() :
    Id(-1),
    Assigned(false),
    Executed(false)
  {
    ThreadArgs.otherArgs = NULL;
  }

  ~ThreadJob()
  {
  }

};

} // End namespace itk

#endif // __itkPThreadJob_h__
