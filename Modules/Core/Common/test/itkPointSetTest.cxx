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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkPointSet.h"
#include "vnl/vnl_sample.h"

#include <iostream>
#include <string>

/**
 * Define a PointSet type that stores a PixelType of "int".  Use the defaults
 * for the other template parameters.
 */
typedef itk::PointSet<int>  PointSet;
typedef PointSet::PointType  PointType;

/**
 * The point set that is created consists of a 100 random points.
 */

int itkPointSetTest(int, char* [] )
{

  /**
   * Create the point set through its object factory.
   */
  PointSet::Pointer pset = PointSet::New();
  pset->Initialize();

  /**
   * Create a simple point set structure that will create a non-degenerate
   * bounding box but will allow simple querying tests.
   */
  PointSet::CoordRepType testPointCoords[3];
  for(int i=1; i <= 100 ; ++i)
    {
    testPointCoords[0] = static_cast<PointSet::CoordRepType>( i );
    testPointCoords[1] = static_cast<PointSet::CoordRepType>( i );
    testPointCoords[2] = static_cast<PointSet::CoordRepType>( i );
    pset->SetPoint( i - 1, PointType( testPointCoords ) );
    }

  /**
   * Perform some geometric operations (coordinate transformations)
   * to see if they are working.
   */
  std::cout << "Test:  FindClosestPoint() 1" << std::endl;

  PointSet::CoordRepType coords[PointSet::PointDimension];
  coords[0] = 50;
  coords[1] = 50;
  coords[2] = 50;

  PointSet::PointIdentifier pointId;
  bool isFound = pset->FindClosestPoint( coords, &pointId );
  if( !isFound || pointId != 49 )
    {
    std::cerr << "Error with FindClosestPoint() 1" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test:  FindClosestPoint() 2" << std::endl;

  PointSet::PointType point;
  point[0] = 50;
  point[1] = 50;
  point[2] = 50;
  isFound = pset->FindClosestPoint( point, &pointId );
  if( !isFound || pointId != 49 )
    {
    std::cerr << "Error with FindClosestPoint() 2" << std::endl;
    return EXIT_FAILURE;
    }

  PointSet::NeighborhoodIdentifierType neighborhood;

  isFound = pset->FindClosestNPoints( coords, 10u, &neighborhood );
  if( !isFound || neighborhood.size() != 10 )
    {
    std::cerr << "Error with FindClosestNPoints() 1" << std::endl;
    return EXIT_FAILURE;
    }

  isFound = pset->FindClosestNPoints( point, 10u, &neighborhood );
  if( !isFound || neighborhood.size() != 10 )
    {
    std::cerr << "Error with FindClosestNPoints() 2" << std::endl;
    return EXIT_FAILURE;
    }

  double radius = vcl_sqrt( 3 * vnl_math_sqr( 5.1 ) );

  isFound = pset->FindPointsWithinRadius( coords, radius, &neighborhood );
  if( !isFound || neighborhood.size() != 10 )
    {
    std::cerr << "Error with FindPointsWithinRadius() 1" << std::endl;
    return EXIT_FAILURE;
    }

  isFound = pset->FindPointsWithinRadius( point, radius, &neighborhood );
  if( !isFound || neighborhood.size() != 10 )
    {
    std::cerr << "Error with FindPointsWithinRadius() 2" << std::endl;
    return EXIT_FAILURE;
    }

  /**
   * Compute the bounding box of the mesh
   */
  PointType minPoint;
  minPoint.Fill( 1.0 );

  PointType maxPoint;
  maxPoint.Fill( 100.0 );

  if( pset->GetBoundingBox()->GetMinimum() != minPoint )
    {
    std::cerr << "Minimum bounding box point is incorrect." << std::endl;
    return EXIT_FAILURE;
    }
  if( pset->GetBoundingBox()->GetMaximum() != maxPoint )
    {
    std::cerr << "Maximum bounding box point is incorrect." << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}

