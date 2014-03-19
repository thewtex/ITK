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
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

namespace itk
{


ThreadPool
::ThreadSemaphorePair
::ThreadSemaphorePair(const ThreadProcessIdType & tph) :
  m_ThreadProcessHandle(tph)
{
#if defined(__APPLE__)
  if( semaphore_create(current_task(), &m_Semaphore, SYNC_POLICY_FIFO, 0) != KERN_SUCCESS)
    {
    itkGenericExceptionMacro(<<std::endl<<"m_Semaphore cannot be initialized. " << strerror(errno));
    }
#else
  sem_init(&m_Semaphore, 0, 0);
#endif
}

int
ThreadPool
::ThreadSemaphorePair
::SemaphoreWait()
{
#if defined(__APPLE__)
  if(semaphore_wait(m_Semaphore) == KERN_SUCCESS)
    {
    return 0;
    }
  else
    {
    return -1;
    }
#else
  return sem_wait(&m_Semaphore);
#endif
}

int
ThreadPool
::ThreadSemaphorePair
::SemaphorePost()
{
#if defined(__APPLE__)
  if(semaphore_signal(m_Semaphore) == KERN_SUCCESS)
    {
    return 0;
    }
  else
    {
    return -1;
    }
#else
  return sem_post(&m_Semaphore);
#endif
}

void
ThreadPool
::AddThread()
{
  pthread_attr_t attr;

  pthread_attr_init(&attr);

#if !defined( __CYGWIN__ )
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
#endif

  SimpleFastMutexLock tcLock;
  tcLock.Lock();
  tcLock.Unlock();

  pthread_t newlyAddedThreadHandle;
  const int rc = pthread_create(&newlyAddedThreadHandle, &attr, &ThreadPool::ThreadExecute, (void *)this );
  if( rc )
    {
    itkDebugStatement(std::cerr << "ERROR; return code from pthread_create() is " << rc << std::endl);
    itkExceptionMacro(<< "Cannot create thread. Error in return code from pthread_create()");
    }
  else
    {

    m_ThreadHandles.insert(newlyAddedThreadHandle);
    m_ThreadProcessIdentifiersVector.push_back(ThreadProcessIdentifiers(JOB_THREADHANDLE_JUST_ADDED,
                                                                   newlyAddedThreadHandle,
                                                                   0));

    m_ThreadSemHandlePairingForWaitQueue.push_back(new ThreadSemaphorePair(newlyAddedThreadHandle));
    m_ThreadSemHandlePairingQueue.push_back(new ThreadSemaphorePair(newlyAddedThreadHandle));

    itkDebugMacro(<< "Thread created with handle :" << newlyAddedThreadHandle << std::endl );
    }

}

bool
ThreadPool
::CompareThreadHandles(ThreadProcessIdType t1, ThreadProcessIdType t2)
{
  return (pthread_equal(t1, t2) == 0 ? false : true);
}


void
ThreadPool
::DeallocateThreadSemSet(ThreadSemHandlePairingQueueType &q)
{
  for(ThreadSemHandlePairingQueueType::iterator it = q.begin(), end = q.end();
      it != end; ++it)
     {
 #if !defined(__APPLE__)
     sem_destroy(&((*it)->m_Semaphore));
 #endif
     delete *it;
     }
  q.clear();
}

/*
  This method first checks if all threads are finished executing their jobs.
  If now, it will wait for "poolTime" seconds and then re-check.
  Once all are done it will Destroy the thread pool. - Need to revisit
*/
void
ThreadPool
::DestroyPool(int pollTime = 2)
{
  m_ScheduleForDestruction = true;
  while( m_NumberOfPendingJobsToBeRun > 0 )
    {
    itkDebugMacro(<< "Work is still incomplete=" << m_NumberOfPendingJobsToBeRun << std::endl );
    usleep(pollTime*1000000); // usleep takes microseconds
    }

  itkDebugMacro(<< "All threads are done" << std::endl << "Terminating threads" << std::endl );
  for(ThreadProcessIdContainerType::iterator it = this->m_ThreadHandles.begin();
      it != this->m_ThreadHandles.end(); ++it)
    {
    const int s = pthread_cancel(*it);
    if( s != 0 )
      {
      itkDebugMacro(<< "Cannot cancel thread with id : " << *it << std::endl );
      }
    }

  DeallocateThreadSemSet(m_ThreadSemHandlePairingQueue);
  DeallocateThreadSemSet(m_ThreadSemHandlePairingForWaitQueue);

  //clear vectors
  m_ThreadProcessIdentifiersVector.clear();
  ThreadProcessIdentifiersVecType(m_ThreadProcessIdentifiersVector).swap(m_ThreadProcessIdentifiersVector);

  m_ThreadHandles.clear();
  ThreadProcessIdContainerType(m_ThreadHandles).swap(m_ThreadHandles);

  m_ActiveJobIds.clear();
  ThreadJobIdsContainerType(m_ActiveJobIds).swap(m_ActiveJobIds);

  m_WorkerQueue.clear();
  ThreadJobContainerType(m_WorkerQueue).swap(m_WorkerQueue);
}

// Thread function
void *
ThreadPool
::ThreadExecute(void *param)
{
  // get the parameters passed in
  ThreadPool *pThreadPool = reinterpret_cast<ThreadPool *>(param);
  if(pThreadPool == ITK_NULLPTR)
    {
    itkGenericExceptionMacro(<< "param can't be converted to ThreadPool type");
    }
  // TODO: What is proper action if pThreadPool is 0?
  const int s = pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, ITK_NULLPTR);

  if( s != 0 )
    {
    //TODO: What is proper action if s != 0?
    itkGenericExceptionMacro(<< "Cannot pthread_setcancelstate" << std::endl );
    }

  while( !pThreadPool->m_ScheduleForDestruction )
    {
    //TODO: Replace in both Pthread and WinThread  the return value of
    // FetchWork.
    ThreadJob currentPThreadJob = pThreadPool->FetchWork(pthread_self() );
    if(currentPThreadJob.m_Assigned == false )
      {
      itkDebugStatement(std::cerr << "\n Empty job returned from FetchWork so ignoring and continuing ..\n\n");
      continue;
      }
    currentPThreadJob.m_ThreadFunction(currentPThreadJob.m_UserData);
    pThreadPool->RemoveActiveId(currentPThreadJob.m_Id );

    //signal that current job is done
    if(pThreadPool->GetSemaphoreForThreadWait(pthread_self())->SemaphorePost() != 0)
      {
      itkGenericExceptionMacro(<<"******************************Error in semaphore post");
      }
    }
  pthread_exit(ITK_NULLPTR);
}

}
