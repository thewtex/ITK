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
#ifndef __itkCastImageFilter_h
#define __itkCastImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkProgressReporter.h"

namespace itk
{
/** \class CastImageFilter
 *
 * \brief Casts input pixels to output pixel type.
 *
 * This filter is templated over the input image type
 * and the output image type.
 *
 * If you need to perform a dimensionaly reduction, you may want
 * to use the ExtractImageFilter instead of the CastImageFilter.
 *
 * \ingroup IntensityImageFilters  Multithreaded
 * \sa UnaryFunctorImageFilter
 * \sa ExtractImageFilter
 * \ingroup ITK-ImageFilterBase
 *
 * \wiki
 * \wikiexample{ImageProcessing/CastImageFilter,Cast an image from one type to another}
 * \endwiki
 */
namespace Functor
{

template< class TInput, class TOutput >
class Cast
{
public:
  Cast() : m_Clamping( false ) {}
  virtual ~Cast() {}
  bool operator!=(const Cast &) const
  {
    return false;
  }

  bool operator==(const Cast & other) const
  {
    return !( *this != other );
  }

  template< bool >
  struct Dispatch {};

  /** Version for non-scalar types. No clamping is performed. */
  inline TOutput Evaluate(const Dispatch< false > &, const TInput & A) const
  {
    return static_cast< TOutput >( A );
  }

  /** Version for scalar-types. Clamping may be performed. */
  inline TOutput Evaluate(const Dispatch< true > &, const TInput & A) const
  {
    if ( m_Clamping )
      {
      double dA = static_cast< double >( A );
      if ( dA > static_cast< double >( NumericTraits< TOutput >::max() ) )
        {
        return NumericTraits< TOutput >::max();
        }
      if ( dA < static_cast< double >( NumericTraits< TOutput >::NonpositiveMin() ) )
        {
        return NumericTraits<TOutput>::NonpositiveMin();
        }
      }

    return static_cast< TOutput >( A );
  }

  /** Version for nonscalar-types. Clamping will not be performed. */
  inline TOutput operator()(const TInput & A) const
  {
    return this->Evaluate(Dispatch< NumericTraits< TInput >::IsNumericType >(), A );
  }

  bool m_Clamping;
};

} // namespace Functor


template< class TInputImage, class TOutputImage >
class ITK_EXPORT CastImageFilter:
  public
  UnaryFunctorImageFilter< TInputImage, TOutputImage,
                           Functor::Cast<
                             typename TInputImage::PixelType,
                             typename TOutputImage::PixelType > >
{
public:
  /** Standard class typedefs. */
  typedef CastImageFilter Self;
  typedef UnaryFunctorImageFilter< TInputImage, TOutputImage,
                                   Functor::Cast<
                                     typename TInputImage::PixelType,
                                     typename TOutputImage::PixelType >
                                   >  Superclass;

  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(CastImageFilter, UnaryFunctorImageFilter);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( InputConvertibleToOutputCheck,
                   ( Concept::Convertible< typename TInputImage::PixelType,
                                           typename TOutputImage::PixelType > ) );
  /** End concept checking */
#endif

  /** Enable/disable clamping. If enabled, output pixel values will be
   * clamped to the range supported by the output pixel type. */
  void SetClamping( bool enable )
  {
    this->GetFunctor().m_Clamping = enable;
  }
  bool GetClamping()
  {
    return this->GetFunctor().m_Clamping;
  }
  itkBooleanMacro( Clamping );

protected:
  CastImageFilter() {}
  virtual ~CastImageFilter() {}

  void GenerateData()
  {
    if ( this->GetInPlace() && this->CanRunInPlace() )
      {
      // nothing to do, so avoid iterating over all the pixels
      // for nothing! Allocate the output, generate a fake progress and exit
      this->AllocateOutputs();
      ProgressReporter progress(this, 0, 1);
      return;
      }
    Superclass::GenerateData();
  }

private:
  CastImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);  //purposely not implemented
};
} // end namespace itk

#endif
