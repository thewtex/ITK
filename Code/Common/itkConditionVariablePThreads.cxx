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
#include "itkConditionVariable.h"

namespace itk
{
ConditionVariable::ConditionVariable()
{
  pthread_mutex_init(&m_ConditionVariable.m_Mutex, NULL);
  pthread_cond_init(&m_ConditionVariable.m_ConditionVariable, NULL);
}

ConditionVariable::~ConditionVariable()
{
  pthread_mutex_destroy(&m_ConditionVariable.m_Mutex);
  pthread_cond_destroy(&m_ConditionVariable.m_ConditionVariable);
}

void ConditionVariable::Signal()
{
  pthread_cond_signal(&m_ConditionVariable.m_ConditionVariable);
}

void ConditionVariable::Broadcast()
{
  pthread_cond_broadcast(&m_ConditionVariable.m_ConditionVariable);
}

void ConditionVariable::Wait(SimpleFastMutexLock *mutex)
{
  pthread_cond_wait( &m_ConditionVariable.m_ConditionVariable, &mutex->GetMutexLock() );
}
} //end of namespace itk
