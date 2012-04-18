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
#ifndef __itkRegistrationParameterScalesEstimator_hxx
#define __itkRegistrationParameterScalesEstimator_hxx

#include "itkRegistrationParameterScalesEstimator.h"
#include "itkPointSet.h"
#include "itkPointSetToPointSetMetric.h"
#include "itkObjectToObjectMetric.h"

namespace itk
{

template< class TMetric >
RegistrationParameterScalesEstimator< TMetric >
::RegistrationParameterScalesEstimator()
{
  // estimate parameter scales of the moving transform
  m_TransformForward = true;

  // number for random sampling
  m_NumberOfRandomSamples = 0;

  // default sampling strategy
  m_SamplingStrategy = FullDomainSampling;

  // the default radius of the central region for sampling
  m_CentralRegionRadius = 5;

  // the metric object must be set before EstimateScales()
}

/** Estimate the trusted scale for steps. It returns the voxel spacing. */
template< class TMetric >
typename RegistrationParameterScalesEstimator< TMetric >::FloatType
RegistrationParameterScalesEstimator< TMetric >
::EstimateMaximumStepSize()
{
  this->CheckAndSetInputs();

  const VirtualSpacingType& spacing = this->m_Metric->GetVirtualSpacing();

  const SizeValueType dim = this->GetDimension();

  FloatType minSpacing = NumericTraits<FloatType>::max();

  for (SizeValueType d=0; d<dim; d++)
    {
    if (minSpacing > spacing[d])
      {
      minSpacing = spacing[d];
      }
    }

  return minSpacing;
}

/** Validate and set metric and its transforms. */
template< class TMetric >
bool
RegistrationParameterScalesEstimator< TMetric >
::CheckAndSetInputs()
{
if (m_Metric.IsNull())
    {
    itkExceptionMacro("RegistrationParameterScalesEstimator: the metric is NULL");
    }

  if (this->m_Metric->GetMovingTransform() == NULL)
    {
    itkExceptionMacro("RegistrationParameterScalesEstimator: m_MovingTransform in the metric is NULL.");
    }
  if (this->m_Metric->GetFixedTransform() == NULL)
    {
    itkExceptionMacro("RegistrationParameterScalesEstimator: m_FixedTransform in the metric is NULL.");
    }

  return true;
}

/** Get the transform being estimated scales for. */
template< class TMetric >
const TransformBase *
RegistrationParameterScalesEstimator< TMetric >
::GetTransform()
{
  if (m_TransformForward)
    {
    return this->m_Metric->GetMovingTransform();
    }
  else
    {
    return this->m_Metric->GetFixedTransform();
    }
}

/** Get the dimension of the target transformed to. */
template< class TMetric >
itk::SizeValueType
RegistrationParameterScalesEstimator< TMetric >
::GetDimension()
{
  if (m_TransformForward)
    {
    return MovingDimension;
    }
  else
    {
    return FixedDimension;
    }
}

/** Check if the transform being optimized has local support. */
template< class TMetric >
bool
RegistrationParameterScalesEstimator< TMetric >
::HasLocalSupport()
{
  if (this->m_TransformForward)
    {
    return this->m_Metric->GetMovingTransform()->HasLocalSupport();
    }
  else
    {
    return this->m_Metric->GetFixedTransform()->HasLocalSupport();
    }
}

/** Get the number of scales. */
template< class TMetric >
SizeValueType
RegistrationParameterScalesEstimator< TMetric >
::GetNumberOfLocalParameters()
{
  if (this->GetTransformForward())
    {
    return this->m_Metric->GetMovingTransform()->GetNumberOfLocalParameters();
    }
  else
    {
    return this->m_Metric->GetFixedTransform()->GetNumberOfLocalParameters();
    }
}

/** Update the transform with a change in parameters. */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::UpdateTransformParameters(const ParametersType &deltaParameters)
{
  // Apply the delta parameters to the transform
  if (this->m_TransformForward)
    {
    typename MovingTransformType::Pointer movingTransform = const_cast<MovingTransformType *>(this->m_Metric->GetMovingTransform());
    ParametersType &step = const_cast<ParametersType &>(deltaParameters);
    movingTransform->UpdateTransformParameters(step);
    }
  else
    {
    typename FixedTransformType::Pointer fixedTransform = const_cast<FixedTransformType *>(this->m_Metric->GetFixedTransform());
    ParametersType &step = const_cast<ParametersType &>(deltaParameters);
    fixedTransform->UpdateTransformParameters(step);
    }
}

/** Transform a physical point to a new physical point.
 *  We want to compute shift in physical space so that the scales is not
 *  sensitive to spacings and directions of voxel sampling.
 */
template< class TMetric >
template< class TTargetPointType >
void
RegistrationParameterScalesEstimator< TMetric >
::TransformPoint(const VirtualPointType &point, TTargetPointType &mappedPoint)
{
  if (this->GetTransformForward())
    {
    mappedPoint = this->m_Metric->GetMovingTransform()->TransformPoint( point );
    }
  else
    {
    mappedPoint = this->m_Metric->GetFixedTransform()->TransformPoint( point );
    }
}

/** Get the squared norms of the transform Jacobians w.r.t parameters at a point */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::ComputeSquaredJacobianNorms( const VirtualPointType  & point, ParametersType & squareNorms )
{
  JacobianType jacobian;
  const SizeValueType numPara = this->GetNumberOfLocalParameters();
  const SizeValueType dim = this->GetDimension();

  if (this->GetTransformForward())
    {
    this->m_Metric->GetMovingTransform()->ComputeJacobianWithRespectToParameters(point, jacobian);

    for (SizeValueType p=0; p<numPara; p++)
      {
      squareNorms[p] = NumericTraits< typename ParametersType::ValueType >::Zero;
      for (SizeValueType d=0; d<dim; d++)
        {
        squareNorms[p] += jacobian[d][p] * jacobian[d][p];
        }
      }
    }
  else
    {
    this->m_Metric->GetFixedTransform()->ComputeJacobianWithRespectToParameters(point, jacobian);

    for (SizeValueType p=0; p<numPara; p++)
      {
      squareNorms[p] = NumericTraits< typename ParametersType::ValueType >::Zero;
      for (SizeValueType d=0; d<dim; d++)
        {
        squareNorms[p] += jacobian[d][p] * jacobian[d][p];
        }
      }
    }
}

/** Sample the virtual domain with phyical points
 *  and store the results into m_SamplePoints.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleVirtualDomain()
{
  if ( !(this->m_SamplingTime < this->GetTimeStamp()) )
    {
    // No modification since last sampling
    return;
    }
  if( this->m_Metric->GetVirtualImage() )
    {
    if( this->m_SamplingTime > this->m_Metric->GetVirtualImage()->GetTimeStamp() )
      {
      // No modification since last sampling
      return;
      }
    }
std::cout << "SampleVirtualDomain: strategy: " << m_SamplingStrategy << std::endl;    
  if (m_SamplingStrategy == VirtualDomainPointSetSampling)
    {
    this->SampleVirtualDomainWithPointSet();
    }
  else if (m_SamplingStrategy == CornerSampling)
    {
    this->SampleVirtualDomainWithCorners();
    }
  else if (m_SamplingStrategy == RandomSampling)
    {
    this->SampleVirtualDomainRandomly();
    }
  else if (m_SamplingStrategy == CentralRegionSampling)
    {
    this->SampleVirtualDomainWithCentralRegion();
    }
  else
    {
    this->SampleVirtualDomainFully();
    }

  this->Modified();
  this->m_SamplingTime = this->GetTimeStamp();
}

/**
 * Set the sampling strategy automatically for scales estimation.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SetScalesSamplingStrategy()
{
  if( this->MetricIsPointSetToPointSetType() )
    {
    this->SetSamplingStrategy(VirtualDomainPointSetSampling);
    }
  else if( this->HasLocalSupport() )
    {
    this->SetSamplingStrategy(CentralRegionSampling);
    }
  else if (this->CheckGeneralAffineTransform())
    {
    this->SetSamplingStrategy(CornerSampling);
    }
  else
    {
    this->SetSamplingStrategy(RandomSampling);
    this->SetNumberOfRandomSamples( SizeOfSmallDomain );
    }
}

/**
 * Set the sampling strategy automatically for step scale estimation.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SetStepScaleSamplingStrategy()
{
  if( this->MetricIsPointSetToPointSetType() )
    {
    this->SetSamplingStrategy(VirtualDomainPointSetSampling);
    }
  else if (this->HasLocalSupport())
    {
    // Have to use FullDomainSampling for a transform with local support
    this->SetSamplingStrategy(FullDomainSampling);
    }
  else if (this->CheckGeneralAffineTransform())
    {
    this->SetSamplingStrategy(CornerSampling);
    }
  else
    {
    this->SetSamplingStrategy(RandomSampling);
    this->SetNumberOfRandomSamples( SizeOfSmallDomain );
    }
}

/**
 * Check if the transform is a general affine transform that maps a line
 * segment to a line segment.
 */
template< class TMetric >
bool
RegistrationParameterScalesEstimator< TMetric >
::CheckGeneralAffineTransform()
{
  if (this->GetTransformForward())
    {
    return this->template CheckGeneralAffineTransformTemplated<MovingTransformType>();
    }
  else
    {
    return this->template CheckGeneralAffineTransformTemplated<FixedTransformType>();
    }
}

/**
 * The templated version of CheckGeneralAffineTransform to check if the
 * transform is a general affine transform that maps a line segment to
 * a line segment.
 *
 * Examples are subclasses of MatrixOffsetTransformBaseType, TranslationTransform,
 * Rigid3DPerspectiveTransform, IdentityTransform, etc.
 */
template< class TMetric >
template< class TTransform >
bool
RegistrationParameterScalesEstimator< TMetric >
::CheckGeneralAffineTransformTemplated()
{
  typedef typename TTransform::ScalarType ScalarType;
  const SizeValueType InputSpaceDimension = TTransform::InputSpaceDimension;
  const SizeValueType OutputSpaceDimension = TTransform::OutputSpaceDimension;

  typedef MatrixOffsetTransformBase<ScalarType, InputSpaceDimension, OutputSpaceDimension>
          MatrixOffsetTransformBaseType;
  typedef TranslationTransform<ScalarType, InputSpaceDimension>
          TranslationTransformType;
  typedef IdentityTransform<ScalarType, InputSpaceDimension>
          IdentityTransformType;
  typedef Rigid3DPerspectiveTransform<ScalarType>
          Rigid3DPerspectiveTransformType;

  const TransformBase *transform = this->GetTransform();

  if ( dynamic_cast< const MatrixOffsetTransformBaseType * >( transform ) != NULL
    || dynamic_cast< const TranslationTransformType * >( transform ) != NULL
    || dynamic_cast< const IdentityTransformType * >( transform ) != NULL
    || dynamic_cast< const Rigid3DPerspectiveTransformType * >( transform ) != NULL
    )
    {
    return true;
    }

  return false;
}

/**
 *  Get the index of the virtual image center.
 */
template< class TMetric >
typename RegistrationParameterScalesEstimator< TMetric >::VirtualIndexType
RegistrationParameterScalesEstimator< TMetric >
::GetVirtualDomainCentralIndex()
{
  VirtualImageConstPointer image = this->m_Metric->GetVirtualImage();
  if( image.IsNull() )
    {
    itkExceptionMacro("VirtualImage has not been defined in metric.");
    }
  VirtualRegionType region = this->m_Metric->GetVirtualRegion();
  const SizeValueType dim = this->GetDimension();

  VirtualIndexType lowerIndex, upperIndex, centralIndex;
  lowerIndex = region.GetIndex();
  upperIndex = region.GetUpperIndex();

  for (SizeValueType d=0; d<dim; d++)
    {
    centralIndex[d] = (IndexValueType)((lowerIndex[d] + upperIndex[d])/2.0);
    }

  return centralIndex;
}

/**
 *  Get the region around the virtual image center.
 */
template< class TMetric >
typename RegistrationParameterScalesEstimator< TMetric >::VirtualRegionType
RegistrationParameterScalesEstimator< TMetric >
::GetVirtualDomainCentralRegion()
{
  VirtualIndexType centralIndex = this->GetVirtualDomainCentralIndex();

  VirtualRegionType region = this->m_Metric->GetVirtualRegion();
  const SizeValueType dim = this->GetDimension();

  VirtualIndexType lowerIndex, upperIndex;
  lowerIndex = region.GetIndex();
  upperIndex = region.GetUpperIndex();

  for (SizeValueType d=0; d<dim; d++)
    {
    if (lowerIndex[d] < centralIndex[d] - this->m_CentralRegionRadius)
      {
      lowerIndex[d] = centralIndex[d] - this->m_CentralRegionRadius;
      }
    if (upperIndex[d] > centralIndex[d] + this->m_CentralRegionRadius)
      {
      upperIndex[d] = centralIndex[d] + this->m_CentralRegionRadius;
      }
    }

  VirtualRegionType centralRegion;
  centralRegion.SetIndex(lowerIndex);
  centralRegion.SetUpperIndex(upperIndex);

  return centralRegion;
}

/**
 *  Sample the virtual domain with the voxels around the center.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleVirtualDomainWithCentralRegion()
{
  VirtualRegionType centralRegion = this->GetVirtualDomainCentralRegion();
  SampleVirtualDomainWithRegion(centralRegion);
}

/**
 *  Sample the virtual domain with all voxels inside a region.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleVirtualDomainWithRegion(VirtualRegionType region)
{
  VirtualImageConstPointer image = this->m_Metric->GetVirtualImage();
  const SizeValueType total = region.GetNumberOfPixels();
  m_SamplePoints.resize(total);

  /* Set up an iterator within the user specified virtual image region. */
  typedef ImageRegionConstIteratorWithIndex<VirtualImageType> RegionIterator;
  RegionIterator regionIter( image, region );

  VirtualPointType point;

  /* Iterate over the image */
  SizeValueType count = 0;
  regionIter.GoToBegin();
  while( !regionIter.IsAtEnd() )
    {
    image->TransformIndexToPhysicalPoint( regionIter.GetIndex(), point );
    m_SamplePoints[count] = point;
    ++regionIter;
    ++count;
    }
}

/**
 *  Sample the virtual domain with the points at image corners.
 *  And store the results into m_SamplePoints.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleVirtualDomainWithCorners()
{
  VirtualImageConstPointer image = this->m_Metric->GetVirtualImage();

  VirtualRegionType region = this->m_Metric->GetVirtualRegion();
  VirtualIndexType firstCorner = region.GetIndex();
  VirtualIndexType corner;
  VirtualPointType point;

  VirtualSizeType size = region.GetSize();
  const int cornerNumber = 1 << VirtualDimension; // 2^Dimension

  m_SamplePoints.resize(cornerNumber);

  for(int i=0; i<cornerNumber; i++)
    {
    int bit;
    for (int d=0; d<VirtualDimension; d++)
      {
      bit = (int) (( i & (1 << d) ) != 0); // 0 or 1
      corner[d] = firstCorner[d] + bit * (size[d] - 1);
      }

    image->TransformIndexToPhysicalPoint(corner, point);
    m_SamplePoints[i] = point;
    }
}

/**
 * Sample the physical points of the virtual domain in a uniform random distribution.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleVirtualDomainRandomly()
{
  VirtualImageConstPointer image = this->m_Metric->GetVirtualImage();

  if (m_NumberOfRandomSamples == 0)
    {
    const SizeValueType total = this->m_Metric->GetVirtualRegion().GetNumberOfPixels();
    if (total <= SizeOfSmallDomain)
      {
      m_NumberOfRandomSamples = total;
      }
    else
      {
      FloatType ratio = 1 + vcl_log((FloatType)total/SizeOfSmallDomain);
      //ratio >= 1 since total/SizeOfSmallDomain > 1

      m_NumberOfRandomSamples = static_cast<int>(SizeOfSmallDomain * ratio);
      if (m_NumberOfRandomSamples > total)
        {
        m_NumberOfRandomSamples = total;
        }
      }
    }

  m_SamplePoints.resize(m_NumberOfRandomSamples);

  // Set up a random iterator within the user specified virtual image region.
  typedef ImageRandomConstIteratorWithIndex<VirtualImageType> RandomIterator;
  RandomIterator randIter( image, this->m_Metric->GetVirtualRegion() );

  VirtualPointType point;

  randIter.SetNumberOfSamples( m_NumberOfRandomSamples );
  randIter.GoToBegin();
  for (SizeValueType i=0; i<m_NumberOfRandomSamples; i++)
    {
    image->TransformIndexToPhysicalPoint( randIter.GetIndex(), point );
    m_SamplePoints[i] = point;
    ++randIter;
    }
}

/**
 * Sample the virtual domain using a point set.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleVirtualDomainWithPointSet()
{
  /* The virtual domain point set must already be supplied, either
   * by the user or automatically by this class in the case of
   * PointSet metrics. */
  if( ! this->m_VirtualDomainPointSet )
    {
    itkExceptionMacro("The virtual domain point set has not been set.");
    }
  if( this->m_VirtualDomainPointSet->GetNumberOfPoints() < 1 )
    {
    itkExceptionMacro("The virtual domain point set has no points.");
    }
    
  m_SamplePoints.resize( this->m_VirtualDomainPointSet->GetNumberOfPoints() );
  
  typename VirtualPointSetType::PointsContainerConstIterator it( this->m_VirtualDomainPointSet->GetPoints()->Begin() );
  SizeValueType count = 0;
  while( it != this->m_VirtualDomainPointSet->GetPoints()->End() )
    {
    m_SamplePoints[count] = it.Value();
    ++count;
    ++it;
    }
}

/**
 * Sample the virtual domain fully with all pixels.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleVirtualDomainFully()
{
  VirtualRegionType region = this->m_Metric->GetVirtualRegion();
  this->SampleVirtualDomainWithRegion(region);
}

template< class TMetric >
bool
RegistrationParameterScalesEstimator< TMetric >
::MetricIsPointSetToPointSetType()
{
  //FIXME: checking name string temporarily. Why won't the cast below to PointSetMetricType work?
  
  std::string name( this->m_Metric->GetNameOfClass() );
  size_t pos = name.find( "PointSetToPointSetMetricv4");
  std::cout << "MetricIsPointSetToPointSetType name-match result: " << name << " " << pos << " " << std::string::npos << " " << (int)(pos != std::string::npos) << std::endl;

  this->m_Metric->Print(std::cout);

  return pos != std::string::npos;

//////////////
  
  typedef typename TMetric::Superclass MetricBase;
  MetricBase * metricbase = dynamic_cast<MetricBase *> (this->m_Metric.GetPointer());
  std::cout<< "metricbase->GetNameOfClass(): " << metricbase->GetNameOfClass() << std::endl;

//fails - why does this fail??:
//  typedef PointSet<typename TMetric::FixedPixelType, TMetric::FixedDimension> FixedPointSetType;
//  typedef PointSet<typename TMetric::MovingPixelType, TMetric::MovingDimension> MovingPointSetType;
//  typedef PointSetToPointSetMetric<FixedPointSetType, MovingPointSetType> PointSetMetricType;
//  PointSetMetricType * metric = dynamic_cast<PointSetMetricType *> ( this->m_Metric.GetPointer() );

//  typedef PointSet<double,2> FixedPointSetType;
//  typedef PointSet<double,2> MovingPointSetType;

// debugging - this doesn't pass, even with the template params taken directly from Print of m_Metric:
  typedef PointSet<double, 2, itk::DefaultStaticMeshTraits<double, 2, 2, float, float, double> > FixedPointSetType;
  typedef PointSet<double, 2, itk::DefaultStaticMeshTraits<double, 2, 2, float, float, double> > MovingPointSetType;
  typedef PointSetToPointSetMetric<FixedPointSetType, MovingPointSetType> PointSetMetricType;
  PointSetMetricType * metric = dynamic_cast<PointSetMetricType *> ( this->m_Metric.GetPointer() );


//passes:
//  typedef typename TMetric::Superclass MetricBase;
//  MetricBase * metric = dynamic_cast<MetricBase *> (this->m_Metric.GetPointer());
//passes:
//  typedef ObjectToObjectMetric<2,2> ObjectMetricType;
//  ObjectMetricType * metric = dynamic_cast<ObjectMetricType *> ( this->m_Metric.GetPointer() );
//passes:
//  TMetric * metric = dynamic_cast<TMetric *> ( this->m_Metric.GetPointer() );

std::cout << "MetricIsPointSetToPointSetType result: " << (int)(metric != NULL) << std::endl;
this->m_Metric->Print(std::cout);
std::cout << std::endl;

  return metric != NULL;  
}

/**
 * Print the information about this class.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "MetricType   = " << std::endl;
  os << indent << typeid(MetricType).name()  << std::endl;

  os << indent << "m_SamplePoints.size = " << std::endl;
  os << indent << this->m_SamplePoints.size()  << std::endl;

  os << indent << "m_TransformForward = " << this->m_TransformForward << std::endl;
  os << indent << "m_SamplingStrategy = " << this->m_SamplingStrategy << std::endl;
  
  os << indent << "m_VirtualDomainPointSet = " << this->m_VirtualDomainPointSet.GetPointer() << std::endl;
}

}  // namespace itk

#endif /* __itkRegistrationParameterScalesEstimator_txx */
