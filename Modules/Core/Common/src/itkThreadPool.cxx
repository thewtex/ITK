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
#include "itkPThreadPool.cxx"  //TODO:  cxx files should not be included, either change to hxx or have conditional compile
#elif defined(ITK_USE_WIN32_THREADS)
#include "itkWinThreadPool.cxx"  //TODO:  cxx files should not be included, either change to hxx or have conditional compile
#endif

#include <algorithm>

namespace itk
{
SimpleFastMutexLock ThreadPool::m_ThreadIDHandlePairingQueueMutex;
SimpleFastMutexLock ThreadPool::m_WorkerQueueMutex;
SimpleFastMutexLock ThreadPool::m_NumberOfPendingJobsToBeRunMutex;
SimpleFastMutexLock ThreadPool::m_ThreadPoolInstanceMutex;

bool         ThreadPool::m_ThreadPoolSingletonExists = false;
ThreadPool * ThreadPool::m_ThreadPoolInstance = 0;

ThreadPool * ThreadPool::GetThreadPool()
{
    {
    MutexLockHolder<SimpleFastMutexLock> mutexHolderAct(m_ThreadPoolInstanceMutex);
    if( !m_ThreadPoolSingletonExists )
      {
      //Create thread pool if it does not exist
      m_ThreadPoolInstance = new ThreadPool();
      //TODO:  Turn debugging off.
      m_ThreadPoolInstance->DebugOn();
      }
    }
  return m_ThreadPoolInstance;
}

ThreadPool::ThreadPool() :
  m_CompletedJobs(0),
  m_ScheduleForDestruction(false),
  m_ThreadCount(0),
  m_NumberOfPendingJobsToBeRun(0),
  m_IdCounter(1)
{
  if ( m_ThreadPoolSingletonExists == true )
    {
    // FAILURE: Only 1 threadpool can exist!
    }
    m_ThreadPoolSingletonExists = true; //Once initialized, then set to true, this should only be set to true one time.
  // TODO:  Initialize with some value so that we can avoid some overhead of copy and replacing.
  m_WorkerQueue.reserve(1000);
  m_ActiveJobIds.reserve(1000);
  m_ThreadHandles.reserve(1000);
  m_ThreadIDHandlePairingQueue.reserve(1000);
}

void ThreadPool::InitializeThreads(unsigned int maximumThreads)
{
  if( maximumThreads == 0 )
    {
    maximumThreads = 1;
    }
  for( unsigned int i = 0; i < maximumThreads; ++i )
    {
    AddThread();
    }
}

ThreadPool::~ThreadPool()
{
  itkDebugMacro(<< std::endl << "Thread pool being destroyed" << std::endl);
}

/*
  Once wait thread is called we will look for the threadhandle's job id
  if it is JOB_THREADHANDLE_IS_FREE it means it is done -
  if it is +ve it means it is running

  if it is JOB_THREADHANDLE_IS_FREE / done set the jid to
  JOB_THREADHANDLE_IS_DONE which means that now assign work can
  assign further work to this thread
  */
bool ThreadPool::WaitForJobOnThreadHandle(ThreadProcessIDType threadHandle)
{
  bool found = false;
  try
    {
    itkDebugMacro(
      << "Waiting for thread with threadHandle " << threadHandle
      << std::endl );
    while( !found )
      {
        {
        MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIDHandlePairingQueueMutex);
        //TODO:  User iterators rather than indexes here
        //for( std::vector<int>::size_type i = 0; i != m_ThreadIDHandlePairingQueue.size(); i++ )
        for( ThreadStructsQueueType::const_iterator tpIter=m_ThreadIDHandlePairingQueue.begin(),
          tpEnd = m_ThreadIDHandlePairingQueue.end(); tpIter != tpEnd; ++tpIter)
          {
          if( tpIter->ThreadProcessHandle == threadHandle )
            {
            // check if the thread is free (JOB_THREADHANDLE_IS_FREE) - this means job is done
            if( tpIter->ThreadNumericId == JOB_THREADHANDLE_IS_FREE )
              {
              found = true;
              itkDebugMacro(
                << "Wait ended for thread with threadHandle " << threadHandle
                << std::endl );
              break;
              }
            }
          } // for
        }   // mutex
      }     // while
    }
  catch( std::exception & itkDebugStatement( e ) )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(
      << "Exception occured while waiting for thread with threadHandle : " << threadHandle << std::endl << e.what()
      << std::endl );
    }
  return true;
}

int ThreadPool::GetCompletedJobs() const
{
  return m_CompletedJobs;
}

template <typename TVectorType>
void RemoveStdVectorElement( TVectorType & inVector, typename TVectorType::size_type indexToRemove)
{
#define _USE_NEW_SWAP_POP 1
#if _USE_NEW_SWAP_POP
#if __cplusplus >= 201103L // ONLY C++11  // http://stackoverflow.com/questions/4442477/remove-ith-item-from-c-stdvector
       inVector[index] = std::move(inVector.back());
       inVector.pop_back();
#else
        std::swap( inVector[indexToRemove], inVector.back()  );
        inVector.pop_back();
#endif
#else
        inVector.erase(inVector.begin() + indexToRemove);
#endif
}

void ThreadPool::RemoveActiveId(int id)
{
  try
    {
    MutexLockHolder<SimpleFastMutexLock> vMutex(m_ThreadIDHandlePairingQueueMutex);
      {
      itkDebugMacro(
        << std::endl << "Setting threadStruct's jobid to JOB_THREADHANDLE_IS_DONE. Id is "
        << id << std::endl );
      // setting ThreadJobStruct
      for( std::vector<int>::size_type i = 0; i != m_ThreadIDHandlePairingQueue.size(); i++ )
        {
        if( m_ThreadIDHandlePairingQueue[i].ThreadNumericId == id )
          {
          m_ThreadIDHandlePairingQueue[i].ThreadNumericId = JOB_THREADHANDLE_IS_FREE; // set this to JOB_THREADHANDLE_IS_DONE so that now the
                                         // thread
                                         // is in wait state
          }
        }

      itkDebugMacro(
        << std::endl << "ActiveThreadids size : " << m_ActiveJobIds.size() << ". Removing id "
        << id << std::endl );

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
        RemoveStdVectorElement( m_ActiveJobIds, index );
        // increase the count of jobs completed
        m_CompletedJobs++;
        itkDebugMacro(
          << std::endl << "Removed id " << id << " from ActiveThreadIds. Now vector size is "
          << m_ActiveJobIds.size()
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
        if( m_WorkerQueue[i].m_Id == id )
          {
          delIndex = i;
          foundToDelete = true;
          break;
          }
        }

      RemoveStdVectorElement( m_WorkerQueue, delIndex );

      itkDebugMacro(
        << std::endl << "Removed index " << delIndex << " from WorkerQueue. Now vector size is " << m_WorkerQueue.size()
        << std::endl );

      if( foundToDelete == false )
        {
        itkDebugMacro(
          << std::endl << "Error occured, couldnt find id in WorkerQueue to mark executed. Id is : "
          << id << std::endl );
        itkExceptionMacro(<< "Error occured in RemoveActiveId, couldnt find id in WorkerQueue to mark executed. ");
        }
      }
    }
  catch( std::exception & e )
    {
    itkExceptionMacro(<< e.what() );
    }
}


// TODO: Do we want to return a reference to the job in the work queue
//      This is returning a copy of the ThreadJob from the Queue rather
//      than a reference to the the job on the Queue.
ThreadJob ThreadPool::FetchWork(ThreadProcessIDType threadHandle)
{
  bool workFound = false;
  int  workId = JOB_THREADHANDLE_IS_DONE;

  itkDebugMacro(
    << "Waiting for work (semaphore wait)-  thread with handle " << threadHandle
    << std::endl );
  {
  while( !workFound )
    {
    MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIDHandlePairingQueueMutex);
    //TODO:  iterate through vector with iterators rather than index locations
    //for( std::vector<int>::size_type i = 0, threadStructSize = m_ThreadIDHandlePairingQueue.size(); i != threadStructSize; ++i )
    for( ThreadStructsQueueType::const_iterator tpIter=m_ThreadIDHandlePairingQueue.begin(),
          tpEnd = m_ThreadIDHandlePairingQueue.end(); tpIter != tpEnd; ++tpIter)
      {
      // > 0 because Job ids start from 1
      //Get const reference to m_ThreadIDHandlePairingQueue[i] to avoid constant dereferening from vector
      if( ( tpIter->ThreadProcessHandle == threadHandle ) && ( tpIter->ThreadNumericId > 0 ) )
        {
        workFound = true;
        workId = tpIter->ThreadNumericId;
        itkDebugMacro(
          << std::endl << "In FetchWork- found work for thread " << tpIter->ThreadProcessHandle << " Job id is  : " << workId
          << std::endl );
        break;
        }
      }
    }
  }

  int indexToReturn = -1;
    {
    //MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_WorkerQueueMutex);  NOTE: Need to lock the ThreadIDHandlePairing not the work queue.  This does not make sense to me though.
    MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIDHandlePairingQueueMutex);
    // get the index of workerthread from queue
    //TODO:  iterate through vector with iterators rather than index locations
    for( std::vector<int>::size_type i = 0, workQueueSize = m_WorkerQueue.size(); i != workQueueSize; ++i )
      {
      // if not Assigned already
      if( m_WorkerQueue[i].m_Assigned == false && m_WorkerQueue[i].m_Id == workId )
        {
        indexToReturn = i;
        m_WorkerQueue[i].m_Assigned = true;
        break;
        }
      }
    itkDebugMacro(<< std::endl << "Getting work from queue at index : " << indexToReturn << std::endl );

    if( indexToReturn == -1 )
      {
      itkDebugMacro(<< std::endl << "Index to return in FetchWork was -1 so returning an empty job " << std::endl );
      // TODO: What does it mean to return an empty uninitizlized job?
      ThreadJob ret;
      return ret;
      }
    else
      {
      // TODO: Should the return type be a reference to
      // the m_workerQueue rather than a copy of this
      // ThreadJob?
      ThreadJob ret = m_WorkerQueue[indexToReturn];
      return ret;
      }
    }
}

bool ThreadPool::DoIAddAThread()
{

  MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIDHandlePairingQueueMutex);
    {
    bool addAThread = true;
    for( std::vector<int>::size_type i = 0, threadStructsSize = m_ThreadIDHandlePairingQueue.size(); i != threadStructsSize; ++i )
      {
      // if any one is JOB_THREADHANDLE_IS_FREE dont add a thread: JOB_THREADHANDLE_IS_FREE means free JOB_THREADHANDLE_IS_DONE means waiting
      if( m_ThreadIDHandlePairingQueue[i].ThreadNumericId == JOB_THREADHANDLE_IS_FREE )
        {
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
      MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_NumberOfPendingJobsToBeRunMutex);
      m_NumberOfPendingJobsToBeRun++;
      }

    if( DoIAddAThread() == true )
      {
      AddThread();
      itkDebugMacro(<< std::endl << "Added a new thread" << std::endl );
      }

      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_ThreadIDHandlePairingQueueMutex);
      // adding to queue

      threadJob.m_Id = m_IdCounter++;
      m_WorkerQueue.push_back(threadJob);
      itkDebugMacro(
        << "Assigning Worker[" << threadJob.Id << "] Address:[" << &threadJob << "] to Queue "
        << std::endl );

      itkDebugMacro(<< std::endl << "Adding id " << threadJob.Id << " to ActiveThreadIds" << std::endl );
      m_ActiveJobIds.push_back(threadJob.m_Id );
      itkDebugMacro(<< std::endl << "ActiveThreadids size : " << m_ActiveJobIds.size() << std::endl );
      // TODO: REMOVE
      ThreadDebugMsg( << std::endl << "ActiveThreadids size : " << m_ActiveJobIds.size() << std::endl);

      ThreadProcessIDType returnValue = NULL; // TODO:  This is being returned
                                       // un-initialzied below
      // now put the job id in the ThreadJobStruct vector
      itkDebugMacro(<< std::endl << "Setting thread struct for a thread - assigning job id " << threadJob.Id
                    << std::endl );
      for( std::vector<int>::size_type i = 0, threadStructSize = m_ThreadIDHandlePairingQueue.size(); i != threadStructSize; ++i )
        {
        if( m_ThreadIDHandlePairingQueue[i].ThreadNumericId == JOB_THREADHANDLE_IS_FREE )         // only if it is JOB_THREADHANDLE_IS_FREE you
          {                                          // assign it a job
          m_ThreadIDHandlePairingQueue[i].ThreadNumericId = threadJob.m_Id; // assign the job id to the
                                                     // thread now
          returnValue = m_ThreadIDHandlePairingQueue[i].ThreadProcessHandle;
          break;  // break because we dont want to keep assigning
          }
        }

#if 0   // TODO: WHAT IS THIS LOOP FOR?
        // post semaphore for all threads so all threads will do 1 loop in the
        // fetch work while loop
      for( std::vector<int>::size_type i = 0, threadHandleSize = m_ThreadHandles.size(); i != threadHandleSize; ++i )
        {
        }
#endif
      // TODO:  ERROR:  THIS IS NOT GOOD AT THIS POINT
      // itkExceptionMacro(<< "INVALID THREAD RETURN CASE" << __FILE__ << " " <<
      // __LINE__ << " " );
      return returnValue;  // TODO:  This is being returned un-initialzied below
      }
    }
  catch( std::exception& e )
    {
    itkDebugMacro(<< std::endl << "Failed to assign work. \n" << e.what() << std::endl );
    m_ExceptionOccured = true;
    itkExceptionMacro(<< e.what() );
    }
}

}
