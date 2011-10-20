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
#if defined( _MSC_VER )
#pragma warning ( disable : 4786 )
#endif

#include "itkVideoIOBase.h"

namespace itk
{

VideoIOBase::VideoIOBase()
{
}

VideoIOBase::~VideoIOBase()
{
}

void VideoIOBase::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

//
// ResetMembers
//
void VideoIOBase::ResetMembers()
{
  // default parameter values, suitable for use when closed/inactive:
  const IOPixelType         pixelTypeDefault       = SCALAR;
  const IOComponentType     componentTypeDefault   = UCHAR;
  const unsigned int        numComponentsDefault   = 1;
  const unsigned int        numDimensionsDefault   = 2;
  std::vector<double> dimensionsDefault(2, 0.0); //= {0.0, 0.0}
  std::vector<double> spacingDefault(   2, 1.0); //= {1.0, 1.0}
  std::vector<double> originDefault(    2, 0.0); //= {0.0, 0.0}

  // Members specific to VideoIOBase
  m_FramesPerSecond = 0;
  m_FrameTotal = 0;
  m_CurrentFrame = 0;
  m_IFrameInterval = 0;
  m_LastIFrame = 0;
  m_Ratio = 0.0;
  m_PositionInMSec = 0.0;
  m_WriterOpen = false;
  m_ReaderOpen = false;

  // Members inherited from ImageIOBase
  m_PixelType = pixelTypeDefault;
  m_ComponentType = componentTypeDefault;
  m_NumberOfComponents = numComponentsDefault;
  m_Dimensions.clear();
  this->SetNumberOfDimensions( numDimensionsDefault );
  for (unsigned int i = 0; i < numDimensionsDefault; ++i)
    {
    m_Spacing[i] = spacingDefault[i];
    m_Origin[i]  = originDefault[i];
    }

  this->Modified();
}

//
// InsureDimensionCount
//
void VideoIOBase::EnsureDimensionCount(unsigned int d)
{
  if (d > this->GetNumberOfDimensions())
    {
    unsigned int oldNumDimensions = m_NumberOfDimensions;

    // The following grows all the appropriate vectors, preserving old values:
    this->SetNumberOfDimensions( d );

    // Fill in remaining values with the value from the closest dimension
    for (unsigned int i = oldNumDimensions; i < d; ++i)
      {
      m_Dimensions[i]  = m_Dimensions[ oldNumDimensions-1 ];
      m_Spacing[i]     = m_Spacing[    oldNumDimensions-1 ];
      m_Origin[i]      = m_Origin[     oldNumDimensions-1 ];
      }

    this->Modified();
    }
}

} //namespace itk end
