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
#ifndef __itkInverseDisplacementFieldImageFilter_h
#define __itkInverseDisplacementFieldImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkKernelTransform.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{
/** \class InverseDisplacementFieldImageFilter
 * \brief Computes the inverse of a displacement field.
 *
 * InverseDisplacementFieldImageFilter takes a displacement field as input and
 * computes the displacement field that is its inverse. If the input displacement
 * field was mapping coordinates from a space A into a space B, the output of
 * this filter will map coordinates from the space B into the space A.
 *
 * Given that both the input and output displacement field are represented as
 * discrete images with pixel type vector, the inverse will be only an
 * estimation and will probably not correspond to a perfect inverse.  The
 * precision of the inverse can be improved at the price of increasing the
 * computation time and memory consumption in this filter.
 *
 * The method used for computing the inverse displacement field is to subsample
 * the input field using a regular grid and create Kerned-Base Spline in which
 * the reference landmarks are the coordinates of the deformed point and the
 * target landmarks are the negative of the displacement vectors. The
 * kernel-base spline is then used for regularly sampling the output space and
 * recover vector values for every single pixel.
 *
 * The subsampling factor used for the regular grid of the input field will
 * determine the number of landmarks in the KernelBased spline and therefore it
 * will have a dramatic effect on both the precision of output displacement
 * field and the computational time required for the filter to complete the
 * estimation. A large subsampling factor will result in few landmarks in the
 * KernelBased spline, therefore on fast computation and low precision.  A
 * small subsampling factor will result in a large number of landmarks in the
 * KernelBased spline, therefore a large memory consumption, long computation
 * time and high precision for the inverse estimation.
 *
 * This filter expects both the input and output images to be of pixel type
 * Vector.
 *
 * \ingroup ImageToImageFilter
 * \ingroup ITKDisplacementField
 */
template< class TInputImage, class TOutputImage >
class ITK_EXPORT InverseDisplacementFieldImageFilter:
  public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef InverseDisplacementFieldImageFilter             Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  typedef TInputImage                           InputImageType;
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename InputImageType::RegionType   InputImageRegionType;
  typedef TOutputImage                          OutputImageType;
  typedef typename OutputImageType::Pointer     OutputImagePointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(InverseDisplacementFieldImageFilter, ImageToImageFilter);

  /** Number of dimensions. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** implement type-specific clone method*/
  itkTransformCloneMacro();

  /** Transform typedef.
   *
   * \todo Check that input and output images have the same number of
     * dimensions; this is required for consistency.  */
  typedef KernelTransform<
    double, itkGetStaticConstMacro(ImageDimension) > KernelTransformType;
  typedef typename KernelTransformType::Pointer KernelTransformPointerType;

  /** Image size typedef. */
  typedef typename OutputImageType::SizeType SizeType;

  /** Image index typedef. */
  typedef typename OutputImageType::IndexType IndexType;

  /** Image pixel value typedef. */
  typedef typename TOutputImage::PixelType    OutputPixelType;
  typedef typename OutputPixelType::ValueType OutputPixelComponentType;

  /** Typedef to describe the output image region type. */
  typedef typename TOutputImage::RegionType OutputImageRegionType;

  /** Image spacing typedef */
  typedef typename TOutputImage::SpacingType SpacingType;
  typedef typename TOutputImage::PointType   OriginPointType;

  /** Set the coordinate transformation.
   * Set the KernelBase spline used for resampling the displacement grid.
   * */
  itkSetObjectMacro(KernelTransform, KernelTransformType);

  /** Get a pointer to the coordinate transform. */
  itkGetObjectMacro(KernelTransform, KernelTransformType);

  /** Set the size of the output image. */
  itkSetMacro(Size, SizeType);

  /** Get the size of the output image. */
  itkGetConstReferenceMacro(Size, SizeType);

  /** Set the output image spacing. */
  itkSetMacro(OutputSpacing, SpacingType);
  virtual void SetOutputSpacing(const double *values);

  /** Get the output image spacing. */
  itkGetConstReferenceMacro(OutputSpacing, SpacingType);

  /** Set the output image origin. */
  itkSetMacro(OutputOrigin, OriginPointType);
  virtual void SetOutputOrigin(const double *values);

  /** Get the output image origin. */
  itkGetConstReferenceMacro(OutputOrigin, OriginPointType);

  /** Set/Get the factor used for subsampling the input displacement field.  A
   * large value in this factor will produce a fast computation of the inverse
   * field but with low precision. A small value of this factor will produce a
   * precise computation of the inverse field at the price of large memory
   * consumption and long computational time. */
  itkSetMacro(SubsamplingFactor, unsigned int);
  itkGetConstMacro(SubsamplingFactor, unsigned int);

  /** InverseDisplacementFieldImageFilter produces an image which is a different size
   * than its input.  As such, it needs to provide an implementation
   * for GenerateOutputInformation() in order to inform the pipeline
   * execution model.  The original documentation of this method is
   * below. \sa ProcessObject::GenerateOutputInformaton() */
  virtual void GenerateOutputInformation();

  /** InverseDisplacementFieldImageFilter needs a different input requested region than
   * the output requested region.  As such, InverseDisplacementFieldImageFilter needs
   * to provide an implementation for GenerateInputRequestedRegion()
   * in order to inform the pipeline execution model.
   * \sa ProcessObject::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion();

  /** Method Compute the Modified Time based on changed to the components. */
  unsigned long GetMTime(void) const;

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( OutputHasNumericTraitsCheck,
                   ( Concept::HasNumericTraits< OutputPixelComponentType > ) );
  /** End concept checking */
#endif
protected:
  InverseDisplacementFieldImageFilter();
  ~InverseDisplacementFieldImageFilter() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

  /**
   * GenerateData() computes the internal KernelBase spline and resamples
   * the displacement field.
   */
  void GenerateData();

  /** Subsample the input displacement field and generate the
   *  landmarks for the kernel base spline
   */
  void PrepareKernelBaseSpline();

private:
  InverseDisplacementFieldImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);                     //purposely not implemented

  SizeType                   m_Size;                 // Size of the output image
  KernelTransformPointerType m_KernelTransform;      // Coordinate transform to
                                                     // use
  SpacingType     m_OutputSpacing;                   // output image spacing
  OriginPointType m_OutputOrigin;                    // output image origin

  unsigned int m_SubsamplingFactor;                  // factor to subsample the
                                                     // input field.
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkInverseDisplacementFieldImageFilter.hxx"
#endif

#endif
