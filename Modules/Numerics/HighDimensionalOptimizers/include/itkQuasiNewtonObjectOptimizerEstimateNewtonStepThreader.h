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
#ifndef __itkQuasiNewtonObjectOptimizerEstimateNewtonStepThreader_h
#define __itkQuasiNewtonObjectOptimizerEstimateNewtonStepThreader_h

#include "itkDomainThreader.h"
#include "itkThreadedIndexedContainerPartitioner.h"

namespace itk
{

class QuasiNewtonObjectOptimizer;

/** \class QuasiNewtonObjectOptimizerEstimateNewtonStepThreader
 * \brief Estimate the quasi-Newton step in a thread.
 * \ingroup ITKHighDimensionalOptimizers
 * */
class QuasiNewtonObjectOptimizerEstimateNewtonStepThreader
  : public DomainThreader< ThreadedIndexedContainerPartitioner, QuasiNewtonObjectOptimizer >
{
public:
  /** Standard class typedefs. */
  typedef QuasiNewtonObjectOptimizerEstimateNewtonStepThreader                              Self;
  typedef DomainThreader< ThreadedIndexedContainerPartitioner, QuasiNewtonObjectOptimizer > Superclass;
  typedef SmartPointer< Self >                                                              Pointer;
  typedef SmartPointer< const Self >                                                        ConstPointer;

  itkTypeMacro( QuasiNewtonObjectOptimizerEstimateNewtonStepThreader, DomainThreader );

  itkNewMacro( Self );

  typedef typename Superclass::DomainType    DomainType;
  typedef typename Superclass::AssociateType AssociateType;
  typedef          DomainType                IndexRangeType;

protected:
  virtual void ThreadedExecution( const IndexRangeType & subrange,
                                  const ThreadIdType threadId );

  QuasiNewtonObjectOptimizerEstimateNewtonStepThreader() {}
  virtual ~QuasiNewtonObjectOptimizerEstimateNewtonStepThreader() {}

private:
  QuasiNewtonObjectOptimizerEstimateNewtonStepThreader( const Self & ); // purposely not implemented
  void operator=( const Self & ); // purposely not implemented
};

} // end namespace itk

#endif
