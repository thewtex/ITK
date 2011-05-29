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
#ifndef __itkVectorContainerToListSampleAdaptor_txx
#define __itkVectorContainerToListSampleAdaptor_txx

#include "itkVectorContainerToListSampleAdaptor.h"

namespace itk
{
namespace Statistics
{
template< class TVectorContainer >
VectorContainerToListSampleAdaptor< TVectorContainer >
::VectorContainerToListSampleAdaptor()
{
  m_VectorContainer = 0;
}

template< class TVectorContainer >
void
VectorContainerToListSampleAdaptor< TVectorContainer >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "VectorContainer: ";
  if ( m_VectorContainer.IsNotNull() )
    {
    os << m_VectorContainer << std::endl;
    }
  else
    {
    os << "not set." << std::endl;
    }
}

template< class TVectorContainer >
void
VectorContainerToListSampleAdaptor< TVectorContainer >
::SetVectorContainer(const TVectorContainer *vectorContainer)
{
  m_VectorContainer = vectorContainer;

  this->Modified();
}

template< class TVectorContainer >
const TVectorContainer *
VectorContainerToListSampleAdaptor< TVectorContainer >
::GetVectorContainer()
{
  if ( m_VectorContainer.IsNull() )
    {
    itkExceptionMacro("Vector container has not been set yet");
    }

  return m_VectorContainer.GetPointer();
}

/** returns the number of measurement vectors in this container*/
template< class TVectorContainer >
typename VectorContainerToListSampleAdaptor< TVectorContainer >::InstanceIdentifier
VectorContainerToListSampleAdaptor< TVectorContainer >
::Size() const
{
  if ( m_VectorContainer.IsNull() )
    {
    itkExceptionMacro("Vector container has not been set yet");
    }

  return m_VectorContainer->Size();
}

template< class TVectorContainer >
inline const typename VectorContainerToListSampleAdaptor< TVectorContainer >::MeasurementVectorType &
VectorContainerToListSampleAdaptor< TVectorContainer >
::GetMeasurementVector(InstanceIdentifier identifier) const
{
  if ( m_VectorContainer.IsNull() )
    {
    itkExceptionMacro("Vector container has not been set yet");
    }

  m_TempPoint = m_VectorContainer->GetElement(identifier);
  return m_TempPoint;
}

template< class TVectorContainer >
inline typename VectorContainerToListSampleAdaptor< TVectorContainer >::AbsoluteFrequencyType
VectorContainerToListSampleAdaptor< TVectorContainer >
::GetFrequency(InstanceIdentifier) const
{
  if ( m_VectorContainer.IsNull() )
    {
    itkExceptionMacro("Vector container has not been set yet");
    }

  return 1;
}

template< class TVectorContainer >
typename VectorContainerToListSampleAdaptor< TVectorContainer >::TotalAbsoluteFrequencyType
VectorContainerToListSampleAdaptor< TVectorContainer >
::GetTotalFrequency() const
{
  if ( m_VectorContainer.IsNull() )
    {
    itkExceptionMacro("Vector container has not been set yet");
    }

  return this->Size();
}
} // end of namespace Statistics
} // end of namespace itk

#endif
