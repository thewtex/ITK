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

#ifndef __itkKittlerIllingworthThresholdImageFilter_h
#define __itkKittlerIllingworthThresholdImageFilter_h

#include "itkHistogramThresholdImageFilter.h"
#include "itkKittlerIllingworthThresholdCalculator.h"

namespace itk {

/** \class KittlerIllingworthThresholdImageFilter
 * \brief Threshold an image using the KittlerIllingworth Threshold
 *
 * This filter creates a binary thresholded image that separates an
 * image into foreground and background components. The filter
 * computes the threshold using the KittlerIllingworthThresholdCalculator and
 * applies that theshold to the input image using the
 * BinaryThresholdImageFilter.
 *
 * \author Richard Beare
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/10380/3279  or
 * http://www.insight-journal.org/browse/publication/811
 *
 * \ingroup Multithreaded
 * \ingroup ITKThresholding
 */

template<class TInputImage, class TOutputImage>
class ITK_EXPORT KittlerIllingworthThresholdImageFilter :
    public HistogramThresholdImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef KittlerIllingworthThresholdImageFilter                      Self;
  typedef HistogramThresholdImageFilter<TInputImage,TOutputImage>     Superclass;
  typedef SmartPointer<Self>                                          Pointer;
  typedef SmartPointer<const Self>                                    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(KittlerIllingworthThresholdImageFilter, HistogramThresholdImageFilter);

  typedef TInputImage                       InputImageType;
  typedef TOutputImage                      OutputImageType;

  /** Image pixel value typedef. */
  typedef typename InputImageType::PixelType   InputPixelType;
  typedef typename OutputImageType::PixelType  OutputPixelType;

  /** Image related typedefs. */
  typedef typename InputImageType::Pointer  InputImagePointer;
  typedef typename OutputImageType::Pointer OutputImagePointer;

  typedef typename InputImageType::SizeType    InputSizeType;
  typedef typename InputImageType::IndexType   InputIndexType;
  typedef typename InputImageType::RegionType  InputImageRegionType;
  typedef typename OutputImageType::SizeType   OutputSizeType;
  typedef typename OutputImageType::IndexType  OutputIndexType;
  typedef typename OutputImageType::RegionType OutputImageRegionType;

  typedef typename Superclass::HistogramType                                     HistogramType;
  typedef KittlerIllingworthThresholdCalculator< HistogramType, InputPixelType > CalculatorType;

  /** Image related typedefs. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      InputImageType::ImageDimension );
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      OutputImageType::ImageDimension );

protected:
  KittlerIllingworthThresholdImageFilter()
    {
    this->SetCalculator( CalculatorType::New() );
    }
  ~KittlerIllingworthThresholdImageFilter(){};

private:
  KittlerIllingworthThresholdImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
}; // end of class

} // end namespace itk

#endif
