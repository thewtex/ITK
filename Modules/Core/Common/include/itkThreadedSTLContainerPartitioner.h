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
#ifndef __itkThreadedSTLContainerPartitioner_h
#define __itkThreadedSTLContainerPartitioner_h

#include "itkThreadedDomainPartitioner.h"

/** \class ThreadedSTLContainerPartitioner
 *  \brief Partitions an STL container for threading.
 *
 * The DomainType is defined to be a two element container of interators:
 * the first iterator defines the start of the domain, and the second element
 * defines one element past the end of the domain.
 *
 * Element counting starts from zero and the identifier at the end of the domain
 * is exclusive.
 *
 * \sa ThreadedIndexedContainerPartitioner
 * \sa ThreadedDomainPartitioner
 * \sa STLContainerInterface
 * \ingroup ITKCommon
 */

namespace itk
{

template< class TContainer >
struct ThreadedSTLContainerPartitionerDomain
{
  typedef TContainer                          ContainerType;
  typedef typename TContainer::const_iterator IteratorType;

  IteratorType m_StartIterator;
  IteratorType m_EndIterator;
};

template< class TContainer >
class ITK_EXPORT ThreadedSTLContainerPartitioner
  : public ThreadedDomainPartitioner< ThreadedSTLContainerPartitionerDomain< TContainer > >
{
public:
  /** Standard class typedefs. */
  typedef ThreadedSTLContainerPartitioner                                                  Self;
  typedef ThreadedDomainPartitioner< ThreadedSTLContainerPartitionerDomain< TContainer > > Superclass;
  typedef SmartPointer< Self >                                                             Pointer;
  typedef SmartPointer< const Self >                                                       ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThreadedSTLContainerPartitioner, ThreadedDomainPartitioner);

  /** Type for convenience of base class methods */
  typedef typename Superclass::DomainType  DomainType;

  typedef TContainer ContainerType;

  /** Split the Domain \into
   * \c requestedTotal sub-domains, returning sub-domain \c i as \c subdomain.
   * This method is called \c requestedTotal times. The
   * pieces will not overlap. The method returns the number of pieces that
   * the routine is capable of splitting the output RequestedObject,
   * i.e. return value is less than or equal to \c requestedTotal. */
  virtual
  ThreadIdType PartitionDomain(const ThreadIdType i,
                           const ThreadIdType requestedTotal,
                           const DomainType& completeDomain,
                           DomainType& subdomain) const;
protected:
  ThreadedSTLContainerPartitioner();
  virtual ~ThreadedSTLContainerPartitioner();

private:
  ThreadedSTLContainerPartitioner(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkThreadedSTLContainerPartitioner.hxx"
#endif

#endif
