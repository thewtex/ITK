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
#ifndef __itkDemonsImageToImageObjectMetric_h
#define __itkDemonsImageToImageObjectMetric_h

#include "itkImageToImageObjectMetric.h"

namespace itk
{

/** \class DemonsImageToImageObjectMetric
 *
 *  \brief Class implementing rudimentary demons metric.
 *
 *  See \c ProcessPoint for algorithm implementation.
 *
 * \ingroup ITKHighDimensionalMetrics
 */
template <class TFixedImage,
          class TMovingImage,
          class TVirtualImage = TFixedImage >
class ITK_EXPORT DemonsImageToImageObjectMetric :
public ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
{
public:

  /** Standard class typedefs. */
  typedef DemonsImageToImageObjectMetric                      Self;
  typedef ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
                                                              Superclass;
  typedef SmartPointer<Self>                                  Pointer;
  typedef SmartPointer<const Self>                            ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DemonsImageToImageObjectMetric, ImageToImageObjectMetric);

  /** Superclass types */
  typedef typename Superclass::MeasureType             MeasureType;
  typedef typename Superclass::DerivativeType          DerivativeType;

  typedef typename Superclass::FixedImagePointType     FixedImagePointType;
  typedef typename Superclass::FixedImagePixelType     FixedImagePixelType;
  typedef typename Superclass::FixedImageGradientType  FixedImageGradientType;

  typedef typename Superclass::MovingImagePointType    MovingImagePointType;
  typedef typename Superclass::MovingImagePixelType    MovingImagePixelType;
  typedef typename Superclass::MovingImageGradientType MovingImageGradientType;

  typedef typename Superclass::MovingTransformType     MovingTransformType;
  typedef typename Superclass::JacobianType            JacobianType;
  typedef typename Superclass::VirtualImageType        VirtualImageType;
  typedef typename Superclass::VirtualIndexType        VirtualIndexType;
  typedef typename Superclass::VirtualPointType        VirtualPointType;
  typedef typename Superclass::VirtualSampledPointSetType
                                                       VirtualSampledPointSetType;

  /* Image dimension accessors */
  itkStaticConstMacro(VirtualImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<TVirtualImage>::ImageDimension);
  itkStaticConstMacro(FixedImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<TFixedImage>::ImageDimension);
  itkStaticConstMacro(MovingImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<TMovingImage>::ImageDimension);

  /** Evaluate and return the metric value.
   * \warning Not yet implemented. */
  virtual MeasureType GetValue() const;

protected:
  DemonsImageToImageObjectMetric();
  virtual ~DemonsImageToImageObjectMetric();

  /** \class DemonsGetValueAndDerivativeThreader
   * \brief Processes points for Demons calculation.
   * \ingroup ITKHighDimensionalMetrics
   * */
  template < class TDomainPartitioner >
  class DemonsGetValueAndDerivativeThreader
    : public ImageToImageObjectMetric< TFixedImage, TMovingImage, TVirtualImage >::template GetValueAndDerivativeThreader< TDomainPartitioner >
  {
  public:
    /** Standard class typedefs. */
    typedef DemonsGetValueAndDerivativeThreader    Self;
    typedef typename ImageToImageObjectMetric< TFixedImage, TMovingImage, TVirtualImage >
      ::template GetValueAndDerivativeThreader< TDomainPartitioner >                 Superclass;
    typedef SmartPointer< Self >                                            Pointer;
    typedef SmartPointer< const Self >                                      ConstPointer;

    itkTypeMacro( DemonsImageToImageObjectMetric::DemonsGetValueAndDerivativeThreader,
      ImageToImageObjectMetric::GetValueAndDerivativeThreader );

    typedef typename Superclass::DomainType         DomainType;
    typedef typename Superclass::EnclosingClassType EnclosingClassType;

  protected:

    /** This function computes the local voxel-wise contribution of
     *  the metric to the global integral of the metric/derivative.
     */
    virtual bool ProcessPoint(
          const VirtualPointType &          virtualPoint,
          const FixedImagePointType &       mappedFixedPoint,
          const FixedImagePixelType &       mappedFixedPixelValue,
          const FixedImageGradientType &    mappedFixedImageGradient,
          const MovingImagePointType &      mappedMovingPoint,
          const MovingImagePixelType &      mappedMovingPixelValue,
          const MovingImageGradientType &   mappedMovingImageGradient,
          MeasureType &                     metricValueReturn,
          DerivativeType &                  localDerivativeReturn,
          const ThreadIdType                threadID ) const;
  };

  /** \class DemonsDenseGetValueAndDerivativeThreader
   * \brief Run ProcessPoint on the points defined by an ImageRegion.
   * \ingroup ITKHighDimensionalMetrics
   * */
  class DemonsDenseGetValueAndDerivativeThreader
    : public DemonsGetValueAndDerivativeThreader< ThreadedImageRegionPartitioner< VirtualImageDimension > >
  {
  public:
    /** Standard class typedef. */
    typedef DemonsDenseGetValueAndDerivativeThreader                              Self;
    typedef DemonsGetValueAndDerivativeThreader< ThreadedImageRegionPartitioner< VirtualImageDimension > >
                                                                            Superclass;
    typedef SmartPointer< Self >                                            Pointer;
    typedef SmartPointer< const Self >                                      ConstPointer;

    itkNewMacro( Self );

    itkTypeMacro( DemonsImageToImageObjectMetric::DemonsDenseGetValueAndDerivativeThreader,
      DemonsImageToImageObjectMetric::GetValueAndDerivativeThreader );

    typedef typename Superclass::DomainType         DomainType;
    typedef typename Superclass::EnclosingClassType EnclosingClassType;

  protected:
    DemonsDenseGetValueAndDerivativeThreader() {}

    virtual void ThreadedExecution( EnclosingClassType * enclosingClass,
                                    const DomainType& domain,
                                    const ThreadIdType threadId );

  private:
    DemonsDenseGetValueAndDerivativeThreader( const Self & ); // purposely not implemented
    void operator=( const Self & ); // purposely not implemented
  };

  /** \class DemonsSparseGetValueAndDerivativeThreader
   * \brief Run \c ProcessPoint on the points defined by a PointSet.
   * \ingroup ITKHighDimensionalMetrics
   * */
  class DemonsSparseGetValueAndDerivativeThreader
    : public DemonsGetValueAndDerivativeThreader< ThreadedIndexedContainerPartitioner >
  {
  public:
    /** Standard class typedef. */
    typedef DemonsSparseGetValueAndDerivativeThreader                                  Self;
    typedef DemonsGetValueAndDerivativeThreader< ThreadedIndexedContainerPartitioner > Superclass;
    typedef SmartPointer< Self >                                                                                Pointer;
    typedef SmartPointer< const Self >                                                                          ConstPointer;

    itkTypeMacro( DemonsImageToImageObjectMetric::DemonsSparseGetValueAndDerivativeThreader,
      DemonsImageToImageObjectMetric::GetValueAndDerivativeThreader );

    itkNewMacro( Self );

    typedef typename Superclass::DomainType         DomainType;
    typedef typename Superclass::EnclosingClassType EnclosingClassType;

  protected:
    DemonsSparseGetValueAndDerivativeThreader() {}

    virtual void ThreadedExecution( EnclosingClassType * enclosingClass,
                                    const DomainType& domain,
                                    const ThreadIdType threadId );

  private:
    DemonsSparseGetValueAndDerivativeThreader( const Self & ); // purposely not implemented
    void operator=( const Self & ); // purposely not implemented
  };

  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  DemonsImageToImageObjectMetric(const Self &); //purposely not implemented
  void operator = (const Self &); //purposely not implemented
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDemonsImageToImageObjectMetric.hxx"
#endif

#endif
