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
SimpleFastMutexLock ThreadPool::m_MutexSync;
SimpleFastMutexLock ThreadPool::m_MutexWorkCompletion;
SimpleFastMutexLock ThreadPool::m_ActiveThreadMutex;
SimpleFastMutexLock ThreadPool::m_ThreadIdAccessMutex;

bool                     ThreadPool::m_InstanceFlag = false;
SmartPointer<ThreadPool> ThreadPool::m_SThreadPoolInstance = 0;

SmartPointer<ThreadPool> ThreadPool::GetThreadPool()
{
  if( !m_InstanceFlag )
    {
    m_SThreadPoolInstance = ThreadPool::New();
    m_SThreadPoolInstance->DebugOn();
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
#if defined(ITK_USE_WIN32_THREADS)
  m_WorkAvailable = CreateSemaphore(
      NULL,       // default security attributes
      0,          // initial count
      INFINITE,   // maximum count
      NULL);      // unnamed semaphore
#else
  sem_init(&m_WorkAvailable, 0, 0);
#endif
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

bool ThreadPool::WaitForJob(int id)
{
  bool found = true;

  try
    {
    while( found )
      {
        {
        MutexLockHolder<SimpleFastMutexLock> mutexHolderAct(m_ActiveThreadMutex);
        /*
        We know that a thread id will be in here because the AssignWork fn adds it in
        So if a thread id is not found in ActiveThreadIds it means it is done.
        */
        found = false;
        for( std::vector<int>::size_type i = 0; i != m_ActiveJobIds.size(); ++i )
          {
          if( m_ActiveJobIds[i] == id )
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
      << "Exception occured while waiting for job with id : " << id << std::endl << e.what() <<
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
    itkDebugMacro(
      << std::endl << "ActiveThreadids size : " << m_ActiveJobIds.size() << ". Removing id " <<
      id << std::endl );
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderAct(m_ActiveThreadMutex);
      int                                  index = -1;
      itkDebugMacro(<< std::endl << "Looping" << std::endl );
      for( std::vector<int>::size_type i = 0; i != m_ActiveJobIds.size(); ++i )
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
      }
    // Delete from vorker queue
    bool foundToDelete = false;
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_MutexSync);

      int delIndex = -1;
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

      //also removing the job id and thread ids from the vectors
      int pos = std::find(m_JobIds.begin(), m_JobIds.end(), id) - m_JobIds.begin();
        {
        MutexLockHolder<SimpleFastMutexLock> threadIdMutexHolder(m_ThreadIdAccessMutex);
        m_JobIds.erase(m_JobIds.begin() + pos);
        m_ThreadIds.erase(m_ThreadIds.begin() + pos);
        }
      itkDebugMacro(
        << std::endl << "Removed index " << delIndex << " from WorkerQueue. Now vector size is " << m_WorkerQueue.size()
        << std::endl );
      }
    if( foundToDelete == false )
      {
      itkDebugMacro(
        << std::endl << "Error occured, couldnt find id in WorkerQueue to mark executed. Id is : " <<
        id << std::endl );
      itkExceptionMacro(<< "Error occured in RemoveActiveId, couldnt find id in WorkerQueue to mark executed. ");
      }
    }
  catch( std::exception & e )
    {
    itkExceptionMacro(<< e.what() );
    }
}

//in this method we do not need to wait because it is guranteed that the map
// will have the value we are finding
int ThreadPool::GetJobIdForThreadHandle(ThreadProcessIDType pt){
    {
    MutexLockHolder<SimpleFastMutexLock> threadIdMutexHolder(m_ThreadIdAccessMutex);
    int                                  pos =
      std::find(m_ThreadIds.begin(), m_ThreadIds.end(), pt) - m_ThreadIds.begin();

    return m_JobIds[pos];
    }
}

}
