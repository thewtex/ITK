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
#ifndef __itkPolyLineParametricPath_txx
#define __itkPolyLineParametricPath_txx

#include "itkPolyLineParametricPath.h"
#include <math.h>

namespace itk
{
template< unsigned int VDimension >
typename PolyLineParametricPath< VDimension >::OutputType
PolyLineParametricPath< VDimension >
::Evaluate(const InputType & input) const
{
  OutputType output;
  PointType  outputPoint;
  VertexType vertex0;
  VertexType vertex1;
  double     fractionOfLineSegment;

  // Handle the endpoint carefully, since there is no following vertex
  if ( input  >=  InputType(m_VertexList->Size() - 1) )
    {
    return m_VertexList->ElementAt(m_VertexList->Size() - 1); // the last vertex
    }

  vertex0 = m_VertexList->ElementAt( int(input) );
  vertex1 = m_VertexList->ElementAt( int(input + 1.0) );

  fractionOfLineSegment = input - int(input);

  outputPoint = vertex0 + ( vertex1 - vertex0 ) * fractionOfLineSegment;

  // For some stupid reason, there is no easy way to cast from a point to a
  // continuous index.
  for ( unsigned int i = 0; i < VDimension; i++ )
    {
    output[i] = outputPoint[i];
    }

  return output;
}

template<unsigned int VDimension>
typename PolyLineParametricPath<VDimension>::OffsetType
PolyLineParametricPath<VDimension>
::IncrementInput(InputType & input) const
{
  unsigned iterationCount;
  bool tooSmall;
  bool tooBig;
  InputType inputStepSize;
  InputType tooSmallInputStepSize;
  InputType tooBigInputStepSize;
  InputType finalInputValue;
  OffsetType offset;
  IndexType currentImageIndex;
  IndexType nextImageIndex;
  IndexType finalImageIndex;

  iterationCount = 0;
  inputStepSize = this->GetDefaultInputStepSize();
  tooSmallInputStepSize = 0;
  tooBigInputStepSize = 0;

  finalInputValue = this->EndOfInput();
  currentImageIndex = this->EvaluateToIndex( input );
  finalImageIndex = this->EvaluateToIndex( finalInputValue );
  offset = finalImageIndex - currentImageIndex;

  if( offset == this->GetZeroOffset() || ( input >= finalInputValue ) )
     return this->GetZeroOffset();

  do
  {
    if( iterationCount++ > 10000 ) { itkExceptionMacro(<<"Too many iterations"); }

    nextImageIndex = this->EvaluateToIndex( input + inputStepSize );
    offset = nextImageIndex - currentImageIndex;
    tooSmall = ( offset == this->GetZeroOffset() );
    tooBig = false;

    if( tooSmall )
    {
      // Increase the input step size, but don't go past the end of the input
      if( (input + inputStepSize) >= finalInputValue )
      {
        inputStepSize = finalInputValue - input;
      }
      else
      {
        tooSmallInputStepSize = inputStepSize;
        if( tooBigInputStepSize == 0 )
          inputStepSize *= 2;
        else
          inputStepSize += ( tooBigInputStepSize - tooSmallInputStepSize ) / 2;
      }
    }
    else
    {
      // Search for an offset dimension that is too big
      for( unsigned i = 0; i < VDimension && ! tooBig; ++i )
      {
        tooBig = ( offset[i] >= 2 || offset[i] <= -2 );
      }
      if( tooBig )
      {
        tooBigInputStepSize = inputStepSize;
        inputStepSize -= (tooBigInputStepSize - tooSmallInputStepSize) / 2;
      }
    }
  }
  while( tooSmall || tooBig );

  input += inputStepSize;
  return offset;
}

//template<unsigned int VDimension>
//typename PolyLineParametricPath<VDimension>::VectorType
//PolyLineParametricPath<VDimension>
//::EvaluateDerivative(const InputType & input) const
//{
//}

/**
 * Constructor
 */
template< unsigned int VDimension >
PolyLineParametricPath< VDimension >
::PolyLineParametricPath()
{
  this->SetDefaultInputStepSize(0.3);
  m_VertexList = VertexListType::New();
}

/**
 * Standard "PrintSelf" method
 */
template< unsigned int VDimension >
void
PolyLineParametricPath< VDimension >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Verticies:  " << m_VertexList << std::endl;
}
} // end namespace itk

#endif
