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
#include "itkSemaphore.h"
#include "itkSimpleFastMutexLock.h"

extern "C" {
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
}

namespace itk
{
static SimpleFastMutexLock mutex;
static int SemaphoreCount = 0;

Semaphore::Semaphore ()
{
  m_Sema = 0;
  m_PThreadsSemaphoreRemoved = false;
}

void Semaphore::Initialize(unsigned int value)
{
  m_PThreadsSemaphoreRemoved = false;

  mutex.Lock();
  time_t t = time(0);
  snprintf(m_SemaphoreName, 254, "MACSEM%d%d", static_cast< int >( t ), SemaphoreCount);
  SemaphoreCount++;
  mutex.Unlock();

  m_Sema  = sem_open(m_SemaphoreName, O_CREAT, 0x0644, value);
  if ( m_Sema == (sem_t *)SEM_FAILED )
    {
    itkExceptionMacro( << "sem_open call failed on " << m_SemaphoreName );
    }
}

void Semaphore::Up()
{
  if ( sem_post(m_Sema) != 0 )
    {
    itkExceptionMacro(<< "sem_post call failed.");
    }
}

void Semaphore::Down()
{
  if ( sem_wait(m_Sema) != 0 )
    {
    itkExceptionMacro(<< "sem_wait call failed.");
    }
}

Semaphore::~Semaphore()
{
  if ( !m_PThreadsSemaphoreRemoved )
    {
    this->Remove();
    }
}

void Semaphore::Remove()
{
  m_PThreadsSemaphoreRemoved = true;
  if ( sem_unlink(m_SemaphoreName) != 0 )
    {
    itkExceptionMacro(<< "sem_unlink call failed. ");
    }
}

} //end if namespace itk
