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

namespace itk
{

/** \class NoiseBaseImageFilter
 *
 * \brief Base class for Noise image filters
 *
 * \sa InPlaceImageFilter
 * \ingroup ITKImageStatistics
 */
template <class TInputImage, class TOutputImage=TInputImage>
class NoiseBaseImageFilter :
    public
InPlaceImageFilter<TInputImage,TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef NoiseBaseImageFilter                               Self;
  typedef InPlaceImageFilter<TInputImage,TOutputImage >      Superclass;
  typedef SmartPointer<Self>                                 Pointer;
  typedef SmartPointer<const Self>                           ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NoiseBaseImageFilter, InPlaceImageFilter);

  itkGetConstMacro(Seed, uint32_t);
  itkSetMacro(Seed, uint32_t);


protected:
  NoiseBaseImageFilter() :m_Seed(0) {};
  virtual ~NoiseBaseImageFilter() {};

  void PrintSelf(std::ostream &os, Indent indent) const
    {
      Superclass::PrintSelf(os, indent);
      os << indent << "Seed: "     << static_cast<typename NumericTraits<uint32_t>::PrintType>(m_Seed) << std::endl;
    }

private:
  NoiseBaseImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  uint32_t m_Seed;

};


} // end namespace itk

#endif //  __itkNoiseBaseImageFilter_h
