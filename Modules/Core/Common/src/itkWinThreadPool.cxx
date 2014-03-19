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
#include "itkThreadPool.h"

namespace itk
{

ThreadPool
::ThreadSemaphorePair
::ThreadSemaphorePair(const ThreadProcessIdType & tph) :
  m_ThreadProcessHandle(tph)
{

  m_Semaphore = CreateSemaphore(
    NULL,     // default security attributes
    0,        // initial count
    1000, // maximum count
    NULL);    // unnamed semaphore
  if (m_Semaphore == NULL)
    {
    itkGenericExceptionMacro(<< "CreateSemaphore error" << GetLastError());
    }
}


int
ThreadPool
::ThreadSemaphorePair
::SemaphoreWait()
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
  ThreadProcessIdentifiers::WinThreadIdType  dwThreadId;

  newlyAddedThreadHandle = CreateThread(
    NULL,
    0,
    (LPTHREAD_START_ROUTINE) ThreadPool::ThreadExecute,     // thread function
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
    m_ThreadProcessIdentifiersVector.push_back(ThreadProcessIdentifiers(JOB_THREADHANDLE_JUST_ADDED,
                                                                   newlyAddedThreadHandle,dwThreadId) );

    m_ThreadSemHandlePairingForWaitQueue.push_back(new ThreadSemaphorePair(newlyAddedThreadHandle));
    m_ThreadSemHandlePairingQueue.push_back(new ThreadSemaphorePair(newlyAddedThreadHandle));
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
::DeallocateThreadSemSet(ThreadSemHandlePairingQueueType &q)
{
  for (ThreadSemHandlePairingQueueType::iterator it = q.begin(), end = q.end();
       it != end; ++it)
    {
    CloseHandle((*it)->m_Semaphore);
    delete *it;
    }
  q.clear();
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
  DeallocateThreadSemSet(m_ThreadSemHandlePairingQueue);
  DeallocateThreadSemSet(m_ThreadSemHandlePairingForWaitQueue);

  //clear vectors
  //clear vectors
  m_ThreadProcessIdentifiersVector.clear();
  ThreadProcessIdentifiersVecType(m_ThreadProcessIdentifiersVector).swap(m_ThreadProcessIdentifiersVector);

  m_ThreadHandles.clear();
  ThreadProcessIdContainerType(m_ThreadHandles).swap(m_ThreadHandles);

  m_WorkerQueue.clear();
  ThreadJobContainerType(m_WorkerQueue).swap(m_WorkerQueue);
}

void *
ThreadPool
::ThreadExecute(void *param)
{
  // get the parameters passed inf
  //ThreadPool *winThreadPool = (ThreadPool *)param;
  //ThreadPool::ThreadArgStruct *threadStruct = (ThreadPool::ThreadArgStruct *)param;
  ThreadPool *winThreadPool = (ThreadPool *)param;
  ThreadIdType myId;
  ThreadProcessIdentifiers::WinThreadIdType threadId = GetCurrentThreadId();
  ThreadProcessIdentifiersVecType::iterator tpIter =
    winThreadPool->m_ThreadProcessIdentifiersVector.begin(),
    end = winThreadPool->m_ThreadProcessIdentifiersVector.end();
  bool threadIdFound(false);
  for (; tpIter != end; ++tpIter)
    {
    if (tpIter->m_WinThreadId == threadId)
      {
      myId = tpIter->m_ThreadNumericId;
      threadIdFound = true;
      break;
      }
    }
  if(!threadIdFound)
    {
    std::cerr << "Can't find thread ID " << threadId << std::endl
              << "candidates are:" << std::endl;
    for(tpIter = winThreadPool->m_ThreadProcessIdentifiersVector.begin(); tpIter != end; ++tpIter)
      {
      std::cerr << tpIter->m_WinThreadId << std::endl;
      }
    std::cerr.flush();
    abort();
    }
  HANDLE handle = winThreadPool->GetThreadHandleForThreadId(myId);

  while( !winThreadPool->m_ScheduleForDestruction )
    {
    const ThreadJob &currentWinJob = winThreadPool->FetchWork( handle);
    if( currentWinJob.m_Id < 0 || currentWinJob.m_Assigned == false )
      {
      itkDebugStatement(std::cerr << "In thread pool thread : Empty job returned from FetchWork so ignoring and continuing .."
                        << std::endl);
      continue;
      }
    currentWinJob.m_ThreadFunction(currentWinJob.m_UserData);
    winThreadPool->RemoveActiveId(currentWinJob.m_Id);
    //signal that current job is done
    if(winThreadPool->GetSemaphoreForThreadWait(handle )->SemaphorePost() != 0)
      {
      itkGenericExceptionMacro(<<"******************************Error in semaphore post");
      }
    }
  return 0;
}

}
