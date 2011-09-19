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
#ifndef __itkVnlRealToComplexForwardFFTImageFilter_h
#define __itkVnlRealToComplexForwardFFTImageFilter_h

#include "itkRealToComplexForwardFFTImageFilter.h"
#include "itkVnlFFTCommon.h"
#include "vnl/algo/vnl_fft_base.h"

namespace itk
{
/** \class VnlRealToComplexForwardFFTImageFilter
 *
 * \brief VNL-based forward Fast Fourier Transform.
 *
 * The input image size in all dimensions must have a prime
 * factorization consisting of 2s, 3s, and 5s.
 *
 * \ingroup FourierTransform
 *
 * \sa RealToComplexForwardFFTImageFilter
 * \ingroup ITKFFT
 *
 */
template< class TInputImage, class TOutputImage=Image< std::complex<typename TInputImage::PixelType>, TInputImage::ImageDimension> >
class VnlRealToComplexForwardFFTImageFilter:
  public RealToComplexForwardFFTImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef TInputImage                            InputImageType;
  typedef typename InputImageType::PixelType     InputPixelType;
  typedef typename InputImageType::SizeType      InputSizeType;
  typedef typename InputImageType::SizeValueType InputSizeValueType;
  typedef TOutputImage                           OutputImageType;
  typedef typename OutputImageType::PixelType    OutputPixelType;
  typedef typename OutputImageType::SizeType     OutputSizeType;

  typedef VnlRealToComplexForwardFFTImageFilter                           Self;
  typedef RealToComplexForwardFFTImageFilter<  TInputImage, TOutputImage> Superclass;
  typedef SmartPointer< Self >                                            Pointer;
  typedef SmartPointer< const Self >                                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VnlRealToComplexForwardFFTImageFilter,
               RealToComplexForwardFFTImageFilter);

  /** Extract the dimensionality of the images. They are assumed to be
   * the same. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( ImageDimensionsMatchCheck,
                   ( Concept::SameDimension< InputImageDimension, OutputImageDimension > ) );
  /** End concept checking */
#endif

protected:
  VnlRealToComplexForwardFFTImageFilter() {}
  ~VnlRealToComplexForwardFFTImageFilter() {}

  void GenerateData();

private:
  VnlRealToComplexForwardFFTImageFilter(const Self &); // purposely not implemented
  void operator=(const Self &);           // purposely not implemented

  typedef vnl_vector< vcl_complex< InputPixelType > > SignalVectorType;
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVnlRealToComplexForwardFFTImageFilter.hxx"
#endif

#endif
