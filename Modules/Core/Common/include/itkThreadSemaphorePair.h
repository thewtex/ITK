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
 #if defined(__unix__) || defined(__APPLE__)
 *=========================================================================*/
#ifndef __itkThreadSemPair_h
#define __itkThreadSemPair_h
#include "itkThreadPool.h"

namespace itk
{
/**
 * This class is used to submit jobs to the thread pool.
 * The thread job maintains important information of the submitted job
 * such as Job Id, information to identify if the job has finished executing.
 * It holds the function pointer that the user sets to the function the
 * user wants to be executed in parallel by the thread pool.
 * Also holds the args pointer - it is passed to the executing function by
 * the thread pool.
 */


  class ThreadPool::ThreadSemaphorePair
  {
public:
    ThreadSemaphorePair(const ThreadProcessIDType & tph);
    int SemaphoreWait();
    int SemaphorePost();
#if defined(__APPLE__)
//    sem_t                *Semaphore;
      semaphore_t           Semaphore;
#elif defined(_WIN32) || defined(_WIN64)
    HANDLE               Semaphore;
#else
    sem_t                Semaphore;
#endif

    ThreadSemaphorePair *Next;
    ThreadProcessIDType  ThreadProcessHandle;

private:
    static int m_SemCount;
    ThreadSemaphorePair(); //purposefully not implemented.
  };
  }

#endif
