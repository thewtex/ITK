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
#ifndef __itkDefaultImageToImageMetricTraitsv4_h
#define __itkDefaultImageToImageMetricTraitsv4_h

#include "itkImage.h"
#include "itkCovariantVector.h"
#include "itkCentralDifferenceImageFunction.h"
#include "itkGradientRecursiveGaussianImageFilter.h"

namespace itk
{
/** \class DefaultImageToImageMetricTraitsv4
 * DefaultImageToImageMetricTraitsv4 is a simple structure that holds type information
 * for a mesh and its cells.  It is used to avoid the passing of many
 * template parameters while still enjoying the benefits of generic
 * programming.
 *
 * Template parameters for DefaultImageToImageMetricTraitsv4:
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
  typename TCoordRep = double
  >
class DefaultImageToImageMetricTraitsv4
{
public:
  /** Standard class typedefs. */
  typedef DefaultImageToImageMetricTraitsv4 Self;

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

  typedef   CovariantVector< CoordinateRepresentationType,
                             itkGetStaticConstMacro(FixedImageDimension) >
                                                      FixedImageGradientType;
  typedef   CovariantVector< CoordinateRepresentationType,
                             itkGetStaticConstMacro(MovingImageDimension) >
                                                      MovingImageGradientType;

  typedef   CovariantVector< CoordinateRepresentationType,
                             itkGetStaticConstMacro(VirtualImageDimension) >
                                                      VirtualImageGradientType;

  typedef FixedImageGradientType   FixedImageComponentGradientType;
  typedef MovingImageGradientType  MovingImageComponentGradientType;
  typedef VirtualImageGradientType VirtualImageComponentGradientType;

  typedef DefaultConvertPixelTraits< FixedImageGradientType >  FixedImageGradientConvertType;
  typedef DefaultConvertPixelTraits< MovingImageGradientType > MovingImageGradientConvertType;

  /** Type of the filter used to calculate the gradients. */
  typedef typename NumericTraits< FixedImagePixelType >::RealType
                                                    FixedRealType;
  typedef CovariantVector< FixedRealType,
                           itkGetStaticConstMacro(FixedImageDimension) >
                                                    FixedGradientPixelType;
  typedef Image< FixedGradientPixelType,
                 itkGetStaticConstMacro(FixedImageDimension) >
                                                FixedImageGradientImageType;

  typedef ImageToImageFilter< FixedImageType, FixedImageGradientImageType >
                                                 FixedImageGradientFilterType;

  typedef typename NumericTraits< MovingImagePixelType >::RealType
                                                 MovingRealType;
  typedef CovariantVector< MovingRealType,
                           itkGetStaticConstMacro(MovingImageDimension) >
                                                 MovingGradientPixelType;
  typedef Image< MovingGradientPixelType,
                 itkGetStaticConstMacro(MovingImageDimension) >
                                                    MovingImageGradientImageType;

  typedef ImageToImageFilter< MovingImageType, MovingImageGradientImageType >
                                                 MovingImageGradientFilterType;

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
                        CovariantVector<double,
                                  itkGetStaticConstMacro( FixedImageDimension )>,
                        CoordinateRepresentationType>
                                            FixedImageGradientCalculatorType;
  typedef ImageFunction<MovingImageType,
                        CovariantVector<double,
                                  itkGetStaticConstMacro( MovingImageDimension )>,
                        CoordinateRepresentationType>
                                            MovingImageGradientCalculatorType;

  typedef CentralDifferenceImageFunction<FixedImageType,
                                         CoordinateRepresentationType>
                                          DefaultFixedImageGradientCalculator;
  typedef CentralDifferenceImageFunction<MovingImageType,
                                         CoordinateRepresentationType>
                                          DefaultMovingImageGradientCalculator;

  //virtual void InitializeDefaultFixedImageGradientFilter( DefaultFixedImageGradientFilter * filter,
  //                                                const FixedImageType * image ) const;

  //virtual void InitializeDefaultMovingImageGradientFilter( DefaultMovingImageGradientFilter * filter,
  //                                                 const MovingImageType * image ) const;

};
} // end namespace itk

//#ifndef ITK_MANUAL_INSTANTIATION
//#include "itkDefaultImageToImageMetricTraitsv4.hxx"
//#endif

#endif
