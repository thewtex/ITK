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

#include "itkParticleSwarmOptimizerDOMReader.h"

namespace itk
{

/** Generate the output object from the implicitly created DOM object. */
void ParticleSwarmOptimizerDOMReader::GenerateOutput( const DOMNode* domnode, const void* )
{
  // Two functions can be used to read a single value or a std::vector/itk::Array from a string:
  //   1. FromString( s, data );
  //   2. s >> data;
  // The two functions are identical when reading a single value.
  // For std::vector/itk::Array, version 1 read all data elements in the string, while version 2
  // reads a fixed number of data elements that is specified by data.size() or data.GetSize().
  //
  // During data reading, a std::ios_base::failure exception will be thrown if error occurred due to
  // wrong data format, missing data elements (for std::vector/itk::Array), etc.
  try
  {
    std::string s;

    // read an integer value from an attribute
    s = domnode->GetAttribute("NumberOfParticles");
    int nop = 0; s >> nop;
    this->m_OutputObject->SetNumberOfParticles( nop );

    // read an integer value from an attribute
    s = domnode->GetAttribute("MaximumNumberOfIterations");
    int noi = 0; s >> noi;
    this->m_OutputObject->SetMaximalNumberOfIterations( noi );

    // read a double value from an attribute
    s = domnode->GetAttribute("InertiaCoefficient");
    double icoef = 0; s >> icoef;
    this->m_OutputObject->SetInertiaCoefficient( icoef );

    // read a double value from an attribute
    s = domnode->GetAttribute("GlobalCoefficient");
    double gcoef = 0; s >> gcoef;
    this->m_OutputObject->SetGlobalCoefficient( gcoef );

    // read a double value from an attribute
    s = domnode->GetAttribute("PersonalCoefficient");
    double pcoef = 0; s >> pcoef;
    this->m_OutputObject->SetPersonalCoefficient( pcoef );

    // read a std::vector of double from an attribute
    const DOMNode* nodelb = domnode->GetChildByID( "lower" );
    s = nodelb->GetAttribute("value");
    std::vector<double> lbound;
    FromString( s, lbound ); // read all data elements in the string
    //
    const DOMNode* nodeub = domnode->GetChildByID( "upper" );
    s = nodeub->GetAttribute("value");
    std::vector<double> ubound;
    FromString( s, ubound ); // read all data elements in the string
    //
    ParticleSwarmOptimizer::ParameterBoundsType bounds;
    for ( unsigned int i=0; i<lbound.size(); i++ )
    {
      std::pair<double,double> value;
      value.first = lbound[i];
      value.second = ubound[i];
      bounds.push_back( value );
    }
    this->m_OutputObject->SetParameterBounds( bounds );

    // read an itk::Array of double from a text child node
    const DOMNode* nodeptols = domnode->GetChild( "ParametersConvergenceTolerance" );
    s = nodeptols->GetTextChild()->GetText();
    itk::Array<double> ptols;
    FromString( s, ptols ); // read all data elements in the string
    this->m_OutputObject->SetParametersConvergenceTolerance( ptols );

    // read a double value from an attribute
    s = domnode->GetAttribute("FunctionConvergenceTolerance");
    double ftol = 0; s >> ftol;
    this->m_OutputObject->SetFunctionConvergenceTolerance( ftol );

    // read a double value from an attribute
    s = domnode->GetAttribute("ConvergedPercentageToStop");
    double stoppercent = 0; s >> stoppercent;
    this->m_OutputObject->SetPercentageParticlesConverged( stoppercent );
  }
  catch ( std::ios_base::failure f )
  {
    // translate the STD exception to ITK exception
    ExceptionObject e( __FILE__, __LINE__ );
    e.SetDescription( f.what() );
    throw e;
  }
}

} // namespace itk
