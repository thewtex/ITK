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
#ifndef itkShapeRelabelLabelMapFilter_hxx
#define itkShapeRelabelLabelMapFilter_hxx

#include "itkLabelMapUtilities.h"

namespace itk
{
template <typename TImage>
ShapeRelabelLabelMapFilter<TImage>::ShapeRelabelLabelMapFilter()
  : m_Attribute(LabelObjectType::NUMBER_OF_PIXELS)
{}

template <typename TImage>
void
ShapeRelabelLabelMapFilter<TImage>::GenerateData()
{
  switch (m_Attribute)
  {
    itkShapeLabelMapFilterDispatchMacro() default : itkExceptionMacro("Unknown attribute type");
    break;
  }
}

template <typename TImage>
void
ShapeRelabelLabelMapFilter<TImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ReverseOrdering: " << m_ReverseOrdering << std::endl;
  os << indent << "Attribute: " << LabelObjectType::GetNameFromAttribute(m_Attribute) << " (" << m_Attribute << ')'
     << std::endl;
}
} // end namespace itk
#endif
