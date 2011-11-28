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
#ifndef __itkWienerDeconvolutionImageFilter_h
#define __itkWienerDeconvolutionImageFilter_h


#include "itkFFTConvolutionImageFilter.h"

namespace itk
{
/** \class WienerDeconvolutionImageFilter
 * \brief The Wiener deconvolution image filter attempts to restore an
 * image convolved with a blurring kernel while reducing noise enhancement.
 *
 * The Wiener filter aims to minimize noise enhancement induced by
 * frequencies with low signal-to-noise ratio. The filter
 * \f$G(\omega)\f$ is defined in the frequency domain as \f$G(\omega)
 * = H^*(\omega) / (|H(\omega)| + (1 / SNR(\omega)))\f$ where
 * \f$H(\omega)\f$ is the Fourier transform of the kernel with which
 * the original image was convolved and the signal-to-noise ratio
 * (\f$SNR(\omega)\f$) defined by \f$P_s(\omega) / P_n(\omega)\f$,
 * which is the power spectrum of the noise divided by the power
 * spectrum of the uncorrupted signal. These spectra are typically
 * unavailable. It is very common that the noise has a flat power
 * spectrum or a power spectrum that decreases significantly more
 * slowly than the power spectrum of a typical image. Hence, an image
 * will typically dominate at low frequencies while noise dominates at
 * high frequencies. Because of this, the filter specifies the
 * signal-to-noise ratio as a constant (SignalToNoiseConstant) which
 * is typically large (1000 to 10000).
 *
 * For further information on the Wiener filters, please see
 * "Digital Signal Processing" by Kenneth R. Castleman, Prentice Hall, 1995
 *
 * \ingroup ITKDeconvolution
 * \sa ConvolutionImageFilter
 *
 */
template< class TInputImage, class TKernelImage = TInputImage, class TOutputImage = TInputImage, class TInternalPrecision=double >
class ITK_EXPORT WienerDeconvolutionImageFilter :
  public FFTConvolutionImageFilter< TInputImage, TKernelImage, TOutputImage >
{
public:
  typedef WienerDeconvolutionImageFilter                         Self;
  typedef FFTConvolutionImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information ( and related methods ) */
  itkTypeMacro(WienerDeconvolutionImageFilter, FFTConvolutionImageFilter);

  /** Dimensionality of input and output data is assumed to be the same. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  typedef TInputImage                           InputImageType;
  typedef TOutputImage                          OutputImageType;
  typedef TKernelImage                          KernelImageType;
  typedef typename InputImageType::PixelType    InputPixelType;
  typedef typename OutputImageType::PixelType   OutputPixelType;
  typedef typename KernelImageType::PixelType   KernelPixelType;
  typedef typename InputImageType::IndexType    InputIndexType;
  typedef typename OutputImageType::IndexType   OutputIndexType;
  typedef typename KernelImageType::IndexType   KernelIndexType;
  typedef typename InputImageType::SizeType     InputSizeType;
  typedef typename OutputImageType::SizeType    OutputSizeType;
  typedef typename KernelImageType::SizeType    KernelSizeType;
  typedef typename InputSizeType::SizeValueType SizeValueType;
  typedef typename InputImageType::RegionType   InputRegionType;
  typedef typename OutputImageType::RegionType  OutputRegionType;
  typedef typename KernelImageType::RegionType  KernelRegionType;

  /** Internal types used by the FFT filters. */
  typedef Image< TInternalPrecision, ImageDimension >  InternalImageType;
  typedef typename InternalImageType::Pointer          InternalImagePointerType;
  typedef std::complex< TInternalPrecision >           InternalComplexType;
  typedef Image< InternalComplexType, ImageDimension > InternalComplexImageType;
  typedef typename InternalComplexImageType::Pointer   InternalComplexImagePointerType;

  /** The signal-to-noise ratio is implemented as a small constant
   * because the power spectrum of noise is typically constant and the
   * power spectrum of the uncorrupted image is unknown. */
  itkSetMacro(SignalToNoiseConstant, double);
  itkGetMacro(SignalToNoiseConstant, double);

protected:
  WienerDeconvolutionImageFilter();
  ~WienerDeconvolutionImageFilter() {}

  /** This filter uses a minipipeline to compute the output. */
  void GenerateData();

  virtual void PrintSelf(std::ostream & os, Indent indent) const;

private:
  WienerDeconvolutionImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);                 //purposely not implemented

  double m_SignalToNoiseConstant;
};

namespace Functor
{
template< class TInput1, class TInput2, class TOutput >
class WienerFunctor
{
public:
   WienerFunctor() {m_NoiseToSignalConstant = 0.0;};
   ~WienerFunctor() {};

   bool operator!=( const WienerFunctor & ) const
     {
     return false;
     }
   bool operator==( const WienerFunctor & other) const
     {
     return !(*this != other);
     }
   inline TOutput operator()(const TInput1 & I, const TInput2 & H) const
     {
     double normH = std::norm( H );
     return static_cast< TOutput >( I * ( std::conj( H ) / ( normH + m_NoiseToSignalConstant) ) );
     }

   double m_NoiseToSignalConstant;
};
} //namespace Functor

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWienerDeconvolutionImageFilter.hxx"
#endif

#endif
