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
#ifndef __itkThreadPool_h
#define __itkThreadPool_h
// this didnt work
// #if defined(ITK_USE_PTHREADS)
// so doing this


#if defined(__unix__) || defined(__APPLE__)
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // for sleep
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#if defined __APPLE__
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#include <mach/task_info.h>
#endif

#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <sstream>
#include <string>
#include <utility>
#include "itkThreadJob.h"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkSimpleFastMutexLock.h"
#include "itkMutexLockHolder.h"


#if defined(VERBOSE_THREAD_DEBUG)
#define ThreadDebugMsg( ostreamMsg )  std::cout ostreamMsg
#else
#define ThreadDebugMsg( ostreamMsg )
#endif

namespace itk
{

/**
* \class ThreadPool
* \brief Thread pool manages the threads for itk.
*  Thread pool is called and initialized from within the MultiThreader.
*  Initially the thread pool is started with zero threads.
* Threads are added as job(s) are submitted to the thread pool and if it cannot be
* executed right away. For example : If the thread pool has three threads and all are
* busy. If a new job is submitted to the thread pool, the thread pool checks to see if
* any threads are free. If not, it adds a new thread and executed the job right away.
* The ThreadJob class is used to submit jobs to the thread pool. The ThreadJob's
* necessary variables need to be set and then the ThreadJob can be passed to the
* ThreaadPool by calling its AssignWork Method which returns the thread id on which
* the job is being executed. One can then wait for the job using the thread id and
* calling the WaitForJob method on the thread pool.
* \ingroup OSSystemObjects
* \ingroup ITKCommon
*/
class ITKCommon_EXPORT ThreadPool : public Object
{
public:

  /** Standard class typedefs. */
  typedef ThreadPool               Self;
  typedef Object                   Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThreadPool, Object);

  /** Get a singleton */
  static ThreadPool * GetThreadPool(); // TODO: This should return a const

  // pointer.  Only 1 thread pool can exist
  // from the singleton.

  /** Waits for maxPollSecs and then stops the threadpool */
  void DestroyPool(int maxPollSecs);

  /** This method is called to assign a job to the thread pool */
  ThreadProcessIDType AssignWork(ThreadJob worker); // TODO:  Do you want to

  // pass a copy of the job,
  // or a reference to the
  // job?

  /** Can call this method if we want to pre-start maxThreads in the thread pool
    */
  void InitializeThreads(unsigned int maxThreads);

  /** This method blocks until the given (job) id has finished executing */
  bool WaitForJobOnThreadHandle(ThreadProcessIDType handle);

  /** Returns number of completed jobs */
  int GetCompletedJobs() const;

  /** Gets the thread handle with which the job is associated */
  ThreadProcessIDType GetThreadHandleForJob(int jobId);

  class ThreadSemaphorePair;
  /** This class is only used on windows */
  class ThreadIdHandlePair;
protected:
  ThreadPool();  // Protected so that only the GetThreadPool can create a thread
                 // pool
  ~ThreadPool();

private:
  /** copy constructor is private */
  ThreadPool(ThreadPool const &); // purposely not implemented

  /** Assignment operator is private */
  ThreadPool & operator=(ThreadPool const &); // purposely not implemented

  /** Count of jobs completed */
  int m_CompletedJobs;

  /** Set when the thread pool is to be stopped */
  bool m_ScheduleForDestruction;

  /** Maintains count of threads */
  int m_ThreadCount;

  /** Used by the thread pool to indicate that work (job) is incomplete across
    all threads */
  int m_NumberOfPendingJobsToBeRun;
  /** To lock on m_NumberOfPendingJobsToBeRun */
  static SimpleFastMutexLock m_NumberOfPendingJobsToBeRunMutex;

  /** counter to assign job ids */
  unsigned int m_IdCounter;

  /** set if exception occurs */
  bool m_ExceptionOccured;
#if defined(_WIN32) || defined(_WIN64)
  typedef DWORD ThreadIdType;
#else
  typedef int ThreadIdType;
#endif
  typedef std::vector<ThreadJob> ThreadJobContainterType;
  typedef std::vector<int> ThreadIntsContainerType;
  typedef std::vector<ThreadProcessIDType> ThreadProcessIdContainerType;



  /** this is a list of jobs(ThreadJob) submitted to the thread pool
      this is the only place where the jobs are submitted.
      We need a worker queue because the thread pool assigns work to a
      thread which is free. So when a job is submitted, it has to be stored
      somewhere*/
  ThreadJobContainterType m_WorkerQueue;
  /** To lock on m_WorkerQueue */
  static SimpleFastMutexLock m_WorkerQueueMutex;

  /** this is the list of active jobs (running) across all threads */
  ThreadIntsContainerType m_ActiveJobIds;

  /** Vector to hold all active thread handles */
  ThreadProcessIdContainerType m_ThreadHandles;

  /** Vector of pairs that hold job ids and their corresponding thread handles
    */
  enum {
    JOB_THREADHANDLE_JUST_ADDED=-3, //means thread just added
    JOB_THREADHANDLE_IS_FREE=-2,    // means this particular threadhandle
                                    // (thread) is free
    JOB_THREADHANDLE_IS_DONE=-1     // means the thread finished with the
                                    // assigned job and is waiting until
                                    // WaitForJobOnThreadHandle method is called
                                    // otherwise threadhandle is actively
                                    // running a job.
    };

  class ThreadProcessStatusPair
  {
public:
    ThreadProcessStatusPair(const int & tnid, const ThreadProcessIDType & tph) :
      m_ThreadNumericId(tnid),
      m_ThreadProcessHandle(tph)
    {
    }

    int                 m_ThreadNumericId;
    ThreadProcessIDType m_ThreadProcessHandle;

private:
    ThreadProcessStatusPair(); //purposefully not implemented.
  };

  typedef std::vector<ThreadProcessStatusPair> ThreadIDHandlePairingContainerType;
  ThreadIDHandlePairingContainerType m_ThreadIDHandlePairingQueue;

  ThreadSemaphorePair *m_ThreadSemHandlePairingQueue;
  ThreadSemaphorePair *m_ThreadSemHandlePairingForWaitQueue;

  /** Used under windows to keep a id - handle pair */
  ThreadIdHandlePair *m_ThreadIdHandleList;

  void AddNodeToTSPariLinkedList(ThreadSemaphorePair *head, ThreadProcessIDType handle);

  /** To lock on the vectors */
  static SimpleFastMutexLock m_ThreadIDHandlePairingQueueMutex;

  /** This function is called by the threads to get jobs to be executed.
      This method is the "first" call from the threads in the thread pool.
      Now this method blocks the thread until a job is available for the thread to execute.
      Once a job is available(known from m_ThreadIDHandlePairingQueue), it gets it from the worker queue and
      returns it   */
  ThreadJob FetchWork(ThreadProcessIDType t);

  /** Used to remove an active job id from the queue because it is done.
      This method is called by the threads once they are done with the job.
      This method removes the job id from m_ActiveJobIds, marks the thread as free
      by setting appropriate values in the m_ThreadIDHandlePairingQueue vector and removes the
      job from m_WorkerQueue  */
  void RemoveActiveId(int id);

  /** Called to add a thread to the thread pool.
      This method add a thread to the thread pool and pushes the thread handle
      into the m_ThreadHandles vector*/
  void AddThread();

  /** To check if the thread pool has to add a thread.
      This method checks if any threads in the thread pool
      are free. If so, it returns false else returns true */
  bool DoIAddAThread();

  static ThreadPool* m_ThreadPoolInstance;
  /** To lock on m_ThreadPoolInstance */
  static SimpleFastMutexLock m_ThreadPoolInstanceMutex;
  /** Used to yield a singleton instance */
  static bool m_ThreadPoolSingletonExists;

  ThreadSemaphorePair* GetSemaphoreForThread(ThreadProcessIDType threadHandle);

  ThreadSemaphorePair* GetSemaphoreForThreadWait(ThreadProcessIDType threadHandle);

  void DeallocateThreadLinkedList(ThreadSemaphorePair *list);

  /** thread function */
  static void * ThreadExecute(void *param);

  /** Method to compare thread handles - true for same false for different */
  bool CompareThreadHandles(ThreadProcessIDType t1, ThreadProcessIDType t2);

  /** Used under windows - gets the thread handle associated with thread id */
  ThreadProcessIDType GetThreadHandleForThreadId(ThreadIdType id);

  /** Used under windows - For adding to linked list m_ThreadIdHandleList*/
  void AddNodeToThreadIdHandleList(ThreadIdHandlePair *head, ThreadProcessIDType handle, ThreadIdType id);

};

}
#endif
