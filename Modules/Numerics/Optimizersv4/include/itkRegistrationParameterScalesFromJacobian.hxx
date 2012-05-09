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
#ifndef __itkRegistrationParameterScalesFromJacobian_hxx
#define __itkRegistrationParameterScalesFromJacobian_hxx

#include "itkRegistrationParameterScalesFromJacobian.h"

namespace itk
{

template< class TMetric >
RegistrationParameterScalesFromJacobian< TMetric >
::RegistrationParameterScalesFromJacobian()
{
}

/** Compute parameter scales from average jacobian norms.
 *  For each parameter, compute the squared norm of its transform Jacobian,
 *  then average the squared norm over the sample points. This average is
 *  used as the scale of this parameter.
 */
template< class TMetric >
void
RegistrationParameterScalesFromJacobian< TMetric >
::EstimateScales(ScalesType &parameterScales)
{
  this->CheckAndSetInputs();
  this->SetScalesSamplingStrategy();
  this->SampleVirtualDomain();

  const SizeValueType numPara = this->GetNumberOfLocalParameters();

  parameterScales.SetSize(numPara);

  ParametersType norms(numPara);

  const SizeValueType numSamples = this->m_SamplePoints.size();

  norms.Fill( NumericTraits< typename ParametersType::ValueType >::Zero );
  parameterScales.Fill( NumericTraits< typename ScalesType::ValueType >::One );

  // checking each sample point
  for (SizeValueType c=0; c<numSamples; c++)
    {
    const VirtualPointType point = this->m_SamplePoints[c];

    ParametersType squaredNorms(numPara);
    this->ComputeSquaredJacobianNorms( point, squaredNorms );

    norms = norms + squaredNorms;
    } //for numSamples

  if (numSamples > 0)
    {
    for (SizeValueType p=0; p<numPara; p++)
      {
      parameterScales[p] = norms[p] / numSamples;
      }
    }
}

/**
 *  Compute the scale for a STEP, the impact of a STEP on the transform.
 */
template< class TMetric >
typename RegistrationParameterScalesFromJacobian< TMetric >::FloatType
RegistrationParameterScalesFromJacobian< TMetric >
::EstimateStepScale(const ParametersType &step)
{
  this->CheckAndSetInputs();
  this->SetStepScaleSamplingStrategy();
  this->SampleVirtualDomain();

  ScalesType sampleScales;
  this->ComputeSampleStepScales(step, sampleScales);

  const SizeValueType numSamples = this->m_SamplePoints.size();
  FloatType scaleSum = NumericTraits< FloatType >::Zero;

  // checking each sample point
  for (SizeValueType c=0; c<numSamples; c++)
    {
    scaleSum += sampleScales[c];
    }

  return scaleSum / numSamples;
}

/**
 * Estimate the scales of local steps. For each voxel, we compute the impact
 * of a STEP on its location as in EstimateStepScale. Then we attribute this
 * impact to the corresponding local parameters.
 */
template< class TMetric >
void
RegistrationParameterScalesFromJacobian< TMetric >
::EstimateLocalStepScales(const ParametersType &step,
    ScalesType &localStepScales)
{
  if (!this->HasLocalSupport())
    {
    itkExceptionMacro(<< "EstimateLocalStepScales: the transform doesn't have local support.");
    }

  this->CheckAndSetInputs();
  this->SetStepScaleSamplingStrategy();
  this->SampleVirtualDomain();

  ScalesType sampleScales;
  this->ComputeSampleStepScales(step, sampleScales);

  const SizeValueType numSamples = this->m_SamplePoints.size();
  const SizeValueType numPara = this->GetNumberOfLocalParameters();
  const SizeValueType numAllPara = this->GetTransform()->GetNumberOfParameters();
  const SizeValueType numLocals = numAllPara / numPara;

  localStepScales.SetSize(numLocals);
  localStepScales.Fill(NumericTraits<typename ScalesType::ValueType>::Zero);

  // checking each sample point
  for (SizeValueType c=0; c<numSamples; c++)
    {
    VirtualPointType &point = this->m_SamplePoints[c];
    IndexValueType localId = this->m_Metric->ComputeParameterOffsetFromVirtualPoint( point, NumericTraits<SizeValueType>::One);
    localStepScales[localId] = sampleScales[c];
    }

  return;

}

/**
 *  Compute the step scales for samples, i.e. the impacts on each sampled
 *  voxel from a change on the transform.
 */
template< class TMetric >
void
RegistrationParameterScalesFromJacobian< TMetric >
::ComputeSampleStepScales(const ParametersType &step, ScalesType &sampleScales)
{
  const SizeValueType numSamples = this->m_SamplePoints.size();
  const SizeValueType dim = this->GetDimension();
  const SizeValueType numPara = this->GetNumberOfLocalParameters();

  sampleScales.SetSize(numSamples);

  itk::Array<FloatType> dTdt(dim);

  // checking each sample point
  for (SizeValueType c=0; c<numSamples; c++)
    {
    const VirtualPointType &point = this->m_SamplePoints[c];

    JacobianType jacobian;
    if (this->GetTransformForward())
      {
      this->m_Metric->GetMovingTransform()->ComputeJacobianWithRespectToParameters(point, jacobian);
      }
    else
      {
      this->m_Metric->GetFixedTransform()->ComputeJacobianWithRespectToParameters(point, jacobian);
      }

    if (!this->HasLocalSupport())
      {
      dTdt = jacobian * step;
      }
    else
      {
      SizeValueType offset = this->m_Metric->ComputeParameterOffsetFromVirtualPoint(point, numPara);

      ParametersType localStep(numPara);
      for (SizeValueType p=0; p<numPara; p++)
        {
        localStep[p] = step[offset + p];
        }
      dTdt = jacobian * localStep;
      }

    sampleScales[c] = dTdt.two_norm();
    }

}

/** Print the information about this class */
template< class TMetric >
void
RegistrationParameterScalesFromJacobian< TMetric >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

}  // namespace itk

#endif /* __itkRegistrationParameterScalesFromJacobian_txx */
