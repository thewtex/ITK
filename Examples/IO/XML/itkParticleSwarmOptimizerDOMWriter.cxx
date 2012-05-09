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

#include "itkParticleSwarmOptimizerDOMWriter.h"

namespace itk
{

/** Generate the internal DOM object from the input user object. */
void ParticleSwarmOptimizerDOMWriter::GenerateDOM( DOMNode* domnode, const void* ) const
{
  ParticleSwarmOptimizer* ipobj = const_cast<ParticleSwarmOptimizer*>(this->m_InputObject);

  domnode->SetName( "optimizer" );
  domnode->SetAttribute( "type", "ParticleSwarmOptimizer" );

  //std::string s;
  StringObject s;

  s << ipobj->GetNumberOfParticles();
  domnode->SetAttribute( "NumberOfParticles", s );

  s << ipobj->GetMaximalNumberOfIterations();
  domnode->SetAttribute( "MaximumNumberOfIterations", s );

  s << ipobj->GetInertiaCoefficient();
  domnode->SetAttribute( "InertiaCoefficient", s );

  s << ipobj->GetGlobalCoefficient();
  domnode->SetAttribute( "GlobalCoefficient", s );

  s << ipobj->GetPersonalCoefficient();
  domnode->SetAttribute( "PersonalCoefficient", s );

  std::vector<double> lbound;
  std::vector<double> ubound;
  ParticleSwarmOptimizer::ParameterBoundsType bounds = ipobj->GetParameterBounds();
  for ( unsigned int i = 0; i < bounds.size(); i++ )
  {
    lbound.push_back( bounds[i].first );
    ubound.push_back( bounds[i].second );
  }
  //
  DOMNode::Pointer nodelb = DOMNode::New();
  nodelb->SetName( "bound" );
  nodelb->SetAttribute( "id", "lower" );
  nodelb->SetAttribute( "value", s << lbound );
  domnode->AddChild( nodelb );
  //
  DOMNode::Pointer nodeub = DOMNode::New();
  nodeub->SetName( "bound" );
  nodeub->SetAttribute( "id", "upper" );
  nodeub->SetAttribute( "value", s << ubound );
  domnode->AddChild( nodeub );

  DOMNode::Pointer nodeptols = DOMNode::New();
  nodeptols->SetName( "ParametersConvergenceTolerance" );
  domnode->AddChild( nodeptols );
  s << (Array<double>)ipobj->GetParametersConvergenceTolerance(); // the casting is necessary to select the right templated function
  nodeptols->AddTextChild( s );

  s << ipobj->GetFunctionConvergenceTolerance();
  domnode->SetAttribute( "FunctionConvergenceTolerance", s );

  s << ipobj->GetPercentageParticlesConverged();
  domnode->SetAttribute( "ConvergedPercentageToStop", s );
}

} // namespace itk
