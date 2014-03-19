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
#ifndef __itkThreadSemaphorePair_h
#define __itkThreadSemaphorePair_h
#include "itkThreadPool.h"

namespace itk
{
/**
 * \class ThreadSemaphorePair
 * \brief Inner class to ThreadPool. Used to create a pair of ThreadID and semahore.
 */


  class ThreadPool::ThreadSemaphorePair
  {
public:
    ThreadSemaphorePair(const ThreadProcessIDType & tph);
    int SemaphoreWait();
    int SemaphorePost();
#if defined(__APPLE__)
      semaphore_t           m_Semaphore;
#elif defined(_WIN32) || defined(_WIN64)
    HANDLE               m_Semaphore;
#else
    sem_t                m_Semaphore;
#endif

    ThreadSemaphorePair *m_Next;
    ThreadProcessIDType  m_ThreadProcessHandle;

private:
    static int m_SemCount;
    ThreadSemaphorePair(); //purposefully not implemented.
  };
  }

#endif
