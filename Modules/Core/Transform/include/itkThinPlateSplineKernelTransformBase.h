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
#ifndef __itkThinPlateSplineKernelTransformBase_h
#define __itkThinPlateSplineKernelTransformBase_h

#include "itkKernelTransform.h"
#include "itkThinPlateSplineKernelTransformBase.h"
#include "itkThinPlateSplineRadialBasisFunctionBase.h"

namespace itk
{
/** \class ThinPlateSplineKernelTransformBase
 *
 * This class defines a base class for the thin plate spline (TPS) elastic transformation.
 * It is implemented in as straightforward a manner as possible from
 * the IEEE TMI paper by Davis, Khotanzad, Flamig, and Harms,
 * Vol. 16 No. 3 June 1997.  Thin-plate splines define a warping from one set of points
 * to another by minimizing the second order bending-energy functional (i.e. m=2 for this
 * implementation).  Using this functional, the smoothness of the warping is guaranteed
 * with the appropriate kernel function.  The resulting warp will be that of a second
 * order polynomial.  This class allows any radial basis function to be used for the
 * kernel, but defaults to the appropriate TPS kernel for 2D and higher interpolation.
 * If the kernels are set to anything other than the defaults, the resulting warp will no
 * longer be a thin-plate spline.  In an N-dimensional image, more than (N+m-1)!/(N!(m-1!)
 * landmarks must be present.
 *
 * The default behavior in 2D defaults to a r^2*ln(r) kernel, and for 3D and greater
 * it defaults to the r kernel (where r is ||(source point)-(target point)||).  It
 * also defaults to interpolating TPS with a stiffness of 0 (i.e. the source and target
 * landmarks will always be perfectly aligned.  This implies that their exact positions
 * are known.).  In order to compensate for fiducial localization error in the source
 * and target positions, approximating TPS should be used.  The stiffness must be set to
 * some non-normalized value greater than 0 (see the Rhor et al. paper for an explanation
 * of stiffness) and the scalar on the || r || kernel must be set to -1.  The higher the
 * stiffness value, the more closely the warping resembles a pure affine transformation.
 *
 * \ingroup ITK-Transform
 * \ingroup Transforms
 */
template< class TScalarType,         // Data type for scalars (float or double)
          unsigned int NDimensions > //Number of dimensions in the image
class ITK_EXPORT ThinPlateSplineKernelTransformBase:
  public KernelTransform< TScalarType, NDimensions >
{
public:
  /** Standard class typedefs. */
  typedef ThinPlateSplineKernelTransformBase             Self;
  typedef KernelTransform<    TScalarType, NDimensions > Superclass;
  typedef SmartPointer< Self >                           Pointer;
  typedef SmartPointer< const Self >                     ConstPointer;

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThinPlateSplineKernelTransformBase, KernelTransform);

  /** Scalar type. */
  typedef typename Superclass::ScalarType ScalarType;

  /** Parameters type. */
  typedef typename Superclass::ParametersType ParametersType;

  /** Jacobian Type */
  typedef typename Superclass::JacobianType JacobianType;

  /** Dimension of the domain space. */
  itkStaticConstMacro(SpaceDimension, unsigned int, Superclass::SpaceDimension);

  /** These (rather redundant) typedefs are needed because on SGI, typedefs
   * are not inherited */
  typedef typename Superclass::InputPointType                        InputPointType;
  typedef typename Superclass::OutputPointType                       OutputPointType;
  typedef typename Superclass::InputVectorType                       InputVectorType;
  typedef typename Superclass::OutputVectorType                      OutputVectorType;
  typedef typename Superclass::InputCovariantVectorType              InputCovariantVectorType;
  typedef typename Superclass::OutputCovariantVectorType             OutputCovariantVectorType;
  typedef typename Superclass::PointsIterator                        PointsIterator;
  typedef typename Superclass::PointsConstIterator                   PointsConstIterator;

  typedef ThinPlateSplineRadialBasisFunctionBase< TScalarType >      FunctionType;
  typedef typename FunctionType::Pointer                             FunctionPointerType;

  /**Sets the kernel function.  For 2D, the default is r^2log(r), where r is the
   * Euclidean norm.  For 3D and higher the default is r. Function objects that inherit from
   * RadialBasisFunctionBase can be set in the ThinPlateSplineKernelTransformBase
   * class and derived classes. Bear in mind that using thin-plate splines  requires a
   * specific set of polyharmonic kernel functions-- |r^3| for 1D, r^2log(r)
   * for 2D, and |r| for 3D.*/
  itkSetObjectMacro(Function, FunctionType);

  /**Get a pointer to the current kernel function object. */
  itkGetObjectMacro(Function, FunctionType);

  /**Get a const pointer to the current kernel function object. */
  itkGetConstObjectMacro(Function, FunctionType);

protected:
  ThinPlateSplineKernelTransformBase();
  virtual ~ThinPlateSplineKernelTransformBase() {}

  typedef typename Superclass::GMatrixType GMatrixType;

  /** Compute G(x)
    * For the thin plate spline, this is:
    * G(x) = r(x)*I
    * \f$ G(x) = r(x)*I \f$
    * where
    * r(x) = Euclidean norm = sqrt[x1^2 + x2^2 + x3^2]
    * \f[ r(x) = \sqrt{ x_1^2 + x_2^2 + x_3^2 }  \f]
    * I = identity matrix. */
  virtual void ComputeG(const InputVectorType & landmarkVector, GMatrixType & gmatrix) const;

  /** Compute the contribution of the difference between each set of homologous points
      to the global deformation of the space  */
  virtual void ComputeDeformationContribution(const InputPointType & inputPoint,
                                              OutputPointType & result) const;

private:
  ThinPlateSplineKernelTransformBase(const Self &); //purposely not implemented
  void operator=(const Self &);                     //purposely not implemented

  FunctionPointerType             m_Function;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkThinPlateSplineKernelTransformBase.txx"
#endif

#endif
