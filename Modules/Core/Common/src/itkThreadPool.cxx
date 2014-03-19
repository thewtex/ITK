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

#if defined(ITK_USE_PTHREADS)
#include "itkPThreadPool.hxx"
#elif defined(ITK_USE_WIN32_THREADS)
#include "itkWinThreadPool.hxx"
#endif

#include <algorithm>

namespace itk
{
SimpleFastMutexLock ThreadPool::m_ThreadIdHandlePairingQueueMutex;
SimpleFastMutexLock ThreadPool::m_WorkerQueueMutex;
SimpleFastMutexLock ThreadPool::m_NumberOfPendingJobsToBeRunMutex;
SimpleFastMutexLock ThreadPool::m_ThreadPoolInstanceMutex;

ThreadPool* ThreadPool::m_ThreadPoolInstance = ITK_NULLPTR;

ThreadPool * ThreadPool::GetThreadPool()
{
    {
    MutexLockHolder<SimpleFastMutexLock> mutexHolderAct(m_ThreadPoolInstanceMutex);
    if( m_ThreadPoolInstance == ITK_NULLPTR )
      {
      //Create thread pool if it does not exist
      m_ThreadPoolInstance = new ThreadPool();
      }
    }
  return m_ThreadPoolInstance;
}

ThreadPool::ThreadPool() :
  m_ScheduleForDestruction(false),
  m_ThreadCount(0),
  m_NumberOfPendingJobsToBeRun(0),
  m_IdCounter(1)
{
  m_ThreadSemHandlePairingQueue = NULL;
  m_ThreadSemHandlePairingForWaitQueue = NULL;
  m_ThreadIdHandleList = NULL;
  m_WorkerQueue.reserve(1000);
  m_ActiveJobIds.reserve(1000);
  m_ThreadHandles.reserve(1000);
  m_ThreadIdHandlePairingQueue.reserve(1000);
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
bool ThreadPool::WaitForJobOnThreadHandle(ThreadProcessIdType threadHandle)
{
  try
    {
    itkDebugMacro(<< "Waiting for thread with threadHandle " << threadHandle);

    if(GetSemaphoreForThreadWait(threadHandle)->SemaphoreWait() != 0 )
      {
      itkExceptionMacro(<<"Error in semaphore wait");
      }

      {
      MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIdHandlePairingQueueMutex);

      for( ThreadIdHandlePairingContainerType::const_iterator tpIter=m_ThreadIdHandlePairingQueue.begin(),
           tpEnd = m_ThreadIdHandlePairingQueue.end(); tpIter != tpEnd; ++tpIter)
        {
        if( CompareThreadHandles(tpIter->m_ThreadProcessHandle, threadHandle) )
          {
          // check if the thread is free (JOB_THREADHANDLE_IS_FREE) - this means
          // job is done
          if( tpIter->m_ThreadNumericId == JOB_THREADHANDLE_IS_FREE )
            {
            itkDebugMacro(<< "Wait ended for thread with threadHandle " << threadHandle);
            break;
            }
          }
        }   // for
      }     // mutex

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

template <typename TVectorType>
void RemoveStdVectorElement( TVectorType & inVector, typename TVectorType::size_type indexToRemove)
{
#if __cplusplus >= 201103L // ONLY C++11  //
                           // http://stackoverflow.com/questions/4442477/remove-ith-item-from-c-stdvector
  inVector[indexToRemove] = std::move(inVector.back() );
  inVector.pop_back();
#else
  std::swap( inVector[indexToRemove], inVector.back()  );
  inVector.pop_back();
#endif
}

void ThreadPool::RemoveActiveId(int id)
{
  try
    {
    MutexLockHolder<SimpleFastMutexLock> vMutex(m_ThreadIdHandlePairingQueueMutex);
      {
      // setting ThreadJobStruct
      for( std::vector<int>::size_type i = 0; i != m_ThreadIdHandlePairingQueue.size(); i++ )
        {
        if( m_ThreadIdHandlePairingQueue[i].m_ThreadNumericId == id )
          {
          m_ThreadIdHandlePairingQueue[i].m_ThreadNumericId = JOB_THREADHANDLE_IS_FREE; //
                                                                                      // set
                                                                                      // this
                                                                                      // to
                                                                                      // JOB_THREADHANDLE_IS_DONE
                                                                                      // so
                                                                                      // that
                                                                                      // now
                                                                                      // the
          // thread
          // is in wait state
          }
        }
      int index = -1;
      for( std::vector<int>::size_type i = 0; i != m_ActiveJobIds.size(); i++ )
        {
        if( m_ActiveJobIds[i] == id )
          {
          index = i;
          break;
          }
        }

      if( index >= 0 )
        {
        RemoveStdVectorElement( m_ActiveJobIds, index );
        }
      else
        {
        itkDebugMacro(<< "Error occured, couldnt find id : " << id );
        itkExceptionMacro(
          << "Error occured in RemoveActiveId, couldnt find id in ActiveThreadIds queue to erase.");
        }

      // Delete from worker queue
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
    {
    MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_NumberOfPendingJobsToBeRunMutex);
    m_NumberOfPendingJobsToBeRun--;
    }
}

void ThreadPool::AddNodeToThreadIdHandleList(ThreadIdHandlePair *head, ThreadProcessIdType handle, ThreadIdType id)
{
  ThreadIdHandlePair *node = head;

  while(node->m_Next != NULL)
    {
    node = node->m_Next;
    }
  //now here node-> m_Next is null
  node->m_Next = new ThreadIdHandlePair(handle, id);
  node->m_Next->m_Next = NULL;
}


void ThreadPool::AddNodeToTSPairLinkedList(ThreadSemaphorePair *head, ThreadProcessIdType handle)
{
  ThreadSemaphorePair *node = head;

  while(node->m_Next != NULL)
    {
    node = node->m_Next;
    }
  //now here node-> m_Next is null
  node->m_Next = new ThreadSemaphorePair(handle);
  node->m_Next->m_Next = NULL;
}

ThreadPool::ThreadSemaphorePair* ThreadPool::GetSemaphoreForThreadWait(ThreadProcessIdType threadHandle)
{

  MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIdHandlePairingQueueMutex);
  ThreadSemaphorePair *                node = m_ThreadSemHandlePairingForWaitQueue;
  do
    {
    if(CompareThreadHandles(node->m_ThreadProcessHandle, threadHandle))
      {
      return node;
      }
    node = node->m_Next;
    } while(node!= NULL);

  if(node == NULL)
    {
    itkExceptionMacro(<< "Error occured finding semaphore for thread handle " << threadHandle);
    }
  else
    {
    return node;
    }
}

ThreadPool::ThreadSemaphorePair* ThreadPool::GetSemaphoreForThread(ThreadProcessIdType threadHandle){
  MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIdHandlePairingQueueMutex);
  ThreadSemaphorePair *                node = m_ThreadSemHandlePairingQueue;
  do
    {
    if(CompareThreadHandles(node->m_ThreadProcessHandle, threadHandle))
      {
      break;
      }
    node = node->m_Next;
    } while(node != NULL);

  if(node == NULL)
    {
    itkExceptionMacro(<< "Error occured finding semaphore for thread handle "<<threadHandle);
    }
  else
    {
    return node;
    }
}

// TODO: Do we want to return a reference to the job in the work queue
//      This is returning a copy of the ThreadJob from the Queue rather
//      than a reference to the the job on the Queue.
ThreadJob ThreadPool::FetchWork(ThreadProcessIdType threadHandle)
{
  int  workId = JOB_THREADHANDLE_IS_DONE;

  if(GetSemaphoreForThread(threadHandle)->SemaphoreWait() != 0 )
    {
    itkExceptionMacro(<<"Error in semaphore wait");
    }

    {
    MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIdHandlePairingQueueMutex);
    //TODO:  iterate through vector with iterators rather than index locations
    //for( std::vector<int>::size_type i = 0, threadStructSize =
    // m_ThreadIdHandlePairingQueue.size(); i != threadStructSize; ++i )
    for( ThreadIdHandlePairingContainerType::const_iterator tpIter=m_ThreadIdHandlePairingQueue.begin(),
         tpEnd = m_ThreadIdHandlePairingQueue.end(); tpIter != tpEnd; ++tpIter)
      {
      // > 0 because Job ids start from 1
      //Get const reference to m_ThreadIdHandlePairingQueue[i] to avoid constant
      // dereferening from vector
      if( ( CompareThreadHandles(tpIter->m_ThreadProcessHandle , threadHandle )) && ( tpIter->m_ThreadNumericId > 0 ) )
        {
        workId = tpIter->m_ThreadNumericId;
        break;
        }
      } //for

    int indexToReturn = -1;

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
    if( indexToReturn == -1 )
      {
      itkExceptionMacro(<< std::endl << "Index to return in FetchWork was -1 so returning an empty job " << std::endl );
      // TODO: What does it mean to return an empty uninitizlized job?
      //ThreadJob ret;
      //return ret;
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

bool ThreadPool::IsNewThreadRequired()
{

  bool addAThread = true;

  for( std::vector<int>::size_type i = 0, threadStructsSize = m_ThreadIdHandlePairingQueue.size();
       i != threadStructsSize; ++i )
    {
    // if any one is JOB_THREADHANDLE_IS_FREE dont add a thread:
    // JOB_THREADHANDLE_IS_FREE means free JOB_THREADHANDLE_IS_DONE means
    // waiting
    if( m_ThreadIdHandlePairingQueue[i].m_ThreadNumericId == JOB_THREADHANDLE_IS_FREE )
      {
      addAThread = false;
      break;
      }
    }
  return addAThread;

}

ThreadProcessIdType ThreadPool::AssignWork(ThreadJob threadJob)
{
  try
    {
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_NumberOfPendingJobsToBeRunMutex);
      m_NumberOfPendingJobsToBeRun++;
      }
#if defined(__APPLE__)
    ThreadProcessIdType returnValue = NULL; // TODO:  This is being returned
#elif defined(_WIN32) || defined(_WIN64)
    ThreadProcessIdType returnValue = NULL; // TODO:  This is being returned
#else
    ThreadProcessIdType returnValue = pthread_self();
#endif
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderSync(m_ThreadIdHandlePairingQueueMutex);
      if( IsNewThreadRequired() == true )
        {
        AddThread();
        itkDebugMacro(<< "Added a new thread"  );
        }

      // adding to queue
      threadJob.m_Id = m_IdCounter++;
      m_WorkerQueue.push_back(threadJob);

      m_ActiveJobIds.push_back(threadJob.m_Id );

      // un-initialzied below
      // now put the job id in the ThreadJobStruct vector

      for( std::vector<int>::size_type i = 0, threadStructSize = m_ThreadIdHandlePairingQueue.size();
           i != threadStructSize; ++i )
        {
        if( m_ThreadIdHandlePairingQueue[i].m_ThreadNumericId <= JOB_THREADHANDLE_IS_FREE ) //
                                                                                          // only
                                                                                          // if
                                                                                          // it
                                                                                          // is
                                                                                          // JOB_THREADHANDLE_IS_FREE
                                                                                          // or
                                                                                          // just
                                                                                          // added
                                                                                          // thread
                                                                                          // you
          {                                                                               //
                                                                                          // assign
                                                                                          // it
                                                                                          // a
                                                                                          // job
          m_ThreadIdHandlePairingQueue[i].m_ThreadNumericId = threadJob.m_Id;               //
                                                                                          // assign
                                                                                          // the
                                                                                          // job
                                                                                          // id
                                                                                          // to
                                                                                          // the
          // thread now
          returnValue = m_ThreadIdHandlePairingQueue[i].m_ThreadProcessHandle;
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
      } //mutex

    if(GetSemaphoreForThread(returnValue)->SemaphorePost() != 0 )
      {
      itkExceptionMacro(<<"Error in semaphore post");
      }

    return returnValue;

    }
  catch( std::exception& e )
    {
    itkDebugMacro(<< std::endl << "Failed to assign work. \n" << e.what() << std::endl );
    m_ExceptionOccured = true;
    itkExceptionMacro(<< e.what() );
    }
}

}
