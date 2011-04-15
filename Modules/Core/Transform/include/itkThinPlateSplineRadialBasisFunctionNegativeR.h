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
#ifndef __itkThinPlateSplineRadialBasisFunctionNegativeR_h
#define __itkThinPlateSplineRadialBasisFunctionNegativeR_h

#include "itkThinPlateSplineRadialBasisFunctionBase.h"

namespace itk
{
/** \class ThinPlateSplineRadialBasisFunctionNegativeR
 *  Given an input (for radial basis function, the
 *  Euclidean norm), returns the input multiplied by
 *  a scalar.
 *
 * \ingroup ITK-Transform
 * \ingroup Functions ThinPlateSplineRadialBasisFunctions
 */
template<class ScalarType>
class ITK_EXPORT ThinPlateSplineRadialBasisFunctionNegativeR:public ThinPlateSplineRadialBasisFunctionBase<ScalarType>
{
public:
  /** Standard class typedefs. */
  typedef ThinPlateSplineRadialBasisFunctionNegativeR        Self;
  typedef ThinPlateSplineRadialBasisFunctionBase<ScalarType> Superclass;
  typedef SmartPointer< Self >                               Pointer;
  typedef SmartPointer<const Self>                           ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThinPlateSplineRadialBasisFunctionNegativeR, ThinPlateSplineRadialBasisFunctionBase);

  /** Evaluate at the specified input position */
  virtual ScalarType Evaluate(const ScalarType & input) const;

protected:
  ThinPlateSplineRadialBasisFunctionNegativeR() { }
  ~ThinPlateSplineRadialBasisFunctionNegativeR() { }
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

private:
  ThinPlateSplineRadialBasisFunctionNegativeR(const Self &); //purposely not implemented
  void operator=(const Self &);         //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkThinPlateSplineRadialBasisFunctionNegativeR.txx"
#endif

#endif
