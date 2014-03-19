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
#include "itkThreadPool.h"
#include <unistd.h>

namespace itk
{

void ThreadPool::AddThread()
{
  std::cout<<"In add thread"<<std::endl;

  this->m_ThreadCount++;
  pthread_t aThreadHandle;
  const int rc = pthread_create(&aThreadHandle, NULL, &ThreadPool::ThreadExecute, (void *)this );
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

int ThreadPool::AssignWork(ThreadJob pthreadJob)
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

      pthreadJob.Id = m_IdCounter++;
      m_WorkerQueue.push_back(pthreadJob);
      itkDebugMacro(
        << "Assigning Worker[" << pthreadJob.Id << "] Address:[" << &pthreadJob << "] to Queue " <<
        std::endl );
      }

      {
      MutexLockHolder<SimpleFastMutexLock> mutexHolderAct(m_ActiveThreadMutex);
      itkDebugMacro(<< std::endl << "Adding id " << pthreadJob.Id << " to ActiveThreadIds" << std::endl );
      m_ActiveJobIds.push_back(pthreadJob.Id );
      itkDebugMacro(<< std::endl << "ActiveThreadids size : " << m_ActiveJobIds.size() << std::endl );
      std::cout<< std::endl << "ActiveThreadids size : " << m_ActiveJobIds.size() << std::endl ;
      }
    sem_t aSemaphore;
    sem_init(&aSemaphore, 0, 0);
    m_JobSemaphores[pthreadJob.Id] = aSemaphore;

    sem_post(&m_WorkAvailable);

    return pthreadJob.Id;
    }
  catch( std::exception& e )
    {
    m_ExceptionOccured = true;
    itkDebugMacro(<< std::endl << "Failed to assign work. \n" << e.what() << std::endl );
    itkExceptionMacro(<< e.what() );
    }
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

//we need to wait here through a semaphore because the thread executing the job
// has to set the map we are accessing
pthread_t ThreadPool::GetThreadHandleForJob(int jobId){
  sem_wait(&m_JobSemaphores[jobId]);
    {
    MutexLockHolder<SimpleFastMutexLock> threadIdMutexHolder(m_ThreadIdAccessMutex);
    int                                  pos = std::find(m_JobIds.begin(), m_JobIds.end(), jobId) - m_JobIds.begin();

    return m_ThreadIds[pos];
    }

}

void * ThreadPool::ThreadExecute(void *param)
{
  // get the parameters passed in
  ThreadPool *pThreadPool = (ThreadPool *)param;
  const int   s = pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  if( s != 0 )
    {
    }

  while( !pThreadPool->m_ScheduleForDestruction )
    {
    try
      {
      ThreadJob currentPThreadJob = pThreadPool->FetchWork();
      std::cout<<"\n Work fetched\n";
      //add the thread id and job id to map
        {
        MutexLockHolder<SimpleFastMutexLock> threadIdMutexHolder(m_ThreadIdAccessMutex);
        pThreadPool->m_ThreadIds.push_back(pthread_self() );
        pThreadPool->m_JobIds.push_back(currentPThreadJob.Id);
        }
      sem_post(&(pThreadPool->m_JobSemaphores[currentPThreadJob.Id]) );
      currentPThreadJob.ThreadFunction(currentPThreadJob.ThreadArgs.otherArgs);

      pThreadPool->RemoveActiveId(currentPThreadJob.Id );

        {
        MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_MutexWorkCompletion);
        pThreadPool->m_IncompleteWork--;
        }
      }
    catch( std::exception & e )
      {
      pThreadPool->m_ExceptionOccured = true;
      }
    }

  return 0;
}

}
