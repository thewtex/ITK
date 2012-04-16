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


#include <iostream>

#include "itkQuadEdgeMesh.h"
#include "itkAutomaticTopologyMeshSource.h"

int
itkAutomaticTopologyQuadEdgeMeshSourceTest(int, char* [] )
{
  // Declare the type of the Mesh
  typedef itk::QuadEdgeMesh< double, 3 >            MeshType;
  typedef MeshType::PointType                       PointType;
  typedef MeshType::CellType                        CellType;

  typedef itk::AutomaticTopologyMeshSource< MeshType >   MeshSourceType;
  typedef MeshSourceType::IdentifierType                 IdentifierType;
  typedef MeshSourceType::IdentifierArrayType            IdentifierArrayType;

  MeshSourceType::Pointer meshSource;

    {
    meshSource = MeshSourceType::New();

    // Begin using various AddPoint functions to add the vertices of a
    // cube.

    // Point expressed as an itk::QuadEdgeMeshPoint.
    PointType p;
    p.Fill( 0 );

    meshSource->AddPoint( p );

    // Point expressed as a C array.

    float a[3];
    a[ 0 ] = 1;
    a[ 1 ] = 0;
    a[ 2 ] = 0;
    meshSource->AddPoint( a );

    // Points expressed using their coordinates.
    meshSource->AddPoint( 0, 1, 0 );
    meshSource->AddPoint( 1, 1, 0 );
    meshSource->AddPoint( 0, 0, 1 );
    meshSource->AddPoint( 1, 0, 1 );
    meshSource->AddPoint( 0, 1, 1 );
    meshSource->AddPoint( 1, 1, 1 );

    // Done adding vertices of a cube.

    // Add a cell of each type using an Array of point IDs.  Here we use
    // the fact that the IDs are assigned in order, and the fact that
    // the Add[Cell] methods only use the first N entries of the array.

    IdentifierArrayType idArray( 8 );
    for( unsigned int i = 0; i < 8; i++ )
      {
      idArray[ i ] = i;
      }

    meshSource->AddVertex( idArray );
    meshSource->AddLine( idArray );
    meshSource->AddTriangle( idArray );
    meshSource->AddQuadrilateral( idArray );
    meshSource->AddTetrahedron( idArray );   // Degenerate; that's OK.
    meshSource->AddHexahedron( idArray );

    // Now add a new cube of points.  It overlaps along one face.

    IdentifierArrayType idArray1( 8 );
    idArray1[ 0 ] = meshSource->AddPoint( 1, 0, 0 );
    idArray1[ 1 ] = meshSource->AddPoint( 2, 0, 0 );
    idArray1[ 2 ] = meshSource->AddPoint( 1, 1, 0 );
    idArray1[ 3 ] = meshSource->AddPoint( 2, 1, 0 );
    idArray1[ 4 ] = meshSource->AddPoint( 1, 0, 1 );
    idArray1[ 5 ] = meshSource->AddPoint( 2, 0, 1 );
    idArray1[ 6 ] = meshSource->AddPoint( 1, 1, 1 );
    idArray1[ 7 ] = meshSource->AddPoint( 2, 1, 1 );

    // Add cells using parameter lists of IDs.

    meshSource->AddVertex( idArray[7] );

    // PROBLEM LINE
    meshSource->AddLine( idArray[6], idArray[7] );

    meshSource->AddTriangle( idArray[5], idArray[6], idArray[7] );

    meshSource->AddQuadrilateral( idArray[4], idArray[5],
                                  idArray[6], idArray[7] );

    meshSource->AddTetrahedron( idArray[0], idArray[2],
                                idArray[4], idArray[6] );

    meshSource->AddHexahedron( idArray[0], idArray[1],
                               idArray[2], idArray[3],
                               idArray[4], idArray[5],
                               idArray[6], idArray[7] );

    // Add cells using new point identifiers.
    meshSource->AddVertex( meshSource->AddPoint( 3, 1, 1 ) );

    // PROBLEM LINES
    meshSource->AddLine(
      meshSource->AddPoint( 2, 1, 1 ),
      meshSource->AddPoint( 3, 1, 1 )
    );

    meshSource->AddTriangle(
      meshSource->AddPoint( 3, 0, 1 ),
      meshSource->AddPoint( 2, 1, 1 ),
      meshSource->AddPoint( 3, 1, 1 )
    );

    meshSource->AddQuadrilateral(
      meshSource->AddPoint( 2, 0, 1 ),
      meshSource->AddPoint( 3, 0, 1 ),
      meshSource->AddPoint( 2, 1, 1 ),
      meshSource->AddPoint( 3, 1, 1 )
    );

    meshSource->AddTetrahedron(
      meshSource->AddPoint( 2, 0, 1 ),
      meshSource->AddPoint( 3, 0, 1 ),
      meshSource->AddPoint( 2, 1, 1 ),
      meshSource->AddPoint( 3, 1, 1 )
    );

    meshSource->AddHexahedron(
      meshSource->AddPoint( 2, 0, 0 ),
      meshSource->AddPoint( 3, 0, 0 ),
      meshSource->AddPoint( 2, 1, 0 ),
      meshSource->AddPoint( 3, 1, 0 ),
      meshSource->AddPoint( 2, 0, 1 ),
      meshSource->AddPoint( 3, 0, 1 ),
      meshSource->AddPoint( 2, 1, 1 ),
      meshSource->AddPoint( 3, 1, 1 )
    );

    // Add cells using C arrays of point coordinates.
    MeshSourceType::CoordinateType points[8][3] =
      {
        {3, 0, 0},
        {4, 0, 0},
        {3, 1, 0},
        {4, 1, 0},
        {3, 0, 1},
        {4, 0, 1},
        {3, 1, 1},
        {4, 1, 1}
      };

    meshSource->AddVertex( points[7] );
    meshSource->AddLine( points[6], points[7] );
    meshSource->AddTriangle( points[5], points[6], points[7] );
    meshSource->AddQuadrilateral( points[4], points[5], points[6], points[7] );
    meshSource->AddTetrahedron( points[3], points[5], points[6], points[7] );
    meshSource->AddHexahedron( points[0], points[1], points[2], points[3],
                               points[4], points[5], points[6], points[7] );

    // Print out the resulting mesh data.
    std::cerr << MeshType::Pointer(meshSource->GetOutput()) << std::endl;
    }

    {
    // Now do a sanity check.  Create a mesh consisting of a pair of
    // tetrahedra sharing a face and a pair of cubes sharing a face, and
    // then check properties.
    meshSource = MeshSourceType::New();

    meshSource->AddTetrahedron(
      meshSource->AddPoint( 0, 0, 0 ),
      meshSource->AddPoint( 1, 0, 0 ),
      meshSource->AddPoint( 0, 1, 0 ),
      meshSource->AddPoint( 0, 0, 1 )
      );

    meshSource->AddTetrahedron(
      meshSource->AddPoint( 1, 1, 1 ),
      meshSource->AddPoint( 1, 0, 0 ),
      meshSource->AddPoint( 0, 1, 0 ),
      meshSource->AddPoint( 0, 0, 1 )
      );

    meshSource->AddHexahedron(
      meshSource->AddPoint( 2, 0, 0 ),
      meshSource->AddPoint( 3, 0, 0 ),
      meshSource->AddPoint( 2, 1, 0 ),
      meshSource->AddPoint( 3, 1, 0 ),
      meshSource->AddPoint( 2, 0, 1 ),
      meshSource->AddPoint( 3, 0, 1 ),
      meshSource->AddPoint( 2, 1, 1 ),
      meshSource->AddPoint( 3, 1, 1 )
      );

    meshSource->AddHexahedron(
      meshSource->AddPoint( 3, 0, 0 ),
      meshSource->AddPoint( 4, 0, 0 ),
      meshSource->AddPoint( 3, 1, 0 ),
      meshSource->AddPoint( 4, 1, 0 ),
      meshSource->AddPoint( 3, 0, 1 ),
      meshSource->AddPoint( 4, 0, 1 ),
      meshSource->AddPoint( 3, 1, 1 ),
      meshSource->AddPoint( 4, 1, 1 )
      );

    try
      {
      meshSource->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Exception thrown during Update() " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
      }

    MeshType* mesh = meshSource->GetOutput();

    // Print out the resulting mesh data.
    std::cerr << MeshType::Pointer( mesh ) << std::endl;

    // ... In more detail.

    MeshType::PointIdentifier i;

    std::cerr << mesh->GetNumberOfPoints() << " points:" << std::endl;
    for( i = 0; i < mesh->GetNumberOfPoints(); i++ )
      {
      PointType point;
      bool dummy = mesh->GetPoint( i, &point );
      if( dummy )
        {
        std::cerr << i << ": " << point << std::endl;
        }
      }
    std::cerr << std::endl;

    std::cerr << mesh->GetNumberOfCells() << " cells:" << std::endl;
    for( i = 0; i < mesh->GetNumberOfCells(); i++ )
      {
      typedef MeshType::CellAutoPointer CellAutoPointer;
      CellAutoPointer cell;
      if( mesh->GetCell( i, cell ) )
        {
        std::cerr << i << ": ";
        typedef CellType::PointIdConstIterator PointIdIterator;
        PointIdIterator pointIter = cell->PointIdsBegin();
        PointIdIterator pointsEnd = cell->PointIdsEnd();
        while( pointIter != pointsEnd  )
          {
          std::cerr << *pointIter << " ";
          ++pointIter;
          }
        std::cerr << std::endl;
        }
      }
    std::cerr << std::endl;

    std::cerr << mesh->GetNumberOfEdges() << " cells:" << std::endl;
    for( i = 0; i < mesh->GetNumberOfEdges(); i++ )
      {
      EdgeCellType* edge = dynamic_cast< EdgeCellType* >( mesh->GetEdgeCells()->GetElement( i ) );

      if( edge )
        {
        std::cerr << i <<": " << edge->GetQEGeom()->GetOrigin()
                  <<" " << edge->GetQEGeom()->GetDestination()
                  << std::endl;
        }
      }
    std::cerr << std::endl;

    for( i = 0; i < mesh->GetNumberOfCells(); i++ )
      {
      typedef MeshType::CellAutoPointer CellAutoPointer;
      CellAutoPointer cell;
      if( mesh->GetCell( i, cell ) )
        {
        if( cell->GetType() == CellType::LINE_CELL )
          {
          std::cerr << "Cell " << i << ":" << std::endl;

          typedef std::set<IdentifierType>       NeighborSet;
          NeighborSet cellSet;

          mesh->GetCellBoundaryFeatureNeighbors( 0, i, 0, &cellSet );
          std::cerr << "Neighbors across vertex 0: ";
          for( NeighborSet::iterator neighborIter = cellSet.begin();
             neighborIter != cellSet.end(); ++neighborIter )
            {
            std::cerr << *neighborIter << " ";
            }
          std::cerr << std::endl;

          mesh->GetCellBoundaryFeatureNeighbors( 0, i, 1, &cellSet );
          std::cerr << "Neighbors across vertex 1: ";
          for( NeighborSet::iterator neighborIter = cellSet.begin();
             neighborIter != cellSet.end(); ++neighborIter )
            {
            std::cerr << *neighborIter << " ";
            }
          std::cerr << std::endl;

          mesh->GetCellNeighbors( i, &cellSet );

          std::cerr << "Neighbors having edge as boundary: ";
          for( NeighborSet::iterator neighborIter = cellSet.begin();
               neighborIter != cellSet.end(); ++neighborIter )
            {
            std::cerr << *neighborIter << " ";
            }
          std::cerr << std::endl << std::endl;
          }
        }
      }

    std::cout << "Number Of Points: " << mesh->GetNumberOfPoints() << std::endl;
    std::cout << "Number Of Faces: " << mesh->GetNumberOfFaces() << std::endl;
    std::cout << "Number Of Edges: " << mesh->GetNumberOfEdges() << std::endl;
    std::cout << "Number Of Cells: " << mesh->GetNumberOfCells() << std::endl;

    // Check that the right number of points has been added.
    unsigned long numPoints = mesh->GetNumberOfPoints();
    if( numPoints != 17 )
      {
      std::cerr << "*** FAILURE ***" << std::endl;
      std::cerr << "Mesh shows " << numPoints
                << " points, but 17 were added." << std::endl;
      std::cerr << "*******" << std::endl;
      return EXIT_FAILURE;
      }

    // Check that the right number of faces has been added.
    unsigned long numFaces = mesh->GetNumberOfFaces();
    if( numFaces != 10 )
      {
      std::cerr << "*** FAILURE ***" << std::endl;
      std::cerr << "Mesh shows "
                << numFaces
                << " faces, but 10 were added." << std::endl;
      std::cerr << "*******" << std::endl;
      return EXIT_FAILURE;
      }

    // Check that the right number of edges has been added.
    unsigned long numEdges = mesh->GetNumberOfEdges();
    if( numEdges != 43 )
      {
      std::cerr << "*** FAILURE ***" << std::endl;
      std::cerr << "Mesh shows "
                << numEdges
                << " edges, but 43 were added." << std::endl;
      std::cerr << "*******" << std::endl;
      return EXIT_FAILURE;
      }

    // Check that the right number of edges has been added.
    unsigned long numCells = mesh->GetNumberOfCells();
    if( numEdges != 53 )
      {
      std::cerr << "*** FAILURE ***" << std::endl;
      std::cerr << "Mesh shows "
                << numCells
                << " cells, but 53 were added." << std::endl;
      std::cerr << "*******" << std::endl;
      return EXIT_FAILURE;
      }

  // Check that the output is not destroyed when calling Update()
  meshSource->Update();

  numPoints = meshSource->GetOutput()->GetNumberOfPoints();
  numFaces  = meshSource->GetOutput()->GetNumberOfFaces();
  numEdges  = meshSource->GetOutput()->GetNumberOfEdges();
  numCells  = meshSource->GetOutput()->GetNumberOfCells();

  if( numPoints != 17 || numFaces != 10 || numEdges != 43 || numCells != 53 )
    {
    std::cerr << "*** FAILURE ***" << std::endl;
    std::cerr << "Mesh is being changed when invoking Update()" << std::endl;
    std::cerr << "*******" << std::endl;
    return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
