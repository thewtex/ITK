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
#ifndef __itkBinaryFunctorWithIndexImageFilter_h
#define __itkBinaryFunctorWithIndexImageFilter_h

#include "itkBinaryFunctorImageFilter.h"
#include "itkSimpleDataObjectDecorator.h"

namespace itk
{
/** \class BinaryFunctorWithIndexImageFilter
 * \brief Implements pixel-wise generic operation on two images,
 * or of an image and a constant.
 *
 * This filter differs from the BinaryFunctorImageFilter in that it
 * passes the index of the pixel to the functor in addition to the
 * pixel values from the input images.
 *
 * This filter was adapted from the Insight Journal article
 * "Deconvolution: infrastructure and reference algorithms"
 * by Gaetan Lehmann, Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * \sa BinaryFunctorImageFilter
 * \sa UnaryFunctorWithIndexImageFilter
 * \sa TernaryFunctorWithIndexImageFilter
 * \sa NaryFunctorWithIndexImageFilter
 *
 * \ingroup IntensityImageFilters   MultiThreaded
 * \ingroup ITKImageFilterBase
 *
 */
template< class TInputImage1, class TInputImage2,
          class TOutputImage, class TFunction    >
class ITK_EXPORT BinaryFunctorWithIndexImageFilter:
    public BinaryFunctorImageFilterBase< TInputImage1, TInputImage2, TOutputImage, TFunction >
{
public:
  /** Standard class typedefs. */
  typedef BinaryFunctorWithIndexImageFilter                         Self;
  typedef BinaryFunctorImageFilterBase< TInputImage1, TInputImage2,
                                        TOutputImage, TFunction >   Superclass;
  typedef SmartPointer< Self >                                      Pointer;
  typedef SmartPointer< const Self >                                ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(BinaryFunctorWithIndexImageFilter, BinaryFunctorImageFilterBase);

  /** Some convenient typedefs. */
  typedef typename Superclass::FunctorType                   FunctorType;
  typedef typename Superclass::Input1ImageType               Input1ImageType;
  typedef typename Superclass::Input1ImagePointer            Input1ImagePointer;
  typedef typename Superclass::Input1ImageRegionType         Input1ImageRegionType;
  typedef typename Superclass::Input1ImagePixelType          Input1ImagePixelType;
  typedef typename Superclass::DecoratedInput1ImagePixelType DecoratedInput1ImagePixelType;

  typedef typename Superclass::Input2ImageType               Input2ImageType;
  typedef typename Superclass::Input2ImagePointer            Input2ImagePointer;
  typedef typename Superclass::Input2ImageRegionType         Input2ImageRegionType;
  typedef typename Superclass::Input2ImagePixelType          Input2ImagePixelType;
  typedef typename Superclass::DecoratedInput2ImagePixelType DecoratedInput2ImagePixelType;

  typedef typename Superclass::OutputImageType               OutputImageType;
  typedef typename Superclass::OutputImagePointer            OutputImagePointer;
  typedef typename Superclass::OutputImageRegionType         OutputImageRegionType;
  typedef typename Superclass::OutputImagePixelType          OutputImagePixelType;

  /** ImageDimension constants */
  itkStaticConstMacro(
    InputImage1Dimension, unsigned int, TInputImage1::ImageDimension);
  itkStaticConstMacro(
    InputImage2Dimension, unsigned int, TInputImage2::ImageDimension);
  itkStaticConstMacro(
    OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( SameDimensionCheck1,
                   ( Concept::SameDimension< itkGetStaticConstMacro(InputImage1Dimension),
                                             itkGetStaticConstMacro(InputImage2Dimension) > ) );
  itkConceptMacro( SameDimensionCheck2,
                   ( Concept::SameDimension< itkGetStaticConstMacro(InputImage1Dimension),
                                             itkGetStaticConstMacro(OutputImageDimension) > ) );
  /** End concept checking */
#endif
protected:
  BinaryFunctorWithIndexImageFilter();
  virtual ~BinaryFunctorWithIndexImageFilter() {}

  /** BinaryFunctorWithIndexImageFilter can be implemented as a multithreaded filter.
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
  BinaryFunctorWithIndexImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);           //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBinaryFunctorWithIndexImageFilter.hxx"
#endif

#endif
