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
#ifndef __itkBSplineDeformableTransformInitializerv4_h
#define __itkBSplineDeformableTransformInitializerv4_h

#include "itkObject.h"
#include "itkObjectFactory.h"

#include "itkFixedArray.h"

#include <iostream>

namespace itk
{
/** \class BSplineDeformableTransformInitializerv4
 * \brief BSplineDeformableTransformInitializerv4 is a helper class intended to
 * initialize the grid parameters of a BSplineDeformableTransform based on the
 * parameters of an image.
 *
 * \ingroup Transforms
 */
template<class TTransform, class TImage>
class ITK_EXPORT BSplineDeformableTransformInitializerv4
  :public Object
{

public:

  /** Standard class typedefs. */
  typedef BSplineDeformableTransformInitializerv4 Self;
  typedef Object                                  Superclass;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;

  /** New macro for creation of through a Smart Pointer. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( BSplineDeformableTransformInitializerv4, Object );

  /** Type of the transform to initialize */
  typedef TTransform TransformType;

  /** Types defined from the input image traits */
  typedef TImage                                   ImageType;
  typedef typename ImageType::ConstPointer         ImagePointer;
  typedef typename ImageType::IndexType            IndexType;
  typedef typename ImageType::PointType            ImagePointType;
  typedef typename ImagePointType::CoordRepType    ImagePointCoordRepType;

  /** Types defined from transform traits */
  typedef typename TransformType::Pointer                 TransformPointer;
  typedef typename TransformType::RegionType              RegionType;
  typedef typename RegionType::SizeType                   SizeType;
  typedef typename TransformType::SpacingType             SpacingType;
  typedef typename TransformType::OriginType              OriginType;
  typedef typename TransformType::DirectionType           DirectionType;
  typedef typename TransformType::PhysicalDimensionsType  PhysicalDimensionsType;
  typedef typename SpacingType::ComponentType             SpacingComponentType;

  /** Dimension of parameters. */
  itkStaticConstMacro( SpaceDimension, unsigned int,
    TransformType::SpaceDimension );

  /** Set the transform to be initialized */
  itkSetObjectMacro( Transform, TransformType );

  /** Set the image to initialize the domain */
  itkSetConstObjectMacro( Image, ImageType );

  /** Initialize the transform using the specified transformation domain */
  virtual void InitializeTransform() const;

protected:
  BSplineDeformableTransformInitializerv4();
  ~BSplineDeformableTransformInitializerv4() {}

  void PrintSelf( std::ostream & os, Indent indent ) const;

private:
  BSplineDeformableTransformInitializerv4( const Self & ); //purposely not
                                                         // implemented
  void operator=( const Self & );                        //purposely not
                                                         // implemented

  ImagePointer                    m_Image;
  TransformPointer                m_Transform;

}; //class BSplineDeformableTransformInitializerv4
}  // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBSplineDeformableTransformInitializerv4.txx"
#endif

#endif /* __itkBSplineDeformableTransformInitializerv4_h */
