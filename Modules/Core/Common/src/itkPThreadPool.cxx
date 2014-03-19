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
#include "itkThreadSemaphorePair.h"
#include "itkThreadIdHandlePair.h"

namespace itk
{


ThreadPool::ThreadSemaphorePair::ThreadSemaphorePair(const ThreadProcessIDType & tph) :
      m_ThreadProcessHandle(tph)
    {
#if defined(__APPLE__)
     if( semaphore_create(current_task(), &m_Semaphore, SYNC_POLICY_FIFO, 0) != KERN_SUCCESS)
     {
      std::cout<<std::endl<<"m_Semaphore cannot be initialized. "<<strerror(errno);
      exit(-1);
     }
#else
       sem_init(&m_Semaphore, 0, 0);
#endif
    }

    int ThreadPool::ThreadSemaphorePair::SemaphoreWait()
    {
#if defined(__APPLE__)
      //return sem_wait(m_Semaphore);
        if(semaphore_wait(m_Semaphore) == KERN_SUCCESS)
         return 0;
        else
         return -1;
#else
      return sem_wait(&m_Semaphore);
#endif
    }

    int ThreadPool::ThreadSemaphorePair::SemaphorePost()
    {
#if defined(__APPLE__)
      //return sem_post(m_Semaphore);
      if(semaphore_signal(m_Semaphore) == KERN_SUCCESS)
       return 0;
      else
       return -1;
#else
      return sem_post(&m_Semaphore);
#endif
    }

void ThreadPool::AddThread()
{
  pthread_attr_t attr;

  pthread_attr_init(&attr);

#if !defined( __CYGWIN__ )
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
#endif
  ThreadDebugMsg( << "In add thread" << std::endl );

  this->m_ThreadCount++;
  pthread_t newlyAddedThreadHandle;
  const int rc = pthread_create(&newlyAddedThreadHandle, &attr, &ThreadPool::ThreadExecute, (void *)this );
  if( rc )
    {
    itkDebugMacro(<< "ERROR; return code from pthread_create() is " << rc << std::endl);
    itkExceptionMacro(<< "Cannot create thread. Error in return code from pthread_create()");
    }
  else
    {

    m_ThreadHandles.push_back(newlyAddedThreadHandle);
    m_ThreadIDHandlePairingQueue.push_back(ThreadProcessStatusPair(JOB_THREADHANDLE_JUST_ADDED,
                                                                   newlyAddedThreadHandle) );

    //first time
    if(m_ThreadSemHandlePairingForWaitQueue == NULL)
      {
      m_ThreadSemHandlePairingForWaitQueue = new ThreadSemaphorePair(newlyAddedThreadHandle);
      m_ThreadSemHandlePairingForWaitQueue->m_Next = NULL;
      }
    else
      {
      AddNodeToTSPariLinkedList(m_ThreadSemHandlePairingForWaitQueue, newlyAddedThreadHandle);
      }
    if(m_ThreadSemHandlePairingQueue == NULL)
      {
      m_ThreadSemHandlePairingQueue = new ThreadSemaphorePair(newlyAddedThreadHandle);
      m_ThreadSemHandlePairingQueue->m_Next = NULL;
      }
    else
      {
      AddNodeToTSPariLinkedList(m_ThreadSemHandlePairingQueue, newlyAddedThreadHandle);
      }

    itkDebugMacro(<< "Thread created with handle :" << m_ThreadHandles[m_ThreadCount - 1] << std::endl );
    }

}

bool ThreadPool::CompareThreadHandles(ThreadProcessIDType t1, ThreadProcessIDType t2)
{
  return (pthread_equal(t1, t2) == 0 ? false : true);
}


void ThreadPool::DeallocateThreadLinkedList(ThreadSemaphorePair *list)
{
  ThreadSemaphorePair *node = list;
  ThreadSemaphorePair *m_Next = list;

  while(node!=NULL)
    {
    m_Next = node->m_Next;
    #if defined(__APPLE__)
    //sem_close(node->m_Semaphore);
    #else
    sem_destroy(&(node->m_Semaphore));
    #endif
    delete node;
    node = m_Next;
    }
}

/*
  This method first checks if all threads are finished executing their jobs.
  If now, it will wait for "poolTime" seconds and then re-check.
  Once all are done it will Destroy the thread pool. - Need to re-visit
*/
void ThreadPool::DestroyPool(int pollTime = 2)
{
  m_ScheduleForDestruction = true;
  while( m_NumberOfPendingJobsToBeRun > 0 )
    {
    itkDebugMacro(<< "Work is still incomplete=" << m_NumberOfPendingJobsToBeRun << std::endl );
    // TODO:  sleep is not portable across platforms
    sleep(pollTime);
    }

  itkDebugMacro(<< "All threads are done" << std::endl << "Terminating threads" << std::endl );
  for( int i = 0; i < m_ThreadCount; ++i )
    {
    const int s = pthread_cancel(m_ThreadHandles[i]);
    if( s != 0 )
      {
      itkDebugMacro(<< "Cannot cancel thread with id : " << m_ThreadHandles[i] << std::endl );
      }
    }

  DeallocateThreadLinkedList(m_ThreadSemHandlePairingQueue);
  DeallocateThreadLinkedList(m_ThreadSemHandlePairingForWaitQueue);

  //clear vectors
  m_ThreadIDHandlePairingQueue.clear();
  std::vector<ThreadPool::ThreadProcessStatusPair>(m_ThreadIDHandlePairingQueue).swap(m_ThreadIDHandlePairingQueue);

  m_ThreadHandles.clear();
  std::vector<ThreadProcessIDType>(m_ThreadHandles).swap(m_ThreadHandles);

  m_ActiveJobIds.clear();
  std::vector<int>(m_ActiveJobIds).swap(m_ActiveJobIds);

  m_WorkerQueue.clear();
  std::vector<ThreadJob>(m_WorkerQueue).swap(m_WorkerQueue);
}

pthread_t ThreadPool::GetThreadHandleForThreadId(ThreadIdType id)
{

  MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIDHandlePairingQueueMutex);
  ThreadIdHandlePair *                node = m_ThreadIdHandleList;
  do
    {
    if(node->m_Id == id)
      {
      return node->m_ThreadProcessHandle;
      }
    node = node->m_Next;
    } while(node!= NULL);

    itkExceptionMacro(<< "Error occured finding thread handle for thread id " << id);
}


// Thread function
void * ThreadPool::ThreadExecute(void *param)
{
  // get the parameters passed in
  ThreadPool *pThreadPool = reinterpret_cast<ThreadPool *>(param);
  // TODO: What is proper action if pThreadPool is NULL?
  const int s = pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  if( s != 0 )
    {
    //TODO: What is proper action if s != 0?
    ThreadDebugMsg(<< "Cannot pthread_setcancelstate" << std::endl );
    }

  while( !pThreadPool->m_ScheduleForDestruction )
    {
    //TODO: Replace in both Pthread and WinThread  the return value of
    // FetchWork.
    ThreadJob currentPThreadJob = pThreadPool->FetchWork(pthread_self() );
    if( currentPThreadJob.m_Id < 0 || currentPThreadJob.m_Assigned == false )
      {
      //TODO:  Remove all std::cout
      ThreadDebugMsg( << "\n Empty job returned from FetchWork so ignoring and continuing ..\n\n");
      continue;
      }
    currentPThreadJob.m_ThreadFunction(currentPThreadJob.m_otherArgs);
    ThreadDebugMsg(<< "\n Thread done with job id :" << currentPThreadJob.m_Id << "\n Now removing...\n\n");
    pThreadPool->RemoveActiveId(currentPThreadJob.m_Id );

    //signal that current job is done
    if(pThreadPool->GetSemaphoreForThreadWait(pthread_self() )->SemaphorePost()!=0)
      {
      ThreadDebugMsg(<<"******************************Error in semaphore post");
      }
    }
  pthread_exit(NULL);
}

}
