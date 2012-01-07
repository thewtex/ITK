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
#ifndef __itkTernaryFunctorImageFilter_h
#define __itkTernaryFunctorImageFilter_h

#include "itkTernaryFunctorImageFilterBase.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace itk
{
/** \class TernaryFunctorImageFilter
 * \brief Implements pixel-wise generic operations on three images.
 *
 * This filter provides the functor with pixel values from the input
 * images.
 *
 * \sa TernaryFunctorWithIndexImageFilter
 * \sa UnaryFunctorImageFilter
 * \sa BinaryFunctorImageFilter
 * \sa NaryFunctorImageFilter
 *
 * \ingroup IntensityImageFilters MultiThreaded
 * \ingroup ITKImageFilterBase
 */
template< class TInputImage1, class TInputImage2,
          class TInputImage3, class TOutputImage, class TFunction    >
class ITK_EXPORT TernaryFunctorImageFilter :
    public TernaryFunctorImageFilterBase< TInputImage1, TInputImage2, TInputImage3,
                                          TOutputImage, TFunction >
{
public:
  /** Standard class typedefs. */
  typedef TernaryFunctorImageFilter                                  Self;
  typedef TernaryFunctorImageFilterBase< TInputImage1, TInputImage2,
                                         TInputImage3, TOutputImage,
                                         TFunction >                 Superclass;
  typedef SmartPointer< Self >                                       Pointer;
  typedef SmartPointer< const Self >                                 ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TernaryFunctorImageFilter, TernaryFunctorImageFilterBase);

  /** Some typedefs. */
  typedef typename Superclass::FunctorType           FunctorType;
  typedef typename Superclass::Input1ImageType       Input1ImageType;
  typedef typename Superclass::Input1ImagePointer    Input1ImagePointer;
  typedef typename Superclass::Input1ImageRegionType Input1ImageRegionType;
  typedef typename Superclass::Input1ImagePixelType  Input1ImagePixelType;
  typedef typename Superclass::Input2ImageType       Input2ImageType;
  typedef typename Superclass::Input2ImagePointer    Input2ImagePointer;
  typedef typename Superclass::Input2ImageRegionType Input2ImageRegionType;
  typedef typename Superclass::Input2ImagePixelType  Input2ImagePixelType;
  typedef typename Superclass::Input3ImageType       Input3ImageType;
  typedef typename Superclass::Input3ImagePointer    Input3ImagePointer;
  typedef typename Superclass::Input3ImageRegionType Input3ImageRegionType;
  typedef typename Superclass::Input3ImagePixelType  Input3ImagePixelType;
  typedef typename Superclass::OutputImageType       OutputImageType;
  typedef typename Superclass::OutputImagePointer    OutputImagePointer;
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;
  typedef typename Superclass::OutputImagePixelType  OutputImagePixelType;

  /** Image dimensions */
  itkStaticConstMacro(Input1ImageDimension, unsigned int,
                      TInputImage1::ImageDimension);
  itkStaticConstMacro(Input2ImageDimension, unsigned int,
                      TInputImage2::ImageDimension);
  itkStaticConstMacro(Input3ImageDimension, unsigned int,
                      TInputImage3::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( SameDimensionCheck1,
                   ( Concept::SameDimension< Input1ImageDimension, Input2ImageDimension > ) );
  itkConceptMacro( SameDimensionCheck2,
                   ( Concept::SameDimension< Input1ImageDimension, Input3ImageDimension > ) );
  itkConceptMacro( SameDimensionCheck3,
                   ( Concept::SameDimension< Input1ImageDimension, OutputImageDimension > ) );
  /** End concept checking */
#endif
protected:
  TernaryFunctorImageFilter();
  virtual ~TernaryFunctorImageFilter() {}

  /** TernaryFunctorImageFilter can be implemented as a multithreaded filter.
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
  TernaryFunctorImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);            //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTernaryFunctorImageFilter.hxx"
#endif

#endif
