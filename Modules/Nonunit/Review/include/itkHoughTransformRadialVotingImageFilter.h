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

#ifndef __itkHoughTransformRadialVotingImageFilter_h
#define __itkHoughTransformRadialVotingImageFilter_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkEllipseSpatialObject.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkGaussianDerivativeImageFunction.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkCastImageFilter.h"
#include "itkGaussianDistribution.h"
#include "itkAddImageFilter.h"
#include "itkImageRegionIterator.h"

namespace itk
{

/**
 * \class HoughTransformRadialVotingImageFilter
 * \brief Performs the Hough Transform to find circles in a 2D image.
 *
 * This filter derives from the base class ImageToImageFilter
 * The input is an image, and all pixels above some threshold are those
 * we want to consider during the process.
 *
 * This filter produces two output:
 *   1) The accumulator array, which represents probability of centers.
 *   2) The array or radii, which has the radius value at each coordinate point.
 *
 *  When the filter found a "correct" point, it computes the gradient at this
 * point and votes on a small region defined using the minimum and maximum
 * radius given by the user, and fill in the array of radii.
 *
 * \ingroup ImageFeatureExtraction
 * \todo Update the doxygen documentation!!!
 * */
template< class TInputImage, class TOutputImage >
class ITK_EXPORT HoughTransformRadialVotingImageFilter :
    public ImageToImageFilter< TInputImage , TOutputImage >
{
public:

  /** Standard "Self" typedef. */
  typedef HoughTransformRadialVotingImageFilter             Self;
  /** Standard "Superclass" typedef. */
  typedef ImageToImageFilter< TInputImage, TOutputImage >   Superclass;
  /** Smart pointer typedef support. */
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  itkStaticConstMacro ( ImageDimension, unsigned int,
    TInputImage::ImageDimension );

  /** Input Image typedef */
  typedef TInputImage                             InputImageType;
  typedef typename InputImageType::Pointer        InputImagePointer;
  typedef typename InputImageType::ConstPointer   InputImageConstPointer;
  typedef typename InputImageType::IndexType      InputIndexType;
  typedef typename InputIndexType::IndexValueType InputIndexValueType;
  typedef typename InputImageType::PixelType      InputPixelType;
  typedef typename InputImageType::SizeType       InputSizeType;
  typedef typename InputSizeType::SizeValueType   InputSizeValueType;
  typedef typename InputImageType::RegionType     InputRegionType;
  typedef typename InputImageType::SpacingType    InputSpacingType;
  typedef typename InputImageType::PointType      InputPointType;
  typedef typename InputPointType::CoordRepType   InputCoordType;

  /** Output Image typedef */
  typedef TOutputImage                          OutputImageType;
  typedef typename OutputImageType::Pointer     OutputImagePointer;
  typedef typename OutputImageType::PixelType   OutputPixelType;
  typedef typename OutputImageType::RegionType  OutputImageRegionType;

  typedef Image< InputCoordType, ImageDimension >     InternalImageType;
  typedef typename InternalImageType::Pointer         InternalImagePointer;
  typedef typename InternalImageType::IndexType       InternalIndexType;
  typedef typename InternalIndexType::IndexValueType  InternalIndexValueType;
  typedef typename InternalImageType::PixelType       InternalPixelType;
  typedef typename InternalImageType::RegionType      InternalRegionType;
  typedef typename InternalImageType::SizeType        InternalSizeType;
  typedef typename InternalSizeType::SizeValueType    InternalSizeValueType;
  typedef typename InternalImageType::SpacingType     InternalSpacingType;

  /** Sphere typedef */
  typedef EllipseSpatialObject< ImageDimension > SphereType;
  typedef typename SphereType::Pointer           SpherePointer;
  typedef typename SphereType::VectorType        SphereVectorType;
  typedef std::list<SpherePointer>               SpheresListType;

  typedef ImageRegionIterator< InternalImageType > InternalIteratorType;
  typedef ImageRegionIterator< OutputImageType >   OutputIteratorType;

  typedef DiscreteGaussianImageFilter< OutputImageType, InternalImageType >
    GaussianFilterType;
  typedef typename GaussianFilterType::Pointer
    GaussianFilterPointer;

  typedef itk::Statistics::GaussianDistribution  GaussianFunctionType;
  typedef typename GaussianFunctionType::Pointer GaussianFunctionPointer;

  typedef GaussianDerivativeImageFunction< InputImageType, InputCoordType >
    DoGFunctionType;
  typedef typename DoGFunctionType::Pointer
    DoGFunctionPointer;
  typedef typename DoGFunctionType::VectorType
    DoGVectorType;

  typedef MinimumMaximumImageCalculator< InternalImageType >  MinMaxCalculatorType;
  typedef typename MinMaxCalculatorType::Pointer              MinMaxCalculatorPointer;

  typedef CastImageFilter< InternalImageType, OutputImageType > CastFilterType;
  typedef typename CastFilterType::Pointer                      CastFilterPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( HoughTransformRadialVotingImageFilter, ImageToImageFilter );

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Set both Minimum and Maximum radius values */
  void SetRadius(InputCoordType radius);

  /** Set the minimum radiu value the filter should look for */
  itkSetMacro( MinimumRadius, InputCoordType );

  /** Set the maximum radius value the filter should look for */
  itkSetMacro(MaximumRadius, InputCoordType );

  /** Set the threshold above which the filter should consider
      the point as a valid point */
  itkSetMacro( Threshold, double );

  /** Get the threshold value */
  itkGetConstMacro( Threshold, double );

  /** Set the threshold above which the filter should consider
      the point as a valid point */
  itkSetMacro( OutputThreshold, InternalPixelType );

  /** Get the threshold value */
  itkGetConstMacro( OutputThreshold, InternalPixelType );

  /** Set the threshold above which the filter should consider
      the point as a valid point */
  itkSetMacro(GradientThreshold, InputCoordType );

  /** Get the threshold value */
  itkGetConstMacro( GradientThreshold, InputCoordType );

  /** Get the radius image */
  itkGetObjectMacro( RadiusImage, InternalImageType );

  /** Get the accumulator image */
  itkGetObjectMacro( AccumulatorImage, InternalImageType );

  /** Set the scale of the derivative function (using DoG) */
  itkSetMacro( SigmaGradient, double );

  /** Get the scale value */
  itkGetConstMacro( SigmaGradient, double );

   /** Get the list of circles. This recomputes the circles */
  SpheresListType& GetSpheres( );

  /** Set/Get the number of circles to extract */
  itkSetMacro( NumberOfSpheres, unsigned int );
  itkGetConstMacro( NumberOfSpheres, unsigned int );

  /** Set/Get the radius of the disc to remove from the accumulator
   *  for each circle found */
  itkSetMacro( SphereRadiusRatio, InputCoordType );
  itkGetConstMacro( SphereRadiusRatio, InputCoordType );

  itkSetMacro( VotingRadiusRatio, InputCoordType );
  itkGetConstMacro( VotingRadiusRatio, InputCoordType );

  /** Set the variance of the gaussian bluring for the accumulator */
  itkSetMacro( Variance, double );
  itkGetConstMacro( Variance, double );

  /** Set the number of threads */
  itkSetMacro( NbOfThreads, unsigned int );
  itkGetConstMacro( NbOfThreads, unsigned int );

  /** Set the number of threads */
  itkSetMacro( SamplingRatio, double );
  itkGetConstMacro( SamplingRatio, double );

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(IntConvertibleToOutputCheck,
    (Concept::Convertible<int, OutputPixelType>));
  itkConceptMacro(InputGreaterThanDoubleCheck,
    (Concept::GreaterThanComparable<InputPixelType, double>));
  itkConceptMacro(OutputPlusIntCheck,
    (Concept::AdditiveOperators<OutputPixelType, int>));
  itkConceptMacro(OutputDividedByIntCheck,
    (Concept::DivisionOperators<OutputPixelType, int>));
  /** End concept checking */
#endif

protected:
  HoughTransformRadialVotingImageFilter();
  virtual ~HoughTransformRadialVotingImageFilter();

  InputCoordType        m_MinimumRadius;
  InputCoordType        m_MaximumRadius;
  double                m_Threshold;
  InputCoordType        m_GradientThreshold;
  InternalPixelType     m_OutputThreshold;
  double                m_SigmaGradient;
  double                m_Variance;
  InputCoordType        m_VotingRadiusRatio;
  InputCoordType        m_SphereRadiusRatio;
  double                m_SamplingRatio;

  InternalImagePointer  m_RadiusImage;
  InternalImagePointer  m_AccumulatorImage;
  SpheresListType       m_SpheresList;
  unsigned int          m_NumberOfSpheres;

  unsigned long         m_OldModifiedTime;

  unsigned int          m_NbOfThreads;
  bool                  m_AllSeedsProcessed;


  /** Method for evaluating the implicit function over the image. */
  virtual void BeforeThreadedGenerateData();
  virtual void AfterThreadedGenerateData();
  virtual void ThreadedGenerateData(const OutputImageRegionType& windowRegion,
                       int threadId);

  void PrintSelf(std::ostream& os, Indent indent) const;

  /** HoughTransformRadialVotingImageFilter needs the entire input. Therefore
   * it must provide an implementation GenerateInputRequestedRegion().
   * \sa ProcessObject::GenerateInputRequestedRegion(). */
  void GenerateInputRequestedRegion();

  /** HoughTransformRadialVotingImageFilter's produces all the output.
   * Therefore, it must provide an implementation of
   * EnlargeOutputRequestedRegion.
   * \sa ProcessObject::EnlargeOutputRequestedRegion() */
  void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output));

  void ComputeMeanRadiusImage( );

private:
  HoughTransformRadialVotingImageFilter(const Self&) {}
  void operator=(const Self&) {}
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHoughTransformRadialVotingImageFilter.txx"
#endif

#endif
