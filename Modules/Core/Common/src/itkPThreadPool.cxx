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

namespace itk
{

void ThreadPool::AddThread()
{
  ThreadDebugMsg( << "In add thread" << std::endl );

  this->m_ThreadCount++;
  pthread_t newlyAddedThreadHandle;
  const int rc = pthread_create(&newlyAddedThreadHandle, NULL, &ThreadPool::ThreadExecute, (void *)this );
  if( rc )
    {
    itkDebugMacro(<< "ERROR; return code from pthread_create() is " << rc << std::endl);
    itkExceptionMacro(<< "Cannot create thread. Error in return code from pthread_create()");
    }
  else
    {
      {
      MutexLockHolder<SimpleFastMutexLock> threadStructMutexHolder(m_ThreadIDHandlePairingQueueMutex);
      m_ThreadHandles.push_back(newlyAddedThreadHandle);
      m_ThreadIDHandlePairingQueue.push_back(ThreadProcessStatusPair(-2, newlyAddedThreadHandle) );
      }

    itkDebugMacro(<< "Thread created with handle :" << m_ThreadHandles[m_ThreadCount - 1] << std::endl );
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
}

// Thread function
void * ThreadPool::ThreadExecute(void *param)
{
  // get the parameters passed in
  ThreadPool *pThreadPool = reinterpret_cast<ThreadPool *>(param);
  // TODO: What is proper action if pThreadPool is NULL?
  const int   s = pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  if( s != 0 )
    {
    //TODO: What is proper action if s != 0?
    itkDebugMacro(<< "Cannot pthread_setcancelstate" << std::endl );
    }

  while( !pThreadPool->m_ScheduleForDestruction )
    {
    //TODO: Replace in both Pthread and WinThread  the return value of FetchWork.
    ThreadJob currentPThreadJob = pThreadPool->FetchWork(pthread_self() );
    if( currentPThreadJob.m_Id < 0 || currentPThreadJob.m_Assigned == false )
      {
      //TODO:  Remove all std::cout
      ThreadDebugMsg( << "\n Empty job returned from FetchWork so ignoring and continuing ..\n\n");
      continue;
      }
    ThreadDebugMsg( << "\n In thread pool thread " << pthread_self() << " : Work fetched. Job id is : " << currentPThreadJob.m_Id << std::endl);
    currentPThreadJob.m_ThreadArgs.m_ThreadFunction(currentPThreadJob.m_ThreadArgs.otherArgs);
    ThreadDebugMsg(<< "\n Thread done with job id :" << currentPThreadJob.m_Id << "\n Now removing...\n\n");
    pThreadPool->RemoveActiveId(currentPThreadJob.m_Id );
      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_NumberOfPendingJobsToBeRunMutex);
      pThreadPool->m_NumberOfPendingJobsToBeRun--;
      }
    }
  pthread_exit(NULL);
}

}
