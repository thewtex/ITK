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
#ifndef __itkParameterScaleEstimator_h
#define __itkParameterScaleEstimator_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkImageMomentsCalculator.h"
#include "itkMatrix.h"

#include <iostream>

namespace itk
{

/** \class ParameterScaleEstimator
 * \brief ParameterScaleEstimator is a helper class intended to
 * initialize the scales of Transform parameters in registration.
 *
 * This class is connected to the fixed image, moving image and transform
 * involved in the registration.
 *
 * \ingroup Optimizers
 */
template < class TFixedImage,
           class TMovingImage,
           class TTransform >
class ITK_EXPORT ParameterScaleEstimator : public Object
{
public:
  /** Standard class typedefs. */
  typedef ParameterScaleEstimator          Self;
  typedef Object                           Superclass;
  typedef SmartPointer<Self>               Pointer;
  typedef SmartPointer<const Self>         ConstPointer;

  /** New macro for creation of through a Smart Pointer. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( ParameterScaleEstimator, Object );

  /** Type of the transform to initialize */
  typedef typename TTransform                       TransformType;
  typedef typename TransformType::Pointer           TransformPointer;
  typedef typename TransformType::ParametersType    ParametersType;

  /** Scale type.
   * This array defines scale to be applied to parameters before
   * being evaluated in the cost function. This allows to
   * map to a more convenient space. In particular this is
   * used to normalize parameter spaces in which some parameters
   * have a different dynamic range.   */
  typedef Array< double > ScalesType;

  itkStaticConstMacro(ImageDimension, unsigned int,
                      TFixedImage::ImageDimension);

  typedef itk::ImageRegion< itkGetStaticConstMacro(ImageDimension) >
                                                              ImageRegionType;
  typedef itk::Size< itkGetStaticConstMacro( ImageDimension ) >   SizeType;

  /** Standard coordinate point type for this class   */
  typedef typename TFixedImage::PointType                         PointType;

  /** Index and Point typedef support. */
  typedef itk::Index< itkGetStaticConstMacro( ImageDimension ) >  IndexType;

  /** Image Types to use in the initialization of the transform */
  typedef   TFixedImage              FixedImageType;
  typedef   TMovingImage             MovingImageType;

  typedef   typename FixedImageType::ConstPointer   FixedImagePointer;
  typedef   typename MovingImageType::ConstPointer  MovingImagePointer;

  /** Set the fixed image used in the registration process */
  itkSetConstObjectMacro( FixedImage,  FixedImageType  );

  /** Set the moving image used in the registration process */
  itkSetConstObjectMacro( MovingImage, MovingImageType );

  /** Set the order of L-norm */
  itkSetMacro(LNorm, int);

  /** Set the global scaling factor */
  itkSetMacro(GlobalScalingFactor, double);

  /** Set the flag for forward direction:
   * m_TransformForward = true when the transform mapps from FixedImage
   * domain to MovingImage domain,
   * m_TransformForward = false when the transform mapps from MovingImage
   * domain to FixedImage domain.
   */
  itkSetMacro(TransformForward, bool);

  /** Compute parameter scales */
  void ComputeParameterScales(typename TransformType::Pointer transform,
                              ScalesType &parameterScales);

  /** Set the sample points for computing pixel shifts */
  void SampleImageDomain();

  /** Get the physical coordinates of image corners */
  template <class ImageType>
  void GetImageCornerPoints(const ImageType *image,
                            std::vector<PointType> &samples);

  /** Compute the maximum shift when deltaParameters is applied onto the
   * current transform. */
  void ComputeMaximumShift(typename TransformType::Pointer transform,
                           ParametersType deltaParameters, double &maxShift);

  /** Compute the L-norm of a point */
  double ComputeLNorm(Point<double, ImageDimension> point);

protected:
  ParameterScaleEstimator();
  ~ParameterScaleEstimator(){};

  void PrintSelf(std::ostream &os, Indent indent) const;

private:
  ParameterScaleEstimator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  FixedImagePointer   m_FixedImage;

  MovingImagePointer  m_MovingImage;

  std::vector<PointType> m_FixedImageSamples;

  std::vector<PointType> m_MovingImageSamples;

  /** Specify how to calculate the distance between two points */
  int m_LNorm;

  /** Change all scales by this denominator */
  double m_GlobalScalingFactor;

  /** Specify the transformation direction. Set to true when the transform
   * mapps from FixedImage domain to MovingImage domain*/
  bool m_TransformForward;
}; //class ParameterScaleEstimator


}  // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkParameterScaleEstimator.txx"
#endif

#endif /* __itkParameterScaleEstimator_h */
