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
#ifndef __itkLaplacianDeformationQuadEdgeMeshFilterBase_hxx
#define __itkLaplacianDeformationQuadEdgeMeshFilterBase_hxx

#include "itkLaplacianDeformationQuadEdgeMeshFilterBase.h"

namespace itk
{
// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::LaplacianDeformationQuadEdgeMeshFilterBase():
  m_CoefficientsMethod( NULL ), m_Order(1), m_AreaComputationType( None )
{}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::SolveLinearSystems(const MatrixType & iM,
                     const VectorType & iBx,
                     const VectorType & iBy,
                     const VectorType & iBz,
                     VectorType & oX,
                     VectorType & oY,
                     VectorType & oZ)
{
  SolverTraits::Solve(iM, iBx, iBy, iBz, oX, oY, oZ);
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
typename
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >::OutputCoordRepType
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::ComputeMixedAreaForGivenVertex(OutputPointIdentifier iId)
{
  OutputMeshPointer output = this->GetOutput();
  OutputQEType *    qe = output->FindEdge(iId);
  OutputQEType *    qe_it = qe;
  OutputQEType *    qe_it2 = qe->GetOnext();

  OutputCoordRepType oW = NumericTraits< OutputCoordRepType >::Zero;

  do
    {
    oW += this->ComputeMixedArea(qe_it, qe_it2);

    qe_it = qe_it2;
    qe_it2 = qe_it2->GetOnext();
    }
  while ( qe != qe_it );

  return oW;
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
typename
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >::OutputCoordRepType
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::ComputeMixedArea(OutputQEType *iQE1, OutputQEType *iQE2)
{
  if ( iQE1->IsLeftSet() )
    {
    OutputMeshPointer output = this->GetOutput();

    typename Superclass::OutputPointsContainerPointer points = output->GetPoints();

    OutputPointIdentifier id[3];

    id[0] = iQE1->GetOrigin();
    id[1] = iQE1->GetDestination();
    id[2] = iQE2->GetDestination();

    OutputPointType p[3];

    for ( int i = 0; i < 3; i++ )
      {
      p[i] = points->GetElement(id[i]);
      }

    OutputCoordRepType area = TriangleType::ComputeMixedArea(p[0], p[1], p[2]);

    if ( area < vnl_math::eps )
      {
      return NumericTraits< OutputCoordRepType >::Zero;
      }
    else
      {
      return ( 1. / ( 2. * area ) );
      }
    }
  else
    {
    return NumericTraits< OutputCoordRepType >::Zero;
    }
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::ComputeVertexIdMapping()
{
  OutputMeshPointer output   = this->GetOutput();

  typename Superclass::OutputPointsContainerPointer points = output->GetPoints();

  OutputPointsContainerIterator pIt = points->Begin();
  OutputPointsContainerIterator pEnd = points->End();

  OutputPointIdentifier k = 0;

  while ( pIt != pEnd )
    {
    this->m_InternalMap.insert( typename OutputMapPointIdentifier::value_type(pIt->Index(), k++) );
    ++pIt;
    }
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::SetConstrainedNode(OutputPointIdentifier id, const OutputPointType & iP)
{
  InputMeshConstPointer input = this->GetInput();
  InputPointType        pOrg = input->GetPoint(id);

  this->SetDisplacement(id, iP - pOrg);
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::SetDisplacement(OutputPointIdentifier id, const OutputVectorType & iV)
{
  m_Constraints[ id ] = iV;
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
bool
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::GetDisplacement(OutputPointIdentifier id,
                  OutputVectorType& oV ) const
{
  typename ConstraintMapType::const_iterator it = m_Constraints.find( id );

  if( it != m_Constraints.end() )
    {
    oV( it->second );
    return true;
    }
  else
    {
    return false;
    }
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::ClearConstraints()
{
  if ( !m_Constraints.empty() )
    {
    m_Constraints.clear();
    }

  if ( !m_InternalMap.empty() )
    {
    m_InternalMap.clear();
    }
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::FillMatrixRow(OutputPointIdentifier iId,
                unsigned int iDegree,
                OutputCoordRepType iWeight,
                std::map< OutputPointIdentifier, OutputCoordRepType > & ioRow)
{
  OutputMeshPointer output = this->GetOutput();

  std::list< Triple > todo;

  Triple t(iId, iWeight, iDegree);
  todo.push_back(t);

  OutputPointIdentifier id;
  unsigned int          degree;
  OutputQEType          *qe, *temp;

  while ( !todo.empty() )
    {
    t = todo.back();
    todo.pop_back();

    id = t.m_Id;
    degree = t.m_Degree;

    if ( degree == 0 )
      {
      typedef typename std::map< OutputPointIdentifier, OutputCoordRepType >::iterator RowIterator;
      RowIterator rIt = ioRow.find(id);

      if ( rIt == ioRow.end() )
        {
        ioRow.insert( std::pair< OutputPointIdentifier, OutputCoordRepType >(id, t.m_Weight) );
        }
      else
        {
        rIt->second += t.m_Weight;
        }
      }
    else
      {
      OutputCoordRepType ww  = NumericTraits< OutputCoordRepType >::Zero;
      OutputCoordRepType w   = NumericTraits< OutputCoordRepType >::Zero;

      qe = output->FindEdge(id);
      if ( qe )
        {
        temp = qe;

        do
          {
          CoefficientMapConstIterator coeffIt = m_CoefficientMap.find( temp );

          if( coeffIt != m_CoefficientMap.end() )
            {
            w = coeffIt->second;
            }
          else
            {
            w = ( *this->m_CoefficientsMethod )(output, temp);
            m_CoefficientMap.insert( std::pair< OutputQEType*, OutputCoordRepType >( temp, w ) );
            }

          if ( degree < iDegree )
            {
            if( m_AreaComputationType != None )
              {
              AreaMapConstIterator mixedIt = m_MixedAreaMap.find( id );
              OutputCoordRepType mixedArea = NumericTraits< OutputCoordRepType >::One;

              if( mixedIt != m_MixedAreaMap.end() )
                {
                mixedArea = mixedIt->second;
                }
              else
                {
                if( m_AreaComputationType == MixedArea )
                  {
                  mixedArea = this->ComputeMixedAreaForGivenVertex(id);
                  }
                m_MixedAreaMap.insert( std::pair< OutputPointIdentifier, OutputCoordRepType >( id, mixedArea ) );
                }
              w *= mixedArea;
              }
            }

          w *= t.m_Weight;
          ww -= w;

          todo.push_back( Triple(temp->GetDestination(), w, degree - 1) );

          temp = temp->GetOnext();
          }
        while ( temp != qe );

        todo.push_back( Triple(id, ww, degree - 1) );
        }
      }
    }
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << "Order: " << m_Order << std::endl;

  os << "AreaComputationType: ";
  if( m_AreaComputationType == None )
    {
    os << "None";
    }
  else
    {
    os << "MixedArea";
    }
  os << std::endl;
}
} // end namespace itk

#endif
