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
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#ifndef __itkMutexLock_h
#define __itkMutexLock_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkThreadSupport.h"

namespace itk
{

/** \class SimpleMutexLock
 * \brief Simple mutual exclusion locking class.

 * SimpleMutexLock allows the locking of variables which are accessed
 * through different threads.  This header file also defines
 * SimpleMutexLock which is not a subclass of Object.
 *
 * \ingroup OSSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT SimpleMutexLock
{
public:
  /** Standard class typedefs.  */
  typedef SimpleMutexLock Self;

  /** Constructor and destructor left public purposely. */
  SimpleMutexLock();
  virtual ~SimpleMutexLock();

  /** Methods for creation and destruction through the object factory. */
  static SimpleMutexLock * New();

  void Delete() { delete this; }

  /** Used for debugging and other run-time purposes. */
  virtual const char * GetNameOfClass() { return "itkSimpleMutexLock"; }

  /** Lock the MutexLock. */
  void Lock(void);

  /** Unlock the MutexLock. */
  void Unlock(void);

  /** Access the MutexType member variable from outside this class */
  MutexType & GetMutexLock()
  {
    return m_MutexLock;
  }

  MutexType GetMutexLock() const
  {
    return *( const_cast< MutexType * >( &m_MutexLock ) );
  }

protected:
  MutexType m_MutexLock;
};

/** \class MutexLock
 * \brief Mutual exclusion locking class.
 *
 * MutexLock allows the locking of variables which are accessed
 * through different threads.  This header file also defines
 * SimpleMutexLock which is not a subclass of itkObject.
 *
 * \ingroup OSSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT MutexLock:public Object
{
public:
  /** Standard class typedefs. */
  typedef MutexLock                  Self;
  typedef Object                     Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation. */
  itkNewMacro(Self);

  /** Run-time information. */
  itkTypeMacro(MutexLock, Object);

  /** Lock the itkMutexLock. */
  void Lock(void);

  /** Unlock the MutexLock. */
  void Unlock(void);

protected:
  MutexLock() {}
  ~MutexLock() {}

  SimpleMutexLock m_SimpleMutexLock;
  virtual void PrintSelf(std::ostream & os, Indent indent) const;

private:
  MutexLock(const Self &);      //purposely not implemented
  void operator=(const Self &); //purposely not implemented
};

inline void MutexLock::Lock(void)
{
  m_SimpleMutexLock.Lock();
}

inline void MutexLock::Unlock(void)
{
  m_SimpleMutexLock.Unlock();
}
} //end itk namespace
#endif
