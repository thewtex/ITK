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
#ifndef __itkNaryFunctorWithIndexImageFilter_h
#define __itkNaryFunctorWithIndexImageFilter_h

#include "itkNaryFunctorImageFilterBase.h"
#include "itkImageIterator.h"
#include "itkArray.h"

namespace itk
{
/** \class NaryFunctorWithIndexImageFilter
 * \brief Perform a generic pixel-wise operation on N images.
 *
 * This filter differs from the TernaryFunctorImageFilter in that it
 * passes the index of the pixel to the functor in addition to the
 * pixel values.
 *
 * \sa NaryFunctorImageFilter
 * \sa UnaryFunctorWithIndexImageFilter
 * \sa BinaryFunctorWithIndexImageFilter
 * \sa TernaryFunctorWithIndexImageFilter
 *
 * \ingroup IntensityImageFilters MultiThreaded
 * \ingroup ITKImageIntensity
 */

template< class TInputImage, class TOutputImage, class TFunction >
class ITK_EXPORT NaryFunctorWithIndexImageFilter :
    public NaryFunctorImageFilterBase< TInputImage, TOutputImage, TFunction >

{
public:
  /** Standard class typedefs. */
  typedef NaryFunctorWithIndexImageFilter                                    Self;
  typedef NaryFunctorImageFilterBase< TInputImage, TOutputImage, TFunction > Superclass;
  typedef SmartPointer< Self >                                               Pointer;
  typedef SmartPointer< const Self >                                         ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NaryFunctorWithIndexImageFilter, NaryFunctorImageFilterBase);

  /** Some typedefs. */
  typedef typename Superclass::FunctorType           FunctorType;
  typedef typename Superclass::InputImageType        InputImageType;
  typedef typename Superclass::InputImagePointer     InputImagePointer;
  typedef typename Superclass::InputImageRegionType  InputImageRegionType;
  typedef typename Superclass::InputImagePixelType   InputImagePixelType;
  typedef typename Superclass::OutputImageType       OutputImageType;
  typedef typename Superclass::OutputImagePointer    OutputImagePointer;
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;
  typedef typename Superclass::OutputImagePixelType  OutputImagePixelType;
  typedef typename Superclass::NaryArrayType         NaryArrayType;

  /** ImageDimension constants */
  itkStaticConstMacro(
    InputImageDimension, unsigned int, TInputImage::ImageDimension);
  itkStaticConstMacro(
    OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( SameDimensionCheck,
                   ( Concept::SameDimension< InputImageDimension, OutputImageDimension > ) );
  itkConceptMacro( OutputHasZeroCheck,
                   ( Concept::HasZero< OutputImagePixelType > ) );
  /** End concept checking */
#endif
protected:
  NaryFunctorWithIndexImageFilter();
  virtual ~NaryFunctorWithIndexImageFilter() {}

  /** NaryFunctorWithIndexImageFilter can be implemented as a multithreaded filter.
   * Therefore, this implementation provides a ThreadedGenerateData() routine
   * which is called for each processing thread. The output image data is
   * allocated automatically by the superclass prior to calling
   * ThreadedGenerateData().  ThreadedGenerateData can only write to the
   * portion of the output image specified by the parameter
   * "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData()  */
  void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                            ThreadIdType threadId);

private:
  NaryFunctorWithIndexImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);         //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNaryFunctorWithIndexImageFilter.hxx"
#endif

#endif
