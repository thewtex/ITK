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
#ifndef __itkThinPlateSplineRadialBasisFunctionR_h
#define __itkThinPlateSplineRadialBasisFunctionR_h

#include "itkThinPlateSplineRadialBasisFunctionBase.h"

namespace itk
{
/** \class ThinPlateSplineRadialBasisFunctionR
 *  Given an input (for radial basis function, the
 *  Euclidean norm), returns the input multiplied by
 *  a scalar.
 *
 * \ingroup ITK-Transform
 * \ingroup Functions ThinPlateSplineRadialBasisFunctions
 */
template<class ScalarType>
class ITK_EXPORT ThinPlateSplineRadialBasisFunctionR:public ThinPlateSplineRadialBasisFunctionBase<ScalarType>
{
public:
  /** Standard class typedefs. */
  typedef ThinPlateSplineRadialBasisFunctionR                Self;
  typedef ThinPlateSplineRadialBasisFunctionBase<ScalarType> Superclass;
  typedef SmartPointer< Self >                               Pointer;
  typedef SmartPointer<const Self>                           ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThinPlateSplineRadialBasisFunctionR, ThinPlateSplineRadialBasisFunctionBase);

  /** Evaluate at the specified input position */
  virtual ScalarType Evaluate(const ScalarType & input) const;

protected:
  ThinPlateSplineRadialBasisFunctionR() { }
  ~ThinPlateSplineRadialBasisFunctionR() { }
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

private:
  ThinPlateSplineRadialBasisFunctionR(const Self &); //purposely not implemented
  void operator=(const Self &);         //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkThinPlateSplineRadialBasisFunctionR.txx"
#endif

#endif
