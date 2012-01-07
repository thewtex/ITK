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
#ifndef __itkUnaryFunctorImageFilterBase_h
#define __itkUnaryFunctorImageFilterBase_h

#include "itkInPlaceImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace itk
{
/** \class UnaryFunctorImageFilterBase
 * \brief Abstract base class for implementing a pixel-wise generic
 * operation on one image.
 *
 * UnaryFunctorImageFilterBase allows the output dimension of the filter
 * to be larger than the input dimension. Thus subclasses of the
 * UnaryFunctorImageFilterBase (like the CastImageFilter) can be used
 * to promote a 2D image to a 3D image, etc.
 *
 * \tparam TInputImage  Input image type
 * \tparam TOutputImage Output image type
 * \tparam TFunction    Functor type that defines the operation to be applied
 *
 * \sa UnaryFunctorImageFilter
 * \sa UnaryFunctorWithIndexImageFilter
 *
 * \ingroup   IntensityImageFilters     MultiThreaded
 * \ingroup ITKCommon
 */
template< class TInputImage, class TOutputImage, class TFunction >
class ITK_EXPORT UnaryFunctorImageFilterBase:public InPlaceImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef UnaryFunctorImageFilterBase                     Self;
  typedef InPlaceImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(UnaryFunctorImageFilterBase, InPlaceImageFilter);

  /** Some typedefs. */
  typedef TFunction FunctorType;

  typedef TInputImage                              InputImageType;
  typedef typename    InputImageType::ConstPointer InputImagePointer;
  typedef typename    InputImageType::RegionType   InputImageRegionType;
  typedef typename    InputImageType::IndexType    InputImageIndexType;
  typedef typename    InputImageType::PixelType    InputImagePixelType;

  typedef TOutputImage                             OutputImageType;
  typedef typename     OutputImageType::Pointer    OutputImagePointer;
  typedef typename     OutputImageType::RegionType OutputImageRegionType;
  typedef typename     OutputImageType::IndexType  OutputImageIndexType;
  typedef typename     OutputImageType::PixelType  OutputImagePixelType;

  /** Get the functor object.  The functor is returned by reference.
   * (Functors do not have to derive from itk::LightObject, so they do
   * not necessarily have a reference count. So we cannot return a
   * SmartPointer.) */
  FunctorType &       GetFunctor() { return m_Functor; }
  const FunctorType & GetFunctor() const { return m_Functor; }

  /** Set the functor object.  This replaces the current Functor with a
   * copy of the specified Functor. This allows the user to specify a
   * functor that has ivars set differently than the default functor.
   * This method requires an operator!=() be defined on the functor
   * (or the compiler's default implementation of operator!=() being
   * appropriate). */
  void SetFunctor(const FunctorType & functor)
  {
    if ( m_Functor != functor )
      {
      m_Functor = functor;
      this->Modified();
      }
  }

protected:
  UnaryFunctorImageFilterBase();
  virtual ~UnaryFunctorImageFilterBase() {}

  /** UnaryFunctorImageFilterBase can produce an image which is a different
   * resolution than its input image.  As such, UnaryFunctorImageFilterBase
   * needs to provide an implementation for
   * GenerateOutputInformation() in order to inform the pipeline
   * execution model.  The original documentation of this method is
   * below.
   *
   * \sa ProcessObject::GenerateOutputInformaton()  */
  virtual void GenerateOutputInformation();

  /** UnaryFunctorImageFilter can be implemented as a multithreaded filter.
   * Therefore, this implementation provides a ThreadedGenerateData() routine
   * which is called for each processing thread. The output image data is
   * allocated automatically by the superclass prior to calling
   * ThreadedGenerateData().  ThreadedGenerateData can only write to the
   * portion of the output image specified by the parameter
   * "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData()  */
  virtual void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                                    ThreadIdType threadId);

  /** Pure virtual method that actually invokes the
   * functor. Subclasses provide an implementation that passes
   * appropriate arguments to the functor. */
  virtual OutputImagePixelType InvokeFunctor(const OutputImageIndexType & index,
                                             const InputImagePixelType & value) = 0;

private:
  UnaryFunctorImageFilterBase(const Self &); //purposely not implemented
  void operator=(const Self &);          //purposely not implemented

  FunctorType m_Functor;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkUnaryFunctorImageFilterBase.hxx"
#endif

#endif
