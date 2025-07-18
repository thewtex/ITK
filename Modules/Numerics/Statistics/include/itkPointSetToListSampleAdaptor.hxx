/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkPointSetToListSampleAdaptor_hxx
#define itkPointSetToListSampleAdaptor_hxx

namespace itk::Statistics
{
template <typename TPointSet>
PointSetToListSampleAdaptor<TPointSet>::PointSetToListSampleAdaptor()
  : m_PointSet(nullptr)
{
  this->SetMeasurementVectorSize(TPointSet::PointDimension);
}

template <typename TPointSet>
void
PointSetToListSampleAdaptor<TPointSet>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PointSet: ";
  if (m_PointSet.IsNotNull())
  {
    os << m_PointSet << std::endl;
  }
  else
  {
    os << "not set." << std::endl;
  }
}

template <typename TPointSet>
void
PointSetToListSampleAdaptor<TPointSet>::SetPointSet(const TPointSet * pointSet)
{
  m_PointSet = pointSet;
  m_PointsContainer = pointSet->GetPoints();

  this->Modified();
}

template <typename TPointSet>
const TPointSet *
PointSetToListSampleAdaptor<TPointSet>::GetPointSet()
{
  if (m_PointSet.IsNull())
  {
    itkExceptionMacro("Point set has not been set yet");
  }

  return m_PointSet.GetPointer();
}

/** returns the number of measurement vectors in this container*/
template <typename TPointSet>
auto
PointSetToListSampleAdaptor<TPointSet>::Size() const -> InstanceIdentifier
{
  if (m_PointSet.IsNull())
  {
    itkExceptionMacro("Point set has not been set yet");
  }

  return m_PointsContainer->Size();
}

template <typename TPointSet>
inline const typename PointSetToListSampleAdaptor<TPointSet>::MeasurementVectorType &
PointSetToListSampleAdaptor<TPointSet>::GetMeasurementVector(InstanceIdentifier identifier) const
{
  if (m_PointSet.IsNull())
  {
    itkExceptionMacro("Point set has not been set yet");
  }

  m_PointSet->GetPoint(identifier, &m_TempPoint);
  return (MeasurementVectorType &)m_TempPoint;
}

template <typename TPointSet>
inline auto
PointSetToListSampleAdaptor<TPointSet>::GetFrequency(InstanceIdentifier) const -> AbsoluteFrequencyType
{
  if (m_PointSet.IsNull())
  {
    itkExceptionMacro("Point set has not been set yet");
  }

  return 1;
}

template <typename TPointSet>
auto
PointSetToListSampleAdaptor<TPointSet>::GetTotalFrequency() const -> TotalAbsoluteFrequencyType
{
  if (m_PointSet.IsNull())
  {
    itkExceptionMacro("Point set has not been set yet");
  }

  return this->Size();
}
} // namespace itk::Statistics

#endif
