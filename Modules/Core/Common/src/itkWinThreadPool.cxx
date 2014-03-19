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
#include "itkWinThreadPool.h"

namespace itk
{
/*
HANDLE ThreadPool::m_MutexSync = CreateMutex(
  NULL,              // default security attributes
  FALSE,             // initially not owned
  NULL);

HANDLE ThreadPool::m_MutexWorkCompletion = CreateMutex(
  NULL,              // default security attributes
  FALSE,             // initially not owned
  NULL);

HANDLE ThreadPool::m_ActiveThreadMutex = CreateMutex(
  NULL,              // default security attributes
  FALSE,             // initially not owned
  NULL);
*/
bool                     ThreadPool::m_InstanceFlag = false;
SmartPointer<ThreadPool> ThreadPool::m_SThreadPoolInstance = 0;
SimpleFastMutexLock      ThreadPool::m_MutexSync;
SimpleFastMutexLock      ThreadPool::m_MutexWorkCompletion;
SimpleFastMutexLock      ThreadPool::m_ActiveThreadMutex;

SmartPointer<ThreadPool> ThreadPool::GetThreadPool()
{
  if( !m_InstanceFlag )
    {
    m_SThreadPoolInstance = ThreadPool::New();
    }
  return m_SThreadPoolInstance;
}

SmartPointer<ThreadPool> ThreadPool::GetNewThreadPool()
{
  m_SThreadPoolInstance = new ThreadPool();
  return m_SThreadPoolInstance;
}

ThreadPool::ThreadPool() : m_IdCounter(1), m_ThreadCount(0)
{
  m_InstanceFlag = true;
  m_CompletedJobs = 0;
  m_ScheduleForDestruction = false;
  m_IncompleteWork = 0;
  m_WorkAvailable = CreateSemaphore(
      NULL,       // default security attributes
      0,          // initial count
      INFINITE,   // maximum count
      NULL);      // unnamed semaphore
}

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

void ThreadPool::InitializeThreads(int maximumThreads)
{

  try
    {
    if( maximumThreads < 1 )
      {
      maximumThreads = 1;
      }
    for( int i = 0; i < maximumThreads; i++ )
      {
      AddThread();
      }
    }
  catch( std::exception& e )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(<<  std::endl << "Initialization failure\n" << e.what() << std::endl);
    }
  itkDebugMacro(<<  "Created thread pool with threads :" << maximumThreads << std::endl);
}

ThreadPool::~ThreadPool()
{

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
/*
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(
        m_MutexWorkCompletion, // handle to mutex
        INFINITE);             // no time-out interval

    switch( dwWaitResult )
      {
      // The thread got ownership of the mutex
      case WAIT_OBJECT_0:
*/
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexWorkCompletion);
      m_IncompleteWork++;

/*        // Release ownership of the mutex object
        if( !ReleaseMutex(m_MutexWorkCompletion) )
          {
          // Handle error.
          }
        break;
      // The thread got ownership of an abandoned mutex
      case WAIT_ABANDONED:
        itkExceptionMacro(<< "Abandoned mutex");
      }
*/
      }
    if( m_IncompleteWork > m_ThreadCount )
      {
      AddThread();
      }
    /*dwWaitResult = WaitForSingleObject(
        m_MutexSync, // handle to mutex
        INFINITE);   // no time-out interval

    switch( dwWaitResult )
      {
      // The thread got ownership of the mutex
      case WAIT_OBJECT_0:
      */
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexSync);
      winJob.Id = m_IdCounter++;
      m_WorkerQueue.push_back(winJob);
      itkDebugMacro(
        << "Assigning Worker[" << (winJob).Id << "] Address:[" << &winJob << "] to Queue " << std::endl);

      // Release ownership of the mutex object
      /*    if( !ReleaseMutex(m_MutexSync) )
            {
            // Handle error.
            }

          break;
        // The thread got ownership of an abandoned mutex
        case WAIT_ABANDONED:
          itkExceptionMacro(<<  "Abandoned mutex");
        }
  */
      }
    /*
      dwWaitResult = WaitForSingleObject(
          m_ActiveThreadMutex, // handle to mutex
          INFINITE);           // no time-out interval

      switch( dwWaitResult )
        {
        // The thread got ownership of the mutex
        case WAIT_OBJECT_0:
  */
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_ActiveThreadMutex);
      itkDebugMacro(<<  std::endl << "Adding id " << winJob.Id << " to m_ActiveThreadIds" << std::endl);
      m_ActiveThreadIds.push_back(winJob.Id);
      itkDebugMacro(<<  std::endl << "ActiveThreadids size : " << m_ActiveThreadIds.size() << std::endl);

      /*       // Release ownership of the mutex object
             if( !ReleaseMutex(m_ActiveThreadMutex) )
               {
               // Handle error.
               }
             break;

           case WAIT_ABANDONED:
             itkExceptionMacro(<<  "Abandoned mutex");
           }
     */
      }
    if( !ReleaseSemaphore(
          m_WorkAvailable, // handle to semaphore
          1,               // increase count by one
          NULL) )          // not interested in previous count
      {
      printf("ReleaseSemaphore error: %d\n", GetLastError() );
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

bool ThreadPool::WaitForThread(int id)
{

  bool found = true;

  try
    {
    while( found )
      {
        {
        MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_ActiveThreadMutex);
        /*
            We know that a thread id will be in here because the AssignWork fn adds it in
            So if a thread id is not found in m_ActiveThreadIds it means it is done.
            */
        found = false;
        for( std::vector<int>::size_type i = 0; i != m_ActiveThreadIds.size(); i++ )
          {

          if( m_ActiveThreadIds[i] == id )
            {
            found = true;

            }
          }
        }
      }

    return true;
    }
  catch( std::exception& e )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(
      << "Exception occured while waiting for job with id : " << id << std::endl << e.what() << std::endl);
    return false;
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

int ThreadPool::GetCompletedJobs() const
{
  return m_CompletedJobs;
}

void ThreadPool::RemoveActiveId(int id)
{
  try
    {
    itkDebugMacro(<<  std::endl << "ActiveThreadids size : " << m_ActiveThreadIds.size() << ". Removing id " << id
                  << std::endl);
    int index = -1;
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_ActiveThreadMutex);
      itkDebugMacro(<<  std::endl << "Looping" << std::endl);
      for( std::vector<int>::size_type i = 0; i != m_ActiveThreadIds.size(); i++ )
        {
        itkDebugMacro(<<  "Id is : " << m_ActiveThreadIds[i] << std::endl);
        if( m_ActiveThreadIds[i] == id )
          {
          index = i;
          break;
          }
        }

      if( index >= 0 )
        {
        m_CompletedJobs++;
        m_ActiveThreadIds.erase(m_ActiveThreadIds.begin() + index);
        itkDebugMacro(<<  std::endl << "Removed id " << id << " from ActiveThreadIds. Now vector size is "
                      << m_ActiveThreadIds.size() << std::endl);
        }
      else
        {

        itkDebugMacro(<<  std::endl << "Error occured, couldnt find id : " << id << std::endl);
        itkExceptionMacro(<<  "Error occured in RemoveActiveId, couldnt find id in ActiveThreadIds queue to erase.");
        }
      }
    bool foundToDelete = false;
    int  delIndex = -1;
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexSync);
      for( std::vector<int>::size_type i = 0; i != m_WorkerQueue.size(); i++ )
        {
        if( m_WorkerQueue[i].Id == id )
          {
          delIndex = i;
          foundToDelete = true;
          break;
          }
        }

      m_WorkerQueue.erase(m_WorkerQueue.begin() + delIndex);
      itkDebugMacro(<<  std::endl << "Removed index " << delIndex << " from m_WorkerQueue. Now vector size is "
                    << m_WorkerQueue.size() << std::endl);
      if( foundToDelete == false )
        {
        itkDebugMacro(
          << std::endl << "Error occured, couldnt find id in WorkerQueue to mark executed. Id is : " << id
          << std::endl);
        itkExceptionMacro(<<  "Error occured in RemoveActiveId, couldnt find id in m_WorkerQueue to mark executed. ");
        }
      }

    }
  catch( std::exception & e )
    {
    itkExceptionMacro(<<  e.what() );
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
