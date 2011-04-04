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

#ifndef __itkMeshFastMarchingTraits2_h
#define __itkMeshFastMarchingTraits2_h

#include "itkFastMarchingTraits.h"

#include "itkConceptChecking.h"

#include "itkQuadEdgeMesh.h"
#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"

namespace itk
{
/**  \class MeshFastMarchingTraits2
  \brief possible traits to be used when using itk::FastMarchingBase
inherited class with itk::QuadEdgeMesh

  \tparam TInputMesh Input Mesh type
  \tparam TOutputMesh Output Mesh type

  \sa MeshFastMarchingTraits
*/
template< class TInputMesh, class TOutputMesh >
class MeshFastMarchingTraits2 :
    public FastMarchingTraits<
      TInputMesh,
      typename TOutputMesh::MeshTraits::PointIdentifier,
      TOutputMesh,
      QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh > >
  {
public:
  typedef MeshFastMarchingTraits2 Self;


  itkStaticConstMacro(PointDimension, unsigned int, TOutputMesh::PointDimension );


#ifdef ITK_USE_CONCEPT_CHECKING

  // make sure TInputMesh and TOutputMesh have the same dimension
  itkStaticConstMacro(InputPointDimension, unsigned int,
                      TInputMesh::PointDimension);

  // Let's make sure TInputImage and TOutputImage have the same dimension
  itkConceptMacro( SameDimension,
                   ( Concept::SameDimension<
                      itkGetStaticConstMacro(InputPointDimension),
                      itkGetStaticConstMacro(PointDimension) > ) );
#endif
  };
}
#endif // __itkMeshFastMarchingTraits2_h
