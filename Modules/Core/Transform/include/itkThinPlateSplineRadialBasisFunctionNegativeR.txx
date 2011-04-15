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
#ifndef __itkThinPlateSplineRadialBasisFunctionNegativeR_txx
#define __itkThinPlateSplineRadialBasisFunctionNegativeR_txx

#include "itkThinPlateSplineRadialBasisFunctionNegativeR.h"

namespace itk
{

/** Evaluate function */
template<class ScalarType>
ScalarType
ThinPlateSplineRadialBasisFunctionNegativeR< ScalarType>
::Evaluate(const ScalarType& input) const
{
  return static_cast<ScalarType>(-1.0)*input;
}

/** Print the object */
template<class ScalarType>
void
ThinPlateSplineRadialBasisFunctionNegativeR<ScalarType>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  os << indent << "ThinPlateSplineRadialBasisFunctionNegativeR(" << this << ")" << std::endl;
  Superclass::PrintSelf( os, indent );
}

} // end namespace itk

#endif
