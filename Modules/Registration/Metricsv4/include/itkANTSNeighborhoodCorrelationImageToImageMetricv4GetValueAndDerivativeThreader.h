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
#ifndef __itkANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader_h
#define __itkANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader_h

#include "itkImageToImageMetricv4GetValueAndDerivativeThreader.h"
#include "itkThreadedImageRegionPartitioner.h"
#include "itkThreadedIndexedContainerPartitioner.h"


namespace itk
{

/** \class ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader
 * \brief Processes points for NeighborhoodScanningWindow calculation.
 *
 * \ingroup ITKMetricsv4
 */

template<class T>
struct IdentityHelper
{
  typedef T MyType;
};

/** \class ANTSNeighborhoodCorrelationImageToImageMetricv4SparseGetValueAndDerivativeThreader
 * \brief Processes points for NeighborhoodScanningWindow calculation.
 *
 * \ingroup ITKMetricsv4
 */
template< class TDomainPartitioner, class TImageToImageMetric, class TNeighborhoodCorrelationMetric >
class ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader
  : public ImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric >
{
public:
  /** Standard class typedefs. */
  typedef ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader Self;
  typedef ImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric >
                                                                                                Superclass;
  typedef SmartPointer< Self >                                                                  Pointer;
  typedef SmartPointer< const Self >                                                            ConstPointer;

  itkTypeMacro( ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader, ImageToImageMetricv4GetValueAndDerivativeThreader );

  itkNewMacro( Self );

  typedef typename Superclass::DomainType    DomainType;
  typedef typename Superclass::AssociateType AssociateType;

  typedef typename Superclass::VirtualImageType        VirtualImageType;
  typedef typename Superclass::VirtualPointType        VirtualPointType;
  typedef typename Superclass::VirtualIndexType        VirtualIndexType;
  typedef typename Superclass::FixedImagePointType     FixedImagePointType;
  typedef typename Superclass::FixedImagePixelType     FixedImagePixelType;
  typedef typename Superclass::FixedImageGradientType  FixedImageGradientType;
  typedef typename Superclass::MovingImagePointType    MovingImagePointType;
  typedef typename Superclass::MovingImagePixelType    MovingImagePixelType;
  typedef typename Superclass::MovingImageGradientType MovingImageGradientType;
  typedef typename Superclass::MeasureType             MeasureType;
  typedef typename Superclass::DerivativeType          DerivativeType;
  typedef typename Superclass::DerivativeValueType     DerivativeValueType;

  typedef TNeighborhoodCorrelationMetric                                 NeighborhoodCorrelationMetricType;
  typedef typename NeighborhoodCorrelationMetricType::ScanIteratorType   ScanIteratorType;
  typedef typename NeighborhoodCorrelationMetricType::ScanMemType        ScanMemType;
  typedef typename NeighborhoodCorrelationMetricType::ScanParametersType ScanParametersType;
  typedef typename NeighborhoodCorrelationMetricType::ImageRegionType    ImageRegionType;
  typedef typename NeighborhoodCorrelationMetricType::InternalComputationValueType InternalComputationValueType;
  typedef typename NeighborhoodCorrelationMetricType::SumQueueType       SumQueueType;
  typedef typename NeighborhoodCorrelationMetricType::ImageDimensionType ImageDimensionType;
  typedef typename NeighborhoodCorrelationMetricType::JacobianType       JacobianType;
  typedef typename NeighborhoodCorrelationMetricType::NumberOfParametersType       NumberOfParametersType;

protected:
  ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader() {}

  /** \c ProcessVirtualPoint and \c ProcessPoint are not used in the dense threader
   * ANTSNeighborhoodScanningWindowDenseGetValueAndDerivativeThreader implementation.
   *  \c ProcessVirtualPoint is used in the sparse threader
   * ANTSNeighborhoodScanningWindowSparseGetValueAndDerivativeThreader implementation.
   * */

  /*
   * Use helper class identity to overload specific partitioner
   * refer: http://stackoverflow.com/questions/3052579/explicit-specialization-in-non-namespace-scope
   */


  /** Method called by the threaders to process the given virtual point.  This
   * in turn calls \c TransformAndEvaluateFixedPoint, \c
   * TransformAndEvaluateMovingPoint, and \c ProcessPoint.
   * And adds entries to m_MeasurePerThread and m_LocalDerivativesPerThread,
   * m_NumberOfValidPointsPerThread. */
  virtual bool ProcessVirtualPoint( const VirtualIndexType & virtualIndex,
                                    const VirtualPointType & virtualPoint,
                                    const ThreadIdType threadId ) {
    return ProcessVirtualPoint_impl(IdentityHelper<TDomainPartitioner>(), virtualIndex, virtualPoint, threadId );
  }

  /* specific overloading for point based CC metric */
  bool ProcessVirtualPoint_impl(
                             IdentityHelper<ThreadedIndexedContainerPartitioner> itkNotUsed(self),
                             const VirtualIndexType & virtualIndex,
                             const VirtualPointType & virtualPoint,
                             const ThreadIdType threadId );

  /* for other default case */
  template<class T>
  bool ProcessVirtualPoint_impl(
                             IdentityHelper<T> itkNotUsed(self),
                             const VirtualIndexType & virtualIndex,
                             const VirtualPointType & virtualPoint,
                             const ThreadIdType threadId ) {
    return Superclass::ProcessVirtualPoint(virtualIndex, virtualPoint, threadId);
  }


  /** \c ProcessPoint() must be reloaded since it is a pure virtual function.
   * It is not used for either sparse or dense threader.
   * */
  virtual bool ProcessPoint(
         const VirtualIndexType &          itkNotUsed(virtualIndex),
         const VirtualPointType &          itkNotUsed(virtualPoint),
         const FixedImagePointType &       itkNotUsed(mappedFixedPoint),
         const FixedImagePixelType &       itkNotUsed(mappedFixedPixelValue),
         const FixedImageGradientType &    itkNotUsed(mappedFixedImageGradient),
         const MovingImagePointType &      itkNotUsed(mappedMovingPoint),
         const MovingImagePixelType &      itkNotUsed(mappedMovingPixelValue),
         const MovingImageGradientType &   itkNotUsed(mappedMovingImageGradient),
         MeasureType &                     itkNotUsed(metricValueReturn),
         DerivativeType &                  itkNotUsed(localDerivativeReturn),
         const ThreadIdType                itkNotUsed(threadID) ) const
     {
     return false;
     }

  virtual void ThreadedExecution( const DomainType& domain,
                                    const ThreadIdType threadId ){
    ThreadedExecution_impl(IdentityHelper<TDomainPartitioner>(), domain, threadId );
  }

  /* specific overloading for dense threader only based CC metric */
  void ThreadedExecution_impl(
                             IdentityHelper<ThreadedImageRegionPartitioner<TImageToImageMetric::VirtualImageDimension> > itkNotUsed(self),
                             const DomainType& domain,
                             const ThreadIdType threadId );

  /* for other default case */
  template<class T>
  void ThreadedExecution_impl(
                             IdentityHelper<T> itkNotUsed(self),
                             const DomainType& domain,
                             const ThreadIdType threadId ) {
    /* call base method */
    /* Store the casted pointer to avoid dynamic casting in tight loops. */
    this->m_ANTSAssociate = dynamic_cast< TNeighborhoodCorrelationMetric * >( this->m_Associate );
    if( this->m_ANTSAssociate == NULL )
      {
      itkExceptionMacro("Dynamic casting of associate pointer failed.");
      }

    Superclass::ThreadedExecution(domain, threadId);
  }

  /** Common functions for computing correlation over scanning windows **/

  /** Update the queues for the next point.  Calls either \c
   * UpdateQueuesAtBeginningOfLine or \c UpdateQueuesToNextScanWindow. */
  void UpdateQueues(const ScanIteratorType &scanIt,
    ScanMemType &scanMem, const ScanParametersType &scanParameters,
    const ThreadIdType threadID) const;

  void UpdateQueuesAtBeginningOfLine(
    const ScanIteratorType &scanIt, ScanMemType &scanMem,
    const ScanParametersType &scanParameters,
    const ThreadIdType threadID) const;

  /** Increment the iterator and check to see if we're at the end of the
   * line.  If so, go to the next line.  Otherwise, add the
   * the values for the next hyperplane. */
  void UpdateQueuesToNextScanWindow(
    const ScanIteratorType &scanIt, ScanMemType &scanMem,
    const ScanParametersType &scanParameters,
    const ThreadIdType threadID) const;

  /** Test to see if there are any voxels we need to handle in the current
   * window. */
  bool ComputeInformationFromQueues(
    const ScanIteratorType &scanIt, ScanMemType &scanMem,
    const ScanParametersType &scanParameters,
    const ThreadIdType threadID) const;

  void ComputeMovingTransformDerivative(
    const ScanIteratorType &scanIt, ScanMemType &scanMem,
    const ScanParametersType &scanParameters, DerivativeType &deriv,
    MeasureType &local_cc, const ThreadIdType threadID) const;

private:
  ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader( const Self & ); // purposely not implemented
  void operator=( const Self & ); // purposely not implemented

  /** Internal pointer to the metric object in use by this threader.
   *  This will avoid costly dynamic casting in tight loops. */
  TNeighborhoodCorrelationMetric * m_ANTSAssociate;
};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader.hxx"
#endif

#endif
