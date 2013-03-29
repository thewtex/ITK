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
#ifndef __itkClampImageFilter_h
#define __itkClampImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkProgressReporter.h"

namespace itk
{

namespace Functor
{

/** \class Clamp
 *
 * \brief Functor used to clamp a value to a specified range.
 *
 * It is templated over an input and an output-type in order
 * to be able to cast the output value.
 *
 * \ingroup ITKImageIntensity
 * \ingroup ITKImageFilterBase
 */
template< class TInput, class TOutput = TInput >
class Clamp
{
public:
  /** Creates the functor and initializes the bounds to the
   * output-type limits.
   */
  Clamp() :
    m_LowerBound(NumericTraits< TOutput >::NonpositiveMin()),
    m_UpperBound(NumericTraits< TOutput >::max())
  {};

  virtual ~Clamp() {};

  TOutput GetLowerBound() const
    { return m_LowerBound; }

  TOutput GetUpperBound() const
    { return m_UpperBound; }

  /** Set the bounds of the range in which the data will be clamped.
   * If the lower-bound is greater than the upper-bound,
   * an itk::ExceptionObject will be thrown.
   */
  void SetBounds( const TOutput lowerBound, const TOutput upperBound)
    {
    if(lowerBound > upperBound)
      {
      itkGenericExceptionMacro("invalid bounds: [" << lowerBound << "; " << upperBound << "]");
      }

    m_LowerBound = lowerBound;
    m_UpperBound = upperBound;
    }

  bool operator!=( const Clamp & other ) const
    {
    return  m_UpperBound != other.m_UpperBound
      || m_LowerBound != other.m_LowerBound;
    }

  bool operator==( const Clamp & other ) const
    {
    return !(*this != other);
    }

  inline TOutput operator()( const TInput & A ) const
    {
    double dA = static_cast< double >( A );

    if ( dA < m_LowerBound )
      {
      return m_LowerBound;
      }

    if ( dA > m_UpperBound )
      {
      return m_UpperBound;
      }

    return static_cast< TOutput >( A );
    }

private:
  TOutput m_LowerBound;
  TOutput m_UpperBound;
};
}

/** \class ClampImageFilter
 *
 * \brief Casts input pixels to output pixel type and clamps the
 * output pixel values to a specified range.
 *
 * This filter is templated over the input image type
 * and the output image type.
 *
 * \ingroup IntensityImageFilters
 * \ingroup MultiThreaded
 * \ingroup ITKImageFilterBase
 *
 * \sa UnaryFunctorImageFilter
 * \sa CastImageFilter
 *
 * \wiki
 * \wikiexample{ImageProcessing/ClampImageFilter,Cast an image from one type to another but clamp to the output value range}
 * \endwiki
 */

template <class TInputImage, class TOutputImage>
class ITK_EXPORT ClampImageFilter :
  public UnaryFunctorImageFilter< TInputImage,TOutputImage,
                                  Functor::Clamp< typename TInputImage::PixelType,
                                                  typename TOutputImage::PixelType > >
{
public:
  /** Standard class typedefs. */
  typedef ClampImageFilter               Self;
  typedef UnaryFunctorImageFilter< TInputImage, TOutputImage,
                                   Functor::Clamp< typename TInputImage::PixelType,
                                                   typename TOutputImage::PixelType > >
                                         Superclass;

  typedef SmartPointer< Self >           Pointer;
  typedef SmartPointer< const Self >     ConstPointer;

  typedef typename TOutputImage::PixelType OutputPixelType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ClampImageFilter, UnaryFunctorImageFilter);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(InputConvertibleToOutputCheck,
    (Concept::Convertible< typename TInputImage::PixelType,
                           typename TOutputImage::PixelType >));
  itkConceptMacro(InputConvertibleToDoubleCheck,
    (Concept::Convertible< typename TInputImage::PixelType,
                           double > ));
  /** End concept checking */
#endif

  OutputPixelType GetLowerBound() const
    {
    return this->GetFunctor().GetLowerBound();
    }

  OutputPixelType GetUpperBound() const
    {
    return this->GetFunctor().GetUpperBound();
    }

  /** Set the bounds of the range in which the data will be clamped.
   * If the lower-bound is greater than the upper-bound,
   * an itk::ExceptionObject will be thrown.
   */
  void SetBounds(const OutputPixelType lowerBound, const OutputPixelType upperBound)
    {
    if ( lowerBound == this->GetFunctor().GetLowerBound() && upperBound == this->GetFunctor().GetUpperBound())
      {
      return;
      }

    this->GetFunctor().SetBounds(lowerBound, upperBound);
    this->Modified();
    }

protected:
  ClampImageFilter() {}
  virtual ~ClampImageFilter() {}

  void GenerateData()
    {
    if( this->GetInPlace() && this->CanRunInPlace()
      && this->GetLowerBound() <= NumericTraits< OutputPixelType >::min()
      && this->GetUpperBound() >= NumericTraits< OutputPixelType >::max() )
      {
      // If the filter is asked to run in-place, is able to run in-place,
      // and the specified bounds are equal to the output-type limits,
      // then there is nothing to do. To avoid iterating over all the pixels for
      // nothing, graft the input to the output, generate a fake progress and exit.
      this->AllocateOutputs();
      ProgressReporter progress(this, 0, 1);
      return;
      }
    Superclass::GenerateData();
    }

private:
  ClampImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

};


} // end namespace itk

#endif
