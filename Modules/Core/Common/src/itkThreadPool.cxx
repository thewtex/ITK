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

/*

Thread pool manages the threads for itk. Thread pool is called and initialized from
within the MultiThreader. Initially the thread pool is started with zero threads.
Threads are added as job(s) are submitted to the thread pool and if it cannot be
executed right away. For example : If the thread pool has three threads and all are
busy. If a new job is submitted to the thread pool, the thread pool checks to see if
any threads are free. If not, it adds a new thread and executed the job right away.
The ThreadJob class is used to submit jobs to the thread pool. The ThreadJob's
necessary variables need to be set and then the ThreadJob can be passed to the
ThreaadPool by calling its AssignWork Method which returns the thread id on which
the job is being executed. One can then wait for the job using the thread id and
calling the WaitForJob method on the thread pool.

*/

#include <stdlib.h>
#include "itkThreadPool.h"
#if defined(ITK_USE_PTHREADS)
#include "itkPThreadPool.cxx"
#elif defined(ITK_USE_WIN32_THREADS)
#include "itkWinThreadPool.cxx"
#endif

namespace itk
{
SimpleFastMutexLock ThreadPool::m_VectorQMutex;
SimpleFastMutexLock ThreadPool::m_MutexWorkCompletion;
SimpleFastMutexLock ThreadPool::m_CreateInstanceMutex;

bool        ThreadPool::m_InstanceFlag = false;
ThreadPool* ThreadPool::m_SThreadPoolInstance = 0;

ThreadPool* ThreadPool::GetThreadPool()
{
    {
    MutexLockHolder<SimpleFastMutexLock> mutexHolderAct(m_CreateInstanceMutex);
    if( !m_InstanceFlag )
      {
      m_SThreadPoolInstance = new ThreadPool();
      m_SThreadPoolInstance->DebugOn();
      }
    }
  return m_SThreadPoolInstance;
}

ThreadPool::ThreadPool() :  m_ThreadCount(0), m_IdCounter(1)
{
  m_InstanceFlag = true;
  m_CompletedJobs = 0;
  m_ScheduleForDestruction = false;
  m_IncompleteWork = 0;
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
  itkDebugMacro(<<std::endl<<"Thread pool being destroyed"<<std::endl);
}

/*
  Once wait thread is called we will look for the threadhandle's job id
  if it is -2 it means it is done -
  if it is +ve it means it is running

  if it is -2 / done set the jid to -1 which means that now assign work can
  assign further work to this thread
  */
bool ThreadPool::WaitForJobOnThreadHandle(ThreadProcessIDType handle)
{
  bool found = false;

  try
    {
    itkDebugMacro(
      << "Waiting for thread with handle "<<handle<<
      std::endl );
    while( !found )
      {
      MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_VectorQMutex);
        {
        for( std::vector<int>::size_type i = 0; i != m_ThreadStructs.size(); i++ )
          {
          if(m_ThreadStructs[i].second == handle)
            {
            //check if th thread is free (-2) - this means job is done
            if(m_ThreadStructs[i].first == -2 )
              {
              found = true;
              itkDebugMacro(
                << "Wait ended for thread with handle "<<handle<<
                std::endl );
              break;
              }
            }
          } //for
        }   //mutex
      }     //while
    }
  catch( std::exception & e )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(
      << "Exception occured while waiting for thread with handle : " << handle << std::endl << e.what() <<
      std::endl );
    }
  return true;
}

int ThreadPool::GetCompletedJobs() const
{
  return m_CompletedJobs;
}

void ThreadPool::RemoveActiveId(int id)
{
  try
    {
    MutexLockHolder<SimpleFastMutexLock> vMutex(m_VectorQMutex);
      {
      itkDebugMacro(
        << std::endl << "Setting threadStruct's jobid to -1. Id is " <<
        id << std::endl );
      //setting ThreadJobStruct

      for( std::vector<int>::size_type i = 0; i != m_ThreadStructs.size(); i++ )
        {
        if(m_ThreadStructs[i].first == id) {
          m_ThreadStructs[i].first = -2; //set this to -1 so that now the thread
                                         // is in wait state
          }
        }

      itkDebugMacro(
        << std::endl << "ActiveThreadids size : " << m_ActiveJobIds.size() << ". Removing id " <<
        id << std::endl );

      int index = -1;
      itkDebugMacro(<< std::endl << "Looping" << std::endl );
      for( std::vector<int>::size_type i = 0; i != m_ActiveJobIds.size(); i++ )
        {
        itkDebugMacro(<< "Id is : " << m_ActiveJobIds[i] << std::endl );
        if( m_ActiveJobIds[i] == id )
          {
          index = i;
          break;
          }
        }

      if( index >= 0 )
        {
        m_ActiveJobIds.erase(m_ActiveJobIds.begin() + index);
        // increase the count of jobs completed
        m_CompletedJobs++;
        itkDebugMacro(
          << std::endl << "Removed id " << id << " from ActiveThreadIds. Now vector size is " <<
          m_ActiveJobIds.size()
          << std::endl );
        }
      else
        {

        itkDebugMacro(<< std::endl << "Error occured, couldnt find id : " << id << std::endl );
        itkExceptionMacro(
          << "Error occured in RemoveActiveId, couldnt find id in ActiveThreadIds queue to erase.");
        }

      // Delete from vorker queue
      bool foundToDelete = false;

      int delIndex = -1;
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
      itkDebugMacro(
        << std::endl << "Removed index " << delIndex << " from WorkerQueue. Now vector size is " << m_WorkerQueue.size()
        << std::endl );

      if( foundToDelete == false )
        {
        itkDebugMacro(
          << std::endl << "Error occured, couldnt find id in WorkerQueue to mark executed. Id is : " <<
          id << std::endl );
        itkExceptionMacro(<< "Error occured in RemoveActiveId, couldnt find id in WorkerQueue to mark executed. ");
        }
      }
    }
  catch( std::exception & e )
    {
    itkExceptionMacro(<< e.what() );
    }
}

ThreadJob ThreadPool::FetchWork(ThreadProcessIDType threadHandle)
{
  bool workFound = false;
  int  workId = -1;

  itkDebugMacro(
    << "Waiting for work (semaphore wait)-  thread with handle "<<threadHandle<<
    std::endl );
  while(!workFound) {

    MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_VectorQMutex);
      {

      for( std::vector<int>::size_type i = 0; i != m_ThreadStructs.size(); i++ )
        {
        // > 0 because Job ids start from 1
        if(m_ThreadStructs[i].second == threadHandle && m_ThreadStructs[i].first > 0)
          {
          workFound = true;
          workId = m_ThreadStructs[i].first;
          itkDebugMacro(
            << std::endl << "In FetchWork- found work for thread "<<m_ThreadStructs[i].second<<" Job id is  : " << workId <<
            std::endl );
          break;
          }
        }
      }

    }

  int indexToReturn = -1;
    {
    MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_VectorQMutex);
    // get the index of workerthread from queue
    for( std::vector<int>::size_type i = 0; i != m_WorkerQueue.size(); i++ )
      {
      // if not Assigned already
      if( m_WorkerQueue[i].Assigned == false && m_WorkerQueue[i].Id == workId)
        {
        indexToReturn = i;
        m_WorkerQueue[i].Assigned = true;
        break;
        }
      }
    itkDebugMacro(<< std::endl << "Getting work from queue at index : " << indexToReturn << std::endl );

    if(indexToReturn == -1)
      {
      itkDebugMacro(<< std::endl << "Index to return in FetchWork was -1 so returning an empty job "<< std::endl );
      ThreadJob ret;
      return ret;
      }
    else
      {
      ThreadJob ret = m_WorkerQueue[indexToReturn];
      return ret;
      }
    }
}

bool ThreadPool::DoIAddAThread(){

  MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_VectorQMutex);
    {
    bool addAThread = true;
    for( std::vector<int>::size_type i = 0; i != m_ThreadStructs.size(); i++ )
      {
      //if any one is -2 dont add a thread: -2 means free -1 means waiting
      if(m_ThreadStructs[i].first == -2) {
        addAThread = false;
        break;
        }
      }

    return addAThread;
    }
}

ThreadProcessIDType ThreadPool::AssignWork(ThreadJob threadJob)
{
  try
    {
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_MutexWorkCompletion);
      m_IncompleteWork++;
      }

    if(DoIAddAThread() == true) {
      AddThread();
      itkDebugMacro(<< std::endl << "Added a new thread" << std::endl );
      }

      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_VectorQMutex);
      // adding to queue

      threadJob.Id = m_IdCounter++;
      m_WorkerQueue.push_back(threadJob);
      itkDebugMacro(
        << "Assigning Worker[" << threadJob.Id << "] Address:[" << &threadJob << "] to Queue " <<
        std::endl );

      itkDebugMacro(<< std::endl << "Adding id " << threadJob.Id << " to ActiveThreadIds" << std::endl );
      m_ActiveJobIds.push_back(threadJob.Id );
      itkDebugMacro(<< std::endl << "ActiveThreadids size : " << m_ActiveJobIds.size() << std::endl );
      std::cout<< std::endl << "ActiveThreadids size : " << m_ActiveJobIds.size() << std::endl ;

      ThreadProcessIDType returnValue;
      //now put the job id in the ThreadJobStruct vector
      itkDebugMacro(<< std::endl << "Setting thread struct for a thread - assigning job id "<<threadJob.Id <<
                    std::endl );
      for( std::vector<int>::size_type i = 0; i != m_ThreadStructs.size(); i++ )
        {
        if(m_ThreadStructs[i].first == -2) {       // only if it is -2 you
                                                   // assign it a job
          m_ThreadStructs[i].first = threadJob.Id; // assign the job id to the
                                                   // thread now
          returnValue = m_ThreadStructs[i].second;
          break;  // break because we dont want to keep assigning
          }
        }
      //post semaphore for all threads so all threads will do 1 loop in the
      // fetch work while loop
      for( std::vector<int>::size_type i = 0; i != m_ThreadHandles.size(); i++ ) {

        }
      return returnValue;
      }
    }
  catch( std::exception& e )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(<< std::endl << "Failed to assign work. \n" << e.what() << std::endl );
    itkExceptionMacro(<< e.what() );
    }
}

}
