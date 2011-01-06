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
#ifndef __itkRigid3DTransform_h
#define __itkRigid3DTransform_h

#include <iostream>
#include "itkVersorRigid3DTransform.h"

namespace itk
{
/** \class Rigid3DTransform
 * \brief Rigid3DTransform of a vector space (e.g. space coordinates)
 *
 * This class has two different behaviours.
 * If ITK_V3_COMPATILITY is ON, the transform behaves as described in
 * Rigid3dTransformBase.
 * If ITK_V3_COMPATILITY is OFF, the transform behaves as described in
 * VersorRigid3DTransform.
 * \ingroup Transforms
 */
template< class TScalarType = double >
// type for scalars (float or double)
class ITK_EXPORT Rigid3DTransform:
#ifdef ITK_V3_COMPATIBILITY
  public Rigid3DTransformBase< TScalarType >
#else
  public VersorRigid3DTransform< TScalarType >
#endif
{
public:
  /** Standard class typedefs. */
  typedef Rigid3DTransform                      Self;
#ifdef ITK_V3_COMPATIBILITY
  typedef Rigid3DTransform< TScalarType >       Superclass;
#else
  typedef VersorRigid3DTransform< TScalarType > Superclass;
#endif
  typedef SmartPointer< Self >                  Pointer;
  typedef SmartPointer< const Self >            ConstPointer;

  /** Run-time type information (and related methods). */
#ifdef ITK_V3_COMPATIBILITY
  itkTypeMacro(Rigid3DTransform, Rigid3DTransformBase);
#else
  itkTypeMacro(Rigid3DTransform, VersorRigid3DTransform);
#endif
  /** New macro for creation of through a Smart Pointer */
  itkNewMacro(Self);

  /** Dimension of the space. */
  itkStaticConstMacro(SpaceDimension, unsigned int, 3);
  itkStaticConstMacro(InputSpaceDimension, unsigned int, 3);
  itkStaticConstMacro(OutputSpaceDimension, unsigned int, 3);
#ifdef ITK_V3_COMPATIBILITY
  itkStaticConstMacro(ParametersDimension, unsigned int, 12);
#else
  itkStaticConstMacro(ParametersDimension, unsigned int, 6);
#endif

  typedef typename Superclass::ParametersType            ParametersType;
  typedef typename Superclass::ParametersValueType       ParametersValueType;
  typedef typename Superclass::JacobianType              JacobianType;
  typedef typename Superclass::ScalarType                ScalarType;
  typedef typename Superclass::InputVectorType           InputVectorType;
  typedef typename Superclass::OutputVectorType          OutputVectorType;
  typedef typename Superclass::OutputVectorValueType     OutputVectorValueType;
  typedef typename Superclass::InputCovariantVectorType  InputCovariantVectorType;
  typedef typename Superclass::OutputCovariantVectorType OutputCovariantVectorType;
  typedef typename Superclass::InputVnlVectorType        InputVnlVectorType;
  typedef typename Superclass::OutputVnlVectorType       OutputVnlVectorType;
  typedef typename Superclass::InputPointType            InputPointType;
  typedef typename Superclass::OutputPointType           OutputPointType;
  typedef typename Superclass::MatrixType                MatrixType;
  typedef typename Superclass::InverseMatrixType         InverseMatrixType;
  typedef typename Superclass::MatrixValueType           MatrixValueType;
  typedef typename Superclass::CenterType                CenterType;
  typedef typename Superclass::TranslationType           TranslationType;
  typedef typename Superclass::OffsetType                OffsetType;

  /** Base inverse transform type. This type should not be changed to the
   * concrete inverse transform type or inheritance would be lost. */
  typedef typename Superclass::InverseTransformBaseType InverseTransformBaseType;
  typedef typename InverseTransformBaseType::Pointer    InverseTransformBasePointer;

protected:
  Rigid3DTransform(){}
#ifdef ITK_V3_COMPATIBILITY
  Rigid3DTransform(unsigned int spaceDim,
                   unsigned int paramDim)
    :Superclass(spaceDim, paramDim)
  {
  }
  Rigid3DTransform(const MatrixType & matrix,
                   const OutputVectorType & offset)
    :Superclass(matrix, offset)
  {
  }
#endif
private:
  Rigid3DTransform(const Self &); //purposely not implemented
  void operator=(const Self &);   //purposely not implemented
};                                //class Rigid3DTransform
}  // namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_Rigid3DTransform(_, EXPORT, TypeX, TypeY)                \
  namespace itk                                                               \
  {                                                                           \
  _( 1 ( class EXPORT Rigid3DTransform< ITK_TEMPLATE_1 TypeX > ) )            \
  namespace Templates                                                         \
  {                                                                           \
  typedef Rigid3DTransform< ITK_TEMPLATE_1 TypeX > Rigid3DTransform##TypeY; \
  }                                                                           \
  }

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkRigid3DTransform+-.h"
#endif

#endif /* __itkRigid3DTransform_h */
