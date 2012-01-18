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
#ifndef __itkCorrelationHelperImageToImageMetricv4GetValueAndDerivativeThreader_h
#define __itkCorrelationHelperImageToImageMetricv4GetValueAndDerivativeThreader_h

#include "itkImageToImageMetricv4GetValueAndDerivativeThreader.h"

namespace itk
{

/** \class CorrelationImageToImageMetricv4GetValueAndDerivativeThreader
 * \brief Helper class for CorrelationImageToImageMetricv4 \c
 * To compute the average pixel intensities of the fixed image and the moving image
 * on the sampled points or inside the virtual image region:
 * \bar f (CorrelationImageToImageMetricv4::m_AverageFix )
 * \bar m (CorrelationImageToImageMetricv4::m_AverageMov )
 *
 * \ingroup ITKMetricsv4
 */
template < class TDomainPartitioner, class TImageToImageMetric, class TCorrelationMetric >
class CorrelationHelperImageToImageMetricv4GetValueAndDerivativeThreader
  : public ImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric >
{
public:
  /** Standard class typedefs. */
  typedef CorrelationHelperImageToImageMetricv4GetValueAndDerivativeThreader                                      Self;
  typedef ImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric > Superclass;
  typedef SmartPointer< Self >                                                                         Pointer;
  typedef SmartPointer< const Self >                                                                   ConstPointer;

  itkTypeMacro( CorrelationHelperImageToImageMetricv4GetValueAndDerivativeThreader, ImageToImageMetricv4GetValueAndDerivativeThreader );

  itkNewMacro( Self );

  typedef typename Superclass::DomainType    DomainType;
  typedef typename Superclass::AssociateType AssociateType;

  typedef typename Superclass::ImageToImageMetricv4Type ImageToImageMetricv4Type;
  typedef typename Superclass::VirtualIndexType         VirtualIndexType;
  typedef typename Superclass::VirtualPointType         VirtualPointType;
  typedef typename Superclass::FixedImagePointType      FixedImagePointType;
  typedef typename Superclass::FixedImagePixelType      FixedImagePixelType;
  typedef typename Superclass::FixedImageGradientType   FixedImageGradientType;
  typedef typename Superclass::MovingImagePointType     MovingImagePointType;
  typedef typename Superclass::MovingImagePixelType     MovingImagePixelType;
  typedef typename Superclass::MovingImageGradientType  MovingImageGradientType;
  typedef typename Superclass::MeasureType              MeasureType;
  typedef typename Superclass::DerivativeType           DerivativeType;
  typedef typename Superclass::DerivativeValueType      DerivativeValueType;

  typedef typename Superclass::InternalComputationValueType InternalComputationValueType;
  typedef typename Superclass::NumberOfParametersType       NumberOfParametersType;

  typedef typename Superclass::FixedOutputPointType     FixedOutputPointType;
  typedef typename Superclass::MovingOutputPointType    MovingOutputPointType;
protected:
  CorrelationHelperImageToImageMetricv4GetValueAndDerivativeThreader() {}

  /** Overload: Resize and initialize per thread objects. */
  virtual void BeforeThreadedExecution();

  /** Overload:
   * Collects the results from each thread and sums them.  Results are stored
   * in the enclosing class \c m_Value and \c m_DerivativeResult.  Behavior
   * depends on m_AverageValueAndDerivativeByNumberOfValuePoints,
   * m_NumberOfValidPoints, to average the value sum, and to average
   * derivative sums for global transforms only (i.e. transforms without local
   * support).  */
  virtual void AfterThreadedExecution();


  /* Overload: don't need to compute the image gradients and store derivatives
   *
   * Method called by the threaders to process the given virtual point.  This
   * in turn calls \c TransformAndEvaluateFixedPoint, \c
   * TransformAndEvaluateMovingPoint, and \c ProcessPoint.
   * And adds entries to m_MeasurePerThread and m_LocalDerivativesPerThread,
   * m_NumberOfValidPointsPerThread. */
  virtual bool ProcessVirtualPoint( const VirtualIndexType & virtualIndex,
                                    const VirtualPointType & virtualPoint,
                                    const ThreadIdType threadId );


  /**
   * Have to re-implemented since it is a pure virtual function in base class.
   * This function computes the local voxel-wise contribution of
   *  the metric to the global integral of the metric/derivative.
   */
  virtual bool ProcessPoint(
        const VirtualPointType &          , //virtualPoint,
        const FixedImagePointType &       , //mappedFixedPoint,
        const FixedImagePixelType &       , //mappedFixedPixelValue,
        const FixedImageGradientType &    , //mappedFixedImageGradient,
        const MovingImagePointType &      , //mappedMovingPoint,
        const MovingImagePixelType &      , //mappedMovingPixelValue,
        const MovingImageGradientType &   , //mappedMovingImageGradient,
        MeasureType &                     , //metricValueReturn,
        DerivativeType &                  , //localDerivativeReturn,
        const ThreadIdType                 //threadID
        ) const { return false; }

private:
  CorrelationHelperImageToImageMetricv4GetValueAndDerivativeThreader( const Self & ); // purposely not implemented
  void operator=( const Self & ); // purposely not implemented

  /* per thread variables for correlation and its derivatives */
  mutable std::vector< InternalComputationValueType > m_FixSumPerThread;
  mutable std::vector< InternalComputationValueType > m_MovSumPerThread;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCorrelationHelperImageToImageMetricv4GetValueAndDerivativeThreader.hxx"
#endif

#endif
