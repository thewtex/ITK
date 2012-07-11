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
#ifndef __itkBSplineExponentialDiffeomorphicTransform_h
#define __itkBSplineExponentialDiffeomorphicTransform_h

#include "itkBSplineSmoothingOnUpdateDisplacementFieldTransform.h"

namespace itk
{

/** \class BSplineExponentialDiffeomorphicTransform
 * \brief Modifies the UpdateTransformParameters method to
 * perform a B-spline smoothing of the constant velocity
 * field after adding the update array.
 *
 * Basically permits a diffeomorphic demons registration with
 * arbitrary similarity metric.
 *
 * This class takes as input a constant velocity field, smooths it on demand using
 * the specified B-spline parameters and then scales-and-squares the smooth field
 * to obtain an exponentiated displacement field.
 *
 * \author Nick Tustison
 * \author Brian Avants
 *
 * \ingroup ITKDisplacementField
 */
template<class TScalar, unsigned int NDimensions>
class ITK_EXPORT BSplineExponentialDiffeomorphicTransform :
  public  BSplineSmoothingOnUpdateDisplacementFieldTransform<TScalar, NDimensions>
{
public:
  /** Standard class typedefs. */
  typedef BSplineExponentialDiffeomorphicTransform                                                Self;
  typedef BSplineSmoothingOnUpdateDisplacementFieldTransform<TScalar, NDimensions>   Superclass;
  typedef DisplacementFieldTransform<TScalar, NDimensions>                           SuperSuperclass;
  typedef SmartPointer<Self>                                                         Pointer;
  typedef SmartPointer<const Self>                                                   ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( BSplineExponentialDiffeomorphicTransform, BSplineSmoothingOnUpdateDisplacementFieldTransform );

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** Dimension of the domain spaces. */
  itkStaticConstMacro( Dimension, unsigned int, NDimensions );

  /** Types from superclass */
  typedef typename Superclass::ScalarType               ScalarType;
  typedef typename Superclass::DerivativeType           DerivativeType;
  typedef typename DerivativeType::ValueType            DerivativeValueType;
  typedef typename Superclass::DisplacementFieldType    DisplacementFieldType;
  typedef typename Superclass::DisplacementFieldPointer DisplacementFieldPointer;
  typedef typename Superclass::DisplacementFieldType    ConstantVelocityFieldType;
  typedef typename Superclass::DisplacementFieldPointer ConstantVelocityFieldPointer;
  typedef typename Superclass::DisplacementVectorType   DisplacementVectorType;

  typedef typename Transform<TScalar,NDimensions,NDimensions>::Pointer TransformPointer;

  /**
   * Update the transform's parameters by the values in \c update.  We
   * assume \c update is of the same length as Parameters. Throw exception
   * otherwise. The update process performs an smoothing on the velocity field
   * field by using BSplines.
   * \c factor is a scalar multiplier for each value in update.
   * \c BSplineSmoothDisplacementField is called after the update is
   * added to the field.
   * See base class for more details.
   */
  virtual void UpdateTransformParameters( const DerivativeType & update, ScalarType factor = 1.0 );

  /**
   * Smooth the constant velocity field in-place.
   */
  virtual ConstantVelocityFieldPointer BSplineSmoothConstantVelocityField( ConstantVelocityFieldType * field, ScalarType scalar )
    {
    return this->BSplineSmoothDisplacementField( field, scalar );
    }


protected:
  BSplineExponentialDiffeomorphicTransform();
  virtual ~BSplineExponentialDiffeomorphicTransform();

private:
  BSplineExponentialDiffeomorphicTransform( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented
};

} // end namespace itk

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkBSplineExponentialDiffeomorphicTransform+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkBSplineExponentialDiffeomorphicTransform.hxx"
#endif

#endif // __itkBSplineExponentialDiffeomorphicTransform_h
