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
#include "itkPThreadPool.h"

//TODO: Not portable sleep function needs this on unix/mac
#include <unistd.h>

namespace itk
{
SimpleFastMutexLock ThreadPool:: m_MutexSync;
SimpleFastMutexLock ThreadPool:: m_MutexWorkCompletion;
SimpleFastMutexLock ThreadPool:: m_ActiveThreadMutex;

bool                     ThreadPool:: m_InstanceFlag = false;
SmartPointer<ThreadPool> ThreadPool:: m_SThreadPoolInstance = 0;

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

ThreadPool::ThreadPool() :  m_ThreadCount(0), m_IdCounter(1)
{
  m_InstanceFlag = true;
  m_CompletedJobs = 0;
  m_ScheduleForDestruction = false;
  m_IncompleteWork = 0;
  sem_init(&m_WorkAvailable, 0, 0);
}

void ThreadPool::AddThread()
{
  // this->m_MaxThreads = maximumThreads;
  this->m_ThreadCount++;
  ThreadProcessIDType aThreadHandle;
  const int           rc = pthread_create(&aThreadHandle, NULL, &ThreadPool::ThreadExecute, (void *)this );
  if( rc )
    {
    itkDebugMacro(<< "ERROR; return code from pthread_create() is " << rc << std::endl);
    itkExceptionMacro(<< "Cannot create thread. Error in return code from pthread_create()");
    }
  else
    {
    m_ThreadHandles.push_back(aThreadHandle);
    itkDebugMacro(<< "Thread created with ptid :" << m_ThreadHandles[m_ThreadCount - 1] << std::endl );
    }
}

void ThreadPool::InitializeThreads(int maximumThreads)
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
    itkDebugMacro(<< "Work is still incomplete=" << m_IncompleteWork << std::endl );
    // TODO:  sleep is not portable across platforms
    sleep(pollTime);
    }

  itkDebugMacro(<< "All threads are done" << std::endl << "Terminating threads" << std::endl );
  for( int i = 0; i < m_ThreadCount; ++i )
    {
    int s = pthread_cancel(m_ThreadHandles[i]);
    if( s != 0 )
      {
      itkDebugMacro(<< "Cannot cancel thread with id : " << m_ThreadHandles[i] << std::endl );
      }
    }

  sem_destroy(&m_WorkAvailable);
}

ThreadProcessIDType ThreadPool::AssignWork(ThreadJob pthreadJob)
{
  try
    {
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_MutexWorkCompletion);
      m_IncompleteWork++;
      }
    if( m_IncompleteWork > m_ThreadCount )
      {
      // If more work is pending than available threads, start a thread
      AddThread();
      }
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexSync);
      // adding to queue

      pthreadJob.SetThreadName( ++m_IdCounter );
      m_WorkerQueue.push_back(pthreadJob);
      itkDebugMacro(
        << "Assigning Worker[" << pthreadJob.GetThreadName() << "] Address:[" << &pthreadJob << "] to Queue " <<
        std::endl );
      }

      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderAct(m_ActiveThreadMutex);
      itkDebugMacro(<< std::endl << "Adding id " << pthreadJob.GetThreadName() << " to ActiveThreadIds" << std::endl );
      m_ActiveThreadIds.push_back(pthreadJob.GetThreadName() );
      itkDebugMacro(<< std::endl << "ActiveThreadids size : " << m_ActiveThreadIds.size() << std::endl );
      }

    sem_post(&m_WorkAvailable);

    return pthreadJob.GetPThreadID();
    }
  catch( std::exception& e )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(<< std::endl << "Failed to assign work. \n" << e.what() << std::endl );
    itkExceptionMacro(<< e.what() );
    }
}

ThreadJob::ThreadNamingType ThreadPool::GetNameFromID( ThreadProcessIDType threadHandle ) const
{
  for( std::vector<int>::size_type i = 0; i != m_WorkerQueue.size(); ++i )
    {
    if( m_WorkerQueue[i].GetPThreadID() == threadHandle )
      {
      return m_WorkerQueue[i].GetThreadName();
      }
    }
  itkExceptionMacro(<< "ERROR:  Threadname not found " );
}

bool ThreadPool::WaitForThread(const ThreadJob::ThreadNamingType & threadName)
{
  bool found = true;

  try
    {
    while( found )
      {
        {
        MutexLockHolder<SimpleFastMutexLock> mutexHolderAct(m_ActiveThreadMutex);
        /*
        We know that a thread threadName will be in here because the AssignWork fn adds it in
        So if a thread threadName is not found in ActiveThreadIds it means it is done.
        */
        found = false;
        for( std::vector<int>::size_type i = 0; i != m_ActiveThreadIds.size(); ++i )
          {
          if( m_ActiveThreadIds[i] == threadName )
            {
            found = true;
            break;
            }
          }

        }
      }
    }
  catch( std::exception & e )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(
      << "Exception occured while waiting for job with threadName : " << threadName << std::endl << e.what() <<
      std::endl );
    }
  return true;
}

ThreadJob ThreadPool::FetchWork()
{

  int indexToReturn = -1;

  sem_wait(&m_WorkAvailable);   // wait to get work
    {
    MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexSync);
    // get the index of workerthread from queue
    for( std::vector<int>::size_type i = 0; i != m_WorkerQueue.size(); ++i )
      {
      // if not Assigned already
      if( m_WorkerQueue[i].Assigned == false )
        {
        indexToReturn = i;
        m_WorkerQueue[i].Assigned = true;
        break;
        }
      }
    itkDebugMacro(<< std::endl << "Getting work from queue at index : " << indexToReturn << std::endl );
    ThreadJob ret = m_WorkerQueue[indexToReturn];

    return ret;
    }
}

int ThreadPool::GetCompletedJobs() const
{
  return m_CompletedJobs;
}

void ThreadPool::RemoveActiveId(const ThreadJob::ThreadNamingType & threadName)
{
  try
    {
    itkDebugMacro(
      << std::endl << "ActiveThreadids size : " << m_ActiveThreadIds.size() << ". Removing threadName " <<
      threadName << std::endl );
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderAct(m_ActiveThreadMutex);
      int                                  index = -1;
      itkDebugMacro(<< std::endl << "Looping" << std::endl );
      for( std::vector<int>::size_type i = 0; i != m_ActiveThreadIds.size(); ++i )
        {
        itkDebugMacro(<< "Id is : " << m_ActiveThreadIds[i] << std::endl );
        if( m_ActiveThreadIds[i] == threadName )
          {
          index = i;
          break;
          }
        }

      if( index >= 0 )
        {
        m_ActiveThreadIds.erase(m_ActiveThreadIds.begin() + index);
        // increase the count of jobs completed
        m_CompletedJobs++;
        itkDebugMacro(
          << std::endl << "Removed threadName " << threadName << " from ActiveThreadIds. Now vector size is " <<
          m_ActiveThreadIds.size()
          << std::endl );
        }
      else
        {

        itkDebugMacro(<< std::endl << "Error occured, couldnt find threadName : " << threadName << std::endl );
        itkExceptionMacro(
          << "Error occured in RemoveActiveId, couldnt find threadName in ActiveThreadIds queue to erase.");
        }
      }
    // Delete from vorker queue
    bool foundToDelete = false;
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexSync);

      int delIndex = -1;
      for( std::vector<int>::size_type i = 0; i != m_WorkerQueue.size(); ++i )
        {
        if( m_WorkerQueue[i].GetThreadName() == threadName )
          {
          delIndex = i;
          foundToDelete = true;
          break;
          }
        }

      m_WorkerQueue.erase(m_WorkerQueue.begin() + delIndex);
      itkDebugMacro(
        << std::endl << "Removed index " << delIndex << " from WorkerQueue. Now vector size is " << m_WorkerQueue.size()
        << std::endl );
      }
    if( foundToDelete == false )
      {
      itkDebugMacro(
        << std::endl << "Error occured, couldnt find threadName in WorkerQueue to mark executed. Id is : " <<
        threadName << std::endl );
      itkExceptionMacro(<< "Error occured in RemoveActiveId, couldnt find threadName in WorkerQueue to mark executed. ");
      }
    }
  catch( std::exception & e )
    {
    itkExceptionMacro(<< e.what() );
    }
}

void * ThreadPool::ThreadExecute(void *param)
{
  // get the parameters passed in
  ThreadPool *pThreadPool = (ThreadPool *)param;
  const int   s = pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  if( s != 0 )
    {
    itkDebugMacro(<< std::endl << "Error setting cancel state for thread" << std::endl );
    }

  while( !pThreadPool->m_ScheduleForDestruction )
    {
    try
      {
      ThreadJob currentPThreadJob = pThreadPool->FetchWork();
      currentPThreadJob.SetPThreadID( pthread_self() );
      itkDebugMacro(
        << "Fetched workerthread (executing now) with ptid: " << currentPThreadJob.GetPThreadID() << std::endl );
      currentPThreadJob.ThreadFunction(currentPThreadJob.ThreadArgs.otherArgs);
      itkDebugMacro(<< std::endl << "Execution done for: " << &currentPThreadJob << std::endl );

      pThreadPool->RemoveActiveId(currentPThreadJob.GetThreadName() );

      itkDebugMacro(<< "Deleted worker thread" << std::endl );
        {
        MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_MutexWorkCompletion);
        pThreadPool->m_IncompleteWork--;
        }
      }
    catch( std::exception & e )
      {
      pThreadPool->m_ExceptionOccured = true;
      itkDebugMacro(<< "Exception occured in thread execution\n" << e.what() << std::endl );
      }
    }

  itkDebugMacro(<< std::endl << "Thread exited ptid:" << pthread_self() << std::endl );
  return 0;
}

}
