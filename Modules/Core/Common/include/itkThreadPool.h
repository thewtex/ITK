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

#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <utility>
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

protected:

  ThreadPool();
  ~ThreadPool();
private:
  static ThreadPool *m_SThreadPoolInstance;

  /** Used to yield a singleton instance */
  static bool m_InstanceFlag;

  /** copy constructor is private */
  ThreadPool(ThreadPool const &);

  /** Assignment operator is private */
  ThreadPool & operator=(ThreadPool const &);

  /** Count of jobs completed */
  int m_CompletedJobs;

  /** Set when the thread pool is to be stopped */
  bool m_ScheduleForDestruction;

  /** Maintains count of threads */
  int m_ThreadCount;

  /** this is a list of jobs(ThreadJob) submitted to the thread pool
      this is the only place where the jobs are submitted.
      We need a worker queue because the thread pool assigns work to a
      thread which is free. So when a job is submitted, it has to be stored
      somewhere*/
  Queue m_WorkerQueue;

  /** this is the list of active jobs (running) across all threads */
  std::vector<int> m_ActiveJobIds;

  /** Used by the thread pool to indicate that work (job) is incomplete across
    all threads */
  int m_IncompleteWork;

  /** Size of worker queue */
  int m_QueueSize;

  /** counter to assign job ids */
  unsigned int m_IdCounter;

  /** set if exception occurs */
  bool m_ExceptionOccured;

  /** Mutexes */
  /** To lock on the vectors */
  static SimpleFastMutexLock m_VectorQMutex;
  /** To lock on m_IncompleteWork */
  static SimpleFastMutexLock m_MutexWorkCompletion;
  /** To lock on m_InstanceFlag */
  static SimpleFastMutexLock m_CreateInstanceMutex;

  /** Vector to hold all active thread handles */
  std::vector<ThreadProcessIDType> m_ThreadHandles;

  /** Vector of pairs that hold job ids and their corresponding thread handles
    */
  /** Here the job id can be -2, -1 or a positive number.
     -2 means this particular threadhandle (thread) is free
     -1 means the thread finished with the assigned job and is waiting until WaitForJobOnThreadHandle method is called
     +ve means the thread is running this job id   */
  std::vector<std::pair<int, ThreadProcessIDType> > m_ThreadStructs;

  /** This function is called by the threads to get jobs to be executed.
      This method is the "first" call from the threads in the thread pool.
      Now this method blocks the thread until a job is available for the thread to execute.
      Once a job is available(known from m_ThreadStructs), it gets it from the worker queue and
      returns it   */
  ThreadJob FetchWork(ThreadProcessIDType t);

  /** thread function */
  static void * ThreadExecute(void *param);

  /** Used to remove an active job id from the queue because it is done.
      This method is called by the threads once they are done with the job.
      This method removes the job id from m_ActiveJobIds, marks the thread as free
      by setting appropriate values in the m_ThreadStructs vector and removes the
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

};

}
#endif
