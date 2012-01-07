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
#ifndef __itkUnaryFunctorWithIndexImageFilter_h
#define __itkUnaryFunctorWithIndexImageFilter_h

#include "itkUnaryFunctorImageFilterBase.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace itk
{
/** \class UnaryFunctorWithIndexImageFilter
 * \brief Implements a pixel-wise generic operation on one image.
 *
 * This filter differs from the UnaryFunctorImageFilter in that it
 * passes the index of the pixel to the functor in addition to the
 * pixel values from the input image.
 *
 * \sa UnaryFunctorImageFilter
 * \sa BinaryFunctorWithIndexImageFilter
 * \sa TernaryFunctorWithIndexImageFilter
 * \sa NaryFunctorWithIndexImageFilter
 *
 * \ingroup   IntensityImageFilters     MultiThreaded
 * \ingroup ITKCommon
 *
 */
template< class TInputImage, class TOutputImage, class TFunction >
class ITK_EXPORT UnaryFunctorWithIndexImageFilter :
    public UnaryFunctorImageFilterBase< TInputImage, TOutputImage, TFunction >
{
public:
  /** Standard class typedefs. */
  typedef UnaryFunctorWithIndexImageFilter                                    Self;
  typedef UnaryFunctorImageFilterBase< TInputImage, TOutputImage, TFunction > Superclass;
  typedef SmartPointer< Self >                                                Pointer;
  typedef SmartPointer< const Self >                                          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(UnaryFunctorWithIndexImageFilter, UnaryFunctorImageFilterBase);

  /** Some typedefs. */
  typedef TFunction FunctorType;

  typedef TInputImage                                InputImageType;
  typedef typename Superclass::InputImagePointer     InputImagePointer;
  typedef typename Superclass::InputImageRegionType  InputImageRegionType;
  typedef typename Superclass::InputImageIndexType   InputImageIndexType;
  typedef typename Superclass::InputImagePixelType   InputImagePixelType;

  typedef TOutputImage                               OutputImageType;
  typedef typename Superclass::OutputImagePointer    OutputImagePointer;
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;
  typedef typename Superclass::OutputImageIndexType  OutputImageIndexType;
  typedef typename Superclass::OutputImagePixelType  OutputImagePixelType;

protected:
  UnaryFunctorWithIndexImageFilter();
  virtual ~UnaryFunctorWithIndexImageFilter() {}

  virtual OutputImagePixelType InvokeFunctor(const OutputImageIndexType & index,
                                             const InputImagePixelType & value);

private:
  UnaryFunctorWithIndexImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);          //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkUnaryFunctorWithIndexImageFilter.hxx"
#endif

#endif
