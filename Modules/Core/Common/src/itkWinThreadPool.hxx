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
#ifndef __itkWinThreadPool_hxx
#define __itkWinThreadPool_hxx
#include "itkThreadPool.h"

namespace itk
{

ThreadPool
::ThreadSemaphorePair::ThreadSemaphorePair(const ThreadProcessIdType & tph) :
  m_ThreadProcessHandle(tph)
{

  m_Semaphore = CreateSemaphore(
    NULL,     // default security attributes
    0,        // initial count
    1000, // maximum count
    NULL);    // unnamed semaphore
  if (m_Semaphore == NULL)
    {
    itkExceptionMacro(<< "CreateSemaphore error" << GetLastError());
    }
}


int
ThreadPool
::ThreadSemaphorePair::SemaphoreWait()
{
  DWORD dwWaitResult = WaitForSingleObject(m_Semaphore,       // handle to semaphore
                                           INFINITE);
  switch( dwWaitResult )
    {
    // The thread got ownership of the mutex
    case WAIT_OBJECT_0:
      return 0; //success
      break;
    case WAIT_ABANDONED:
      return -1;
    case WAIT_FAILED:
      return -1;
    default:
      return -1;
    }
}

int
ThreadPool
::ThreadSemaphorePair::SemaphorePost()
{
  if(!ReleaseSemaphore(
       m_Semaphore,   // handle to semaphore
       1,               // increase count by one
       NULL))
    {
    return -1;
    }
  return 0;
}


void
ThreadPool
::AddThread()
{
  m_ThreadCount++;
  HANDLE newlyAddedThreadHandle;
  DWORD  dwThreadId = (DWORD) m_ThreadCount;

  newlyAddedThreadHandle = CreateThread(
    NULL,
    0,
    (LPTHREAD_START_ROUTINE) ThreadPool
    ::ThreadExecute,     // thread function
    this,
    0,
    &dwThreadId);
  if( newlyAddedThreadHandle == NULL )
    {
    itkDebugMacro(<< "ERROR; adding thread to thread pool");
    itkExceptionMacro(<< "Cannot create thread.");
    }
  else
    {

    m_ThreadHandles.insert(newlyAddedThreadHandle);
    m_ThreadIdHandlePairingQueue.push_back(ThreadProcessStatusPair(JOB_THREADHANDLE_JUST_ADDED,
                                                                   newlyAddedThreadHandle) );

    //first time
    if(m_ThreadSemHandlePairingForWaitQueue == NULL)
      {
      m_ThreadSemHandlePairingForWaitQueue = new ThreadSemaphorePair(newlyAddedThreadHandle);
      m_ThreadSemHandlePairingForWaitQueue->m_Next = NULL;
      }
    else
      {
      AddNodeToTSPairLinkedList(m_ThreadSemHandlePairingForWaitQueue, newlyAddedThreadHandle);
      }
    //first time
    if(m_ThreadSemHandlePairingQueue == NULL)
      {
      m_ThreadSemHandlePairingQueue = new ThreadSemaphorePair(newlyAddedThreadHandle);
      m_ThreadSemHandlePairingQueue->m_Next = NULL;
      }
    else
      {
      AddNodeToTSPairLinkedList(m_ThreadSemHandlePairingQueue, newlyAddedThreadHandle);
      }

    if (m_ThreadIdHandleList == NULL)
      {
      m_ThreadIdHandleList = new ThreadIdHandlePair(newlyAddedThreadHandle, dwThreadId);
      m_ThreadIdHandleList->m_Next = NULL;
      }
    else
      {
      AddNodeToThreadIdHandleList(m_ThreadIdHandleList, newlyAddedThreadHandle, dwThreadId);
      }

    }

}

bool
ThreadPool
::CompareThreadHandles(ThreadProcessIdType t1, ThreadProcessIdType t2)
{
  if (t1 == t2) return true;
  return false;
}

void
ThreadPool
::DeallocateThreadLinkedList(ThreadSemaphorePair *list)
{
  ThreadSemaphorePair *node = list;
  ThreadSemaphorePair *m_Next = list;

  while(node!=NULL)
    {
    m_Next = node->m_Next;
    CloseHandle(node->m_Semaphore);
    delete node;
    node = m_Next;
    }
}

/*
This method first checks if all threads are finished executing their jobs.
If now, it will wait for "poolTime" seconds and then re-check.
Once all are done it will Destroy the thread pool.
*/
void
ThreadPool
::DestroyPool(int pollTime = 2)
{
  m_ScheduleForDestruction = true;
  while( m_NumberOfPendingJobsToBeRun > 0 )
    {
    itkDebugMacro(<<  "Work is still incomplete=" << m_NumberOfPendingJobsToBeRun << std::endl);
    Sleep(pollTime);
    }

  itkDebugMacro(<<  "All threads are done" << std::endl << "Terminating threads" << std::endl);
  for(ThreadProcessIdContainerType::iterator it = this->m_ThreadHandles.begin();
      it != this->m_ThreadHandles.end(); ++it)
    {
    DWORD dwExit = 0;
    TerminateThread((*it), dwExit);
    CloseHandle((*it));

    }
  DeallocateThreadLinkedList(m_ThreadSemHandlePairingQueue);
  DeallocateThreadLinkedList(m_ThreadSemHandlePairingForWaitQueue);

  //clear vectors
  m_ThreadIdHandlePairingQueue.clear();
  std::vector<ThreadPool
              ::ThreadProcessStatusPair>(m_ThreadIdHandlePairingQueue).swap(m_ThreadIdHandlePairingQueue);

  m_ThreadHandles.clear();
  ThreadProcessIdContainerType(m_ThreadHandles).swap(m_ThreadHandles);

  m_ActiveJobIds.clear();
  std::vector<int>(m_ActiveJobIds).swap(m_ActiveJobIds);

  m_WorkerQueue.clear();
  std::vector<ThreadJob>(m_WorkerQueue).swap(m_WorkerQueue);

}

HANDLE
ThreadPool::
GetThreadHandleForThreadId(ThreadIdType id)
{

  MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIdHandlePairingQueueMutex);
  ThreadIdHandlePair *                node = m_ThreadIdHandleList;
  do
    {
    if(node->m_Id == id)
      {
      return node->m_ThreadProcessHandle;
      }
    node = node->m_Next;
    } while(node!= NULL);

  itkExceptionMacro(<< "Error occured finding thread handle for thread id " << id);
}

void *
ThreadPool
::ThreadExecute(void *param)
{
  // get the parameters passed in
  //ThreadPool *winThreadPool = (ThreadPool *)param;
  //ThreadPool::ThreadArgStruct *threadStruct = (ThreadPool::ThreadArgStruct *)param;
  ThreadPool *winThreadPool = (ThreadPool *)param;
  ThreadIdType myId = GetThreadId(GetCurrentThread());//threadStruct->MyId;
  HANDLE handle = winThreadPool->GetThreadHandleForThreadId(myId);

  while( !winThreadPool->m_ScheduleForDestruction )
    {
    ThreadJob currentWinJob = winThreadPool->FetchWork( handle);
    if( currentWinJob.m_Id < 0 || currentWinJob.m_Assigned == false )
      {
      itkDebugMacro(<< "In thread pool thread : Empty job returned from FetchWork so ignoring and continuing ..\n");
      continue;
      }
    currentWinJob.m_ThreadFunction(currentWinJob.m_UserData);
    winThreadPool->RemoveActiveId(currentWinJob.m_Id);
    //signal that current job is done
    if(winThreadPool->GetSemaphoreForThreadWait(handle )->SemaphorePost() != 0)
      {
      itkExceptionMacro(<<"******************************Error in semaphore post");
      }
    }
  return 0;
}

}
#endif // __itkWinThreadPool_hxx
