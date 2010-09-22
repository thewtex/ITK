/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageToImageMetric.h
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageToImageMetric_h
#define __itkImageToImageMetric_h

#include "itkSingleValuedCostFunction.h"
#include "itkImageBase.h"
#include "itkTransform.h"
#include "itkInterpolateImageFunction.h"
#include "itkExceptionObject.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkSpatialObject.h"
#include "itkBSplineDeformableTransform.h"
#include "itkCentralDifferenceImageFunction.h"
#include "itkCovariantVector.h"

#include "itkMultiThreader.h"

#include "itkBSplineInterpolateImageFunction.h"

namespace itk
{
/** \class ImageToImageMetric
 * \brief Computes similarity between regions of two images.
 *
 * This Class is templated over the type of the two input images.
 * It expects a Transform and an Interpolator to be plugged in.
 * This particular class is the base class for a hierarchy of
 * similarity metrics.
 *
 * This class computes a value that measures the similarity
 * between the Fixed image and the transformed Moving image.
 * The Interpolator is used to compute intensity values on
 * non-grid positions resulting from mapping points through
 * the Transform.
 *
 *
 * \ingroup RegistrationMetrics
 *
 */

template< class TFixedImage,  class TMovingImage >
class ITK_EXPORT ImageToImageMetric:
  public SingleValuedCostFunction
{
public:
  /** Standard class typedefs. */
  typedef ImageToImageMetric         Self;
  typedef SingleValuedCostFunction   Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Type used for representing point components  */
  typedef typename Superclass::ParametersValueType CoordinateRepresentationType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToImageMetric, SingleValuedCostFunction);

  /**  Type of the moving Image. */
  typedef TMovingImage                           MovingImageType;
  typedef typename TMovingImage::PixelType       MovingImagePixelType;
  typedef typename MovingImageType::ConstPointer MovingImageConstPointer;

  /**  Type of the fixed Image. */
  typedef TFixedImage                           FixedImageType;
  typedef typename TFixedImage::PixelType       FixedImagePixelType;
  typedef typename FixedImageType::ConstPointer FixedImageConstPointer;
  typedef typename FixedImageType::RegionType   FixedImageRegionType;

  /** Constants for the image dimensions */
  itkStaticConstMacro(MovingImageDimension,
                      unsigned int,
                      TMovingImage::ImageDimension);
  itkStaticConstMacro(FixedImageDimension,
                      unsigned int,
                      TFixedImage::ImageDimension);

  /**  Type of the Transform Base class */
  typedef Transform< CoordinateRepresentationType,
                     itkGetStaticConstMacro(MovingImageDimension),
                     itkGetStaticConstMacro(FixedImageDimension) >
  TransformType;

  typedef typename TransformType::Pointer         TransformPointer;
  typedef typename TransformType::InputPointType  InputPointType;
  typedef typename TransformType::OutputPointType OutputPointType;
  typedef typename TransformType::ParametersType  TransformParametersType;
  typedef typename TransformType::JacobianType    TransformJacobianType;

  /** Index and Point typedef support. */
  typedef typename FixedImageType::IndexType           FixedImageIndexType;
  typedef typename FixedImageIndexType::IndexValueType FixedImageIndexValueType;
  typedef typename MovingImageType::IndexType          MovingImageIndexType;
  typedef typename TransformType::InputPointType       FixedImagePointType;
  typedef typename TransformType::OutputPointType      MovingImagePointType;

  typedef std::vector< FixedImageIndexType > FixedImageIndexContainer;

  /**  Type of the Interpolator Base class */
  typedef InterpolateImageFunction< MovingImageType, CoordinateRepresentationType > InterpolatorType;

  /** Gaussian filter to compute the gradient of the Moving Image */
  typedef typename NumericTraits< MovingImagePixelType >::RealType                   RealType;
  typedef CovariantVector< RealType, itkGetStaticConstMacro(MovingImageDimension) >  GradientPixelType;
  typedef Image< GradientPixelType, itkGetStaticConstMacro(MovingImageDimension) >   GradientImageType;
  typedef SmartPointer< GradientImageType >                                          GradientImagePointer;
  typedef GradientRecursiveGaussianImageFilter< MovingImageType, GradientImageType > GradientImageFilterType;
  typedef typename GradientImageFilterType::Pointer                                  GradientImageFilterPointer;

  typedef typename InterpolatorType::Pointer InterpolatorPointer;

  /**  Type for the mask of the fixed image. Only pixels that are "inside"
       this mask will be considered for the computation of the metric */
  typedef SpatialObject< itkGetStaticConstMacro(FixedImageDimension) > FixedImageMaskType;
  typedef typename FixedImageMaskType::Pointer                         FixedImageMaskPointer;
  typedef typename FixedImageMaskType::ConstPointer                    FixedImageMaskConstPointer;

  /**  Type for the mask of the moving image. Only pixels that are "inside"
       this mask will be considered for the computation of the metric */
  typedef SpatialObject< itkGetStaticConstMacro(MovingImageDimension) > MovingImageMaskType;
  typedef typename MovingImageMaskType::Pointer                         MovingImageMaskPointer;
  typedef typename MovingImageMaskType::ConstPointer                    MovingImageMaskConstPointer;

  /**  Type of the measure. */
  typedef typename Superclass::MeasureType MeasureType;

  /**  Type of the derivative. */
  typedef typename Superclass::DerivativeType DerivativeType;

  /**  Type of the parameters. */
  typedef typename Superclass::ParametersType ParametersType;

  /** Connect the Fixed Image.  */
  itkSetConstObjectMacro(FixedImage, FixedImageType);

  /** Get the Fixed Image. */
  itkGetConstObjectMacro(FixedImage, FixedImageType);

  /** Connect the Moving Image.  */
  itkSetConstObjectMacro(MovingImage, MovingImageType);

  /** Get the Moving Image. */
  itkGetConstObjectMacro(MovingImage, MovingImageType);

  /** Connect the Transform. */
  itkSetObjectMacro(Transform, TransformType);

  /** Get a pointer to the Transform.  */
  itkGetConstObjectMacro(Transform, TransformType);

  /** Connect the Interpolator. */
  itkSetObjectMacro(Interpolator, InterpolatorType);

  /** Get a pointer to the Interpolator.  */
  itkGetConstObjectMacro(Interpolator, InterpolatorType);

  /** Get the number of pixels considered in the computation. */
  unsigned long GetNumberOfMovingImageSamples(void)
  {
    return this->GetNumberOfPixelsCounted();
  }

  itkGetConstReferenceMacro(NumberOfPixelsCounted, unsigned long);

  /** Set the region over which the metric will be computed */
  void SetFixedImageRegion(const FixedImageRegionType reg);

  /** Get the region over which the metric will be computed */
  itkGetConstReferenceMacro(FixedImageRegion, FixedImageRegionType);

  /** Set/Get the moving image mask. */
  itkSetObjectMacro(MovingImageMask, MovingImageMaskType);
  itkSetConstObjectMacro(MovingImageMask, MovingImageMaskType);
  itkGetConstObjectMacro(MovingImageMask, MovingImageMaskType);

  /** Set/Get the fixed image mask. */
  itkSetObjectMacro(FixedImageMask, FixedImageMaskType);
  itkSetConstObjectMacro(FixedImageMask, FixedImageMaskType);
  itkGetConstObjectMacro(FixedImageMask, FixedImageMaskType);

  /** Set the fixed image indexes to be used as the samples when
   *   computing the match metric */
  void SetFixedImageIndexes(const FixedImageIndexContainer & indexes);

  void SetUseFixedImageIndexes(bool useIndex);

  itkGetConstReferenceMacro(UseFixedImageIndexes, bool);

  /** Set/Get number of threads to use for computations. */
  void SetNumberOfThreads(unsigned int numberOfThreads);

  itkGetConstReferenceMacro(NumberOfThreads, unsigned int);

  /** Set/Get gradient computation. */
  itkSetMacro(ComputeGradient, bool);
  itkGetConstReferenceMacro(ComputeGradient, bool);
  itkBooleanMacro(ComputeGradient);

  /** Computes the gradient image and assigns it to m_GradientImage */
  virtual void ComputeGradient(void);

  /** Get Gradient Image. */
  itkGetConstObjectMacro(GradientImage, GradientImageType);

  /** Set the parameters defining the Transform. */
  void SetTransformParameters(const ParametersType & parameters) const;

  /** Return the number of parameters required by the Transform */
  unsigned int GetNumberOfParameters(void) const
  {
    return m_Transform->GetNumberOfParameters();
  }

  /** Initialize the Metric by making sure that all the components
   *  are present and plugged together correctly     */
  virtual void Initialize(void)
  throw ( ExceptionObject );

  /** Initialize the components related to supporting multiple threads */
  virtual void MultiThreadingInitialize(void)
  throw ( ExceptionObject );

  /** Number of spatial samples to used to compute metric
   *   This sets the number of samples.  */
  virtual void SetNumberOfFixedImageSamples(unsigned long numSamples);

  itkGetConstReferenceMacro(NumberOfFixedImageSamples, unsigned long);

  /** Number of spatial samples to used to compute metric
   *   This sets the number of samples.  */
  void SetNumberOfSpatialSamples(unsigned long num)
  {
    this->SetNumberOfFixedImageSamples(num);
  }

  unsigned long GetNumberOfSpatialSamples(void)
  {
    return this->GetNumberOfFixedImageSamples();
  }

  /** Minimum fixed-image intensity needed for a sample to be used in the
   *  metric computation */
  void SetFixedImageSamplesIntensityThreshold(const FixedImagePixelType & thresh);

  itkGetConstReferenceMacro(FixedImageSamplesIntensityThreshold, FixedImagePixelType);

  void SetUseFixedImageSamplesIntensityThreshold(bool useThresh);

  itkGetConstReferenceMacro(UseFixedImageSamplesIntensityThreshold, bool);

  /** Select whether the metric will be computed using all the pixels on the
   * fixed image region, or only using a set of randomly selected pixels.
   * This value override IntensityThreshold, Masks, and SequentialSampling. */
  void SetUseAllPixels(bool useAllPixels);

  void UseAllPixelsOn(void)
  {
    this->SetUseAllPixels(true);
  }

  void UseAllPixelsOff(void)
  {
    this->SetUseAllPixels(false);
  }

  itkGetConstReferenceMacro(UseAllPixels, bool);

  /** If set to true, then every pixel in the fixed image will be scanned to
   * determine if it should be used in registration metric computation.  A
   * pixel will be chosen if it meets any mask or threshold limits set.  If
   * set to false, then UseAllPixels will be set to false. */
  void SetUseSequentialSampling(bool sequentialSampling);

  itkGetConstReferenceMacro(UseSequentialSampling, bool);

  /** Reinitialize the seed of the random number generator that selects the
   * sample of pixels used for estimating the image histograms and the joint
   * histogram. By nature, this metric is not deterministic, since at each run
   * it may select a different set of pixels. By initializing the random number
   * generator seed to the same value you can restore determinism. On the other
   * hand, calling the method ReinitializeSeed() without arguments will use the
   * clock from your machine in order to have a very random initialization of
   * the seed. This will indeed increase the non-deterministic behavior of the
   * metric. */
  void ReinitializeSeed();

  void ReinitializeSeed(int seed);

  /** This boolean flag is only relevant when this metric is used along
   * with a BSplineDeformableTransform. The flag enables/disables the
   * caching of values computed when a physical point is mapped through
   * the BSplineDeformableTransform. In particular it will cache the
   * values of the BSpline weights for that points, and the indexes
   * indicating what BSpline-grid nodes are relevant for that specific
   * point. This caching is made optional due to the fact that the
   * memory arrays used for the caching can reach large sizes even for
   * moderate image size problems. For example, for a 3D image of
   * 256^3, using 20% of pixels, these arrays will take about 1
   * Gigabyte of RAM for storage. The ratio of computing time between
   * using the cache or not using the cache can reach 1:5, meaning that
   * using the caching can provide a five times speed up. It is
   * therefore, interesting to enable the caching, if enough memory is
   * available for it. The caching is enabled by default, in order to
   * preserve backward compatibility with previous versions of ITK. */
  itkSetMacro(UseCachingOfBSplineWeights, bool);
  itkGetConstReferenceMacro(UseCachingOfBSplineWeights, bool);
  itkBooleanMacro(UseCachingOfBSplineWeights);

  typedef MultiThreader MultiThreaderType;
  /** Get the Threader. */
  itkGetConstObjectMacro(Threader, MultiThreaderType);
  const TransformPointer * GetThreaderTransform()
  {
    return m_ThreaderTransform;
  }

protected:
  ImageToImageMetric();
  virtual ~ImageToImageMetric();

  void PrintSelf(std::ostream & os, Indent indent) const;

  /** \class FixedImageSamplePoint
   * A fixed image spatial sample consists of the fixed domain point
   * and the fixed image value at that point. */
  /// @cond
  class FixedImageSamplePoint
  {
public:
    FixedImageSamplePoint()
    {
      point.Fill(0.0);
      value = 0;
      valueIndex = 0;
    }

    ~FixedImageSamplePoint() {}
public:
    FixedImagePointType point;
    double              value;
    unsigned int        valueIndex;
  };
  /// @endcond

  bool                     m_UseFixedImageIndexes;
  FixedImageIndexContainer m_FixedImageIndexes;

  bool                m_UseFixedImageSamplesIntensityThreshold;
  FixedImagePixelType m_FixedImageSamplesIntensityThreshold;

  /** FixedImageSamplePoint typedef support. */
  typedef std::vector< FixedImageSamplePoint > FixedImageSampleContainer;

  /** Uniformly select a sample set from the fixed image domain. */
  virtual void SampleFixedImageRegion(FixedImageSampleContainer & samples) const;

  virtual void SampleFixedImageIndexes(FixedImageSampleContainer &
                                       samples) const;

  /** Gather all the pixels from the fixed image domain. */
  virtual void SampleFullFixedImageRegion(FixedImageSampleContainer &
                                          samples) const;

  /** Container to store a set of points and fixed image values. */
  FixedImageSampleContainer m_FixedImageSamples;

  unsigned long          m_NumberOfParameters;
  mutable ParametersType m_Parameters;

  unsigned long m_NumberOfFixedImageSamples;
  //m_NumberOfPixelsCounted must be mutable because the const
  //thread consolidation functions merge each threads valus
  //onto this accumulator variable.
  mutable unsigned long m_NumberOfPixelsCounted;

  FixedImageConstPointer  m_FixedImage;
  MovingImageConstPointer m_MovingImage;

  /** Main transform to be used in thread = 0 */
  TransformPointer m_Transform;
  /** Copies of Transform helpers per thread (N-1 of them, since m_Transform
   * will do the work for thread=0. */
  TransformPointer *m_ThreaderTransform;

  InterpolatorPointer m_Interpolator;

  bool                 m_ComputeGradient;
  GradientImagePointer m_GradientImage;

  FixedImageMaskConstPointer  m_FixedImageMask;
  MovingImageMaskConstPointer m_MovingImageMask;

  unsigned int m_NumberOfThreads;

  bool m_UseAllPixels;
  bool m_UseSequentialSampling;

  bool m_ReseedIterator;

  int m_RandomSeed;

  /** Types and variables related to BSpline deformable transforms.
    * If the transform is of type third order BSplineDeformableTransform,
    * then we can speed up the metric derivative calculation by
    * only inspecting the parameters within the support region
    * of a mapped point.  */

  /** Boolean to indicate if the transform is BSpline deformable. */
  bool m_TransformIsBSpline;

  /** The number of BSpline transform weights is the number of
    * of parameter in the support region (per dimension ). */
  unsigned long m_NumBSplineWeights;

  itkStaticConstMacro(DeformationSplineOrder, unsigned int, 3);

  typedef BSplineDeformableTransform< CoordinateRepresentationType,
                                      ::itk::GetImageDimension< FixedImageType >::ImageDimension,
                                      itkGetStaticConstMacro(DeformationSplineOrder) >             BSplineTransformType;

  typedef typename BSplineTransformType::WeightsType      BSplineTransformWeightsType;
  typedef typename BSplineTransformWeightsType::ValueType WeightsValueType;
  typedef          Array2D< WeightsValueType >            BSplineTransformWeightsArrayType;

  typedef typename BSplineTransformType::ParameterIndexArrayType
  BSplineTransformIndexArrayType;
  typedef typename BSplineTransformIndexArrayType::ValueType IndexValueType;
  typedef          Array2D< IndexValueType >                 BSplineTransformIndicesArrayType;

  typedef          std::vector< MovingImagePointType > MovingImagePointArrayType;
  typedef          std::vector< bool >                 BooleanArrayType;
  typedef          FixedArray< unsigned long,
                               ::itk::GetImageDimension< FixedImageType >
                               ::ImageDimension >            BSplineParametersOffsetType;
  /**
   * If a BSplineInterpolationFunction is used, this class obtain
   * image derivatives from the BSpline interpolator. Otherwise,
   * image derivatives are computed using central differencing.
   */
  typedef BSplineInterpolateImageFunction< MovingImageType,
                                           CoordinateRepresentationType >
  BSplineInterpolatorType;
  /** Typedefs for using central difference calculator. */
  typedef CentralDifferenceImageFunction< MovingImageType,
                                          CoordinateRepresentationType >
  DerivativeFunctionType;
  typedef          CovariantVector< double,
                                    itkGetStaticConstMacro(MovingImageDimension) >
  ImageDerivativesType;

  typename BSplineTransformType::Pointer m_BSplineTransform;

  BSplineTransformWeightsArrayType m_BSplineTransformWeightsArray;
  BSplineTransformIndicesArrayType m_BSplineTransformIndicesArray;
  MovingImagePointArrayType        m_BSplinePreTransformPointsArray;
  BooleanArrayType                 m_WithinBSplineSupportRegionArray;

  BSplineParametersOffsetType m_BSplineParametersOffset;

  // Variables needed for optionally caching values when using a BSpline
  // transform.
  bool                                   m_UseCachingOfBSplineWeights;
  mutable BSplineTransformWeightsType    m_BSplineTransformWeights;
  mutable BSplineTransformIndexArrayType m_BSplineTransformIndices;

  mutable BSplineTransformWeightsType *   m_ThreaderBSplineTransformWeights;
  mutable BSplineTransformIndexArrayType *m_ThreaderBSplineTransformIndices;

  virtual void PreComputeTransformValues(void);

  /** Transform a point from FixedImage domain to MovingImage domain.
   * This function also checks if mapped point is within support region. */
  virtual void TransformPoint(unsigned int sampleNumber,
                              MovingImagePointType & mappedPoint,
                              bool & sampleWithinSupportRegion,
                              double & movingImageValue,
                              unsigned int threadID) const;

  virtual void TransformPointWithDerivatives(unsigned int sampleNumber,
                                             MovingImagePointType & mappedPoint,
                                             bool & sampleWithinSupportRegion,
                                             double & movingImageValue,
                                             ImageDerivativesType & gradient,
                                             unsigned int threadID) const;

  /** Boolean to indicate if the interpolator BSpline. */
  bool m_InterpolatorIsBSpline;
  /** Pointer to BSplineInterpolator. */
  typename BSplineInterpolatorType::Pointer m_BSplineInterpolator;

  /** Pointer to central difference calculator. */
  typename DerivativeFunctionType::Pointer m_DerivativeCalculator;

  /** Compute image derivatives at a point. */
  virtual void ComputeImageDerivatives(const MovingImagePointType & mappedPoint,
                                       ImageDerivativesType & gradient,
                                       unsigned int threadID) const;

  /**
   * Types and variables related to multi-threading
   */

  struct MultiThreaderParameterType {
    ImageToImageMetric *metric;
  };

  MultiThreaderType::Pointer m_Threader;
  MultiThreaderParameterType m_ThreaderParameter;
  mutable unsigned int *     m_ThreaderNumberOfMovingImageSamples;
  bool                       m_WithinThreadPreProcess;
  bool                       m_WithinThreadPostProcess;

  void                           GetValueMultiThreadedPreProcessInitiate(void) const;

  void                           GetValueMultiThreadedInitiate(void) const;

  void                           GetValueMultiThreadedPostProcessInitiate(void) const;

  static ITK_THREAD_RETURN_TYPE  GetValueMultiThreadedPreProcess(void *arg);

  static ITK_THREAD_RETURN_TYPE  GetValueMultiThreaded(void *arg);

  static ITK_THREAD_RETURN_TYPE  GetValueMultiThreadedPostProcess(void *arg);

  virtual inline void       GetValueThread(unsigned int threadID) const;

  virtual inline void       GetValueThreadPreProcess(
    unsigned int itkNotUsed(threadID),
    bool itkNotUsed(withinSampleThread) ) const
  {}
  virtual inline bool       GetValueThreadProcessSample(
    unsigned int itkNotUsed(threadID),
    unsigned long itkNotUsed(fixedImageSample),
    const MovingImagePointType & itkNotUsed(mappedPoint),
    double itkNotUsed(movingImageValue) ) const
  { return false; }
  virtual inline void       GetValueThreadPostProcess(
    unsigned int itkNotUsed(threadID),
    bool itkNotUsed(withinSampleThread) ) const
  {}

  void                          GetValueAndDerivativeMultiThreadedPreProcessInitiate(void) const;

  void                          GetValueAndDerivativeMultiThreadedInitiate(void) const;

  void                          GetValueAndDerivativeMultiThreadedPostProcessInitiate(void) const;

  static ITK_THREAD_RETURN_TYPE GetValueAndDerivativeMultiThreadedPreProcess(void *arg);

  static ITK_THREAD_RETURN_TYPE GetValueAndDerivativeMultiThreaded(void *arg);

  static ITK_THREAD_RETURN_TYPE GetValueAndDerivativeMultiThreadedPostProcess(void *arg);

  virtual inline void  GetValueAndDerivativeThread(unsigned int threadID) const;

  virtual inline void  GetValueAndDerivativeThreadPreProcess(
    unsigned int itkNotUsed(threadID),
    bool itkNotUsed(withinSampleThread) ) const
  {}
  virtual inline bool  GetValueAndDerivativeThreadProcessSample(
    unsigned int itkNotUsed(threadID),
    unsigned long itkNotUsed(fixedImageSample),
    const MovingImagePointType & itkNotUsed(mappedPoint),
    double itkNotUsed(movingImageValue),
    const ImageDerivativesType & itkNotUsed(movingImageGradientValue) ) const
  { return false; }
  virtual inline void  GetValueAndDerivativeThreadPostProcess(
    unsigned int itkNotUsed(threadID),
    bool itkNotUsed(withinSampleThread) ) const
  {}

  /** Synchronizes the threader transforms with the transform
   *   member variable.
   */
  virtual void SynchronizeTransforms() const;

private:
  ImageToImageMetric(const Self &); //purposely not implemented
  void operator=(const Self &);     //purposely not implemented

  FixedImageRegionType m_FixedImageRegion;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageToImageMetric.txx"
#endif

#endif
