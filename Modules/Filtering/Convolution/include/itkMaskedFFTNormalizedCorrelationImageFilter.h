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
#ifndef __itkMaskedFFTNormalizedCorrelationImageFilter_h
#define __itkMaskedFFTNormalizedCorrelationImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"

namespace itk
{
/**
 * \class MaskedFFTNormalizedCorrelationImageFilter
 * \brief Calculate masked normalized cross correlation using FFTs.
 *
 * This filter calculates the normalized cross correlation (NCC) of two
 * images under masks defined by binary images.
 * It calculates the masked NCC using FFTs instead of spatial
 * correlation.  It is therefore much faster than spatial correlation
 * for larger structuring elements.
 *
 * Four input images are required as inputs: fixedImage, movingImage,
 * fixedMask and movingMask.
 * The fixedMask and movingMask should consist of only 1s and 0s, where 1
 * indicates locations of useful information in the corresponding image,
 * and 0 indicates locations that should be masked (ignored).
 * fixedImage and movingImage need not be the same size, but fixedMask
 * must be the same size as fixedImage, and movingMask must be the same
 * size as movingImage.
 * The output is an image of RealPixelType that is the NCC of the two
 * images and its values range from -1.0 to 1.0.
 * The size of this NCC image is, by definition,
 * size(fixedImage) + size(movingImage) - 1.
 *
 * Example filter usage:
 * \code
 * typedef itk::MaskedFFTNormalizedCorrelationImageFilter< ShortImageType, DoubleImageType > FilterType;
 * FilterType::Pointer filter = FilterType::New();
 * filter->SetFixedImageInput( fixedImage );
 * filter->SetMovingImageInput( movingImage );
 * filter->SetFixedMaskInput( fixedMask );
 * filter->SetMovingMaskInput( movingMask );
 * filter->Update();
 * \endcode
 *
 * \warning The pixel type of the output image must be of real type
 * (float or double). ConceptChecking is used to enforce the output pixel
 * type. You will get a compilation error if the pixel type of the
 * output image is not float or double.
 *
 * References: D. Padfield. "Masked FFT registration". In Proc. Computer
 * Vision and Pattern Recognition, 2010.
 *
 * \author: Dirk Padfield, GE Global Research, padfield@research.ge.com
 * \ingroup ITKConvolution
 */

template <class TInputImage, class TOutputImage >
class ITK_EXPORT MaskedFFTNormalizedCorrelationImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef MaskedFFTNormalizedCorrelationImageFilter                     Self;
  typedef ImageToImageFilter < TInputImage, TOutputImage >              Superclass;
  typedef SmartPointer<Self>                                            Pointer;
  typedef SmartPointer<const Self>                                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MaskedFFTNormalizedCorrelationImageFilter, MaskedFFTNormalizedCrossCorrelationImageFilter);

  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Extract some information from the image types. */
  typedef TInputImage                               InputImageType;
  typedef TOutputImage                              OutputImageType;
  typedef typename OutputImageType::Pointer         OutputImagePointer;
  typedef typename InputImageType::ConstPointer     InputImageConstPointer;
  typedef typename TOutputImage::PixelType          OutputPixelType;

  typedef double                                    RealPixelType;
  typedef Image< RealPixelType, ImageDimension>     RealImageType;
  typedef typename RealImageType::Pointer           RealImagePointer;
  typedef typename RealImageType::IndexType         RealIndexType;
  typedef typename RealImageType::SizeType          RealSizeType;
  typedef typename RealImageType::RegionType        RealRegionType;

  typedef Image< unsigned long, ImageDimension >    ULongImageType;
  typedef typename ULongImageType::Pointer          ULongImagePointer;

  typedef Image< std::complex<RealPixelType>, ImageDimension >  FFTImageType;
  typedef typename FFTImageType::Pointer                        FFTImagePointer;

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

  /** Set the fixed mask */
  void SetFixedMaskInput(InputImageType *input)
    {
      // Process object is not const-correct so the const casting is required.
      this->SetNthInput(2, const_cast<InputImageType *>(input) );
    }
  /** Get the fixed mask */
  InputImageType * GetFixedMaskInput()
    {
      return static_cast<InputImageType*>(const_cast<DataObject *>(this->ProcessObject::GetInput(2)));
    }

  /** Set the moving mask */
  void SetMovingMaskInput(InputImageType *input)
    {
      // Process object is not const-correct so the const casting is required.
      this->SetNthInput(3, const_cast<InputImageType *>(input) );
    }
  /** Get the moving mask */
  InputImageType * GetMovingMaskInput()
    {
      return static_cast<InputImageType*>(const_cast<DataObject *>(this->ProcessObject::GetInput(3)));
    }

  /** Get the numberOfOverlapPixels image */
  ULongImageType * GetNumberOfOverlapPixels( void )
    {
      return this->m_NumberOfOverlapPixels;
    }


#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( OutputPixelTypeIsFloatingPointCheck,
                   ( Concept::IsFloatingPoint< OutputPixelType > ) );
  /** End concept checking */
#endif


protected:
  MaskedFFTNormalizedCorrelationImageFilter()
  {
    this->SetNumberOfRequiredInputs(4);
    /*
    ULongImageType::SizeType outputSize;
    for( unsigned int i = 0; i < ImageDimension; ++i )
    {
      outputSize[i] = this->GetFixedImage()->GetLargestPossibleRegion().GetSize()[i] + this->GetMovingImage()->GetLargestPossibleRegion().GetSize()[i] - 1;
    }
    m_NumberOfOverlapPixels =
    */
    m_NumberOfOverlapPixels = NULL;
  }
  virtual ~MaskedFFTNormalizedCorrelationImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Standard pipeline method.*/
  void GenerateData();

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

  template< class GenericImageType >
  void PreprocessImages( GenericImageType * inputImage, GenericImageType * inputMask);

  typename Image< std::complex<double>, TOutputImage::ImageDimension >::Pointer CalculateForwardFFT( RealImageType * inputImage, RealSizeType & FFTImageSize );

  typename Image< double, TOutputImage::ImageDimension>::Pointer CalculateInverseFFT( FFTImageType * inputImage, RealSizeType & combinedImageSize );

  // Helper math methods.
  template< class GenericImageType >
  typename GenericImageType::Pointer ElementProduct( GenericImageType * inputImage1, GenericImageType * inputImage2 );

  template< class GenericImageType >
  typename GenericImageType::Pointer ElementQuotient( GenericImageType * inputImage1, GenericImageType * inputImage2 );

  template< class GenericImageType >
  typename GenericImageType::Pointer ElementSubtraction( GenericImageType * inputImage1, GenericImageType * inputImage2 );

  template< class GenericImageType >
  void ElementPositive( GenericImageType * inputImage );

  template< class GenericImageType >
  void ElementRound( GenericImageType * inputImage );

  template< class GenericImageType >
  void ElementSqrt( GenericImageType * inputImage );

  // This function factorizes the image size uses factors of 2, 3, and
  // 5.  After this factorization, if there are any remaining values,
  // the function returns this value.
  int FactorizeNumber( int n );

  // Find the closest valid dimension above the desired dimension.  This
  // will be a combination of 2s, 3s, and 5s.
  int FindClosestValidDimension( int n );

private:
  MaskedFFTNormalizedCorrelationImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // Member variables.
  ULongImagePointer m_NumberOfOverlapPixels;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMaskedFFTNormalizedCorrelationImageFilter.hxx"
#endif

#endif
