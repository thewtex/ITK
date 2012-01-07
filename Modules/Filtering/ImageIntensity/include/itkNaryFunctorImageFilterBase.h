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
#ifndef __itkNaryFunctorImageFilterBase_h
#define __itkNaryFunctorImageFilterBase_h

#include "itkInPlaceImageFilter.h"
#include "itkImageIterator.h"
#include "itkArray.h"

namespace itk
{
/** \class NaryFunctorImageFilterBase
 * \brief Base class for implementing pixel-wise generic operations on
 * N images.
 *
 * This class is templated over the types of the input images
 * and the type of the output image.  It is also templated by the
 * operation to be applied.  A Functor style is used to represent the
 * function.
 *
 * All the input images must be of the same type.
 *
 * \ingroup IntensityImageFilters MultiThreaded
 * \ingroup ITKImageIntensity
 */

template< class TInputImage, class TOutputImage, class TFunction >
class ITK_EXPORT NaryFunctorImageFilterBase:
  public InPlaceImageFilter< TInputImage, TOutputImage >

{
public:
  /** Standard class typedefs. */
  typedef NaryFunctorImageFilterBase                      Self;
  typedef InPlaceImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(NaryFunctorImageFilterBase, InPlaceImageFilter);

  /** Some typedefs. */
  typedef TFunction                            FunctorType;
  typedef TInputImage                          InputImageType;
  typedef typename InputImageType::Pointer     InputImagePointer;
  typedef typename InputImageType::RegionType  InputImageRegionType;
  typedef typename InputImageType::PixelType   InputImagePixelType;
  typedef TOutputImage                         OutputImageType;
  typedef typename OutputImageType::Pointer    OutputImagePointer;
  typedef typename OutputImageType::RegionType OutputImageRegionType;
  typedef typename OutputImageType::PixelType  OutputImagePixelType;
  typedef std::vector< InputImagePixelType >   NaryArrayType;

  /** Get the functor object.  The functor is returned by reference.
   * (Functors do not have to derive from itk::LightObject, so they do
   * not necessarily have a reference count. So we cannot return a
   * SmartPointer). */
  FunctorType & GetFunctor() { return m_Functor; }

  /** Set the functor object.  This replaces the current Functor with a
   * copy of the specified Functor. This allows the user to specify a
   * functor that has ivars set differently than the default functor.
   * This method requires an operator!=() be defined on the functor
   * (or the compiler's default implementation of operator!=() being
   * appropriate). */
  void SetFunctor(FunctorType & functor)
  {
    if ( m_Functor != functor )
      {
      m_Functor = functor;
      this->Modified();
      }
  }

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
  NaryFunctorImageFilterBase();
  virtual ~NaryFunctorImageFilterBase() {}

  /** This method is made pure virtual to prevent instantiation of
   * this class. */
  virtual void ThreadedGenerateData(const OutputImageRegionType &, ThreadIdType) = 0;

private:
  NaryFunctorImageFilterBase(const Self &); //purposely not implemented
  void operator=(const Self &);         //purposely not implemented

  FunctorType m_Functor;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNaryFunctorImageFilterBase.hxx"
#endif

#endif
