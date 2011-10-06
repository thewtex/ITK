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

#ifndef __itkJointHistogramMutualInformationImageToImageObjectMetric_h
#define __itkJointHistogramMutualInformationImageToImageObjectMetric_h

#include "itkImageToImageObjectMetric.h"
#include "itkImage.h"
#include "itkBSplineDerivativeKernelFunction.h"

namespace itk
{
/** \class JointHistogramMutualInformationImageToImageMetric
 * \brief Computes the mutual information between two images to be
 * registered using the method of Mattes et al.
 *
 * References:
 * [1] "Optimization of Mutual Information for MultiResolution Image
 *      Registration"
 *      P. Thevenaz and M. Unser
 *      IEEE Transactions in Image Processing, 9(12) December 2000.
 *
 * \ingroup ITKHighDimensionalMetrics
 */

template<class TFixedImage,class TMovingImage,class TVirtualImage = TFixedImage>
class ITK_EXPORT JointHistogramMutualInformationImageToImageObjectMetric :
  public ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
{
public:

  /** Standard class typedefs. */
  typedef JointHistogramMutualInformationImageToImageObjectMetric            Self;
  typedef ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage> Superclass;
  typedef SmartPointer<Self>                                                 Pointer;
  typedef SmartPointer<const Self>                                           ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(JointHistogramMutualInformationImageToImageObjectMetric,
                ImageToImageObjectMetric);

  /** Type used for representing parameter values  */
  typedef typename Superclass::CoordinateRepresentationType
                                                  CoordinateRepresentationType;
  /** Type used internally for computations */
  typedef typename Superclass::InternalComputationValueType
                                                  InternalComputationValueType;
  /**  Type of the parameters. */
  typedef typename Superclass::ParametersType         ParametersType;
  typedef typename Superclass::ParametersValueType    ParametersValueType;
  typedef typename Superclass::NumberOfParametersType NumberOfParametersType;

  /** Superclass typedefs */
  typedef typename Superclass::MeasureType              MeasureType;
  typedef typename Superclass::DerivativeType           DerivativeType;
  typedef typename Superclass::FixedImagePointType      FixedImagePointType;
  typedef typename Superclass::FixedImagePixelType      FixedImagePixelType;
  typedef typename Superclass::FixedGradientPixelType
                                                        FixedImageGradientType;
  typedef typename Superclass::MovingImagePointType     MovingImagePointType;
  typedef typename Superclass::MovingImagePixelType     MovingImagePixelType;
  typedef typename Superclass::MovingGradientPixelType
                                                        MovingImageGradientType;
  typedef typename Superclass::FixedTransformType::JacobianType
                                                        FixedTransformJacobianType;
  typedef typename Superclass::MovingTransformType::JacobianType
                                                        MovingTransformJacobianType;
  typedef typename Superclass::VirtualImageType         VirtualImageType;
  typedef typename Superclass::VirtualIndexType         VirtualIndexType;
  typedef typename Superclass::VirtualPointType         VirtualPointType;
  typedef typename Superclass::VirtualSampledPointSetType
                                                        VirtualSampledPointSetType;

  /* Image dimension accessors */
  itkStaticConstMacro(VirtualImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<TVirtualImage>::ImageDimension);
  itkStaticConstMacro(MovingImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<TMovingImage>::ImageDimension);

  /** Value type of the PDF */
  typedef InternalComputationValueType                  PDFValueType;

  /** Typedef for the joint PDF and marginal PDF are stored as ITK Images. */
  typedef Image<PDFValueType,1>                 MarginalPDFType;
  typedef typename MarginalPDFType::IndexType   MarginalPDFIndexType;
  typedef typename MarginalPDFType::PointType   MarginalPDFPointType;
  typedef Image< PDFValueType, 2>               JointPDFType;
  typedef typename JointPDFType::IndexType      JointPDFIndexType;
  typedef typename JointPDFType::PointType      JointPDFPointType;
  typedef typename JointPDFType::IndexValueType JointPDFIndexValueType;

  itkGetConstReferenceMacro(JointPDF,typename JointPDFType::Pointer);

  // Declare the type for the derivative calculation
  typedef itk::GradientRecursiveGaussianImageFilter< JointPDFType >
                                                         JPDFGradientFilterType;

  typedef typename JPDFGradientFilterType::OutputImageType JPDFGradientImageType;

  typedef typename JPDFGradientImageType::Pointer JPDFGradientImagePointer;

  typedef itk::GradientRecursiveGaussianImageFilter< MarginalPDFType >
                                                  MarginalGradientFilterType;
  typedef typename MarginalGradientFilterType::OutputImageType
                                                  MarginalGradientImageType;
  typedef typename MarginalGradientImageType::Pointer
                                                  MarginalGradientImagePointer;

  /** pdf interpolator */
  typedef LinearInterpolateImageFunction<JointPDFType,double>
                                                     JointPDFInterpolatorType;
  typedef typename JointPDFInterpolatorType::Pointer JointPDFInterpolatorPointer;
  typedef LinearInterpolateImageFunction<MarginalPDFType,double>
                                                     MarginalPDFInterpolatorType;
  typedef typename MarginalPDFInterpolatorType::Pointer
                                                  MarginalPDFInterpolatorPointer;

  /** Get/Set the number of histogram bins */
  itkSetClampMacro( NumberOfHistogramBins, SizeValueType,
                    5, NumericTraits< SizeValueType >::max() );
  itkGetConstReferenceMacro(NumberOfHistogramBins, SizeValueType );

  /** Get/Set option to smooth the joint pdf after it's updated */
  itkSetMacro(VarianceForJointPDFSmoothing, InternalComputationValueType);
  itkGetMacro(VarianceForJointPDFSmoothing, InternalComputationValueType);

  /** Initialize the metric. Make sure all essential inputs are plugged in. */
  virtual void Initialize() throw (itk::ExceptionObject);

  /** Get the value */
  MeasureType GetValue() const;

protected:
  JointHistogramMutualInformationImageToImageObjectMetric();
  virtual ~JointHistogramMutualInformationImageToImageObjectMetric();

  /** Update the histograms for use in GetValueAndDerivative
   *  This implementation single-threads the JH computation but it
   *  could be multi-threaded in the future.
   *  Results are returned in \c value and \c derivative.
   */
  virtual void InitializeForIteration() const;

  /** Compute the point location with the JointPDF image.  Returns false if the
   * point is not inside the image. */
  inline bool  ComputeJointPDFPoint( const FixedImagePixelType fixedImageValue,
                               const MovingImagePixelType movingImageValue,
                               JointPDFPointType & jointPDFpoint ) const;

  /** \class JointHistogramMutualInformationComputeJointPDFThreader
   * \brief Compute the JointPDF image.
   * \ingroup ITKHighDimensionalMetrics
   */
  template< class TDomainPartitioner >
  class JointHistogramMutualInformationComputeJointPDFThreader
    : public DomainThreader< TDomainPartitioner,
                             JointHistogramMutualInformationImageToImageObjectMetric< TFixedImage, TMovingImage, TVirtualImage > >
  {
  public:
    /** Standard class typedef. */
    typedef JointHistogramMutualInformationComputeJointPDFThreader  Self;
    typedef DomainThreader< TDomainPartitioner,
                            JointHistogramMutualInformationImageToImageObjectMetric< TFixedImage, TMovingImage, TVirtualImage > >
                                                                    Superclass;
    typedef SmartPointer< Self >                                    Pointer;
    typedef SmartPointer< const Self >                              ConstPointer;

    itkTypeMacro( JointHistogramMutualInformationImageToImageObjectMetric::JointHistogramMutualInformationComputeJointPDFThreader,
      DomainThreader );

    typedef typename Superclass::DomainType         DomainType;
    typedef typename Superclass::EnclosingClassType EnclosingClassType;

  protected:
    JointHistogramMutualInformationComputeJointPDFThreader() {}

    /** Create the \c m_JointPDFPerThread's. */
    virtual void BeforeThreadedExecution();
    /** Called by the \c ThreadedExecution of derived classes. */
    virtual void ProcessPoint( const VirtualIndexType & virtualIndex,
                       const VirtualPointType & virtualPoint,
                       const ThreadIdType threadId );
    /** Collect the results per and normalize. */
    virtual void AfterThreadedExecution();

    typedef Image< SizeValueType, 2 > JointHistogramType;
    std::vector< typename JointHistogramType::Pointer > m_JointHistogramPerThread;
    std::vector< SizeValueType > m_JointHistogramCountPerThread;

  private:
    JointHistogramMutualInformationComputeJointPDFThreader( const Self & ); // purposely not implemented
    void operator=( const Self & ); // purposely not implemented
  };

  /** \class JointHistogramMutualInformationDenseComputeJointPDFThreader
   * \brief Compute the joint histogram from the entire virtual domain.
   * \ingroup ITKHighDimensionalMetrics
   */
  class JointHistogramMutualInformationDenseComputeJointPDFThreader
    : public JointHistogramMutualInformationComputeJointPDFThreader< ThreadedImageRegionPartitioner< VirtualImageDimension > >
  {
  public:
    /** Standard class typedefs. */
    typedef JointHistogramMutualInformationDenseComputeJointPDFThreader Self;
    typedef JointHistogramMutualInformationComputeJointPDFThreader< ThreadedImageRegionPartitioner< VirtualImageDimension > >
                                                                        Superclass;
    typedef SmartPointer< Self >                                        Pointer;
    typedef SmartPointer< const Self >                                  ConstPointer;

    itkTypeMacro( JointHistogramMutualInformationImageToImageObjectMetric::JointHistogramMutualInformationDenseComputeJointPDFThreader,
      JointHistogramMutualInformationImageToImageObjectMetric::JointHistogramMutualInformationComputeJointPDFThreader );

    itkNewMacro( Self );

    typedef typename Superclass::DomainType         DomainType;
    typedef typename Superclass::EnclosingClassType EnclosingClassType;

  protected:
    JointHistogramMutualInformationDenseComputeJointPDFThreader() {}

    virtual void ThreadedExecution( EnclosingClassType * enclosingClass,
                                    const DomainType& domain,
                                    const ThreadIdType threadId );

  private:
    JointHistogramMutualInformationDenseComputeJointPDFThreader( const Self & ); // purposely not implemented
    void operator=( const Self & ); // purposely nrot implemented
  };
  typename JointHistogramMutualInformationDenseComputeJointPDFThreader::Pointer m_JointHistogramMutualInformationDenseComputeJointPDFThreader;

  /** \class JointHistogramMutualInformationSparseComputeJointPDFThreader
   * \brief Compute the joint histogram from the fixed image point set.
   * \ingroup ITKHighDimensionalMetrics
   */
  class JointHistogramMutualInformationSparseComputeJointPDFThreader
    : public JointHistogramMutualInformationComputeJointPDFThreader< ThreadedIndexedContainerPartitioner >
  {
  public:
    /** Standard class typedefs. */
    typedef JointHistogramMutualInformationSparseComputeJointPDFThreader Self;
    typedef JointHistogramMutualInformationComputeJointPDFThreader< ThreadedIndexedContainerPartitioner >
                                                                        Superclass;
    typedef SmartPointer< Self >                                        Pointer;
    typedef SmartPointer< const Self >                                  ConstPointer;

    itkTypeMacro( JointHistogramMutualInformationImageToImageObjectMetric::JointHistogramMutualInformationSparseComputeJointPDFThreader,
      JointHistogramMutualInformationImageToImageObjectMetric::JointHistogramMutualInformationComputeJointPDFThreader );

    itkNewMacro( Self );

    typedef typename Superclass::DomainType         DomainType;
    typedef typename Superclass::EnclosingClassType EnclosingClassType;

  protected:
    JointHistogramMutualInformationSparseComputeJointPDFThreader() {}

    virtual void ThreadedExecution( EnclosingClassType * enclosingClass,
                                    const DomainType& domain,
                                    const ThreadIdType threadId );

  private:
    JointHistogramMutualInformationSparseComputeJointPDFThreader( const Self & ); // purposely not implemented
    void operator=( const Self & ); // purposely not implemented
  };
  typename JointHistogramMutualInformationSparseComputeJointPDFThreader::Pointer m_JointHistogramMutualInformationSparseComputeJointPDFThreader;

  /** \class JointHistogramMutualInformationGetValueAndDerivativeThreader
   * \brief Processes points for JointHistogramMutualInformation calculation.
   * \ingroup ITKHighDimensionalMetrics
   * */
  template < class TDomainPartitioner >
  class JointHistogramMutualInformationGetValueAndDerivativeThreader
    : public ImageToImageObjectMetric< TFixedImage, TMovingImage, TVirtualImage >::template GetValueAndDerivativeThreader< TDomainPartitioner >
  {
  public:
    /** Standard class typedefs. */
    typedef JointHistogramMutualInformationGetValueAndDerivativeThreader    Self;
    typedef typename ImageToImageObjectMetric< TFixedImage, TMovingImage, TVirtualImage >
      ::template GetValueAndDerivativeThreader< TDomainPartitioner >                 Superclass;
    typedef SmartPointer< Self >                                            Pointer;
    typedef SmartPointer< const Self >                                      ConstPointer;

    itkTypeMacro( JointHistogramMutualInformationImageToImageObjectMetric::JointHistogramMutualInformationGetValueAndDerivativeThreader,
      ImageToImageObjectMetric::GetValueAndDerivativeThreader );

    typedef typename Superclass::DomainType         DomainType;
    typedef typename Superclass::EnclosingClassType EnclosingClassType;

  protected:

    virtual void BeforeThreadedExecution();

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

    inline InternalComputationValueType ComputeFixedImageMarginalPDFDerivative(
                                          const MarginalPDFPointType & margPDFpoint,
                                          const ThreadIdType threadID ) const;

    inline InternalComputationValueType ComputeMovingImageMarginalPDFDerivative(
                                          const MarginalPDFPointType & margPDFpoint,
                                          const ThreadIdType threadID ) const;

    inline InternalComputationValueType ComputeJointPDFDerivative(
                                            const JointPDFPointType & jointPDFpoint,
                                            const ThreadIdType threadID,
                                            const SizeValueType ind ) const;

    std::vector< JointPDFInterpolatorPointer >    m_JointPDFInterpolatorPerThread;
    std::vector< MarginalPDFInterpolatorPointer > m_FixedImageMarginalPDFInterpolatorPerThread;
    std::vector< MarginalPDFInterpolatorPointer > m_MovingImageMarginalPDFInterpolatorPerThread;
  };

  /** \class JointHistogramMutualInformationDenseGetValueAndDerivativeThreader
   * \brief Run ProcessPoint on the points defined by an ImageRegion.
   * \ingroup ITKHighDimensionalMetrics
   * */
  class JointHistogramMutualInformationDenseGetValueAndDerivativeThreader
    : public JointHistogramMutualInformationGetValueAndDerivativeThreader< ThreadedImageRegionPartitioner< VirtualImageDimension > >
  {
  public:
    /** Standard class typedef. */
    typedef JointHistogramMutualInformationDenseGetValueAndDerivativeThreader                              Self;
    typedef JointHistogramMutualInformationGetValueAndDerivativeThreader< ThreadedImageRegionPartitioner< VirtualImageDimension > >
                                                                            Superclass;
    typedef SmartPointer< Self >                                            Pointer;
    typedef SmartPointer< const Self >                                      ConstPointer;

    itkNewMacro( Self );

    itkTypeMacro( JointHistogramMutualInformationImageToImageObjectMetric::JointHistogramMutualInformationDenseGetValueAndDerivativeThreader,
      JointHistogramMutualInformationImageToImageObjectMetric::GetValueAndDerivativeThreader );

    typedef typename Superclass::DomainType         DomainType;
    typedef typename Superclass::EnclosingClassType EnclosingClassType;

  protected:
    JointHistogramMutualInformationDenseGetValueAndDerivativeThreader() {}

    virtual void ThreadedExecution( EnclosingClassType * enclosingClass,
                                    const DomainType& domain,
                                    const ThreadIdType threadId );

  private:
    JointHistogramMutualInformationDenseGetValueAndDerivativeThreader( const Self & ); // purposely not implemented
    void operator=( const Self & ); // purposely not implemented
  };

  /** \class JointHistogramMutualInformationSparseGetValueAndDerivativeThreader
   * \brief Run \c ProcessPoint on the points defined by a PointSet.
   * \ingroup ITKHighDimensionalMetrics
   * */
  class JointHistogramMutualInformationSparseGetValueAndDerivativeThreader
    : public JointHistogramMutualInformationGetValueAndDerivativeThreader< ThreadedIndexedContainerPartitioner >
  {
  public:
    /** Standard class typedef. */
    typedef JointHistogramMutualInformationSparseGetValueAndDerivativeThreader                                  Self;
    typedef JointHistogramMutualInformationGetValueAndDerivativeThreader< ThreadedIndexedContainerPartitioner > Superclass;
    typedef SmartPointer< Self >                                                                                Pointer;
    typedef SmartPointer< const Self >                                                                          ConstPointer;

    itkTypeMacro( JointHistogramMutualInformationImageToImageObjectMetric::JointHistogramMutualInformationSparseGetValueAndDerivativeThreader,
      JointHistogramMutualInformationImageToImageObjectMetric::GetValueAndDerivativeThreader );

    itkNewMacro( Self );

    typedef typename Superclass::DomainType         DomainType;
    typedef typename Superclass::EnclosingClassType EnclosingClassType;

  protected:
    JointHistogramMutualInformationSparseGetValueAndDerivativeThreader() {}

    virtual void ThreadedExecution( EnclosingClassType * enclosingClass,
                                    const DomainType& domain,
                                    const ThreadIdType threadId );

  private:
    JointHistogramMutualInformationSparseGetValueAndDerivativeThreader( const Self & ); // purposely not implemented
    void operator=( const Self & ); // purposely not implemented
  };

  void PrintSelf(std::ostream & os, Indent indent) const;

private:

  //purposely not implemented
  JointHistogramMutualInformationImageToImageObjectMetric(const Self &);
  //purposely not implemented
  void operator=(const Self &);

  /** The fixed image marginal PDF */
  typename MarginalPDFType::Pointer m_FixedImageMarginalPDF;

  /** The moving image marginal PDF. */
  typename MarginalPDFType::Pointer m_MovingImageMarginalPDF;

  /** The joint PDF and PDF derivatives. */
  mutable typename JointPDFType::Pointer            m_JointPDF;
  JointPDFInterpolatorPointer                       m_JointPDFInterpolator;

  /** Flag to control smoothing of joint pdf */
  InternalComputationValueType        m_VarianceForJointPDFSmoothing;

  /** Joint PDF types */
  typedef typename JointPDFType::PixelType             JointPDFValueType;
  typedef typename JointPDFType::RegionType            JointPDFRegionType;
  typedef typename JointPDFType::SizeType              JointPDFSizeType;
  typedef typename JointPDFType::SpacingType           JointPDFSpacingType;

  /** Variables to define the marginal and joint histograms. */
  SizeValueType                       m_NumberOfHistogramBins;
  InternalComputationValueType        m_FixedImageTrueMin;
  InternalComputationValueType        m_FixedImageTrueMax;
  InternalComputationValueType        m_MovingImageTrueMin;
  InternalComputationValueType        m_MovingImageTrueMax;
  InternalComputationValueType        m_FixedImageBinSize;
  InternalComputationValueType        m_MovingImageBinSize;

  InternalComputationValueType        m_JointPDFSum;
  JointPDFSpacingType                 m_JointPDFSpacing;

  InternalComputationValueType        m_Log2;
  JointPDFIndexValueType              m_Padding;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkJointHistogramMutualInformationImageToImageObjectMetric.hxx"
#endif

#endif
