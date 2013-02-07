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
#ifndef __itkPolyLineParametricPath_hxx
#define __itkPolyLineParametricPath_hxx

#include "itkPolyLineParametricPath.h"
#include <cmath>

namespace itk
{
template< unsigned int VDimension >
typename PolyLineParametricPath< VDimension >::OutputType
PolyLineParametricPath< VDimension >
::Evaluate(const InputType & input) const
{
  // Handle the endpoint carefully, since there is no following vertex
  const InputType endPoint = static_cast< InputType >( m_VertexList->Size() - 1 );
  if ( input > endPoint || itk::Math::FloatAlmostEqual( input, endPoint ) )
    {
    return static_cast<const VertexListType*>(this->m_VertexList)->ElementAt(m_VertexList->Size() - 1); // the last vertex
    }

  const VertexType vertex0 = static_cast<const VertexListType*>(this->m_VertexList)->ElementAt( int(input) );
  const VertexType vertex1 = static_cast<const VertexListType*>(this->m_VertexList)->ElementAt( int(input + 1.0) );

  const double fractionOfLineSegment = input - int(input);

  const PointType outputPoint = vertex0 + ( vertex1 - vertex0 ) * fractionOfLineSegment;

  // For some stupid reason, there is no easy way to cast from a point to a
  // continuous index.
  OutputType output;
  for ( unsigned int i = 0; i < VDimension; i++ )
    {
    output[i] = outputPoint[i];
    }

  return output;
}

template<unsigned int VDimension>
typename PolyLineParametricPath<VDimension>::VectorType
PolyLineParametricPath<VDimension>
::EvaluateDerivative(const InputType & input) const
{
  //Get next integral time-point
  const InputType next_timepoint = std::min(std::floor(input + 1.0), this->EndOfInput());

  //Get previous integral time-point
  const InputType prev_timepoint = next_timepoint - 1.0;

  //Calculate the continuous index for both points
  const ContinuousIndexType next_index = this->Evaluate(next_timepoint);
  const ContinuousIndexType prev_index = this->Evaluate(prev_timepoint);

  //For some reason, there's no way to convert ContinuousIndexType to VectorType
  VectorType partial_derivatives;
  for (int i = 0; i < VDimension; ++i)
    {
    partial_derivatives[i] = next_index[i] - prev_index[i];
    }

  return partial_derivatives;
}

template<unsigned int VDimension>
typename PolyLineParametricPath<VDimension>::OffsetType
PolyLineParametricPath<VDimension>
::IncrementInput(InputType & input) const
{
  const OutputType original_index = this->EvaluateToIndex(input); //Save this input index since we will use it to calculate the offset

  InputType potential_timestep = 0.0;
  bool timeStepSmallEnough = false;
  while (!timeStepSmallEnough)
    {
    if (input == this->EndOfInput())
      return this->GetZeroOffset();

    //Check to make sure we aren't already at a place with an offset of 1 pixel
    const OutputType potential_index = this->EvaluateToIndex(input);
    //For some reason, there's no way to convert OutputType to OffsetType
    OffsetType offset;
    for (int i = 0; i < VDimension; ++i)
      {
      offset[i] = potential_index[i] - original_index[i];
      }

    if (offset != this->GetZeroOffset())
      {
        return offset;
      }
    //Get the derivative at the current input
    VectorType partial_derivatives = this->EvaluateDerivative(input);

    //std::cout << "Partial derivatives: " << partial_derivatives << std::endl;

    //Find the largest of all the partial derivatives
    unsigned int max_partial_deriv_index = 0;
    for (unsigned int i = 1; i < VDimension; ++i)
      {
      if (std::abs(partial_derivatives[i]) > std::abs(partial_derivatives[max_partial_deriv_index]))
        {
        max_partial_deriv_index = i;
        }
      }

    //Calculate the timestep required to effect a 1 pixel change
    potential_timestep = 1.0/std::abs(partial_derivatives[max_partial_deriv_index]);

    //Check to make sure the timestep doesn't put the input past the next integral timestep
    //(since the derivatives can change)
    if (input + potential_timestep > std::floor(input + 1.0))
      {
      input = std::floor(input + 1.0); //Set the input to the next integral time-step
      }
    else
      {
        timeStepSmallEnough = true;
      }
    }

  //Finalize the potential timestep into THE timestep
  const InputType timestep = potential_timestep;

  //Get the index at the next timestep so we can calculate the offset
  const OutputType next_index = this->EvaluateToIndex(input + timestep);

  //For some reason, there's no way to convert OutputType to OffsetType
  OffsetType offset;
  for (int i = 0; i < VDimension; ++i)
    {
    offset[i] = next_index[i] -  original_index[i];
    }

  //Update input timestep
  input += timestep;

  //Return the offset
  return offset;
}

template< unsigned int VDimension >
PolyLineParametricPath< VDimension >
::PolyLineParametricPath()
{
  this->SetDefaultInputStepSize(0.3);
  this->m_VertexList = VertexListType::New();
}

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
