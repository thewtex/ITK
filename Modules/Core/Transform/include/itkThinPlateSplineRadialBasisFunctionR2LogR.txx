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
#ifndef __itkThinPlateSplineRadialBasisFunctionR2LogR_txx
#define __itkThinPlateSplineRadialBasisFunctionR2LogR_txx

#include "itkThinPlateSplineRadialBasisFunctionR2LogR.h"
#include "vcl_cmath.h"

#include <math.h>

namespace itk
{

/** Constructor */
template<class ScalarType>
ThinPlateSplineRadialBasisFunctionR2LogR< ScalarType>
::ThinPlateSplineRadialBasisFunctionR2LogR()
{
}

/** Destructor */
template<class ScalarType>
ThinPlateSplineRadialBasisFunctionR2LogR< ScalarType>
::~ThinPlateSplineRadialBasisFunctionR2LogR()
{
}

/** Evaluate function */
template<class ScalarType>
ScalarType
ThinPlateSplineRadialBasisFunctionR2LogR< ScalarType>
::Evaluate(const ScalarType & input) const
{
  const ScalarType & retValue = ( input > static_cast<ScalarType>(1e-8) ) ? input*input*vcl_log(input) : NumericTraits<ScalarType >::Zero;
  return retValue;
}

/** Print the object */
template<class ScalarType>
void
ThinPlateSplineRadialBasisFunctionR2LogR<ScalarType>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  os << indent << "ThinPlateSplineRadialBasisFunctionR2LogR(" << this << ")" << std::endl;
  Superclass::PrintSelf( os, indent );
}

} // end namespace itk

#endif
