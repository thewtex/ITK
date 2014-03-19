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
#ifndef __itkPthreadPool_h
#define __itkPthreadPool_h
#if defined(ITK_USE_WIN32_THREADS)
#else
#include <pthread.h>
#include <semaphore.h>
#endif
#include <iostream>
#include <vector>
#include "itkThreadJob.h"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkSimpleFastMutexLock.h"
#include "itkMutexLockHolder.h"

namespace itk
{

class ITKCommon_EXPORT ThreadPool : public Object
{
public:

  /** Standard class typedefs. */
  typedef ThreadPool               Self;
  typedef Object                   Superclass;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef std::vector<ThreadJob>   Queue;
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThreadPool, Object);

  static SmartPointer<ThreadPool> GetThreadPool();

  static SmartPointer<ThreadPool> GetNewThreadPool();

  static void DeleteSingletonInstance();

  /** Waits for maxPollSecs and then stops the threadpool */
  void DestroyPool(int maxPollSecs);

  /** This method is called to assign a job to the thread pool */
  int AssignWork(ThreadJob worker);

  /** Can call this method if we want to pre-start maxThreads in the thread pool */
  void InitializeThreads(int maxThreads);

  /** This method blocks until the given (job) id has finished executing */
  bool WaitForJob(int id);

  /** Returns number of completed jobs */
  int GetCompletedJobs() const;

  /** Gets the thread handle with which the job is associated */
  ThreadProcessIDType GetThreadHandleForJob(int jobId);

  /** Gets the job id which is currently associated with the thread handle */
  int GetJobIdForThreadHandle(ThreadProcessIDType pt);

protected:

  ThreadPool();
  ~ThreadPool();

private:

  /** Count of jobs completed */
  int                             m_CompletedJobs;

  /** Set when the thread pool is to be stopped */
  bool                            m_ScheduleForDestruction;

  static SmartPointer<ThreadPool> m_SThreadPoolInstance;

  /** Used to yield a singleton instance */
  static bool                     m_InstanceFlag;

  /** copy constructor is private */
  ThreadPool(ThreadPool const &);

  /** Assignment operator is private */
  ThreadPool & operator=(ThreadPool const &);

  /* Maintains count of threads */
  int m_ThreadCount;

  /** Semaphore used by threads to wait until work (job) is available */
#if defined(ITK_USE_WIN32_THREADS)
  HANDLE m_WorkAvailable;
#else
  sem_t m_WorkAvailable;
#endif

  /** this is a list of jobs submitted to the thread pool */
  Queue                      m_WorkerQueue;

  /** this is the list of active jobs (running) */
  std::vector<int>           m_ActiveJobIds;

  /** Used by the thread pool to indicate that work (job) is incomplete */
  int                        m_IncompleteWork;

  /** Size of worker queue */
  int                        m_QueueSize;

  /** counter to assign job ids */
  unsigned int               m_IdCounter;

  /** set if exception occurs */
  bool                       m_ExceptionOccured;

  /** Mutexes */
  static SimpleFastMutexLock m_MutexSync;
  static SimpleFastMutexLock m_ActiveThreadMutex;
  static SimpleFastMutexLock m_MutexWorkCompletion;
  static SimpleFastMutexLock m_ThreadIdAccessMutex;

  /** Vector to hold all active thread handles */
  std::vector<ThreadProcessIDType> m_ThreadHandles;

  /** vectors to hold job ids and corresponding thread ids */
  std::vector<int>                 m_JobIds;
  std::vector<ThreadProcessIDType> m_ThreadIds;

  /* map of semaphores used to wait when GetThreadHandleForJob method is called */
#if defined(ITK_USE_WIN32_THREADS)
  std::map <int, HANDLE> m_JobSemaphores;
#else
  std::map <int, sem_t> m_JobSemaphores;
#endif

  /* This function is called by the threads to get jobs to be executed */
  ThreadJob FetchWork();

  /** thread function */
  static void * ThreadExecute(void *param);

  /** Used to remove an active job id from the queue because it is done */
  void RemoveActiveId(int id);

  /* Called to add a thread to the thread pool */
  void AddThread();

};

}
#endif
