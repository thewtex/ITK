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
#ifndef __itkAzimuthElevationToCartesianTransform_h
#define __itkAzimuthElevationToCartesianTransform_h

#include "itkAffineTransform.h"
#include "vnl/vnl_math.h"

namespace itk
{
/** \class AzimuthElevationToCartesianTransform
 * \brief Transforms from an azimuth, elevation, radius coordinate system to
 * a Cartesian coordinate system, or vice versa.
 *
 * The three coordinate axis are azimuth, elevation, and range.
 *
 * The azimuth elevation coordinate system is defined similarly to spherical
 * coordinates but is slightly different in that the azimuth and elevation are
 * measured in degrees between the r-axis (i.e z axis) and the projection on
 * the x-z and y-z planes, respectively.  Range, or r, is the distance from
 * the origin.
 *
 * The equations form performing the conversion from azimuth-elevation
 * coordinates to cartesian coordinates are as follows:
 * z = vcl_sqrt((r^2*(cos(azimuth))^2)/(1 + (cos(azimuth))^2 *
 *     (tan(elevation))^2);
 * x = z * vcl_tan(azimuth)
 * y = z * vcl_tan(elevation)
 *
 * The reversed transforms are:
 * azimuth = arctan(x/y)
 * elevation = arctan(y/z)
 * r = vcl_sqrt(x^2 + y^2 + z^2)
 *
 * In this class, we can also set what a "forward" transform means.  If we call
 * SetForwardAzimuthElevationToCartesian(), a forward transform will return
 * cartesian coordinates when passed azimuth,elevation,r coordinates.  Calling
 * SetForwardCartesianToAzimuthElevation() will cause the forward transform
 * to return azimuth,elevation,r coordinates from cartesian coordinates.
 *
 * Setting the FirstSampleDistance to a non-zero value means that a r value
 * of 12 is actually (12 + FirstSampleDistance) distance from the origin.
 *
 * There are two template parameters for this class:
 *
 * ScalarT       The type to be used for scalar numeric values.  Either
 *               float or double.
 *
 * NDimensions   The number of dimensions of the vector space (must be >=3).
 *
 * \ingroup Transforms
 *
 * \todo Is there any real value in allowing the user to template
 * over the scalar type?  Perhaps it should always be double, unless
 * there's a compatibility problem with the Point class.
 *
 * \todo  Derive this class from a yet undefined TransformBase class.
 *        Currently, this class derives from AffineTransform, although
 *        it is not an affine transform.
 * \ingroup ITK-Transform
 * \wikiexample{Utilities/AzimuthElevationToCartesianTransform,Cartesian to AzimuthElevation and vice-versa}
 */
template< class TScalarType = float,  // Data type for scalars
          unsigned int NDimensions = 3 >
// (e.g. float or double)
class ITK_EXPORT AzimuthElevationToCartesianTransform:
  public AffineTransform< TScalarType, NDimensions >
{
public:
  /** Standard class typedefs.   */
  typedef AzimuthElevationToCartesianTransform         Self;
  typedef AffineTransform<  TScalarType, NDimensions > Superclass;
  typedef SmartPointer< Self >                         Pointer;
  typedef SmartPointer< const Self >                   ConstPointer;

  /** Dimension of the domain space. */
  itkStaticConstMacro(SpaceDimension, unsigned int, NDimensions);
  itkStaticConstMacro( ParametersDimension, unsigned int,
                       NDimensions * ( NDimensions + 1 ) );

  /** Run-time type information (and related methods).   */
  itkTypeMacro(AzimuthElevationToCartesianTransform, AffineTransform);

  /** New macro for creation of through a Smart Pointer.   */
  itkNewMacro(Self);

  /** Parameters type.   */
  typedef typename Superclass::ParametersType ParametersType;

  /** Jacobian type.   */
  typedef typename Superclass::JacobianType JacobianType;

  /** Standard scalar type for this class. */
  typedef typename Superclass::ScalarType ScalarType;

  /** Standard coordinate point type for this class */
  typedef  typename Superclass::InputPointType  InputPointType;
  typedef  typename Superclass::OutputPointType OutputPointType;

  /** Standard matrix type for this class.   */
  typedef Matrix< TScalarType, itkGetStaticConstMacro(SpaceDimension),
                  itkGetStaticConstMacro(SpaceDimension) > MatrixType;

  /** Set the transformation parameters. */
  void SetAzimuthElevationToCartesianParameters(
    const double sampleSize,
    const double blanking,
    const long maxAzimuth,
    const long maxElevation,
    const double azimuthAngleSeparation,
    const double elevationAngleSeparation);

  void SetAzimuthElevationToCartesianParameters(const double sampleSize,
                                                const double blanking,
                                                const long maxAzimuth,
                                                const long maxElevation);

  /** Transform from azimuth-elevation to cartesian. */
  OutputPointType     TransformPoint(const InputPointType  & point) const;

  /** Back transform from cartesian to azimuth-elevation.  */
  inline InputPointType  BackTransform(const OutputPointType  & point) const
  {
    InputPointType result;

    if ( m_ForwardAzimuthElevationToPhysical )
      {
      result = static_cast< InputPointType >( TransformCartesianToAzEl(point) );
      }
    else
      {
      result = static_cast< InputPointType >( TransformAzElToCartesian(point) );
      }
    return result;
  }

  inline InputPointType  BackTransformPoint(const OutputPointType  & point) const
  {
    return BackTransform(point);
  }

  /** Defines that the forward transform goes from azimuth,elevation to
   *  cartesian. */
  void SetForwardAzimuthElevationToCartesian();

  /** Defines that the forward transform goes from cartesian to azimuth,
   *  elevation.  */
  void SetForwardCartesianToAzimuthElevation();

  /** Perform conversion from Azimuth Elevation coordinates to Cartesian
   *  Coordinates. */
  OutputPointType TransformAzElToCartesian(const InputPointType & point) const;

  /** Perform conversion from Cartesian Coordinates to Azimuth Elevation
   *  coordinates.  */
  OutputPointType TransformCartesianToAzEl(const OutputPointType & point) const;

  /**  Set the maximum azimuth.
   *  The maximum azimuth and elevation can be set so that the resulting
   *  cartesian space is symmetric about the z axis.  Therefore, the line
   *  defined by azimuth/2,elevation/2 = z-axis. */
  itkSetMacro(MaxAzimuth, long);

  /**  Set the maximum elevation
   *  The maximum azimuth and elevation can be set so that the resulting
   *  cartesian space is symmetric about the z axis.  Therefore, the line
   *  defined by azimuth/2,elevation/2 = z-axis. */
  itkSetMacro(MaxElevation, long);

  /**  Set the number of cartesian units between each unit along the R . */
  itkSetMacro(RadiusSampleSize, double);

  /**  Set the number of degrees between each azimuth unit. */
  itkSetMacro(AzimuthAngularSeparation, double);

  /**  Set the number of degrees between each elevation unit. */
  itkSetMacro(ElevationAngularSeparation, double);

  /**  Set the distance to add to the radius. */
  itkSetMacro(FirstSampleDistance, double);
protected:
  /** Create an AzimuthElevationToCartesianTransform object. */
  AzimuthElevationToCartesianTransform();

  /** Destroy an AzimuthElevationToCartesianTransform object. */
  virtual ~AzimuthElevationToCartesianTransform();

  /** Print contents of an AzimuthElevationTransform. */
  void PrintSelf(std::ostream & s, Indent indent) const;

private:
  AzimuthElevationToCartesianTransform(const Self &); // purposely not
                                                      // implemented
  void operator=(const Self &);                       //purposely not

  // implemented

  long   m_MaxAzimuth;
  long   m_MaxElevation;
  double m_RadiusSampleSize;
  double m_AzimuthAngularSeparation;
  double m_ElevationAngularSeparation;
  double m_FirstSampleDistance;
  bool   m_ForwardAzimuthElevationToPhysical;
}; //class AzimuthElevationToCartesianTransform
}  // namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_AzimuthElevationToCartesianTransform(_, EXPORT, TypeX, TypeY)     \
  namespace itk                                                                        \
  {                                                                                    \
  _( 2 ( class EXPORT AzimuthElevationToCartesianTransform< ITK_TEMPLATE_2 TypeX > ) ) \
  namespace Templates                                                                  \
  {                                                                                    \
  typedef AzimuthElevationToCartesianTransform< ITK_TEMPLATE_2 TypeX >                 \
  AzimuthElevationToCartesianTransform##TypeY;                                       \
  }                                                                                    \
  }

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkAzimuthElevationToCartesianTransform+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkAzimuthElevationToCartesianTransform.txx"
#endif

#endif /* __itkAzimuthElevationToCartesianTransform_h */
