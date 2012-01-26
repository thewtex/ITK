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
#ifndef __itkVectorImageToImageMetricTraitsv4_h
#define __itkVectorImageToImageMetricTraitsv4_h

#include "itkImage.h"
#include "itkCovariantVector.h"
#include "itkVariableLengthVector.h"


namespace itk
{
/** \class VectorImageToImageMetricTraitsv4
 * VectorImageToImageMetricTraitsv4 is a simple structure that holds type information
 * for a mesh and its cells.  It is used to avoid the passing of many
 * template parameters while still enjoying the benefits of generic
 * programming.
 *
 * Template parameters for VectorImageToImageMetricTraitsv4:
 *
 * TPixelType =
 *    The type stored as data for an entity (cell, point, or boundary).
 *
 * VPointDimension =
 *    Geometric dimension of space.
 *
 * VMaxTopologicalDimension =
 *    Max topological dimension of a cell that can be inserted into this mesh.
 *
 * TCoordRep =
 *    Numerical type with which to represent each coordinate value.
 *
 * TInterpolationWeight =
 *    Numerical type to store interpolation weights.
 *
 * \ingroup MeshObjects
 * \ingroup ITKCommon
 */
template<
  class    TFixedImageType,
  class    TMovingImageType,
  class    TVirtualImageType,
  unsigned int NumberOfComponents,
  typename TCoordRep = double
  >
class VectorImageToImageMetricTraitsv4
{
public:
  /** Standard class typedefs. */
  typedef VectorImageToImageMetricTraitsv4 Self;

  typedef TFixedImageType   FixedImageType;
  typedef TMovingImageType  MovingImageType;
  typedef TVirtualImageType VirtualImageType;

  typedef typename FixedImageType::PixelType  FixedImagePixelType;
  typedef typename MovingImageType::PixelType MovingImagePixelType;

  typedef TCoordRep            CoordinateRepresentationType;

  /* Image dimension accessors */
  typedef unsigned int   ImageDimensionType;
  itkStaticConstMacro(FixedImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<FixedImageType>::ImageDimension);
  itkStaticConstMacro(MovingImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<MovingImageType>::ImageDimension);
  itkStaticConstMacro(VirtualImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<VirtualImageType>::ImageDimension);

  typedef Vector< CoordinateRepresentationType, FixedImageDimension*NumberOfComponents >   FixedImageGradientType;
  typedef Vector< CoordinateRepresentationType, MovingImageDimension*NumberOfComponents >  MovingImageGradientType;
  typedef Vector< CoordinateRepresentationType, VirtualImageDimension*NumberOfComponents > VirtualImageGradientType;

  typedef DefaultConvertPixelTraits< FixedImageGradientType >  FixedImageGradientConvertType;
  typedef DefaultConvertPixelTraits< MovingImageGradientType > MovingImageGradientConvertType;

  /** Type of the filter used to calculate the gradients. */
  typedef typename NumericTraits< FixedImagePixelType >::RealType    FixedRealType;
  typedef typename NumericTraits< MovingImagePixelType >::RealType   MovingRealType;

  typedef FixedImageGradientType                               FixedGradientPixelType;
  typedef MovingImageGradientType                              MovingGradientPixelType;

  typedef Image< FixedGradientPixelType,
                 itkGetStaticConstMacro(FixedImageDimension) >
                                                FixedImageGradientImageType;

  typedef ImageToImageFilter< FixedImageType, FixedImageGradientImageType >
                                                 FixedImageGradientFilterType;

  typedef Image< MovingGradientPixelType,
                 itkGetStaticConstMacro(MovingImageDimension) >
                                                    MovingImageGradientImageType;

  typedef ImageToImageFilter< MovingImageType, MovingImageGradientImageType >
                                                 MovingImageGradientFilterType;

  typedef CovariantVector<CoordinateRepresentationType, FixedImageDimension>    FixedImageComponentGradientType;
  typedef CovariantVector<CoordinateRepresentationType, MovingImageDimension>   MovingImageComponentGradientType;
  typedef CovariantVector<CoordinateRepresentationType, VirtualImageDimension>  VirtualImageComponentGradientType;

  /** Default image gradient filter types */
  typedef GradientRecursiveGaussianImageFilter< FixedImageType,
                                                FixedImageGradientImageType >
                                                  DefaultFixedImageGradientFilter;
  typedef GradientRecursiveGaussianImageFilter< MovingImageType,
                                                MovingImageGradientImageType >
                                                  DefaultMovingImageGradientFilter;

  /** Image gradient calculator types. The TOutput template parameter
   * is chosen to match that of CentralDiffererenceImageFunction. */
  typedef ImageFunction<FixedImageType,
                        FixedImageGradientType,
                        CoordinateRepresentationType>
                                            FixedImageGradientCalculatorType;
  typedef ImageFunction<MovingImageType,
                        MovingImageGradientType,
                        CoordinateRepresentationType>
                                            MovingImageGradientCalculatorType;

  typedef CentralDifferenceImageFunction<FixedImageType,
                                         CoordinateRepresentationType,
                                         FixedImageGradientType>
                                          DefaultFixedImageGradientCalculator;
  typedef CentralDifferenceImageFunction<MovingImageType,
                                         CoordinateRepresentationType,
                                         MovingImageGradientType>
                                          DefaultMovingImageGradientCalculator;

/*
  virtual void InitializeDefaultFixedImageGradientFilter( DefaultFixedImageGradientFilter * filter,
                                                  const FixedImageType * image ) const;

  virtual void InitializeDefaultMovingImageGradientFilter( DefaultMovingImageGradientFilter * filter,
                                                   const MovingImageType * image ) const;
*/
};
} // end namespace itk

#endif
