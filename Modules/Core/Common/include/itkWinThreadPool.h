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

#ifndef __itkWinThreadPool_h
#define __itkWinThreadPool_h

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <iostream>
#include <vector>
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkWinJob.h"
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

/** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThreadPool, Object);

  static Pointer GetThreadPool();

  static Pointer GetNewThreadPool();

  static void DeleteSingletonInstance();

  void DestroyPool(int maxPollSecs);

  int AssignWork(WinJob worker);

  void InitializeThreads(int maxThreads);

  bool WaitForThread(int id);

  int GetCompletedJobs() const;

protected:

  ThreadPool();
  ~ThreadPool();

private:
  int                             m_CompletedJobs;
  bool                            m_ScheduleForDestruction;
  static SmartPointer<ThreadPool> m_SThreadPoolInstance;
  static bool                     m_InstanceFlag;
  ThreadPool(ThreadPool const &);              // copy constructor is private
  ThreadPool & operator=(ThreadPool const &);  // assignment operator is

  void RemoveActiveId(int id);

  int                 m_ThreadCount;
  HANDLE              m_WorkAvailable;     // semaphore
  std::vector<WinJob> m_WorkerQueue;
  std::vector<int>    m_ActiveThreadIds;
  int                 m_IncompleteWork;
  unsigned int        m_IdCounter;
  bool                m_ExceptionOccured;
  std::vector<HANDLE> m_ThreadHandles;
  //static HANDLE       m_MutexSync;           // mutex
  //static HANDLE       m_ActiveThreadMutex;   // mutex
  //static HANDLE       m_MutexWorkCompletion; // mutex
  static SimpleFastMutexLock m_MutexSync;
  static SimpleFastMutexLock m_ActiveThreadMutex;
  static SimpleFastMutexLock m_MutexWorkCompletion;

  WinJob FetchWork();

  static void * ThreadExecute(void *param);

  void AddThread();

};

}
#endif
#endif
