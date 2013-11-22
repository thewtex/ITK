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
#ifndef __itkBSplineControlPointImageFunction_h
#define __itkBSplineControlPointImageFunction_h

#include "itkImageFunction.h"

#include "itkBSplineKernelFunction.h"
#include "itkCoxDeBoorBSplineKernelFunction.h"
#include "itkFixedArray.h"
#include "itkImage.h"
#include "itkPointSet.h"
#include "itkVariableSizeMatrix.h"
#include "itkVector.h"
#include "itkVectorContainer.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

namespace itk
{
/**
 * \class BSplineControlPointImageFunction
 *
 * \brief Evaluate a B-spline object given a grid of control points.
 *
 * \par  The output of the class itkBSplineScatteredDataPointSetToImageFilter
 * is a control point grid defining a B-spline object.  This class is used to
 * hold various routines meant to operate on that control point grid.  In
 * addition to specifying the control point grid as the input, the user
 * must also supply the spline order and the parametric domain (i.e. size,
 * domain, origin, spacing).
 *
 * Operations include
 *   1. Evaluation of the B-spline object at any point in the domain.
 *   2. Evaluation of the gradient of the B-spline object at any point in the
 *      domain.
 *   3. Evaluation of the Hessian of the B-spline object at any point in the
 *      domain.
 *   4. Inverse estimation.  Given a user-specified data point, one can
 *      find the parameters which minimize the "distance" between the evaluated
 *      data point and the B-spline object evaluated at those parameters.
 *      This is useful, for example, in determining the parametric values of
 *      a point on the curve closest to a user-specified point.
 *
 * \author Nicholas J. Tustison
 *
 * \ingroup ITKImageGrid
 */
template <typename TInputImage, typename TCoordRep = double>
class BSplineControlPointImageFunction
  : public ImageFunction<TInputImage, typename TInputImage::PixelType, TCoordRep>
{
public:
  typedef BSplineControlPointImageFunction Self;
  typedef ImageFunction<TInputImage,
                        typename TInputImage::PixelType, TCoordRep>         Superclass;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Extract dimension from input image. */
  itkStaticConstMacro( ImageDimension, unsigned int, TInputImage::ImageDimension );

  /** Image typedef support */
  typedef TInputImage                         ControlPointLatticeType;
  typedef TInputImage                         InputImageType;
  typedef TCoordRep                           CoordRepType;
  typedef typename InputImageType::PixelType  PixelType;
  typedef typename InputImageType::RegionType RegionType;
  typedef typename InputImageType::IndexType  IndexType;
  typedef typename InputImageType::PointType  PointType;
  typedef typename InputImageType::RegionType InputImageRegionType;

  typedef typename InputImageType::SpacingType SpacingType;
  typedef typename InputImageType::PointType   OriginType;
  typedef typename InputImageType::SizeType    SizeType;

  /** Output typedef support */
  typedef PixelType                        OutputType;
  typedef VariableSizeMatrix<CoordRepType> GradientType;
  typedef VariableSizeMatrix<CoordRepType> HessianComponentType;

  /** Other typedef */
  typedef FixedArray<unsigned, ImageDimension>     ArrayType;
  typedef Image<CoordRepType, ImageDimension>      RealImageType;
  typedef typename RealImageType::Pointer          RealImagePointer;
  typedef typename Superclass::ContinuousIndexType ContinuousIndexType;

  /** Interpolation kernel type (default spline order = 3) */
  typedef CoxDeBoorBSplineKernelFunction<3> KernelType;
  typedef BSplineKernelFunction<0>          KernelOrder0Type;
  typedef BSplineKernelFunction<1>          KernelOrder1Type;
  typedef BSplineKernelFunction<2>          KernelOrder2Type;
  typedef BSplineKernelFunction<3>          KernelOrder3Type;

  /**
   * Set the input image.  Note that the size, spacing, origin, and spline
   * order must be called prior to setting the input image.
   */
  virtual void SetInputImage( const InputImageType * );

  /**
   * Set the spline order of the B-spline object for all parametric dimensions.
   * Default = 3.
   */
  void SetSplineOrder( const unsigned int );

  /**
   * Set the spline order array where each element of the array corresponds to
   * a single parametric dimension of the B-spline object.  Default = 3.
   */
  void SetSplineOrder( const ArrayType & );

  /**
   * Get the spline order array of the B-spline object.  Default = 3.
   */
  itkGetConstReferenceMacro( SplineOrder, ArrayType );

  /**
   * Set the boolean array indicating the periodicity of the B-spline object.
   * This array of 0/1 values defines whether a particular dimension of the
   * parametric space is to be considered periodic or not. For example, if you
   * are using interpolating along a 1D closed curve, the array type will have
   * size 1, and you should set the first element of this array to the value
   * "1". In the case that you were interpolating in a planar surface with
   * cylindrical topology, the array type will have two components, and you
   * should set to "1" the component that goes around the cylinder, and set to
   * "0" the component that goes from the top of the cylinder to the bottom.
   * This will indicate the periodity of that parameter to the filter.
   * Internally, in order to make periodic the domain of the parameter, the
   * filter will reuse some of the points at the beginning of the domain as if
   * they were also located at the end of the domain. The number of points to
   * be reused will depend on the spline order. As a user, you don't need to
   * replicate the points, the filter will do this for you. */
  itkSetMacro( CloseDimension, ArrayType );

  /**
   * Get the boolean array indicating which dimensions are closed.
   */
  itkGetConstReferenceMacro( CloseDimension, ArrayType );

  /**
   * Set the parametric spacing of the B-spline object domain.
   */
  itkSetMacro( Spacing, SpacingType );

  /**
   * Get the parametric spacing of the B-spline object domain.
   */
  itkGetConstMacro( Spacing, SpacingType );

  /**
   * Set the parametric origin of the B-spline object domain.
   */
  itkSetMacro( Origin, OriginType );

  /**
   * Get the parametric origin of the B-spline object domain.
   */
  itkGetConstMacro( Origin, OriginType );

  /**
   * Set the parametric size of the B-spline object domain.
   */
  itkSetMacro( Size, SizeType );

  /**
   * Get the parametric size of the B-spline object domain.
   */
  itkGetConstMacro( Size, SizeType );

  /**
   * Evaluate the resulting B-spline object at a specified point in the
   * parametric domain.
   */
  OutputType EvaluateAtParametricPoint( const PointType & ) const;

  /**
   * Evaluate the resulting B-spline object at a specified index in the
   * parametric domain.
   */
  virtual OutputType EvaluateAtIndex( const IndexType & ) const;

  /**
   * Evaluate the resulting B-spline object at a specified continuous index in
   * the parametric domain.
   */
  virtual OutputType EvaluateAtContinuousIndex(
    const ContinuousIndexType & ) const;

  /**
   * Evaluate the resulting B-spline object at a specified internal parameteric
   * point.  Note that the internal parameterization over each dimension of the
   * B-spline object is [0, 1).
   */
  virtual OutputType Evaluate( const PointType & ) const;

  /**
   * Evaluate the gradient of the resulting B-spline object at a specified point
   * in the parametric domain.
   */
  GradientType EvaluateGradientAtParametricPoint( const PointType & ) const;

  /**
   * Evaluate the gradient of the resulting B-spline object at a specified index
   * in the parametric domain.
   */
  GradientType EvaluateGradientAtIndex( const IndexType & ) const;

  /**
   * Evaluate the gradient of the resulting B-spline object at a specified
   * continuous index in the parametric domain.
   */
  GradientType EvaluateGradientAtContinuousIndex(
    const ContinuousIndexType & ) const;

  /**
   * Evaluate the gradient of the resulting B-spline object at a specified
   * internal parameteric point.  Note that the internal parameterization over
   * each dimension of the B-spline object is [0, 1).
   */
  GradientType EvaluateGradient( const PointType & ) const;

  /**
   * Evaluate the Hessian of the resulting B-spline object at a specified
   * point within the parametric domain.  Since the Hessian for a vector
   * function is a 3-tensor, one must specify the component.
   */
  HessianComponentType EvaluateHessianAtParametricPoint(
    const PointType &, const unsigned int ) const;

  /**
   * Evaluate the Hessian of the resulting B-spline object at a specified
   * index within the parametric domain.  Since the Hessian for a vector
   * function is a 3-tensor, one must specify the component.
   */
  HessianComponentType EvaluateHessianAtIndex(
    const IndexType &, const unsigned int ) const;

  /**
   * Evaluate the Hessian of the resulting B-spline object at a specified con-
   * tinuous index within the parametric domain.  Since the Hessian for a vector
   * function is a 3-tensor, one must specify the component.
   */
  HessianComponentType EvaluateHessianAtContinuousIndex(
    const ContinuousIndexType &, const unsigned int ) const;

  /**
   * Evaluate the hessian of the resulting B-spline object at a specified
   * internal parameteric point.  Note that the internal parameterization over
   * each dimension of the B-spline object is [0, 1).
   */
  HessianComponentType EvaluateHessian(
    const PointType &, const unsigned int ) const;

protected:
  BSplineControlPointImageFunction();
  virtual
  ~BSplineControlPointImageFunction();
  void PrintSelf( std::ostream& os, Indent indent ) const;

private:
  BSplineControlPointImageFunction( const Self& ); //purposely not implemented
  void operator=( const Self& );                   //purposely not implemented

  /** Parameters for the B-spline object domain */
  SizeType    m_Size;
  SpacingType m_Spacing;
  OriginType  m_Origin;

  ArrayType m_NumberOfControlPoints;
  ArrayType m_CloseDimension;
  ArrayType m_SplineOrder;

  RealImagePointer m_NeighborhoodWeightImage;

  typename KernelType::Pointer                 m_Kernel[ImageDimension];
  typename KernelOrder0Type::Pointer           m_KernelOrder0;
  typename KernelOrder1Type::Pointer           m_KernelOrder1;
  typename KernelOrder2Type::Pointer           m_KernelOrder2;
  typename KernelOrder3Type::Pointer           m_KernelOrder3;

  CoordRepType m_BSplineEpsilon;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBSplineControlPointImageFunction.hxx"
#endif

#endif
