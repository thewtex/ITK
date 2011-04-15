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
#ifndef __itkThinPlateSplineRadialBasisFunctionBase_h
#define __itkThinPlateSplineRadialBasisFunctionBase_h

#include "itkFunctionBase.h"
#include "itkArray.h"

namespace itk
{

  /**
   * \class ThinPlateSplineRadialBasisFunctionBase
   * This class defines a base class for the thin plate spline (TPS) elastic transformation.
   *
   * This pure abstract base class serves as a base class for radial basis functions
   * suitable for use in the ThinPlateSpline Kernel constructions.
   *
   * \ingroup ITK-Transform
   */
template<class ScalarType>
class ThinPlateSplineRadialBasisFunctionBase : public FunctionBase<ScalarType,ScalarType>
{
public:

  /** Standard class typedefs. */
  typedef ThinPlateSplineRadialBasisFunctionBase Self;
  typedef FunctionBase<ScalarType,ScalarType>    Superclass;
  typedef SmartPointer<Self>                     Pointer;
  typedef SmartPointer<const Self>               ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThinPlateSplineRadialBasisFunctionBase, FunctionBase);

  /** Evaluate at the specified input position */
  virtual ScalarType Evaluate(const ScalarType& input) const=0;

protected:

  ThinPlateSplineRadialBasisFunctionBase() { }
  ~ThinPlateSplineRadialBasisFunctionBase() { }

  /** Method to print the object. */
  virtual void PrintSelf( std::ostream& os, Indent indent ) const
    {
    os << indent << "ThinPlateSplineRadialBasisFunctionBase(" << this << ")" << std::endl;
    Superclass::PrintSelf( os, indent );
    }

private:
  ThinPlateSplineRadialBasisFunctionBase(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk

#endif
