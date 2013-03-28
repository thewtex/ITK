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
#ifndef __itkMeanSquaresImageToImageMetricComputationTypeTemplatev4_h
#define __itkMeanSquaresImageToImageMetricComputationTypeTemplatev4_h

#include "itkImageToImageMetricComputationTypeTemplatev4.h"
#include "itkMeanSquaresImageToImageMetricv4GetValueAndDerivativeThreaderTemplate.h"
#include "itkDefaultImageToImageMetricTraitsv4.h"

namespace itk
{

/** \class MeanSquaresImageToImageMetricComputationTypeTemplatev4
 *
 *  \brief Class implementing a mean squares metric.
 *
 *  This class supports vector images of type VectorImage
 *  and Image< VectorType, imageDimension >.
 *
 *  See
 *  MeanSquaresImageToImageMetricv4GetValueAndDerivativeThreaderTemplate::ProcessPoint for algorithm implementation.
 *
 * \ingroup ITKMetricsv4
 */
template <class InternalComputationValueType, class TFixedImage, class TMovingImage, class TVirtualImage = TFixedImage,
          class TMetricTraits = DefaultImageToImageMetricTraitsv4<TFixedImage,TMovingImage,TVirtualImage, InternalComputationValueType>
         >
class ITK_EXPORT MeanSquaresImageToImageMetricComputationTypeTemplatev4 :
  public ImageToImageMetricComputationTypeTemplatev4<InternalComputationValueType, TFixedImage, TMovingImage, TVirtualImage, TMetricTraits>
{
public:
  /** Standard class typedefs. */
  typedef MeanSquaresImageToImageMetricComputationTypeTemplatev4                                      Self;

  typedef ImageToImageMetricComputationTypeTemplatev4
          <InternalComputationValueType, TFixedImage, TMovingImage, TVirtualImage, TMetricTraits>     Superclass;

  typedef SmartPointer<Self>                                                                          Pointer;
  typedef SmartPointer<const Self>                                                                    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MeanSquaresImageToImageMetricComputationTypeTemplatev4, ImageToImageMetricComputationTypeTemplatev4);

  /** Superclass types */
  typedef typename Superclass::MeasureType             MeasureType;
  typedef typename Superclass::DerivativeType          DerivativeType;

  typedef typename Superclass::FixedImagePointType     FixedImagePointType;
  typedef typename Superclass::FixedImagePixelType     FixedImagePixelType;
  typedef typename Superclass::FixedImageGradientType  FixedImageGradientType;

  typedef typename Superclass::MovingImagePointType    MovingImagePointType;
  typedef typename Superclass::MovingImagePixelType    MovingImagePixelType;
  typedef typename Superclass::MovingImageGradientType MovingImageGradientType;

  typedef typename Superclass::MovingTransformType        MovingTransformType;
  typedef typename Superclass::JacobianType               JacobianType;
  typedef typename Superclass::VirtualImageType           VirtualImageType;
  typedef typename Superclass::VirtualIndexType           VirtualIndexType;
  typedef typename Superclass::VirtualPointType           VirtualPointType;
  typedef typename Superclass::VirtualPointSetType        VirtualPointSetType;

  /* Image dimension accessors */
  itkStaticConstMacro(VirtualImageDimension, ImageDimensionType, TVirtualImage::ImageDimension);
  itkStaticConstMacro(FixedImageDimension, ImageDimensionType,  TFixedImage::ImageDimension);
  itkStaticConstMacro(MovingImageDimension, ImageDimensionType, TMovingImage::ImageDimension);

  protected:
  MeanSquaresImageToImageMetricComputationTypeTemplatev4();
  virtual ~MeanSquaresImageToImageMetricComputationTypeTemplatev4();

  friend class MeanSquaresImageToImageMetricv4GetValueAndDerivativeThreaderTemplate< ThreadedImageRegionPartitioner< Superclass::VirtualImageDimension >, Superclass, Self >;
  friend class MeanSquaresImageToImageMetricv4GetValueAndDerivativeThreaderTemplate< ThreadedIndexedContainerPartitioner, Superclass, Self >;
  typedef MeanSquaresImageToImageMetricv4GetValueAndDerivativeThreaderTemplate< ThreadedImageRegionPartitioner< Superclass::VirtualImageDimension >, Superclass, Self >
  MeanSquaresDenseGetValueAndDerivativeThreaderType;
  typedef MeanSquaresImageToImageMetricv4GetValueAndDerivativeThreaderTemplate< ThreadedIndexedContainerPartitioner, Superclass, Self >
  MeanSquaresSparseGetValueAndDerivativeThreaderType;

  void PrintSelf(std::ostream& os, Indent indent) const;

  private:
  MeanSquaresImageToImageMetricComputationTypeTemplatev4(const Self &); //purposely not implemented
  void operator = (const Self &); //purposely not implemented
  };

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMeanSquaresImageToImageMetricComputationTypeTemplatev4.hxx"
#endif

#endif
