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

#include "itkFEMLoadNoisyLandmark.h"

namespace itk
{
namespace fem
{

void LoadNoisyLandmark::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Confidence: " << this->m_Confidence << std::endl;
  os << indent << "Error Norm: " << this->m_ErrorNorm << std::endl;
  os << indent << "Simulated Displacement: " << this->m_SimulatedDisplacement
      << std::endl;
  os << indent << "Real Displacement: " << this->m_RealDisplacement << std::endl;
}

}
}  // end namespace itk::fem
