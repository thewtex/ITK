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
#ifndef __itkThreadIdHandlePair_h
#define __itkThreadIdHandlePair_h
#include "itkThreadPool.h"

namespace itk
{
/**
 * \class ThreadIdHandlePair
 * \brief Inner class to the ThreadPool
 * \ingroup OSSystemObjects
 * \ingroup ITKCommon
 */

  class ThreadPool::ThreadIdHandlePair
  {
public:
ThreadIdHandlePair(const ThreadProcessIDType & tph, const ThreadIdType & id ):
    m_Id(id),
    m_ThreadProcessHandle(tph)
{}
    ThreadIdType         m_Id;
    ThreadIdHandlePair  *m_Next;
    ThreadProcessIDType  m_ThreadProcessHandle;

private:
    ThreadIdHandlePair(); //purposefully not implemented.
  };
  }

#endif
