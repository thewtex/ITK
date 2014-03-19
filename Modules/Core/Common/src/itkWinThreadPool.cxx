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

#include <stdlib.h>
#include "itkThreadPool.h"

namespace itk
{

void ThreadPool::AddThread()
{
  this->m_ThreadCount++;
  HANDLE aThreadHandle;
  DWORD  dwThreadId;
  aThreadHandle = CreateThread(
      NULL,                                                   // default
                                                              // security
                                                              // attributes
      0,                                                      // use default
                                                              // stack size
      (LPTHREAD_START_ROUTINE) ThreadPool::ThreadExecute,     // thread function
                                                              // name
      this,                                                   // argument to
                                                              // thread function
      0,                                                      // use default
                                                              // creation flags
      &dwThreadId);                                           // returns the
                                                              // thread
                                                              // identifier
  if( aThreadHandle == NULL )
    {
    ExitProcess(3);
    }
    {
    MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_VectorQMutex);
    m_ThreadHandles.push_back(aThreadHandle);
    m_ThreadStructs.push_back(std::make_pair(-2,aThreadHandle) );
    }
  itkDebugMacro(<< "Thread created with handle :" << m_ThreadHandles[m_ThreadCount - 1] << std::endl );
}

/*
This method first checks if all threads are finished executing their jobs.
If now, it will wait for "poolTime" seconds and then re-check.
Once all are done it will Destroy the thread pool.
*/
void ThreadPool::DestroyPool(int pollTime = 2)
{
  m_ScheduleForDestruction = true;
  while( m_IncompleteWork > 0 )
    {
    itkDebugMacro(<<  "Work is still incomplete=" << m_IncompleteWork << std::endl);
    Sleep(pollTime);
    }

  itkDebugMacro(<<  "All threads are done" << std::endl << "Terminating threads" << std::endl);
  for( int i = 0; i < m_ThreadCount; i++ )
    {
    DWORD dwExit = 0;
    TerminateThread(m_ThreadHandles[i], dwExit);
    CloseHandle(m_ThreadHandles[i]);

    }
}

void * ThreadPool::ThreadExecute(void *param)
{
  // get the parameters passed in
  ThreadPool *winThreadPool = (ThreadPool *)param;

  while( !winThreadPool->m_ScheduleForDestruction )
    {

    ThreadJob currentWinJob = winThreadPool->FetchWork(GetCurrentThread() );
    if(currentWinJob.Id < 0 || currentWinJob.Assigned == false)
      {
      std::cout<<"\nIn thread pool thread : Empty job returned from FetchWork so ignoring and continuing ..\n";
      continue;
      }
    std::cout<<"\n In thread pool thread "<<GetCurrentThread()<<" : Work fetched. Job id is : "<<currentWinJob.Id<<
      std::endl;
    currentWinJob.ThreadFunction(currentWinJob.ThreadArgs.otherArgs);
    std::cout<<"\n Thread done with job id :"<<currentWinJob.Id<<"\n Now removing...\n\n";
    winThreadPool->RemoveActiveId(currentWinJob.Id);
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexWorkCompletion);
      winThreadPool->m_IncompleteWork--;
      }

    }

  return 0;
}

}
