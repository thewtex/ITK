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
#include "itkMeshFileTestHelper.h"

template< class TMesh >
void GeneratePolyline( TMesh* mesh )
{
  typedef typename TMesh::PointIdentifier PointIdentifier;
  typedef typename TMesh::CellIdentifier  CellIdentifier;
  typedef typename TMesh::CellType        CellType;
  typedef typename TMesh::CellAutoPointer CellAutoPointer;

  typedef itk::LineCell< CellType >       LineCellType;

  PointIdentifier N = 10;

  mesh->GetPoints()->Reserve( N );

  typedef typename TMesh::PointType PointType;
  PointType p;

  for( PointIdentifier i = 0; i < N; i++ )
    {
    p[0] = static_cast< float >( vcl_cos( 2. * vnl_math::pi / static_cast< double >( N ) ) );
    p[1] = static_cast< float >( vcl_sin( 2. * vnl_math::pi / static_cast< double >( N ) ) );
    p[2] = 0.;

    mesh->SetPoint( i, p );
    }

  CellIdentifier id = 0;

  for( PointIdentifier i = 0; i < N; i++ )
    {
    CellAutoPointer cell;
    LineCellType *  lineCell = new LineCellType;
    lineCell->SetPointId(0, i);

    if( i != N-1 )
      {
      lineCell->SetPointId( 1, i+1 );
      }
    else
      {
      lineCell->SetPointId( 1, 0 );
      }
    cell.TakeOwnership(lineCell);
    mesh->SetCell(id++, cell);
    }
}

template< class TMesh >
int test( char* filename )
{
  typename TMesh::Pointer mesh = TMesh::New();
  GeneratePolyline< TMesh >( mesh.GetPointer() );

  typedef itk::MeshFileWriter< TMesh > WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetInput( mesh );
  writer->SetFileName( filename );
  writer->Update();

  typedef itk::MeshFileReader< TMesh > ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( filename );
  reader->Update();

  // Test points
  if( TestPointsContainer< TMesh >(  reader->GetOutput()->GetPoints(),
                                        mesh->GetPoints() ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }


  // Test cells
  if( TestCellsContainer< TMesh >( reader->GetOutput()->GetCells(),
                                      mesh->GetCells() ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

int itkPolylineWriteTest(int argc, char * argv[])
{
  if( argc != 3 )
    {
    return EXIT_FAILURE;
    }

  const unsigned int dimension = 3;
  typedef float      PixelType;

  std::cerr << "Test for itk::Mesh" << std::endl;

  typedef itk::Mesh< PixelType, dimension >         MeshType;

  if( test< MeshType >( argv[1] ) == EXIT_FAILURE )
    {
    std::cerr << "Failure" << std::endl;
    return EXIT_FAILURE;
    }

  std::cerr << "Test for itk::QuadEdgeMesh" << std::endl;

  typedef itk::QuadEdgeMesh< PixelType, dimension > QuadEdgeMeshType;

  if( test< QuadEdgeMeshType >( argv[1] ) == EXIT_FAILURE )
    {
    std::cerr << "Failure" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
