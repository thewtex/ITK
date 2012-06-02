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
#ifndef __itkLabeledPointSetToPointSetMetricv4_h
#define __itkLabeledPointSetToPointSetMetricv4_h

#include "itkPointSetToPointSetMetricv4.h"

#include <vector>

namespace itk
{
/** \class LabeledPointSetToPointSetMetricv4
 * \brief Computes the distance metric and gradient between two
 * labeled point sets.
 *
 * This class is generic in that it takes an unlabeled point set metric,
 * such as one of the following options
 *
 * \li \c EuclideanDistancePointSetToPointSetMetricv4 (default)
 * \li \c ExpectationBasedPointSetToPointSetMetricv4
 * \li \c JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4
 *
 * where it is assumed that the point set \c PixelType associated with
 * each point is a label.  The class partitions the moving and fixed
 * point sets based on labels, applies the point set metric to each
 * label group, and accumulates the result into a total value/gradient.
 *
 * \author Nick Tustison
 *
 * \ingroup ITKMetricsv4
 */
template<class TFixedPointSet, class TMovingPointSet = TFixedPointSet>
class ITK_EXPORT LabeledPointSetToPointSetMetricv4:
  public PointSetToPointSetMetricv4<TFixedPointSet, TMovingPointSet>
{
public:

  /** Standard class typedefs. */
  typedef LabeledPointSetToPointSetMetricv4                            Self;
  typedef PointSetToPointSetMetricv4<TFixedPointSet, TMovingPointSet>  Superclass;
  typedef SmartPointer<Self>                                           Pointer;
  typedef SmartPointer<const Self>                                     ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( LabeledPointSetToPointSetMetricv4, PointSetToPointSetMetricv4 );

  typedef TFixedPointSet                              FixedPointSetType;
  typedef typename FixedPointSetType::Pointer         FixedPointSetPointer;
  typedef TMovingPointSet                             MovingPointSetType;
  typedef typename MovingPointSetType::Pointer        MovingPointSetPointer;

  /** Types transferred from the base class */
  typedef typename Superclass::MeasureType            MeasureType;
  typedef typename Superclass::DerivativeType         DerivativeType;
  typedef typename Superclass::LocalDerivativeType    LocalDerivativeType;
  typedef typename Superclass::PointType              PointType;
  typedef typename Superclass::PointIdentifier        PointIdentifier;

  typedef typename Superclass::PixelType              LabelType;
  typedef std::vector<LabelType>                      LabelSetType;

  typedef Superclass                                  PointSetMetricType;
  typedef typename PointSetMetricType::Pointer        PointSetMetricPointer;

  /**
   * Initialize the metric by making sure that all the components
   *  are present and plugged together correctly.
   */
  virtual void Initialize( void ) throw ( ExceptionObject );

  /**
   * Calculates the local metric value for a single point.
   */
  virtual MeasureType GetLocalNeighborhoodValue( const PointType &, const LabelType & ) const;

  /**
   * Calculates the local value and derivative for a single point.
   */
  virtual void GetLocalNeighborhoodValueAndDerivative( const PointType &,
    MeasureType &, LocalDerivativeType &, const LabelType & ) const;

  /**
   * Set/get the specific unlabeled point set metric type.  Default is
   * the \c EuclideanDistancePointSetToPointSetMetricv4.
   */
  itkSetObjectMacro( PointSetMetric, PointSetMetricType );
  itkGetObjectMacro( PointSetMetric, PointSetMetricType );

protected:
  LabeledPointSetToPointSetMetricv4();
  virtual ~LabeledPointSetToPointSetMetricv4();

  /** PrintSelf function */
  void PrintSelf( std::ostream & os, Indent indent ) const;

private:
  LabeledPointSetToPointSetMetricv4(const Self &); //purposely not implemented
  void operator=(const Self &);               //purposely not implemented

  /**
   * Private function to find the common label set for the moving
   * and fixed point sets.
   */
  void DetermineCommonPointSetLabels();

  /**
   * Private function to create a fixed point set from the input fixed point
   * set with a specific label
   */
  FixedPointSetPointer GetLabeledFixedPointSet( const LabelType ) const;

  /**
   * Private function to create a moving point set from the input moving point
   * set with a specific label
   */
  MovingPointSetPointer GetLabeledMovingPointSet( const LabelType ) const;

  PointSetMetricPointer                         m_PointSetMetric;
  std::vector<PointSetMetricPointer>            m_PointSetMetricClones;

  LabelSetType                                  m_FixedPointSetLabels;
  LabelSetType                                  m_MovingPointSetLabels;
  LabelSetType                                  m_CommonPointSetLabels;

};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLabeledPointSetToPointSetMetricv4.hxx"
#endif

#endif
