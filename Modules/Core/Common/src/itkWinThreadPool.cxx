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
  m_ThreadHandles.push_back(aThreadHandle);
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

  CloseHandle(m_WorkAvailable);
  //CloseHandle(m_MutexSync);
  //CloseHandle(m_MutexWorkCompletion);
  //CloseHandle(m_ActiveThreadMutex);

}

int ThreadPool::AssignWork(WinJob winJob)
{
  try
    {

      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexWorkCompletion);
      m_IncompleteWork++;

      }
    if( m_IncompleteWork > m_ThreadCount )
      {
      AddThread();
      }
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexSync);
      winJob.Id = m_IdCounter++;
      m_WorkerQueue.push_back(winJob);
      itkDebugMacro(
        << "Assigning Worker[" << (winJob).Id << "] Address:[" << &winJob << "] to Queue " << std::endl);

      }
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_ActiveThreadMutex);
      itkDebugMacro(<<  std::endl << "Adding id " << winJob.Id << " to m_ActiveThreadIds" << std::endl);
      m_ActiveThreadIds.push_back(winJob.Id);
      itkDebugMacro(<<  std::endl << "ActiveThreadids size : " << m_ActiveThreadIds.size() << std::endl);

      }
    if( !ReleaseSemaphore(
          m_WorkAvailable, // handle to semaphore
          1,               // increase count by one
          NULL) )          // not interested in previous count
      {
      //error
      }

    return (winJob).Id;
    }
  catch( std::exception& e )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(<<  std::endl << "Failed to assign work. \n" << e.what() << std::endl);
    itkExceptionMacro(<<  e.what() );
    }
}

WinJob ThreadPool::FetchWork()
{

  int    indexToReturn = -100;
  DWORD  dwWaitResult;
  WinJob ret;

  dwWaitResult = WaitForSingleObject(
      m_WorkAvailable,   // handle to semaphore
      INFINITE);

    {
    MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexSync);
    // get the index of workerthread from queue
    for( std::vector<int>::size_type i = 0; i != m_WorkerQueue.size(); i++ )
      {
      // if not Assigned already
      if( m_WorkerQueue[i].Assigned == false )
        {
        indexToReturn = i;
        m_WorkerQueue[i].Assigned = true;
        break;
        }
      }
    itkDebugMacro(<<  std::endl << "Getting work from queue at index : " << indexToReturn << std::endl);
    ret = m_WorkerQueue[indexToReturn];
    return ret;
    }
}

//we need to wait here through a semaphore because the thread executing the job
// has to set the map we are accessing
ThreadProcessIDType ThreadPool::GetThreadHandleForJob(int jobId){

  dwWaitResult = WaitForSingleObject(
      m_JobSemaphores[jobId],   // handle to semaphore
      INFINITE);

    {
    MutexLockHolder<SimpleFastMutexLock> threadIdMutexHolder(m_ThreadIdAccessMutex);
    int                                  pos = std::find(m_JobIds.begin(), m_JobIds.end(), jobId) - m_JobIds.begin();

    return m_ThreadIds[pos];
    }

}

void * ThreadPool::ThreadExecute(void *param)
{
  // get the parameters passed in
  ThreadPool *winThreadPool = (ThreadPool *)param;

  while( !winThreadPool->m_ScheduleForDestruction )
    {
    try
      {
      WinJob winJob = winThreadPool->FetchWork();
      //add the thread id and job id to map
        {
        MutexLockHolder<SimpleFastMutexLock> threadIdMutexHolder(m_ThreadIdAccessMutex);
        winThreadPool->m_ThreadIds.push_back(pthread_self() );
        winThreadPool->m_JobIds.push_back(currentPThreadJob.Id);
        }
      if( !ReleaseSemaphore(
            winThreadPool->m_JobSemaphores[currentPThreadJob.Id], // handle to
                                                                  // semaphore
            1,                                                    // increase
                                                                  // count by
                                                                  // one
            NULL) )                                               // not
                                                                  // interested
                                                                  // in previous
                                                                  // count
        {
        //error
        }

      winJob.ThreadFunction(winJob.ThreadArgs.otherArgs);
      winThreadPool->RemoveActiveId(winJob.Id);
        {
        MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexWorkCompletion);
        winThreadPool->m_IncompleteWork--;

        }
      }
    catch( std::exception& e )
      {
      winThreadPool->m_ExceptionOccured = true;
      }

    }

  return 0;
}

}
