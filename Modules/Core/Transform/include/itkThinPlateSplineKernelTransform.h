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
#ifndef __itkThinPlateSplineKernelTransform_h
#define __itkThinPlateSplineKernelTransform_h

#include "itkThinPlateSplineKernelTransformBase.h"
#include "itkThinPlateSplineRadialBasisFunctionR.h"

namespace itk
{
/** \class ThinPlateSplineKernelTransform
 *
 * This class is a specific derivation of the ThinPlateSplineKernelTransformBase
 * that implements a backwards compatible class for using the \$ || r || \$ as the
 * kernel function.
 *
 * \ingroup ITK-Transform
 * \ingroup Transforms
 */
template <class TScalarType, unsigned int NDimensions = 3>
  class ITK_EXPORT ThinPlateSplineKernelTransform:
    public ThinPlateSplineKernelTransformBase< TScalarType, NDimensions >
{
public:
  /** Standard class typedefs. */
  typedef ThinPlateSplineKernelTransform                                  Self;
  typedef ThinPlateSplineKernelTransformBase< TScalarType, NDimensions >  Superclass;
  typedef SmartPointer< Self >                                            Pointer;
  typedef SmartPointer< const Self>                                       ConstPointer;

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThinPlateSplineKernelTransform,
               ThinPlateSplineKernelTransformBase);

protected:
  /** Create new instance.  Computes r for use as the kernel in the G matrix. */
  ThinPlateSplineKernelTransform()
    {
    this->SetFunction( ThinPlateSplineRadialBasisFunctionR< TScalarType >::New() );
    }
  virtual ~ThinPlateSplineKernelTransform() {}

private:
  ThinPlateSplineKernelTransform(const Self &); //purposely not implemented
  void operator=(const Self &);  //purposely not implemented
};
} // end namespace itk

#endif
