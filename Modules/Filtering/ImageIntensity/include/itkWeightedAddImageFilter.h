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
#ifndef __itkWeightedAddImageFilter_h
#define __itkWeightedAddImageFilter_h

#include "itkBinaryFunctorImageFilter.h"
#include "itkNumericTraits.h"

namespace itk
{
/** \class WeightedAddImageFilter
 * \brief Implements an operator for computing a weighted sum of two images pixel-wise.
 *
 * This class is parametrized over the types of the two
 * input images and the type of the output image.
 * Numeric conversions (castings) are done by the C++ defaults.
 *
 * The pixel type of the input 1 image must have a valid defintion of
 * the operator+ with a pixel type of the image 2. This condition is
 * required because internally this filter will perform the operation
 *
 *        pixel_from_image_1 * alpha +  pixel_from_image_2 * (1.0 - alpha)
 *
 * Additionally the type resulting from the sum will be cast to
 * the pixel type of the output image.
 *
 * The total operation over one pixel will be
 *
 *  output_pixel = static_cast<OutputPixelType>(
 *      input1_pixel * alpha + input2_pixel * (1-alpha) )
 *
 * The alpha parameter is set using SetAlpha.
 *
 * \warning No numeric overflow checking is performed in this filter.
 *
 * \ingroup IntensityImageFilters  Multithreaded
 * \ingroup ITK-ImageIntensity
 */
namespace Functor
{
template< class TInput1, class TInput2, class TOutput >
class WeightedAdd2
{
public:
  typedef typename NumericTraits< TInput1 >::AccumulateType AccumulatorType;
  typedef typename NumericTraits< TInput1 >::RealType       RealType;
  WeightedAdd2() {}
  ~WeightedAdd2() {}
  bool operator!=(const WeightedAdd2 & other) const
  {
    if ( m_Alpha != other.m_Alpha )
      {
      return true;
      }
    return false;
  }

  bool operator==(const WeightedAdd2 & other) const
  {
    return !( *this != other );
  }

  inline TOutput operator()(const TInput1 & A, const TInput2 & B) const
  {
    const RealType sum1 = A * m_Alpha;
    const RealType sum2 = B * m_Beta;

    return static_cast< TOutput >( sum1 + sum2 );
  }

  void SetAlpha(RealType alpha)
  {
    m_Alpha = alpha;
    m_Beta  = NumericTraits< RealType >::One - m_Alpha;
  }

  RealType GetAlpha() const
  {
    return m_Alpha;
  }

private:
  RealType m_Alpha;
  RealType m_Beta;     // auxiliary var to avoid a subtraction at every pixel
};
}
template< class TInputImage1, class TInputImage2, class TOutputImage >
class ITK_EXPORT WeightedAddImageFilter:
  public
  BinaryFunctorImageFilter< TInputImage1, TInputImage2, TOutputImage,
                            Functor::WeightedAdd2<
                              typename TInputImage1::PixelType,
                              typename TInputImage2::PixelType,
                              typename TOutputImage::PixelType >   >

{
public:
  /** Standard class typedefs. */
  typedef WeightedAddImageFilter Self;
  typedef BinaryFunctorImageFilter< TInputImage1, TInputImage2, TOutputImage,
                                    Functor::WeightedAdd2<
                                      typename TInputImage1::PixelType,
                                      typename TInputImage2::PixelType,
                                      typename TOutputImage::PixelType >
                                    >                                 Superclass;

  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  typedef typename Superclass::FunctorType FunctorType;
  typedef typename FunctorType::RealType   RealType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(WeightedAddImageFilter,
               BinaryFunctorImageFilter);

  /** Set the weight for the first operand of the weighted addition */
  void SetAlpha(RealType alpha)
  {
    this->GetFunctor().SetAlpha(alpha);
    this->Modified();
  }

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( Input1HasNumericTraitsCheck,
                   ( Concept::HasNumericTraits< typename TInputImage1::PixelType > ) );
  itkConceptMacro( Input1RealTypeMultiplyCheck,
                   ( Concept::MultiplyOperator< typename TInputImage1::PixelType,
                                                RealType, RealType > ) );
  itkConceptMacro( Input2RealTypeMultiplyCheck,
                   ( Concept::MultiplyOperator< typename TInputImage2::PixelType,
                                                RealType, RealType > ) );
  /** End concept checking */
#endif
protected:
  WeightedAddImageFilter() {}
  virtual ~WeightedAddImageFilter() {}
private:
  WeightedAddImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);         //purposely not implemented
};
} // end namespace itk

#endif
