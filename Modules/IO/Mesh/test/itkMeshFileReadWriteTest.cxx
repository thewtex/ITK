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

#include "itkMesh.h"
#include "itkQuadEdgeMesh.h"
#include "itksys/SystemTools.hxx"

#include "itkMeshFileTestHelper.h"

template< class TPixel, unsigned int VDimension >
int MeshFileReadWriteTest( char *INfilename, char *OUTfilename, bool IsBinary )
{
  typedef TPixel                                        PixelType;
  typedef itk::Mesh<PixelType, VDimension >             MeshType;
  typedef itk::QuadEdgeMesh<PixelType, VDimension >     QEMeshType;

  if( test< MeshType   >( INfilename, OUTfilename, IsBinary ) )
    {
    return EXIT_FAILURE;
    }
  if( test< QEMeshType >( INfilename, OUTfilename, IsBinary ) )
    {
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

int itkMeshFileReadWriteTest(int argc, char * argv[])
{
  if(argc < 4)
  {
  std::cerr << "Invalid commands, You need input, output mesh file name and the dimension." << std::endl;
  return EXIT_FAILURE;
  }

  int dim = atoi( argv[3] );
  bool IsBinary = ( argc > 4 );

  typedef float PixelType;

  switch( dim )
    {
    case 2:
      {
      const unsigned int dimension = 2;
      return MeshFileReadWriteTest< PixelType, dimension >( argv[1], argv[2], IsBinary );
      }
    default:
    case 3:
      {
      const unsigned int dimension = 3;
      return MeshFileReadWriteTest< PixelType, dimension >( argv[1], argv[2], IsBinary );
      }
    case 4:
      {
      const unsigned int dimension = 4;
      return MeshFileReadWriteTest< PixelType, dimension >( argv[1], argv[2], IsBinary );
      }
    }

  return EXIT_SUCCESS;
}
