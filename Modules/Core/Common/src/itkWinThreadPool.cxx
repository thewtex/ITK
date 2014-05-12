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
  HANDLE newlyAddedThreadHandle;
  DWORD  dwThreadId;
  newlyAddedThreadHandle = CreateThread(
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
  if( newlyAddedThreadHandle == NULL )
    {
    itkDebugMacro(<< "ERROR; adding thread to thread pool");
    itkExceptionMacro(<< "Cannot create thread.");
    }
  else
    {

    m_ThreadHandles.push_back(newlyAddedThreadHandle);
    m_ThreadIDHandlePairingQueue.push_back(ThreadProcessStatusPair(JOB_THREADHANDLE_JUST_ADDED,
                                                                   newlyAddedThreadHandle) );

    //first time
    if(m_ThreadSemHandlePairingForWaitQueue == NULL)
      {
      m_ThreadSemHandlePairingForWaitQueue = new ThreadSemaphorePair(newlyAddedThreadHandle);
      m_ThreadSemHandlePairingForWaitQueue->Next = NULL;
      }
    else
      {
      AddNodeToTSPariLinkedList(m_ThreadSemHandlePairingForWaitQueue, newlyAddedThreadHandle);
      }
    //first time
    if(m_ThreadSemHandlePairingQueue == NULL)
      {
      m_ThreadSemHandlePairingQueue = new ThreadSemaphorePair(newlyAddedThreadHandle);
      m_ThreadSemHandlePairingQueue->Next = NULL;
      }
    else
      {
      AddNodeToTSPariLinkedList(m_ThreadSemHandlePairingQueue, newlyAddedThreadHandle);
      }

    itkDebugMacro(<< "Thread created with handle :" << m_ThreadHandles[m_ThreadCount - 1] << std::endl );
    }

}

bool ThreadPool::CompareThreadHandles(ThreadProcessIDType t1, ThreadProcessIDType t2)
{
  return (t1 == t2 ? true : false);
}

/*
This method first checks if all threads are finished executing their jobs.
If now, it will wait for "poolTime" seconds and then re-check.
Once all are done it will Destroy the thread pool.
*/
void ThreadPool::DestroyPool(int pollTime = 2)
{
  m_ScheduleForDestruction = true;
  while( m_NumberOfPendingJobsToBeRun > 0 )
    {
    itkDebugMacro(<<  "Work is still incomplete=" << m_NumberOfPendingJobsToBeRun << std::endl);
    Sleep(pollTime);
    }

  itkDebugMacro(<<  "All threads are done" << std::endl << "Terminating threads" << std::endl);
  for( int i = 0; i < m_ThreadCount; i++ )
    {
    DWORD dwExit = 0;
    TerminateThread(m_ThreadHandles[i], dwExit);
    CloseHandle(m_ThreadHandles[i]);

    }
  DeallocateThreadLinkedList(m_ThreadSemHandlePairingQueue);
  DeallocateThreadLinkedList(m_ThreadSemHandlePairingForWaitQueue);

  //clear vectors
  m_ThreadIDHandlePairingQueue.clear();
  std::vector<ThreadPool::ThreadProcessStatusPair>(m_ThreadIDHandlePairingQueue).swap(m_ThreadIDHandlePairingQueue);

  m_ThreadHandles.clear();
  std::vector<ThreadProcessIDType>(m_ThreadHandles).swap(m_ThreadHandles);

  m_ActiveJobIds.clear();
  std::vector<int>(m_ActiveJobIds).swap(m_ActiveJobIds);

  m_WorkerQueue.clear();
  std::vector<ThreadJob>(m_WorkerQueue).swap(m_WorkerQueue);

}

void * ThreadPool::ThreadExecute(void *param)
{
  // get the parameters passed in
  ThreadPool *winThreadPool = (ThreadPool *)param;

  while( !winThreadPool->m_ScheduleForDestruction )
    {

    ThreadJob currentWinJob = winThreadPool->FetchWork(GetCurrentThread() );
    if( currentWinJob.Id < 0 || currentWinJob.Assigned == false )
      {
      ThreadDebugMsg( << "\nIn thread pool thread : Empty job returned from FetchWork so ignoring and continuing ..\n");
      continue;
      }
    ThreadDebugMsg(
        << "\n In thread pool thread " << GetCurrentThread() << " : Work fetched. Job id is : " << currentWinJob.Id <<
      std::endl);
    currentWinJob.ThreadFunction(currentWinJob.ThreadArgs.otherArgs);
    ThreadDebugMsg( << "\n Thread done with job id :" << currentWinJob.Id << "\n Now removing...\n\n");
    winThreadPool->RemoveActiveId(currentWinJob.Id);
    //signal that current job is done
    if(winThreadPool->GetSemaphoreForThreadWait(pthread_self() )->SemaphorePost()!=0)
      {
      ThreadDebugMsg(<<"******************************Error in semaphore post");
      }

    }

  return 0;
}

}
