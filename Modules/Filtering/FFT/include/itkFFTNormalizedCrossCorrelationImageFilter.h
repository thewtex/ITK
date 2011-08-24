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
#ifndef __itkFFTNormalizedCrossCorrelationImageFilter_h
#define __itkFFTNormalizedCrossCorrelationImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"

namespace itk
{
/**
 * \class FFTNormalizedCrossCorrelationImageFilter
 * \brief Base class to calculate Normalized Cross Correlation using FFTs.
 *
 * This is a base class for calculating the Normalized Cross Correlation using FFTs.
 * This is an abstract base class: the actual implementation is
 * provided by the best child class available on the system when the object is
 * created via the object factory system.
 *
 * This filter calculates the Normalized Cross Correlation (NCC) using FFTs
 * instead of spatial correlation.  It is therefore much faster than spatial
 * correlation for larger structuring elements.
 *
 * Two input images are required as inputs: the fixedImage and movingImage.
 * fixedImage and movingImage need not be the same size.
 * The output is an image of RealPixelType that is the NCC of the two
 * images and its values range from -1.0 to 1.0.
 * The size of this NCC image is, by definition,
 * size(fixedImage) + size(movingImage) - 1.
 *
 * References: D. Padfield. "Masked FFT registration". In Proc. Computer
 * Vision and Pattern Recognition, 2010.
 *
 * Author: Dirk Padfield, GE Global Research, padfield@research.ge.com

 */
template <class TInputImage, class TOutputImage >
class ITK_EXPORT FFTNormalizedCrossCorrelationImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef FFTNormalizedCrossCorrelationImageFilter          Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage >   Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(FFTNormalizedCrossCorrelationImageFilter, ImageToImageFilter);

  /** Image type information. */
  typedef TInputImage                           InputImageType;
  typedef TOutputImage                          OutputImageType;
  typedef typename OutputImageType::Pointer     OutputImagePointer;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;

  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  typedef typename TOutputImage::PixelType          OutputPixelType;
  typedef typename TOutputImage::InternalPixelType  OutputInternalPixelType;
  typedef typename TInputImage::PixelType           InputPixelType;
  typedef typename TInputImage::InternalPixelType   InputInternalPixelType;
  typedef typename TInputImage::SizeType            SizeType;

  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Customized object creation methods that support configuration-based
    * selection of FFT implementation.
    *
    * Default implementation is VnlFFT. */
  static Pointer New(void);

  /** Set the fixed image */
  void SetFixedImageInput(InputImageType *input)
    {
      // Process object is not const-correct so the const casting is required.
      this->SetNthInput(0, const_cast<InputImageType *>(input) );
    }

  /** Get the fixed image */
  InputImageType * GetFixedImageInput()
    {
      return static_cast<InputImageType*>(const_cast<DataObject *>(this->ProcessObject::GetInput(0)));
    }

  /** Set the moving image */
  void SetMovingImageInput(InputImageType *input)
    {
      // Process object is not const-correct so the const casting is required.
      this->SetNthInput(1, const_cast<InputImageType *>(input) );
    }

  /** Get the moving image */
  InputImageType * GetMovingImageInput()
    {
      return static_cast<InputImageType*>(const_cast<DataObject *>(this->ProcessObject::GetInput(1)));
    }

  /** Get the numberOfOverlapPixels image */
  OutputImageType * GetNumberOfOverlapPixels( void )
    {
      return this->m_NumberOfOverlapPixels;
    }

protected:
  FFTNormalizedCrossCorrelationImageFilter() {}
  virtual ~FFTNormalizedCrossCorrelationImageFilter() {}

  /** This filter needs a different input requested region than the output
   * requested region.  As such, it needs to provide an
   * implementation for GenerateInputRequestedRegion() in order to inform the
   * pipeline execution model.
   * \sa ProcessObject::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion();

  /** Since the output of this filter is a different
   * size than the input, it must provide an implementation of
   * GenerateOutputInformation.
   * \sa ProcessObject::GenerateOutputRequestedRegion() */
  void GenerateOutputInformation();

private:
  FFTNormalizedCrossCorrelationImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#ifndef __itkVnlFFTNormalizedCrossCorrelationImageFilter_h
#ifndef __itkVnlFFTNormalizedCrossCorrelationImageFilter_hxx
#include "itkFFTNormalizedCrossCorrelationImageFilter.hxx"
#endif
#endif
#endif

#endif
