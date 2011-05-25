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

#include "itkFEM.h"
#include "itkFEMElement1DStress.txx"
#include "itkFEMElement2DC0LinearLine.h"
#include "itkFEMElement2DC0LinearLineStress.h"
#include "itkFEMElement2DC0LinearQuadrilateral.h"
#include "itkFEMElement2DC0LinearQuadrilateralMembrane.h"
#include "itkFEMElement2DC0LinearQuadrilateralStrain.h"
#include "itkFEMElement2DC0LinearQuadrilateralStress.h"
#include "itkFEMElement2DC0LinearTriangular.h"
#include "itkFEMElement2DC0LinearTriangularMembrane.h"
#include "itkFEMElement2DC0LinearTriangularStrain.h"
#include "itkFEMElement2DC0LinearTriangularStress.h"
#include "itkFEMElement2DC0QuadraticTriangular.h"
#include "itkFEMElement2DC0QuadraticTriangularStrain.h"
#include "itkFEMElement2DC0QuadraticTriangularStress.h"
#include "itkFEMElement2DC1Beam.h"
#include "itkFEMElement2DMembrane.txx"
#include "itkFEMElement2DStrain.txx"
#include "itkFEMElement2DStress.txx"
#include "itkFEMElement3DC0LinearHexahedron.h"
#include "itkFEMElement3DC0LinearHexahedronMembrane.h"
#include "itkFEMElement3DC0LinearHexahedronStrain.h"
#include "itkFEMElement3DC0LinearTetrahedron.h"
#include "itkFEMElement3DC0LinearTetrahedronMembrane.h"
#include "itkFEMElement3DC0LinearTetrahedronStrain.h"
#include "itkFEMElement3DMembrane.txx"
#include "itkFEMElement3DStrain.txx"
#include "itkFEMElementBase.h"
#include "itkFEMElementStd.txx"
#include "itkFEMElements.h"
#include "itkFEMException.h"
#include "itkFEMGenerateMesh.h"
#include "itkFEMImageMetricLoad.txx"
#include "itkFEMImageMetricLoadImplementation.h"
#include "itkFEMInitialization.h"
#include "itkFEMItpackSparseMatrix.h"
#include "itkFEMLightObject.h"
#include "itkFEMLinearSystemWrapper.h"
#include "itkFEMLinearSystemWrapperDenseVNL.h"
#include "itkFEMLinearSystemWrapperItpack.h"
#include "itkFEMLinearSystemWrapperVNL.h"
#include "itkFEMLinearSystemWrappers.h"
#include "itkFEMLoadBC.h"
#include "itkFEMLoadBCMFC.h"
#include "itkFEMLoadBase.h"
#include "itkFEMLoadEdge.h"
#include "itkFEMLoadElementBase.h"
#include "itkFEMLoadGrav.h"
#include "itkFEMLoadImplementationGenericBodyLoad.h"
#include "itkFEMLoadImplementationGenericLandmarkLoad.h"
#include "itkFEMLoadImplementationTest.h"
#include "itkFEMLoadLandmark.h"
#include "itkFEMLoadNode.h"
#include "itkFEMLoadPoint.h"
#include "itkFEMLoadTest.h"
#include "itkFEMLoads.h"
#include "itkFEMMacro.h"
#include "itkFEMMaterialBase.h"
#include "itkFEMMaterialLinearElasticity.h"
#include "itkFEMMaterials.h"
#include "itkFEMObjectFactory.h"
#include "itkFEMP.h"
#include "itkFEMPArray.h"
#include "itkFEMSolution.h"
#include "itkFEMSolver.h"
#include "itkFEMSolverCrankNicolson.h"
#include "itkFEMSolverHyperbolic.h"
#include "itkFEMUtility.h"
#include "itkVisitorDispatcher.h"
#include "itpack.h"


int itkFEMHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
