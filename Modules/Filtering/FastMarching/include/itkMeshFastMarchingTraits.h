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

#ifndef __itkMeshFastMarchingTraits_h
#define __itkMeshFastMarchingTraits_h

#include "itkFastMarchingTraits.h"

#include "itkQuadEdgeMesh.h"
#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"

namespace itk
{
/**  \class MeshFastMarchingTraits
  \brief possible traits to be used when using itk::FastMarchingBase
inherited class with itk::QuadEdgeMesh

  \tparam VDimension Point Dimension
  \tparam InputPixel Input Data type
  \tparam TInputMeshTraits Input Mesh traits
  \tparam TOutputPixel Output Data type
  \tparam TOutputMeshTraits Output Mesh traits

  \sa MeshFastMarchingTraits2
*/
template< unsigned int VDimension,
          typename TInputPixel,
          class TInputMeshTraits, //= QuadEdgeMeshTraits< TInputPixel, VDimension, bool, bool >,
          typename TOutputPixel, //= TInputPixel,
          class TOutputMeshTraits //= QuadEdgeMeshTraits< TOutputPixel, VDimension, bool, bool >
         >
class MeshFastMarchingTraits :
    public FastMarchingTraits<
      QuadEdgeMesh< TInputPixel, VDimension, TInputMeshTraits >,
      typename TInputMeshTraits::PointIdentifier,
      QuadEdgeMesh< TOutputPixel, VDimension, TOutputMeshTraits >,
      QuadEdgeMeshToQuadEdgeMeshFilter<
        QuadEdgeMesh< TInputPixel, VDimension, TInputMeshTraits >,
        QuadEdgeMesh< TOutputPixel, VDimension, TOutputMeshTraits > >
    >
  {
public:
  itkStaticConstMacro(PointDimension, unsigned int, VDimension);
  };

}
#endif // __itkMeshFastMarchingTraits_h
