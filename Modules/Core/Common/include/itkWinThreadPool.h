

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
#include "itkThreadPoolDebug.h"

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

  static ThreadPool & GetPoolInstance();

  static ThreadPool & GetPoolInstance(int maxThreads);

  static SmartPointer<ThreadPool> GetSmartPoolInstance(int maxThreads);

  static void DeleteInstance();

  void DestroyPool(int maxPollSecs);

  int AssignWork(WinJob worker);

  void InitializeThreads(int maxThreads);

  bool WaitForThread(int id);
  
  int GetCompletedJobs();

protected:

  ThreadPool();
  ~ThreadPool();
private:
  int CompletedJobs;
  bool Destroy;
  static ThreadPool * ThreadPoolInstance;
  static SmartPointer<ThreadPool> SThreadPoolInstance;
  static bool InstanceFlag;
  ThreadPool(ThreadPool const &);              // copy constructor is private
  ThreadPool & operator=(ThreadPool const &);  // assignment operator is

  // private

  void RemoveActiveId(int id);

  int                 MaxThreads;
  HANDLE              WorkAvailable;     // semaphore
  std::vector<WinJob> WorkerQueue;
  std::vector<int>    ActiveThreadIds;
  int                 IncompleteWork;
  int                 QueueSize;
  int                 IdCounter;
  bool                ExceptionOccured;
  HANDLE*             ThreadHandles;
  static HANDLE       MutexSync;           // mutex
  static HANDLE       ActiveThreadMutex;   // mutex
  static HANDLE       MutexWorkCompletion; // mutex

  WinJob FetchWork();

  static void * ThreadExecute(void *param);

};

}
#endif
#endif
