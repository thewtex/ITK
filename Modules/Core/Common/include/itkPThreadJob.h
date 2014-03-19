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
#ifndef __itkPThreadJob_h__
#define __itkPThreadJob_h__
#include <pthread.h>

namespace itk
{

/** The ThreadJob class does
 * blah blah blah
 * TODO: Umang:  What is this class for?
 */
class ThreadJob
{
public:
  struct ThreadArgs
    {
    void *otherArgs;
    };
  //typedef int ThreadNamingType;
  typedef uint64_t ThreadNamingType; // Using 64bit type to force type errors
  ThreadJob() :
    m_ThreadName(123456789),
    m_PThreadID(),
    Assigned(false),
    Executed(false),
    UserData(NULL),
    ThreadHandle()
  {
    ThreadArgs.otherArgs = NULL;
    itkDebugMacro(<<   "Starting thread \t address=" << this << std::endl );
  }

  ~ThreadJob()
  {
    itkDebugMacro(<<  std::endl << "Thread finished. Ptid is  " << Ptid << "\t address=" << this << std::endl );
  }

  ThreadNamingType GetThreadName() const
  {
    return m_ThreadName;
  }

  void SetThreadName( const ThreadNamingType & tn )
  {
    this->m_ThreadName = tn;
  }

  ThreadProcessIDType GetPThreadID() const
  {
    return this->m_PThreadID;
  }

  void SetPThreadID( ThreadProcessIDType PThreadID )
  {
    this->m_PThreadID = PThreadID;
  }

private:
  ThreadNamingType    m_ThreadName;
  ThreadProcessIDType m_PThreadID;

public:
  bool   Assigned;
  bool   Executed;
  void * UserData;    // any data user wants the function to use

  ThreadFunctionType  ThreadFunction;  //void * (*ThreadFunction)(void *ptr);
  ThreadProcessIDType ThreadHandle;
  ThreadArgs          ThreadArgs;
};

} // End namespace itk

#endif // __itkPThreadJob_h__
