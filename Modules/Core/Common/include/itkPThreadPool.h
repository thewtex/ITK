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

#ifdef __unix__
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <vector>
#include "itkPThreadJob.h"
#include "itkObject.h"
#include "itkObjectFactory.h"


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

  void DestroyPool(int maxPollSecs);

  int AssignWork(ThreadJob worker);

  void InitializeThreads(int maxThreads);

  bool WaitForThread(int id);

  int GetCompletedJobs() const;

protected:

  ThreadPool();
  ~ThreadPool();
private:
  int m_CompletedJobs;
  bool m_ScheduleForDestruction;
  static SmartPointer<ThreadPool> m_SThreadPoolInstance;
  static bool m_InstanceFlag;
  ThreadPool(ThreadPool const &);              // copy constructor is private
  ThreadPool & operator=(ThreadPool const &);  // assignment operator is
  int                    m_ThreadCount;
  sem_t                  m_WorkAvailable;
  Queue 		 m_WorkerQueue;
  std::vector<int>       m_ActiveThreadIds;
  int                    m_IncompleteWork;
  int                    m_QueueSize;
  unsigned int           m_IdCounter;
  bool                   m_ExceptionOccured;
  static pthread_mutex_t m_MutexSync;
  static pthread_mutex_t m_ActiveThreadMutex;
  static pthread_mutex_t m_MutexWorkCompletion;
  std::vector<pthread_t> m_ThreadHandles;

  ThreadJob FetchWork();
  static void * ThreadExecute(void *param);
  void RemoveActiveId(int id);
  void AddThread();
};

}
#endif
#endif

