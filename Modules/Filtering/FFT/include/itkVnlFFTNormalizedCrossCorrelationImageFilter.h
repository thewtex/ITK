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
#ifndef __itkVnlFFTNormalizedCrossCorrelationImageFilter_h
#define __itkVnlFFTNormalizedCrossCorrelationImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"

namespace itk
{
/**
 * \class VnlFFTNormalizedCrossCorrelationImageFilter
 * \brief VNL implementation to calculate Normalized Cross Correlation using FFTs.
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
class ITK_EXPORT VnlFFTNormalizedCrossCorrelationImageFilter :
    public FFTNormalizedCrossCorrelationImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef VnlFFTNormalizedCrossCorrelationImageFilter                           Self;
  typedef FFTNormalizedCrossCorrelationImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer<Self>                                                    Pointer;
  typedef SmartPointer<const Self>                                              ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VnlFFTNormalizedCrossCorrelationImageFilter, FFTNormalizedCrossCorrelationImageFilter);

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

  /** Typedef of double containers */
  typedef FixedArray<double, itkGetStaticConstMacro(ImageDimension)> ArrayType;

  typedef double RealPixelType;

  /** Vector type */
  typedef vnl_vector< RealPixelType >           VectorType;
  typedef vnl_vector< RealPixelType >::iterator VectorIteratorType;

  /** Matrix type */
  typedef vnl_matrix< RealPixelType >           MatrixType;
  typedef vnl_matrix< RealPixelType >::iterator MatrixIteratorType;

  /** Complex matrix type */
  typedef vcl_complex< RealPixelType >                          ComplexType;
  typedef vnl_matrix< vcl_complex< RealPixelType > >            ComplexMatrixType;
  typedef vnl_matrix< vcl_complex< RealPixelType > >::iterator  ComplexMatrixIteratorType;

protected:
  VnlFFTNormalizedCrossCorrelationImageFilter()
  {
    this->SetNumberOfRequiredInputs(2);
    m_NumberOfOverlapPixels = NULL;
  }
  virtual ~VnlFFTNormalizedCrossCorrelationImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Standard pipeline method.*/
  void GenerateData();

private:
  VnlFFTNormalizedCrossCorrelationImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // Member variables.
  typename OutputImageType::Pointer m_NumberOfOverlapPixels;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVnlFFTNormalizedCrossCorrelationImageFilter.hxx"
#endif

#endif
