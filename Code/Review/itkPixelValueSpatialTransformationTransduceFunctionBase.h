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
#ifndef __itkPixelValueSpatialTransformationTransduceFunctionBase_h
#define __itkPixelValueSpatialTransformationTransduceFunctionBase_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkTransform.h"

namespace itk
{
/**
 * \class PixelValueSpatialTransformationTransduceFunctionBase
 * \brief Base class for pixel-value transducer function objects.
 *
 * This class is the base class for pixel-value transducer function objects,
 * functioning as an identity transducer.
 *
 * Specifically, the ()-operator is overloaded to modify a pixel value at a
 * given point in the input space. The base class implementation simply returns
 * the input value, acting as an identity transducer.
 *
 * Subclasses must override ()-operator to perform any meaningful operation.
 * Note, however, that because this is not an abstract class, subclasses
 * must not be used using a pointer to this base class.
 *
 * This class is intended for use within ResampleImageFilter, providing a
 * method for modifying the filter's output on a per-pixel basis.
 *
 * The user can set a transform via SetTransform, to be used in the ()-operator as
 * needed. By default, the transform is set to NULL. After setting the transform
 * and before use, Prepare must be called to perform any required
 * initialization. When used in ResampleImageFilter, the transform is set
 * automatically, after which Prepare is called.
 *
 * Template parameter TInputOutput refers to the internal pixel value used by
 * the function, and is expected to match ResampleImageFilter::Interpolator::
 * OutputType in order to preserve precision for the case when Interpolator::
 * OutputType is of greater precision than the ResampleImageFilter::PixelType.
 * Parameter TPoint refers to the input space, and NInputDimensions and
 * NOutputDimensions, along with TPoint, define the transform parameters.
 *
 * Thread-safety
 * This class and derived classes aren't expected to be thread-safe. When used
 * in itkResampleImageFilter with multi-threading enabled, a copy of the object
 * is made for each thread, similarly to as is done for transforms.
 *
 * QUESTIONS
 *  If we template over TPoint, NInputDim and NOutputDim, we might instead
 *  template over transform type? Or did you imagine uses that didn't require
 *  a txf?
 *
 * \ingroup Functions
 * \sa ResampleImageFilter
 *
 */
template< class TInputOutput,
          class TPoint,
          unsigned int NInputDimensions,
          unsigned int NOutputDimensions >
class ITK_EXPORT PixelValueSpatialTransformationTransduceFunctionBase:public Object
{
public:
  /** Standard class typedefs. */
  typedef PixelValueSpatialTransformationTransduceFunctionBase     Self;

  typedef Object                     Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(PixelValueSpatialTransformationTransduceFunctionBase, Object);

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro(Self);

  /** Dimension of the domain space. */
  itkStaticConstMacro(InputSpaceDimension, unsigned int, NInputDimensions);
  itkStaticConstMacro(OutputSpaceDimension, unsigned int, NOutputDimensions);

  /** Input type */
  typedef TInputOutput InputType;

  /** Output type */
  typedef TInputOutput OutputType;

  /** Point type */
  typedef TPoint  PointType;

  /** Transform type */
  typedef Transform<typename TPoint::CoordRepType, NInputDimensions, NOutputDimensions> TransformType;

  /** Prepare the transducer for use. This should be called before
   * using a transducer to allow it to initialization as needed. */
   virtual void Prepare( void ) {};

  /** Function operator - evaluate at the specified input position.
   * This base class is simply an identity operator.
   * This is not virtual in order to allow compiler optimizations.
   * Thus, when using a derived class, always use a ptr to the dervied
   * class, rather than to this base class. */
  inline
  const OutputType& operator()
    (const PointType & point, const InputType & input) const
    {
    return input;
    }

  /** Set/Get the Transform */
  itkSetConstObjectMacro(Transform, TransformType);
  itkGetConstObjectMacro(Transform, TransformType);

protected:
  PixelValueSpatialTransformationTransduceFunctionBase(){m_Transform = NULL;}
  virtual ~PixelValueSpatialTransformationTransduceFunctionBase(){};

  virtual
  void PrintSelf(std::ostream & os, Indent indent) const
    {
    Superclass::PrintSelf(os, indent);
    }

  typename TransformType::ConstPointer  m_Transform;

private:
  //purposely not implemented
  PixelValueSpatialTransformationTransduceFunctionBase(const Self &);
  void operator=(const Self &); //purposely not implemented
};
} // end namespace itk

#endif
