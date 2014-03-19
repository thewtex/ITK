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
HANDLE ThreadPool:: MutexSync = CreateMutex(
    NULL,              // default security attributes
    FALSE,             // initially not owned
    NULL);
HANDLE ThreadPool:: MutexWorkCompletion = CreateMutex(
    NULL,              // default security attributes
    FALSE,             // initially not owned
    NULL);
HANDLE ThreadPool:: ActiveThreadMutex = CreateMutex(
    NULL,              // default security attributes
    FALSE,             // initially not owned
    NULL);

ThreadPool * ThreadPool:: ThreadPoolInstance = 0;
bool ThreadPool::         InstanceFlag = false;
SmartPointer<ThreadPool> ThreadPool:: SThreadPoolInstance = 0;

ThreadPool & ThreadPool::GetPoolInstance()
{
  if( !InstanceFlag )
    {
    ThreadPoolInstance = new ThreadPool();
    ThreadPoolInstance->InitializeThreads(2);
    }
  return *ThreadPoolInstance;
}

ThreadPool & ThreadPool::GetPoolInstance(int maxThreads)
{
  if( !InstanceFlag )
    {
    ThreadPoolInstance = new ThreadPool();
    ThreadPoolInstance->InitializeThreads(maxThreads);
    }
  return *ThreadPoolInstance;
}

SmartPointer<ThreadPool> ThreadPool::GetSmartPoolInstance(int maxThreads)
{
  SThreadPoolInstance = new ThreadPool();
  SThreadPoolInstance->InitializeThreads(maxThreads);    
  return SThreadPoolInstance;
}

ThreadPool::ThreadPool() : IdCounter(1)
{
  CompletedJobs = 0;
  InstanceFlag = true;
}

void ThreadPool::InitializeThreads(int maximumThreads)
{

  try
    {
    if( maximumThreads < 1 )
      {
      maximumThreads = 1;
      }
    this->MaxThreads = maximumThreads;
    this->QueueSize = maximumThreads;
    Destroy = false;

    IncompleteWork = 0;
    WorkAvailable = CreateSemaphore(
        NULL,       // default security attributes
        0,          // initial count
        MaxThreads, // maximum count
        NULL);      // unnamed semaphore
    Destroy = false;
    ThreadHandles = new HANDLE[MaxThreads];
    DWORD *dwThreadIdArray = new DWORD[MaxThreads];
    for( int i = 0; i < MaxThreads; i++ )
      {
      // int rc;
      // rc = pthread_create(&ThreadHandles[i], NULL,
      // &ThreadPool::ThreadExecute, (void *)this );
      ThreadHandles[i] = CreateThread(
          NULL,                                               // default
                                                              // security
                                                              // attributes
          0,                                                  // use default
                                                              // stack size
          (LPTHREAD_START_ROUTINE) ThreadPool::ThreadExecute, // thread function
                                                              // name
          this,                                               // argument to
                                                              // thread function
          0,                                                  // use default
                                                              // creation flags
          &dwThreadIdArray[i]);                               // returns the
                                                              // thread
                                                              // identifier
      THREAD_DIAGNOSTIC_PRINT( "Thread createdd with ptid :" << ThreadHandles[i] << std::endl);
      if( ThreadHandles[i] == NULL )
        {
        // ErrorHandler(TEXT("CreateThread"));
        ExitProcess(3);
        }

      }

    }
  catch( std::exception& e )
    {
    ExceptionOccured = true;
    THREAD_DIAGNOSTIC_PRINT( std::endl << "Initialization failure\n" << e.what() << std::endl);
    }
  THREAD_DIAGNOSTIC_PRINT( "Created thread pool with threads :" << maximumThreads << std::endl);
}

// Can call if needed to delete the created singleton instance
void ThreadPool::DeleteInstance()
{
  try
    {
    if( InstanceFlag == true )
      {
      THREAD_DIAGNOSTIC_PRINT( "Deleting instance" << std::endl);
      InstanceFlag = false;
      ThreadPoolInstance->DestroyPool(2);
      ThreadPoolInstance->WorkerQueue.clear();

      delete[] ThreadPoolInstance->ThreadHandles;

      delete ThreadPoolInstance;

      THREAD_DIAGNOSTIC_PRINT( "Destroyed" << std::endl);

      }
    }
  catch( std::exception& e )
    {
    THREAD_DIAGNOSTIC_PRINT( std::endl << "Cannot delete instance \n" << e.what() << std::endl);
    ThreadPoolInstance->ExceptionOccured = true;
    }
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
  Destroy = true;
  while( IncompleteWork > 0 )
    {
    THREAD_DIAGNOSTIC_PRINT( "Work is still incomplete=" << IncompleteWork << std::endl);
    Sleep(pollTime);
    }

  THREAD_DIAGNOSTIC_PRINT( "All threads are done" << std::endl << "Terminating threads" << std::endl);
  for( int i = 0; i < MaxThreads; i++ )
    {
    // int s = pthread_cancel(ThreadHandles[i]);
    DWORD dwExit = 0;
    TerminateThread(ThreadHandles[i], dwExit);
    THREAD_DIAGNOSTIC_PRINT(
      "Thread with thread handle " << ThreadHandles[i] << " exited with exit code : " << dwExit << std::endl);
    CloseHandle(ThreadHandles[i]);

    }

  CloseHandle(WorkAvailable);
  CloseHandle(MutexSync);
  CloseHandle(MutexWorkCompletion);
  CloseHandle(ActiveThreadMutex);

}

int ThreadPool::AssignWork(WinJob winJob)
{
  try
    {
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(
        MutexWorkCompletion, // handle to mutex
        INFINITE);           // no time-out interval

    // pthread_mutex_lock(&MutexWorkCompletion);
    switch( dwWaitResult )
      {
      // The thread got ownership of the mutex
      case WAIT_OBJECT_0:
        IncompleteWork++;

        // Release ownership of the mutex object
        if( !ReleaseMutex(MutexWorkCompletion) )
          {
          // Handle error.
          }
        break;
      // The thread got ownership of an abandoned mutex
      case WAIT_ABANDONED:
        throw "Abandoned mutex";
      }
    dwWaitResult = WaitForSingleObject(
        MutexSync,  // handle to mutex
        INFINITE);  // no time-out interval

    // pthread_mutex_lock(&MutexWorkCompletion);
    switch( dwWaitResult )
      {
      // The thread got ownership of the mutex
      case WAIT_OBJECT_0:
        winJob.Id = IdCounter++;
        WorkerQueue.push_back(winJob);
        THREAD_DIAGNOSTIC_PRINT(
          "Assigning Worker[" << (winJob).Id << "] Address:[" << &winJob << "] to Queue " << std::endl);

        // Release ownership of the mutex object
        if( !ReleaseMutex(MutexSync) )
          {
          // Handle error.
          }

        break;
      // The thread got ownership of an abandoned mutex
      case WAIT_ABANDONED:
        throw "Abandoned mutex";
      }

    dwWaitResult = WaitForSingleObject(
        ActiveThreadMutex, // handle to mutex
        INFINITE);         // no time-out interval

    // pthread_mutex_lock(&MutexWorkCompletion);
    switch( dwWaitResult )
      {
      // The thread got ownership of the mutex
      case WAIT_OBJECT_0:

        THREAD_DIAGNOSTIC_PRINT( std::endl << "Adding id " << winJob.Id << " to ActiveThreadIds" << std::endl);
        ActiveThreadIds.push_back(winJob.Id);
        THREAD_DIAGNOSTIC_PRINT( std::endl << "ActiveThreadids size : " << ActiveThreadIds.size() << std::endl);

        // Release ownership of the mutex object
        if( !ReleaseMutex(ActiveThreadMutex) )
          {
          // Handle error.
          }
        break;

      case WAIT_ABANDONED:
        throw "Abandoned mutex";
      }

    if( !ReleaseSemaphore(
          WorkAvailable, // handle to semaphore
          1,             // increase count by one
          NULL) )        // not interested in previous count
      {
      printf("ReleaseSemaphore error: %d\n", GetLastError() );
      }

    return (winJob).Id;
    }
  catch( std::exception& e )
    {
    ExceptionOccured = true;
    THREAD_DIAGNOSTIC_PRINT( std::endl << "Failed to assign work. \n" << e.what() << std::endl);
    throw e;
    }
}

bool ThreadPool::WaitForThread(int id)
{

  bool  found = true;
  DWORD dwWaitResult;

  try
    {
    while( found )
      {
      dwWaitResult = WaitForSingleObject(
          ActiveThreadMutex, // handle to mutex
          INFINITE);         // no time-out interval

      // pthread_mutex_lock(&MutexWorkCompletion);
      switch( dwWaitResult )
        {
        // The thread got ownership of the mutex
        case WAIT_OBJECT_0:
          /*
          We know that a thread id will be in here because the AssignWork fn adds it in
          So if a thread id is not found in ActiveThreadIds it means it is done.
          */
          found = false;
          for( std::vector<int>::size_type i = 0; i != ActiveThreadIds.size(); i++ )
            {

            if( ActiveThreadIds[i] == id )
              {
              found = true;

              }
            }

          // Release ownership of the mutex object
          if( !ReleaseMutex(ActiveThreadMutex) )
            {
            // Handle error.
            }

          break;

        // The thread got ownership of an abandoned mutex
        case WAIT_ABANDONED:
          throw "Abondoned mutex";
        }

      }

    return true;
    }
  catch( std::exception& e )
    {
    ExceptionOccured = true;
    THREAD_DIAGNOSTIC_PRINT(
      "Exception occured while waiting for job with id : " << id << std::endl << e.what() << std::endl);

    }
}

WinJob ThreadPool::FetchWork()
{

  int    indexToReturn = -100;
  DWORD  dwWaitResult;
  WinJob ret;

  dwWaitResult = WaitForSingleObject(
      WorkAvailable,   // handle to semaphore
      INFINITE);

  dwWaitResult = WaitForSingleObject(
      MutexSync,  // handle to mutex
      INFINITE);  // no time-out interval

  // pthread_mutex_lock(&MutexWorkCompletion);
  switch( dwWaitResult )
    {
    // The thread got ownership of the mutex
    case WAIT_OBJECT_0:
      // get the index of workerthread from queue
      for( std::vector<int>::size_type i = 0; i != WorkerQueue.size(); i++ )
        {
        // if not Assigned already
        if( WorkerQueue[i].Assigned == false )
          {
          indexToReturn = i;
          WorkerQueue[i].Assigned = true;
          break;
          }
        }
      THREAD_DIAGNOSTIC_PRINT( std::endl << "Getting work from queue at index : " << indexToReturn << std::endl);
      ret = WorkerQueue[indexToReturn];

      // Release ownership of the mutex object
      if( !ReleaseMutex(MutexSync) )
        {
        // Handle error.
        }
      break;
    case WAIT_ABANDONED:
      throw "Abondoned mutex";

    }

  return ret;
}

int ThreadPool::GetCompletedJobs()
{
  return CompletedJobs;
}

void ThreadPool::RemoveActiveId(int id)
{
  try
    {
    THREAD_DIAGNOSTIC_PRINT( std::endl << "ActiveThreadids size : " << ActiveThreadIds.size() << ". Removing id " << id
                                       << std::endl);
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(
        ActiveThreadMutex, // handle to mutex
        INFINITE);         // no time-out interval
    // pthread_mutex_lock(&MutexWorkCompletion);
    int index = -1;

    switch( dwWaitResult )
      {
      // The thread got ownership of the mutex
      case WAIT_OBJECT_0:

        THREAD_DIAGNOSTIC_PRINT( std::endl << "Looping" << std::endl);
        for( std::vector<int>::size_type i = 0; i != ActiveThreadIds.size(); i++ )
          {
          THREAD_DIAGNOSTIC_PRINT( "Id is : " << ActiveThreadIds[i] << std::endl);
          if( ActiveThreadIds[i] == id )
            {
            index = i;
            break;
            }
          }

        if( index >= 0 )
          {
	      CompletedJobs++;
          ActiveThreadIds.erase(ActiveThreadIds.begin() + index);
          THREAD_DIAGNOSTIC_PRINT( std::endl << "Removed id " << id << " from ActiveThreadIds. Now vector size is "
                                             << ActiveThreadIds.size() << std::endl);
          }
        else
          {

          THREAD_DIAGNOSTIC_PRINT( std::endl << "Error occured, couldnt find id : " << id << std::endl);
          throw "Error occured in RemoveActiveId, couldnt find id in ActiveThreadIds queue to erase.";
          }
        // Release ownership of the mutex object
        if( !ReleaseMutex(ActiveThreadMutex) )
          {
          // Handle error.
          }
        break;
      case WAIT_ABANDONED:
        throw "Abondoned mutex";
      }

    // Delete from worker queue
    dwWaitResult = WaitForSingleObject(
        MutexSync,  // handle to mutex
        INFINITE);  // no time-out interval
    // pthread_mutex_lock(&MutexWorkCompletion);
    bool foundToDelete = false;
    int  delIndex = -1;

    switch( dwWaitResult )
      {
      // The thread got ownership of the mutex
      case WAIT_OBJECT_0:
        for( std::vector<int>::size_type i = 0; i != WorkerQueue.size(); i++ )
          {
          if( WorkerQueue[i].Id == id )
            {
            delIndex = i;
            foundToDelete = true;
            break;
            }
          }

        WorkerQueue.erase(WorkerQueue.begin() + delIndex);
        THREAD_DIAGNOSTIC_PRINT( std::endl << "Removed index " << delIndex << " from WorkerQueue. Now vector size is "
                                           << WorkerQueue.size() << std::endl);
        if( foundToDelete == false )
          {
          THREAD_DIAGNOSTIC_PRINT(
            std::endl << "Error occured, couldnt find id in WorkerQueue to mark executed. Id is : " << id
                      << std::endl);
          throw "Error occured in RemoveActiveId, couldnt find id in WorkerQueue to mark executed. ";
          }
        // Release ownership of the mutex object
        if( !ReleaseMutex(MutexSync) )
          {
          // Handle error.
          }
        break;
      case WAIT_ABANDONED:
        throw "Abondoned mutex";
      }
    }
  catch( std::exception & e )
    {
    throw e;
    }

}

void * ThreadPool::ThreadExecute(void *param)
{
  // get the parameters passed in
  ThreadPool *winThreadPool = (ThreadPool *)param;

  // int          s = pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  while( !winThreadPool->Destroy )
    {
    try
      {
      WinJob winJob = winThreadPool->FetchWork();
      winJob.Ptid = GetCurrentThreadId();
      THREAD_DIAGNOSTIC_PRINT( "Fetched workerthread (executing now) with ptid: " << winJob.Ptid << std::endl);
      winJob.ThreadFunction(winJob.ThreadArgs.otherArgs);
      THREAD_DIAGNOSTIC_PRINT( std::endl << "Execution done for: " << &winJob << std::endl);

      winThreadPool->RemoveActiveId(winJob.Id);

      THREAD_DIAGNOSTIC_PRINT( "Deleted worker thread" << std::endl);
      DWORD dwWaitResult;
      dwWaitResult = WaitForSingleObject(
          MutexWorkCompletion, // handle to mutex
          INFINITE);           // no time-out interval

      // pthread_mutex_lock(&MutexWorkCompletion);
      switch( dwWaitResult )
        {
        // The thread got ownership of the mutex
        case WAIT_OBJECT_0:
          winThreadPool->IncompleteWork--;

          // Release ownership of the mutex object
          if( !ReleaseMutex(MutexWorkCompletion) )
            {
            // Handle error.
            }
          break;
        // The thread got ownership of an abandoned mutex
        case WAIT_ABANDONED:
          throw "Error occured : got abandoned mutex";
        }

      }
    catch( std::exception& e )
      {
      winThreadPool->ExceptionOccured = true;
      THREAD_DIAGNOSTIC_PRINT( "Exception occured in thread execution\n" << e.what() << std::endl);
      }

    }

  THREAD_DIAGNOSTIC_PRINT( std::endl << "Thread exited ptid:" << GetCurrentThreadId() << std::endl);
  return 0;
}

}
