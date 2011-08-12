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
#ifndef __itkLevelSetDomainPartitionBase_h
#define __itkLevelSetDomainPartitionBase_h

#include "itkLightObject.h"
#include "itkIntTypes.h"

#include <list>

namespace itk
{
/** \class LevelSetDomainPartitionBase
 *
 * \brief Helper class used to partition domain and efficiently compute overlap.
 *
 */
template< class TDomain >
class LevelSetDomainPartitionBase : public LightObject
{
public:

  typedef LevelSetDomainPartitionBase           Self;
  typedef LightObject                           Superclass;
  typedef SmartPointer< Self >                  Pointer;
  typedef SmartPointer< const Self >            ConstPointer;

  itkTypeMacro(LevelSetDomainPartitionBase, LightObject);

  typedef std::list< IdentifierType >                 IdentifierListType;
  typedef typename IdentifierListType::iterator       IdentifierListIterator;
  typedef typename IdentifierListType::const_iterator IdentifierListConstIterator;

  /** Set the number of level set functions */
  void SetFunctionCount(const IdentifierType & n)
  {
    this->m_FunctionCount = n;
  }

  /** Get the number of level set functions */
  IdentifierType GetFunctionCount() const
  {
    return this->m_FunctionCount;
  }

protected:

  /** \brief Constructor */
  LevelSetDomainPartitionBase() :
    Superclass(), m_FunctionCount( 1 )
  {}

  /** \brief Destructor */
  virtual ~LevelSetDomainPartitionBase(){}

  virtual void AllocateListDomain() = 0;
  virtual void PopulateListDomain() = 0;

  IdentifierType m_FunctionCount;

private:
  /** purposely not implemented */
  LevelSetDomainPartitionBase(const Self &);
  void operator=(const Self &);
};
} //end namespace itk

#endif
