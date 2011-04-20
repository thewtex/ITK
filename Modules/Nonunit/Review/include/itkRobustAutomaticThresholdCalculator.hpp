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
#ifndef __itkRobustAutomaticThresholdCalculator_hpp
#define __itkRobustAutomaticThresholdCalculator_hpp
#include "itkRobustAutomaticThresholdCalculator.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{
template< class TInputImage, class TGradientImage >
RobustAutomaticThresholdCalculator< TInputImage, TGradientImage >
::RobustAutomaticThresholdCalculator(void)
{
  m_Valid = false;
  m_Input = NULL;
  m_Gradient = NULL;
  m_Output = NumericTraits< InputPixelType >::Zero;
  m_Pow = 1;
}

template< class TInputImage, class TGradientImage >
void
RobustAutomaticThresholdCalculator< TInputImage, TGradientImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Input: " << m_Input.GetPointer() << std::endl;
  os << indent << "Gradient: " << m_Gradient.GetPointer() << std::endl;
  os << indent << "Valid: " << m_Valid << std::endl;
  os << indent << "Pow: " << m_Pow << std::endl;
  os << indent << "Output: " << m_Output << std::endl;
}

template< class TInputImage, class TGradientImage >
void
RobustAutomaticThresholdCalculator< TInputImage, TGradientImage >
::Compute()
{
  typedef typename InputImageType::IndexType IndexType;

  if ( !m_Input || !m_Gradient )
    {
    return;
    }

  ImageRegionConstIteratorWithIndex< InputImageType > iIt( m_Input,
                                                           m_Input->GetRequestedRegion() );
  iIt.GoToBegin();
  ImageRegionConstIteratorWithIndex< GradientImageType > gIt( m_Gradient,
                                                              m_Gradient->GetRequestedRegion() );
  gIt.GoToBegin();

  // init the values
  double n = 0;
  double d = 0;

  while ( !iIt.IsAtEnd() )
    {
    double g = vcl_pow(static_cast< double >( gIt.Get() ), m_Pow);
    n += iIt.Get() * g;
    d += g;
    ++iIt;
    ++gIt;
    }

//   std::cout << "n: " << n << "  d: " << d << std::endl;
  m_Output = static_cast< InputPixelType >( n / d );
  m_Valid = true;
}

template< class TInputImage, class TGradientImage >
const typename RobustAutomaticThresholdCalculator< TInputImage, TGradientImage >::InputPixelType &
RobustAutomaticThresholdCalculator< TInputImage, TGradientImage >
::GetOutput() const
{
  if ( !m_Valid )
    {
    itkExceptionMacro(<< "GetOutput() invoked, but the output have not been computed. Call Compute() first.");
    }
  return m_Output;
}
} // end namespace itk

#endif
