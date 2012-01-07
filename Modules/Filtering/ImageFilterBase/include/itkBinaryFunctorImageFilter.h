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
#ifndef __itkBinaryFunctorImageFilter_h
#define __itkBinaryFunctorImageFilter_h

#include "itkBinaryFunctorImageFilterBase.h"
#include "itkSimpleDataObjectDecorator.h"

namespace itk
{
/** \class BinaryFunctorImageFilter
 * \brief Implements pixel-wise generic operation of two images,
 * or of an image and a constant.
 *
 * This filter provides the functor with pixel values from the input
 * images.
 *
 * \sa BinaryFunctorWithIndexImageFilter
 * \sa UnaryFunctorImageFilter
 * \sa TernaryFunctorImageFilter
 * \sa NaryFunctorImageFilter
 *
 * \ingroup IntensityImageFilters   MultiThreaded
 * \ingroup ITKImageFilterBase
 *
 * \wiki
 * \wikiexample{ImageProcessing/BinaryFunctorImageFilter,Apply a predefined operation to corresponding pixels in two images}
 * \wikiexample{ImageProcessing/BinaryFunctorImageFilterCustom,Apply a custom operation to corresponding pixels in two images}
 * \endwiki
 */
template< class TInputImage1, class TInputImage2,
          class TOutputImage, class TFunction    >
class ITK_EXPORT BinaryFunctorImageFilter :
    public BinaryFunctorImageFilterBase< TInputImage1, TInputImage2, TOutputImage, TFunction >
{
public:
  /** Standard class typedefs. */
  typedef BinaryFunctorImageFilter                              Self;
  typedef BinaryFunctorImageFilter< TInputImage1, TInputImage2,
                                    TOutputImage, TFunction >   Superclass;
  typedef SmartPointer< Self >                                  Pointer;
  typedef SmartPointer< const Self >                            ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(BinaryFunctorImageFilter, BinaryFunctorWithIndexImageFilter);

  /** Some convenient typedefs. */
  typedef typename Superclass::FunctorType                   FunctorType;
  typedef typename Superclass::Input1ImageType               Input1ImageType;
  typedef typename Superclass::Input1ImagePointer            Input1ImagePointer;
  typedef typename Superclass::Input1ImageRegionType         Input1ImageRegionType;
  typedef typename Superclass::Input1ImageIndexType          Input1ImageIndexType;
  typedef typename Superclass::Input1ImagePixelType          Input1ImagePixelType;
  typedef typename Superclass::DecoratedInput1ImagePixelType DecoratedInput1ImagePixelType;

  typedef typename Superclass::Input2ImageType               Input2ImageType;
  typedef typename Superclass::Input2ImagePointer            Input2ImagePointer;
  typedef typename Superclass::Input2ImageRegionType         Input2ImageRegionType;
  typedef typename Superclass::Input2ImageIndexType          Input2ImageIndexType;
  typedef typename Superclass::Input2ImagePixelType          Input2ImagePixelType;
  typedef typename Superclass::DecoratedInput2ImagePixelType DecoratedInput2ImagePixelType;

  typedef typename Superclass::OutputImageType               OutputImageType;
  typedef typename Superclass::OutputImagePointer            OutputImagePointer;
  typedef typename Superclass::OutputImageRegionType         OutputImageRegionType;
  typedef typename Superclass::OutputImageIndexType          OutputImageIndexType;
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
  BinaryFunctorImageFilter();
  virtual ~BinaryFunctorImageFilter() {}

  virtual OutputImagePixelType InvokeFunctor(const OutputImageIndexType & index,
                                             const Input1ImagePixelType & value1,
                                             const Input2ImagePixelType & value2);

private:
  BinaryFunctorImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);           //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBinaryFunctorImageFilter.hxx"
#endif

#endif
