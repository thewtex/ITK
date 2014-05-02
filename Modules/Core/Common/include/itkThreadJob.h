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
  ThreadJob() :
    m_Id(-1),
    m_Assigned(false),
    m_Executed(false),
    m_ThreadFunction(NULL)
  {
    m_ThreadArgs.otherArgs = NULL;
  }

  ~ThreadJob() {}

//  private:
  /** Stores the user's data that needs to be passed into the function */
  struct ThreadArgs
    {
    void *otherArgs;
    };

  /** This is the Job's id. If it is -1 it means the job hasn't been
    initialized*/
  int m_Id;

  /** Set if the job is assigned to a thread */
  bool m_Assigned;

  /**  set if job is finished */
  bool m_Executed;

  /** Declaring function thatwill be called */
#if defined(ITK_USE_WIN32_THREADS)
  unsigned int ( __stdcall *m_ThreadFunction )( void * ptr );
#else
  void * (*m_ThreadFunction)(void *ptr);
#endif

  /** declaring the struct */
  ThreadArgs m_ThreadArgs;


};

} // End namespace itk

#endif // __itkThreadJob_h__
