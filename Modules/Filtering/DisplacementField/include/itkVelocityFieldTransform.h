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
#ifndef __itkVelocityFieldTransform_h
#define __itkVelocityFieldTransform_h

#include "itkConstantVelocityFieldTransform.h"

namespace itk
{

/** \class VelocityFieldTransform
 * \brief Provides local/dense/high-dimensionality transformation via a
 * a velocity field.
 *
 * \author Nick Tustison
 * \author Brian Avants
 *
 * \ingroup ITKDisplacementField
 */
template
<class TScalar, unsigned int NDimensions>
class ITK_EXPORT VelocityFieldTransform :
  public ConstantVelocityFieldTransform<TScalar, NDimensions>
{
public:
  /** Standard class typedefs. */
  typedef VelocityFieldTransform                                Self;
  typedef ConstantVelocityFieldTransform<TScalar, NDimensions>  Superclass;
  typedef SmartPointer<Self>                                    Pointer;
  typedef SmartPointer<const Self>                              ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( VelocityFieldTransform, ConstantVelocityFieldTransform );

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** InverseTransform type. */
  typedef typename Superclass:: InverseTransformBasePointer InverseTransformBasePointer;

  /** Scalar type. */
  typedef typename Superclass::ScalarType ScalarType;

  /** Type of the input parameters. */
  typedef typename Superclass::ParametersType          ParametersType;
  typedef typename Superclass::ParametersValueType     ParametersValueType;

  /** Transform category type. */
  typedef typename Superclass::TransformCategoryType TransformCategoryType;

  /** The number of parameters defininig this transform. */
  typedef typename Superclass::NumberOfParametersType NumberOfParametersType;

  /** Standard coordinate point type for this class. */
  typedef typename Superclass::InputPointType  InputPointType;
  typedef typename Superclass::OutputPointType OutputPointType;

  /** Standard vector type for this class. */
  typedef typename Superclass::InputVectorType  InputVectorType;
  typedef typename Superclass::OutputVectorType OutputVectorType;

  typedef typename Superclass::InputVectorPixelType  InputVectorPixelType;
  typedef typename Superclass::OutputVectorPixelType OutputVectorPixelType;

  /** Derivative type */
  typedef typename Superclass::DerivativeType DerivativeType;

  /** Dimension of the velocity field . */
  itkStaticConstMacro( VelocityFieldDimension, unsigned int, NDimensions + 1 );

  /** Dimension of the vector spaces. */
  itkStaticConstMacro( Dimension, unsigned int, NDimensions );

  /** Define the displacement field type and corresponding interpolator type. */
  typedef typename Superclass::DisplacementFieldType   DisplacementFieldType;
  typedef typename DisplacementFieldType::Pointer      DisplacementFieldPointer;

  /** Define the displacement field type and corresponding interpolator type. */
  typedef Image<OutputVectorType, VelocityFieldDimension>       VelocityFieldType;
  typedef typename VelocityFieldType::Pointer                   VelocityFieldPointer;

  /** Standard types for the velocity Field */
  typedef typename VelocityFieldType::IndexType      IndexType;
  typedef typename VelocityFieldType::RegionType     RegionType;
  typedef typename VelocityFieldType::SizeType       SizeType;
  typedef typename VelocityFieldType::SpacingType    SpacingType;
  typedef typename VelocityFieldType::DirectionType  DirectionType;
  typedef typename VelocityFieldType::PointType      PointType;
  typedef typename VelocityFieldType::PixelType      PixelType;

  typedef VectorInterpolateImageFunction<VelocityFieldType, ScalarType> VelocityFieldInterpolatorType;
  typedef typename VelocityFieldInterpolatorType::Pointer               VelocityFieldInterpolatorPointer;

  /** Define the internal parameter helper used to access the field */
  typedef ImageVectorOptimizerParametersHelper<ScalarType, Dimension, VelocityFieldDimension>
    OptimizerParametersHelperType;

  /** Get/Set the velocity field. */
  itkGetObjectMacro( VelocityField, VelocityFieldType );
  /** Set the displacement field. Create special set accessor to update
   * interpolator and assign displacement field to transform parameters
   * container. */
  virtual void SetVelocityField( VelocityFieldType * );

  virtual void SetFixedParameters( const ParametersType & );

  /** Get/Set the interpolator. */
  itkGetObjectMacro( VelocityFieldInterpolator, VelocityFieldInterpolatorType );
  /* Create out own set accessor that assigns the velocity field */
  virtual void SetVelocityFieldInterpolator( VelocityFieldInterpolatorType * );

  /** Get the modification time of velocity field */
  itkGetConstReferenceMacro( VelocityFieldSetTime, unsigned long );

  /**
   * Set the deformation field. We want to override the base class
   * implementation since we don't want to optimize over the deformation
   * field for this class but rather the time-varying velocity field
   */
  itkSetObjectMacro( DisplacementField, DisplacementFieldType );

  /** Return an inverse of this transform. */
  bool GetInverse( Self *inverse ) const;

  /** Return an inverse of this transform. */
  virtual InverseTransformBasePointer GetInverseTransform() const;

  /** Trigger the computation of the displacement field by integrating the velocity field. */
  virtual void IntegrateVelocityField() {};

protected:

  VelocityFieldTransform();
  virtual ~VelocityFieldTransform();
  void PrintSelf( std::ostream& os, Indent indent ) const;

  /** Clone the current transform */
  virtual typename LightObject::Pointer InternalClone() const;

  typename DisplacementFieldType::Pointer CopyDisplacementField( const DisplacementFieldType * ) const;

  VelocityFieldPointer                      m_VelocityField;

  /** The interpolator. */
  typename VelocityFieldInterpolatorType::Pointer          m_VelocityFieldInterpolator;

  /** Track when the VELOCITY field was last set/assigned, as
   * distinct from when it may have had its contents modified. */
  unsigned long m_VelocityFieldSetTime;

private:
  VelocityFieldTransform( const Self & ); // purposely not implemented
  void operator=( const Self & );             // purposely not implemented

  /**
   * Convenience method which reads the information from the current
   * velocity field into m_FixedParameters.
   */
  virtual void SetFixedParametersFromVelocityField() const;

};

} // end namespace itk

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkVelocityFieldTransform+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkVelocityFieldTransform.hxx"
#endif

#endif // __itkVelocityFieldTransform_h
