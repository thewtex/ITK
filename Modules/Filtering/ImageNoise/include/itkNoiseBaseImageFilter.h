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
#ifndef __itkNoiseBaseImageFilter_h
#define __itkNoiseBaseImageFilter_h

#include "itkInPlaceImageFilter.h"
#include <vcl_ctime.h>

namespace itk
{

/** \class NoiseBaseImageFilter
 *
 * \brief Base class for Noise image filters
 *
 * \sa InPlaceImageFilter
 * \ingroup ITKImageNoise
 */
template <class TInputImage, class TOutputImage=TInputImage>
class NoiseBaseImageFilter :
    public InPlaceImageFilter<TInputImage,TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef NoiseBaseImageFilter                               Self;
  typedef InPlaceImageFilter<TInputImage,TOutputImage >      Superclass;
  typedef SmartPointer<Self>                                 Pointer;
  typedef SmartPointer<const Self>                           ConstPointer;

  typedef typename Superclass::OutputImagePixelType  OutputImagePixelType;


  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NoiseBaseImageFilter, InPlaceImageFilter);

  /** Seed for random initialization
   *
   * If the method with not seed value is provided then seed will be
   * randomly initialized based on the clock.
   */
  itkGetConstMacro(Seed, uint32_t);
  itkSetMacro(Seed, uint32_t);
  virtual void SetSeed()
    {
      vcl_time_t t;
      vcl_time(&t);
      this->SetSeed(Hash(t,vcl_clock()));
    }


protected:
  NoiseBaseImageFilter()
    {
      Self::SetSeed();
      this->InPlaceOff();
    };
  virtual ~NoiseBaseImageFilter() {};

  void PrintSelf(std::ostream &os, Indent indent) const
    {
      Superclass::PrintSelf(os, indent);
      os << indent << "Seed: "     << static_cast<typename NumericTraits<uint32_t>::PrintType>(m_Seed) << std::endl;
    }

  static uint32_t Hash(uint32_t a, uint32_t b)
    {
      //  Knuth's Multiplicative Method for hashing
      return (a+b)*2654435761;
    }

  static OutputImagePixelType ClampCast(const double &value)
    {
      if (value >= (double)NumericTraits<OutputImagePixelType>::max())
        {
        return NumericTraits<OutputImagePixelType>::max();
        }
      else if(value <= (double)NumericTraits<OutputImagePixelType>::NonpositiveMin())
        {
        return (double)NumericTraits<OutputImagePixelType>::NonpositiveMin();
        }
      else if (NumericTraits<OutputImagePixelType>::is_integer)
        {
        return Math::Round<OutputImagePixelType>(value);
        }
      else
        {
        return static_cast<OutputImagePixelType>(value);
        }
    }

private:
  NoiseBaseImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  uint32_t m_Seed;

};


} // end namespace itk

#endif //  __itkNoiseBaseImageFilter_h
