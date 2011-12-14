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
#ifndef __itkQuasiNewtonOptimizerv4EstimateNewtonStepThreaderEstimateNewtonStepThreader_h
#define __itkQuasiNewtonOptimizerv4EstimateNewtonStepThreaderEstimateNewtonStepThreader_h

#include "itkDomainThreader.h"
#include "itkThreadedIndexedContainerPartitioner.h"

namespace itk
{

class QuasiNewtonOptimizerv4EstimateNewtonStepThreader;

/** \class QuasiNewtonOptimizerv4EstimateNewtonStepThreaderEstimateNewtonStepThreader
 * \brief Estimate the quasi-Newton step in a thread.
 * \ingroup ITKOptimizersv4
 * */
class QuasiNewtonOptimizerv4EstimateNewtonStepThreaderEstimateNewtonStepThreader
  : public DomainThreader< ThreadedIndexedContainerPartitioner, QuasiNewtonOptimizerv4EstimateNewtonStepThreader >
{
public:
  /** Standard class typedefs. */
  typedef QuasiNewtonOptimizerv4EstimateNewtonStepThreaderEstimateNewtonStepThreader                              Self;
  typedef DomainThreader< ThreadedIndexedContainerPartitioner, QuasiNewtonOptimizerv4EstimateNewtonStepThreader > Superclass;
  typedef SmartPointer< Self >                                                              Pointer;
  typedef SmartPointer< const Self >                                                        ConstPointer;

  itkTypeMacro( QuasiNewtonOptimizerv4EstimateNewtonStepThreaderEstimateNewtonStepThreader, DomainThreader );

  itkNewMacro( Self );

  typedef Superclass::DomainType    DomainType;
  typedef Superclass::AssociateType AssociateType;
  typedef DomainType                IndexRangeType;

protected:
  virtual void ThreadedExecution( const IndexRangeType & subrange,
                                  const ThreadIdType threadId );

  QuasiNewtonOptimizerv4EstimateNewtonStepThreaderEstimateNewtonStepThreader() {}
  virtual ~QuasiNewtonOptimizerv4EstimateNewtonStepThreaderEstimateNewtonStepThreader() {}

private:
  QuasiNewtonOptimizerv4EstimateNewtonStepThreaderEstimateNewtonStepThreader( const Self & ); // purposely not implemented
  void operator=( const Self & ); // purposely not implemented
};

} // end namespace itk

#endif
