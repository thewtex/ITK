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

namespace itk
{

pthread_mutex_t ThreadPool:: m_MutexSync = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool:: m_MutexWorkCompletion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool:: m_ActiveThreadMutex = PTHREAD_MUTEX_INITIALIZER;

ThreadPool * ThreadPool:: m_ThreadPoolInstance = 0;
bool ThreadPool::         m_InstanceFlag = false;
SmartPointer<ThreadPool> ThreadPool:: m_SThreadPoolInstance = 0;

ThreadPool & ThreadPool::GetThreadPool()
{
  if( !m_InstanceFlag )
    {
    m_ThreadPoolInstance = new ThreadPool();
    m_ThreadPoolInstance->InitializeThreads(2);
    }
  return *m_ThreadPoolInstance;
}

ThreadPool & ThreadPool::GetThreadPool(int maxThreads)
{
  if( !m_InstanceFlag )
    {
    m_ThreadPoolInstance = new ThreadPool();
    m_ThreadPoolInstance->InitializeThreads(maxThreads);
    }
  return *m_ThreadPoolInstance;
}

SmartPointer<ThreadPool> ThreadPool::GetNewThreadPool(int maxThreads)
{
  m_SThreadPoolInstance = new ThreadPool();
  m_SThreadPoolInstance->InitializeThreads(maxThreads);
  return m_SThreadPoolInstance;
}

ThreadPool::ThreadPool() : m_IdCounter(1)
{
  m_InstanceFlag = true;
  m_CompletedJobs = 0;
}

void ThreadPool::InitializeThreads(int maximumThreads)
{

  try
    {
    if( maximumThreads < 1 )
      {
      maximumThreads = 1;
      }
    this->m_MaxThreads = maximumThreads;
    this->m_QueueSize = maximumThreads;
    m_ScheduleForDestruction = false;
    m_IncompleteWork = 0;
    sem_init(&m_WorkAvailable, 0, 0);
    m_ScheduleForDestruction = false;
    m_ThreadHandles = new pthread_t[m_MaxThreads];
    for( int i = 0; i < m_MaxThreads; i++ )
      {
      int rc;
      rc = pthread_create(&m_ThreadHandles[i], NULL, &ThreadPool::ThreadExecute, (void *)this );
      if( rc )
        {
    itkDebugMacro(<< "ERROR; return code from pthread_create() is " << rc << std::endl);
    itkExceptionMacro(<< "Cannot create thread. Error in return code from pthread_create()");
        }
      itkDebugMacro(<< "Thread createdd with ptid :" << m_ThreadHandles[i] << std::endl );
      }
    }
  catch( std::exception &e  )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(<< std::endl << "Initialization failure\n" << e.what() << std::endl );
    }
  itkDebugMacro(<< "Created thread pool with threads :" << maximumThreads << std::endl );

}

// Can call if needed to delete the created singleton instance
void ThreadPool::DeleteSingletonInstance()
{
  try
    {
    if( m_InstanceFlag == true )
      {
      itkDebugMacro(<< "Deleting instance" << std::endl );
      m_InstanceFlag = false;
      m_ThreadPoolInstance->DestroyPool(2);
      m_ThreadPoolInstance->m_WorkerQueue.clear();

      delete[] m_ThreadPoolInstance->m_ThreadHandles;

      delete m_ThreadPoolInstance;

      itkDebugMacro(<< "Destroyed" << std::endl );

      }
    }
  catch( std::exception& e )
    {
    itkDebugMacro(<< std::endl << "Cannot delete instance \n" << e.what() << std::endl );
    m_ThreadPoolInstance->m_ExceptionOccured = true;
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
    sleep(pollTime);
    }

  itkDebugMacro(<< "All threads are done" << std::endl << "Terminating threads" << std::endl );
  for( int i = 0; i < m_MaxThreads; ++i )
    {
    int s = pthread_cancel(m_ThreadHandles[i]);
    if( s != 0 )
      {
      itkDebugMacro(<< "Cannot cancel thread with id : " << m_ThreadHandles[i] << std::endl );
      }
    }

  sem_destroy(&m_WorkAvailable);

  pthread_mutex_destroy(&m_MutexSync);
  pthread_mutex_destroy(&m_MutexWorkCompletion);
}

int ThreadPool::AssignWork(ThreadJob pthreadJob)
{
  try
    {
    pthread_mutex_lock(&m_MutexWorkCompletion);
    m_IncompleteWork++;
    pthread_mutex_unlock(&m_MutexWorkCompletion);

    pthread_mutex_lock(&m_MutexSync);
    // adding to queue
    pthreadJob.Id = m_IdCounter++;
    m_WorkerQueue.push_back(pthreadJob);
    itkDebugMacro(<<
      "Assigning Worker[" << (pthreadJob).Id << "] Address:[" << &pthreadJob << "] to Queue " << std::endl );
    pthread_mutex_unlock(&m_MutexSync);

    pthread_mutex_lock(&m_ActiveThreadMutex);
    itkDebugMacro(<< std::endl << "Adding id " << pthreadJob.Id << " to ActiveThreadIds" << std::endl );
    m_ActiveThreadIds.push_back(pthreadJob.Id);
    itkDebugMacro(<< std::endl << "ActiveThreadids size : " << m_ActiveThreadIds.size() << std::endl );
    pthread_mutex_unlock(&m_ActiveThreadMutex);

    sem_post(&m_WorkAvailable);

    return (pthreadJob).Id;
    }
  catch( std::exception& e )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(<< std::endl << "Failed to assign work. \n" << e.what() << std::endl );
    itkExceptionMacro(<< e.what());
    }
}

bool ThreadPool::WaitForThread(int id)
{
  bool found = true;

  try
    {
    while( found )
      {
      pthread_mutex_lock(&m_ActiveThreadMutex);
      /*
      We know that a thread id will be in here because the AssignWork fn adds it in
      So if a thread id is not found in ActiveThreadIds it means it is done.
      */
      found = false;
      for( std::vector<int>::size_type i = 0; i != m_ActiveThreadIds.size(); ++i )
        {
        if( m_ActiveThreadIds[i] == id )
          {
          found = true;
          break;
          }
        }
      pthread_mutex_unlock(&m_ActiveThreadMutex);
      }
    }
  catch( std::exception &e )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(<<
      "Exception occured while waiting for job with id : " << id << std::endl << e.what() << std::endl );
    }
  return true;
}

ThreadJob ThreadPool::FetchWork()
{

  int indexToReturn = -1;

  sem_wait(&m_WorkAvailable);   // wait to get work
  pthread_mutex_lock(&m_MutexSync);
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
  pthread_mutex_unlock(&m_MutexSync);

  return ret;
}


int ThreadPool::GetCompletedJobs() const
{
  return m_CompletedJobs;
}

void ThreadPool::RemoveActiveId(int id)
{
  try
    {
    itkDebugMacro(<<
      std::endl << "ActiveThreadids size : " << m_ActiveThreadIds.size() << ". Removing id " << id << std::endl );
    pthread_mutex_lock(&m_ActiveThreadMutex);
    int index = -1;
    itkDebugMacro(<< std::endl << "Looping" << std::endl );
    for( std::vector<int>::size_type i = 0; i != m_ActiveThreadIds.size(); ++i )
      {
      itkDebugMacro(<< "Id is : " << m_ActiveThreadIds[i] << std::endl );
      if( m_ActiveThreadIds[i] == id )
        {
        index = i;
        break;
        }
      }

    if( index >= 0 )
      {
      m_ActiveThreadIds.erase(m_ActiveThreadIds.begin() + index);
      //increase the count of jobs completed
      m_CompletedJobs++;
      itkDebugMacro(<<
        std::endl << "Removed id " << id << " from ActiveThreadIds. Now vector size is " << m_ActiveThreadIds.size()
                  << std::endl );
      }
    else
      {

      itkDebugMacro(<< std::endl << "Error occured, couldnt find id : " << id << std::endl );
      itkExceptionMacro(<< "Error occured in RemoveActiveId, couldnt find id in ActiveThreadIds queue to erase.");
      }
    pthread_mutex_unlock(&m_ActiveThreadMutex);

    // Delete from vorker queue
    pthread_mutex_lock(&m_MutexSync);
    bool foundToDelete = false;
    int  delIndex = -1;
    for( std::vector<int>::size_type i = 0; i != m_WorkerQueue.size(); ++i )
      {
      if( m_WorkerQueue[i].Id == id )
        {
        delIndex = i;
        foundToDelete = true;
        break;
        }
      }

    m_WorkerQueue.erase(m_WorkerQueue.begin() + delIndex);
    itkDebugMacro(<<
      std::endl << "Removed index " << delIndex << " from WorkerQueue. Now vector size is " << m_WorkerQueue.size()
                << std::endl );
    pthread_mutex_unlock(&m_MutexSync);
    if( foundToDelete == false )
      {
      itkDebugMacro(<<
        std::endl << "Error occured, couldnt find id in WorkerQueue to mark executed. Id is : " << id << std::endl );
      itkExceptionMacro(<< "Error occured in RemoveActiveId, couldnt find id in WorkerQueue to mark executed. ");
      }
    }
  catch( std::exception & e )
    {
    itkExceptionMacro(<< e.what());
    }

}

void * ThreadPool::ThreadExecute(void *param)
{
  // get the parameters passed in
  ThreadPool *pThreadPool = (ThreadPool *)param;
  int         s = pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  if( s != 0 )
    {
    itkDebugMacro(<< std::endl << "Error setting cancel state for thread" << std::endl );
    }

  while( !pThreadPool->m_ScheduleForDestruction )
    {
    try
      {
      ThreadJob pthreadJob = pThreadPool->FetchWork();
      pthreadJob.Ptid = pthread_self();
      itkDebugMacro(<< "Fetched workerthread (executing now) with ptid: " << pthreadJob.Ptid << std::endl );
      pthreadJob.ThreadFunction(pthreadJob.ThreadArgs.otherArgs);
      itkDebugMacro(<< std::endl << "Execution done for: " << &pthreadJob << std::endl );

      pThreadPool->RemoveActiveId(pthreadJob.Id);

      itkDebugMacro(<< "Deleted worker thread" << std::endl );
      pthread_mutex_lock( &(pThreadPool)->m_MutexWorkCompletion);
      pThreadPool->m_IncompleteWork--;
      pthread_mutex_unlock( &(pThreadPool)->m_MutexWorkCompletion);
      }
    catch( std::exception &e )
      {
      pThreadPool->m_ExceptionOccured = true;
      itkDebugMacro(<< "Exception occured in thread execution\n" << e.what() << std::endl );
      }

    }

  itkDebugMacro(<< std::endl << "Thread exited ptid:" << pthread_self() << std::endl );
  return 0;
}

}
