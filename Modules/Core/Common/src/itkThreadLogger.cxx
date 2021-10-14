/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include <iostream>
#include "itkThreadLogger.h"
#include "itksys/SystemTools.hxx"

namespace itk
{

ThreadLogger::ThreadLogger()
{
  m_Delay = 300; // ms
  m_TerminationRequested = false;
#ifndef __wasi__
  m_Thread = std::thread(&ThreadLogger::ThreadFunction, this);
#endif
}

ThreadLogger::~ThreadLogger()
{
#ifndef __wasi__
  if (m_Thread.joinable())
  {
    m_TerminationRequested = true;
    m_Thread.join(); // waits for it to finish if necessary
  }
#endif
}

void
ThreadLogger::SetPriorityLevel(PriorityLevelEnum level)
{
#ifndef __wasi__
  this->m_Mutex.lock();
#endif
  this->m_OperationQ.push(SET_PRIORITY_LEVEL);
  this->m_LevelQ.push(level);
#ifndef __wasi__
  this->m_Mutex.unlock();
#endif
}

Logger::PriorityLevelEnum
ThreadLogger::GetPriorityLevel() const
{
#ifndef __wasi__
  this->m_Mutex.lock();
#endif
  PriorityLevelEnum level = this->m_PriorityLevel;
#ifndef __wasi__
  this->m_Mutex.unlock();
#endif
  return level;
}

void
ThreadLogger::SetLevelForFlushing(PriorityLevelEnum level)
{
#ifndef __wasi__
  this->m_Mutex.lock();
#endif
  this->m_LevelForFlushing = level;
  this->m_OperationQ.push(SET_LEVEL_FOR_FLUSHING);
  this->m_LevelQ.push(level);
#ifndef __wasi__
  this->m_Mutex.unlock();
#endif
}

Logger::PriorityLevelEnum
ThreadLogger::GetLevelForFlushing() const
{
#ifndef __wasi__
  this->m_Mutex.lock();
#endif
  PriorityLevelEnum level = this->m_LevelForFlushing;
#ifndef __wasi__
  this->m_Mutex.unlock();
#endif
  return level;
}

void
ThreadLogger::SetDelay(DelayType delay)
{
#ifndef __wasi__
  this->m_Mutex.lock();
#endif
  this->m_Delay = delay;
#ifndef __wasi__
  this->m_Mutex.unlock();
#endif
}

ThreadLogger::DelayType
ThreadLogger::GetDelay() const
{
#ifndef __wasi__
  this->m_Mutex.lock();
#endif
  DelayType delay = this->m_Delay;
#ifndef __wasi__
  this->m_Mutex.unlock();
#endif
  return delay;
}

void
ThreadLogger::AddLogOutput(OutputType * output)
{
#ifndef __wasi__
  this->m_Mutex.lock();
#endif
  this->m_OperationQ.push(ADD_LOG_OUTPUT);
  this->m_OutputQ.push(output);
#ifndef __wasi__
  this->m_Mutex.unlock();
#endif
}

void
ThreadLogger::Write(PriorityLevelEnum level, std::string const & content)
{
#ifndef __wasi__
  this->m_Mutex.lock();
#endif
  this->m_OperationQ.push(WRITE);
  this->m_MessageQ.push(content);
  this->m_LevelQ.push(level);
  if (this->m_LevelForFlushing >= level)
  {
    this->InternalFlush();
  }
#ifndef __wasi__
  this->m_Mutex.unlock();
#endif
}

void
ThreadLogger::Flush()
{
#ifndef __wasi__
  this->m_Mutex.lock();
#endif
  this->m_OperationQ.push(FLUSH);
  this->InternalFlush();
#ifndef __wasi__
  this->m_Mutex.unlock();
#endif
}

void
ThreadLogger::InternalFlush()
{
  // m_Mutex must already be held here!

  while (!this->m_OperationQ.empty())
  {
    switch (this->m_OperationQ.front())
    {
      case ThreadLogger::SET_PRIORITY_LEVEL:
        this->m_PriorityLevel = this->m_LevelQ.front();
        this->m_LevelQ.pop();
        break;

      case ThreadLogger::SET_LEVEL_FOR_FLUSHING:
        this->m_LevelForFlushing = this->m_LevelQ.front();
        this->m_LevelQ.pop();
        break;

      case ThreadLogger::ADD_LOG_OUTPUT:
        this->m_Output->AddLogOutput(this->m_OutputQ.front());
        this->m_OutputQ.pop();
        break;

      case ThreadLogger::WRITE:
        this->Logger::Write(this->m_LevelQ.front(), this->m_MessageQ.front());
        this->m_LevelQ.pop();
        this->m_MessageQ.pop();
        break;
      case ThreadLogger::FLUSH:
        this->Logger::Flush();
        break;
    }
    this->m_OperationQ.pop();
  }
  this->m_Output->Flush();
}

void
ThreadLogger::ThreadFunction()
{
  while (!m_TerminationRequested)
  {
#ifndef __wasi__
    m_Mutex.lock();
#endif
    while (!m_OperationQ.empty())
    {
      switch (m_OperationQ.front())
      {
        case ThreadLogger::SET_PRIORITY_LEVEL:
          m_PriorityLevel = m_LevelQ.front();
          m_LevelQ.pop();
          break;

        case ThreadLogger::SET_LEVEL_FOR_FLUSHING:
          m_LevelForFlushing = m_LevelQ.front();
          m_LevelQ.pop();
          break;

        case ThreadLogger::ADD_LOG_OUTPUT:
          m_Output->AddLogOutput(m_OutputQ.front());
          m_OutputQ.pop();
          break;

        case ThreadLogger::WRITE:
          Logger::Write(m_LevelQ.front(), m_MessageQ.front());
          m_LevelQ.pop();
          m_MessageQ.pop();
          break;
        case ThreadLogger::FLUSH:
          Logger::Flush();
          break;
      }
      m_OperationQ.pop();
    }
#ifndef __wasi__
    m_Mutex.unlock();
#endif
    itksys::SystemTools::Delay(this->GetDelay());
  }
}

void
ThreadLogger::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

#ifndef __wasi__
  os << indent << "Thread ID: " << this->m_Thread.get_id() << std::endl;
#endif
  os << indent << "Low-priority Message Delay: " << this->m_Delay << std::endl;
  os << indent << "Operation Queue Size: " << this->m_OperationQ.size() << std::endl;
  os << indent << "Message Queue Size: " << this->m_MessageQ.size() << std::endl;
  os << indent << "Level Queue Size: " << this->m_LevelQ.size() << std::endl;
  os << indent << "Output Queue Size: " << this->m_OutputQ.size() << std::endl;
}

} // namespace itk
