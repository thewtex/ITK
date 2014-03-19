

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

#ifndef __ThreadPoolFactory_h
#define __ThreadPoolFactory_h

#ifdef __linux__
#include "itkPThreadPool.h"
#elif _WIN32
#include "itkWinThreadPool.h"
#elif _WIN64
#include "itkWinThreadPool.h"
#endif

namespace itk
{

class ThreadPoolFactory
{
public:
  ThreadPool & GetThreadPool(int maxThreads);
  SmartPointer<ThreadPool> GetSmartThreadPool(int maxThreads);

};
}
#endif

