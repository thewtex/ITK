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

#include <iostream>

#include "itkAutomaticTopologyMeshSource.hpp"
#include "itkBinaryMask3DMeshSource.hpp"
#include "itkConnectedRegionsMeshFilter.hpp"
#include "itkImageToMeshFilter.hpp"
#include "itkImageToParametricSpaceFilter.hpp"
#include "itkInteriorExteriorMeshFilter.hpp"
#include "itkMeshRegion.h"
#include "itkMeshSource.hpp"
#include "itkMeshToMeshFilter.hpp"
#include "itkMesh.hpp"
#include "itkParametricSpaceToImageSpaceMeshFilter.hpp"
#include "itkRegularSphereMeshSource.hpp"
#include "itkSimplexMeshAdaptTopologyFilter.hpp"
#include "itkSimplexMeshToTriangleMeshFilter.hpp"
#include "itkSimplexMeshVolumeCalculator.hpp"
#include "itkSphereMeshSource.hpp"
#include "itkTransformMeshFilter.hpp"
#include "itkTriangleMeshToBinaryImageFilter.hpp"
#include "itkTriangleMeshToSimplexMeshFilter.hpp"
#include "itkVTKPolyDataReader.hpp"
#include "itkVTKPolyDataWriter.hpp"
#include "itkWarpMeshFilter.hpp"


int itkMeshHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
