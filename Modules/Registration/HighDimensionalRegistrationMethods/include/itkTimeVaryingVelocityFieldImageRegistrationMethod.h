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
#ifndef __itkTimeVaryingVelocityFieldImageRegistrationMethod_h
#define __itkTimeVaryingVelocityFieldImageRegistrationMethod_h

#include "itkSimpleImageRegistrationMethod.h"

#include "itkGaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform.h"

namespace itk
{
//Forward-declare these because of module dependency conflict.
//They will soon be moved to a different module, at which
// time this can be removed.
template <unsigned int VDimension, class TDataHolder>
class ImageToData;
template <class TDataHolder>
class Array1DToData;

/** \class TimeVaryingVelocityFieldImageRegistrationMethod
 * \brief Interface method for the current registration framework
 * using the time varying velocity field transform.
 *
 *
 * Output: The output is the updated transform which has been added to the
 * composite transform.
 *
 * This derived class from the SimpleImageRegistrationMethod class
 * is specialized to handle time-varying velocity field transforms
 * of which there are currently 2:
 *
 * 1) TimeVaryingDisplacementFieldTransform
 * 2) GaussianSmoothingOnUpdateTimeVaryingDisplacementFieldTransform
 *
 * The latter is derived from the former and performs an optional
 * spatial and temporal smoothing on the update and total velocity
 * fields. Integration of the velocity field is performed using
 * 4th order Runge Kutta and is performed using the class
 * itkTimeVaryingVelocityFieldIntegrationImageFilter.
 *
 * Optimization occurs in an iterative fashion where for each
 * sample time point, t, in the velocity field, we integrate
 * the velocity field in the range [0, t] to yield the
 * displacement field which warps fixed image to time point
 * t. Simultaneously, we integrate the velocity field in
 * the range [t, 1] to yield the displacement field transform
 * which warps the moving image to time point t.  The metric
 * derivative for each time point of the velocity field
 * calculated in this way produces the normalized update field
 * (or gradient) of the velocity field to be added to the total
 * field at each iteration after being multiplied by the
 * learning rate and optionally smoothed.  Mathematically,
 * this can be described as

 * v_{total} = G_1( v_{total} + \lambda * G_2( v_{update} ) )
 *
 * where

 * G_1 = gaussian smoothing on the total field
 * G_2 = gaussian smoothing on the update field
 * \lambda = learning rate
 * v_{update} = the normalized velocity field where we
 * normalize the velocity field at each time point
 * separately by the max norm of the field at that time
 * point. This is done due to a weakly necessary
 * (but not sufficient) condition being that the velocity
 * field have a constant norm for all time points.
 *
 * \author Nick Tustison
 * \author Brian Avants
 *
 * \ingroup ITKHighDimensionalRegistrationMethods
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform =
  GaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform<double, GetImageDimension<TFixedImage>::ImageDimension> >
class ITK_EXPORT TimeVaryingVelocityFieldImageRegistrationMethod
: public SimpleImageRegistrationMethod<TFixedImage, TMovingImage, TTransform>
{
public:
  /** Standard class typedefs. */
  typedef TimeVaryingVelocityFieldImageRegistrationMethod                       Self;
  typedef SimpleImageRegistrationMethod<TFixedImage, TMovingImage, TTransform>  Superclass;
  typedef SmartPointer<Self>                                                    Pointer;
  typedef SmartPointer<const Self>                                              ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** ImageDimension constants */
  itkStaticConstMacro( ImageDimension, unsigned int, TFixedImage::ImageDimension );

  /** Run-time type information (and related methods). */
  itkTypeMacro( TimeVaryingVelocityFieldImageRegistrationMethod, SimpleImageRegistrationMethod );

  /** Input typedefs for the images and transforms. */
  typedef TFixedImage                                                 FixedImageType;
  typedef typename FixedImageType::Pointer                            FixedImagePointer;
  typedef TMovingImage                                                MovingImageType;
  typedef typename MovingImageType::Pointer                           MovingImagePointer;

  /** Metric and transform typedefs */
  typedef typename Superclass::MetricType                             MetricType;
  typedef typename MetricType::Pointer                                MetricPointer;
  typedef typename MetricType::VirtualImageType                       VirtualImageType;

  typedef TTransform                                                  TransformType;
  typedef typename TransformType::Pointer                             TransformPointer;
  typedef typename TransformType::ScalarType                          RealType;
  typedef typename TransformType::DerivativeType                      DerivativeType;
  typedef typename TransformType::DerivativeValueType                 DerivativeValueType;
  typedef typename TransformType::TimeVaryingVelocityFieldType        TimeVaryingVelocityFieldType;
  typedef typename TimeVaryingVelocityFieldType::Pointer              TimeVaryingVelocityFieldPointer;
  typedef typename TransformType::DisplacementFieldType               DisplacementFieldType;
  typedef typename DisplacementFieldType::Pointer                     DisplacementFieldPointer;
  typedef typename TimeVaryingVelocityFieldType::PixelType            DisplacementVectorType;

  typedef CompositeTransform<RealType, ImageDimension>                CompositeTransformType;

  typedef typename Superclass::TransformOutputType                    TransformOutputType;
  typedef typename TransformOutputType::Pointer                       TransformOutputPointer;

  typedef Array<SizeValueType>                                        NumberOfIterationsArrayType;

  /** Set/Get the learning rate. */
  itkSetMacro( LearningRate, RealType );
  itkGetConstMacro( LearningRate, RealType );

  /** Set/Get the number of integration steps. */
  itkSetMacro( NumberOfIntegrationStepsPerTimeIndex, SizeValueType );
  itkGetConstMacro( NumberOfIntegrationStepsPerTimeIndex, SizeValueType );

  /** Set/Get the number of iterations per level. */
  itkSetMacro( NumberOfIterationsPerLevel, NumberOfIterationsArrayType );
  itkGetConstMacro( NumberOfIterationsPerLevel, NumberOfIterationsArrayType );

  /** Set/Get the convergence threshold */
  itkSetMacro( ConvergenceThreshold, RealType );
  itkGetConstMacro( ConvergenceThreshold, RealType );

protected:
  TimeVaryingVelocityFieldImageRegistrationMethod();
  virtual ~TimeVaryingVelocityFieldImageRegistrationMethod();
  virtual void PrintSelf( std::ostream & os, Indent indent ) const;

  /** Perform the registration. */
  virtual void  GenerateData();

  /** Handle optimization internally */
  virtual void StartOptimization();

private:
  TimeVaryingVelocityFieldImageRegistrationMethod( const Self & );   //purposely not
                                                             // implemented
  void operator=( const Self & );                            //purposely not

  SizeValueType                                                   m_NumberOfIntegrationStepsPerTimeIndex;

  RealType                                                        m_LearningRate;

  RealType                                                        m_ConvergenceThreshold;

  NumberOfIterationsArrayType                                     m_NumberOfIterationsPerLevel;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTimeVaryingVelocityFieldImageRegistrationMethod.hxx"
#endif

#endif
