/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkQuadEdgeMeshEulerOperatorSplitFaceTest.cxx
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <string>

#include "itkQuadEdgeMesh.h"
#include "itkQuadEdgeMeshLineCell.h"
#include "itkQuadEdgeMeshPolygonCell.h"

#include "itkQuadEdgeMeshFunctionBase.h"
#include "itkQuadEdgeMeshEulerOperatorSplitFacetFunction.h"
#include "itkQuadEdgeMeshEulerOperatorJoinFacetFunction.h"
#include "itkQuadEdgeMeshEulerOperatorsTestHelper.h"

int itkQuadEdgeMeshEulerOperatorSplitFaceTest( int, char * [] )
{
  
  typedef itk::QuadEdgeMesh< double, 3 >                      MeshType;
  typedef MeshType::Pointer                                   MeshPointer;
  typedef MeshType::QEType                                    QEType;
  typedef MeshType::PointIdentifier                           PointIdentifier;
  typedef MeshType::PointType                                 PointType;
  typedef MeshType::CellType                                  CellType;
  typedef itk::QuadEdgeMeshLineCell< CellType >               LineCellType;

  typedef itk::QuadEdgeMeshEulerOperatorSplitFacetFunction< MeshType, QEType>
    SplitFacet;
  typedef itk::QuadEdgeMeshEulerOperatorJoinFacetFunction< MeshType, QEType >
    JoinFacet;

  /////////////////////////////////////////
  //
  //         Split Facet
  //
  /////////////////////////////////////////

  // Split the facet again in order to restore the original situation:
  std::cout << "Checking SplitFacet." << std::endl;
  
  SplitFacet::Pointer splitFacet = SplitFacet::New( );
  std::cout << "     " << "Test No Mesh Input";
  if( splitFacet->Evaluate( (QEType*)1, (QEType*)2 ) )
    {
    std::cout << "FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "OK" << std::endl;
  
  (void)splitFacet->GetNameOfClass(); 

  MeshPointer mesh = MeshType::New();
  CreateSquareTriangularMesh<MeshType>( mesh );

  splitFacet->SetInput( mesh );

  std::cout << "     " << "Test No QE Input";
  if( splitFacet->Evaluate( (QEType*)0, (QEType*)0 ) )
    {
    std::cout << "FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "OK" << std::endl;

  std::cout << "     " << "Test two QE Input not sharing the same left";
  if( splitFacet->Evaluate( mesh->FindEdge( 10, 16 ),
                            mesh->FindEdge( 13, 19 ) ) )
    {
    std::cout << "FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "OK" << std::endl;

  std::cout << "     " << "Test twice same non-null QE Input";
  if( splitFacet->Evaluate( (QEType*)1, (QEType*)1 ) )
    {
    std::cout << "FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "OK" << std::endl;

  std::cout << "     " << "Test two consecutive QE Input";
  if( splitFacet->Evaluate( mesh->FindEdge( 10, 16 ),
                            mesh->FindEdge( 10, 16 )->GetLnext( ) ) )
    {
    std::cout << "FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "OK" << std::endl;

  JoinFacet::Pointer joinFacet = JoinFacet::New();
  QEType* DeletedEdge = mesh->FindEdge( 12, 7 );
  QEType* G = DeletedEdge->GetSym( )->GetLprev( );
  QEType* H = joinFacet->Evaluate( DeletedEdge );

  if( !splitFacet->Evaluate( H, G ) )
    {
    std::cout << "FAILED." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "     " << "Split a face (possible)";
  // The number of edges and faces must be respectively identical to
  // the original number edges and faces:
  if( ! AssertTopologicalInvariants< MeshType >
          ( mesh, 25, 56, 32, 1, 0 ) )
    {
    std::cout << "FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  if ( mesh->GetPoint( 12 ).GetValence( ) != 6 )
    {
    std::cout << "FAILED [wrong valence of "
              << mesh->GetPoint( 12 ).GetValence( )
              << " ]." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "OK" << std::endl;
  std::cout << "Checking SplitFacet. OK" << std::endl << std::endl;
  
  std::cout << "Checking JoinFacet( SplitFacet( edge ) ) invariance.";
  G = mesh->FindEdge( 12, 7 )->GetSym( )->GetLprev( );
  H = joinFacet->Evaluate( mesh->FindEdge( 12, 7 ) );
  if( !H )
    {
    std::cout << "FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  if( !joinFacet->Evaluate( splitFacet->Evaluate( H, G ) ) )
    {
    std::cout << "FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  if( ! AssertTopologicalInvariants< MeshType >
          ( mesh, 25, 55, 31, 1, 0 ) )
    {
    std::cout << "FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  if ( mesh->GetPoint( 12 ).GetValence( ) != 5 )
    {
    std::cout << "FAILED [wrong valence of "
              << mesh->GetPoint( 12 ).GetValence( )
              << " ]." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << ".OK" << std::endl << std::endl;

  return EXIT_SUCCESS;
}
