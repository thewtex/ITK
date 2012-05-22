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
#ifndef __itkFEMRobustSolver_hxx
#define __itkFEMRobustSolver_hxx

#include "itkFEMRobustSolver.h"

#include "itkFEMLoadNode.h"
#include "itkFEMLoadElementBase.h"
#include "itkFEMLoadBC.h"
#include "itkFEMLoadBCMFC.h"
#include "itkFEMLoadLandmark.h"

namespace itk
{
namespace fem
{
/**
 * Default constructor which sets the indices
 * for the matrix and vector storage.
 */
template <unsigned int VDimension>
RobustSolver<VDimension>
::RobustSolver()
{
  m_ForceIndex = 0;
  m_LandmarkForceIndex = 1;
  m_ExternalForceIndex = 2;
  m_SolutionIndex = 0;
  m_MeshStiffnessMatrixIndex = 1;
  m_LandmarkStiffnessMatrixIndex = 2;
  m_StiffnessMatrixIndex = 0;

  m_Iterations = 0;

  m_OutlierRejectionSteps = 5;
  m_ApproximationSteps = 5;

  m_ToleranceToLargestDisplacement = 1.0;
  m_ConjugateGradientPrecision = 1e-3;
  m_FractionErrorRejected =.25;

  m_TradeOffImageMeshEnergy = 1.0;

  // initialize parameters for the interpolation grid
  InterpolationGridSizeType size;
  size.Fill(0);
  m_Region.SetSize(size);
  m_Direction.SetIdentity();
  m_Origin.Fill(0.0);
  m_Spacing.Fill(1.0);
  m_UseInterpolationGrid = true;

  m_MaximumNonZeroMatrixEntriesFactor = 100;
}

template <unsigned int VDimension>
void
RobustSolver<VDimension>
::Initialization()
{
  this->SetLinearSystemWrapper(&m_Itpack);
  unsigned int maxNumberOfNonZeroValues = this->m_NGFN * m_MaximumNonZeroMatrixEntriesFactor;

  if(maxNumberOfNonZeroValues > NumericTraits<unsigned int>::max())
    {
    itkExceptionMacro("Too large system of equations");
    }

  m_Itpack.SetMaximumNonZeroValuesInMatrix(maxNumberOfNonZeroValues);

  // the NGFN is determined once the FEMObject is finalized
  this->m_ls->SetSystemOrder(this->m_NGFN);
  this->m_ls->SetNumberOfVectors(3);
  this->m_ls->SetNumberOfSolutions(1);
  this->m_ls->SetNumberOfMatrices(3);
  this->m_ls->InitializeMatrix(m_MeshStiffnessMatrixIndex);
  this->m_ls->InitializeMatrix(m_LandmarkStiffnessMatrixIndex);
  this->m_ls->InitializeMatrix(m_StiffnessMatrixIndex);
  this->m_ls->InitializeVector(m_ForceIndex);
  this->m_ls->InitializeVector(m_LandmarkForceIndex);
  this->m_ls->InitializeVector(m_ExternalForceIndex);
  this->m_ls->InitializeSolution(m_SolutionIndex);

  this->InitializeInterpolationGrid(m_Region, m_Origin, m_Spacing, m_Direction);

  this->InitializeLandmark();
}

/**
 * Record the element, in which the landmark is located, and the shape function
 * value.
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::InitializeLandmark()
{
  InterpolationGridIndexType index;
  VectorType globalPosition;
  InterpolationGridPointType point;

  LoadContainerType *container = this->m_FEMObject->GetLoadContainer();

  if(!container)
    {
    itkExceptionMacro("Missing container");
    }

  LoadContainerIterator it = container->Begin();

  for(; it != container->End(); ++it)
    {
    Load::Pointer load = it.Value();

    LoadNoisyLandmark *landmark = dynamic_cast<LoadNoisyLandmark*>(load.GetPointer());

    if(landmark == NULL)
      {
      itkExceptionMacro("Encounter landmark that is not a LoadNoisyLandmark");
      }

    globalPosition = landmark->GetSource();
    for( unsigned int i = 0; i < this->FEMDimension; i++ )
      {
      point[i] = globalPosition[i];
      }

    if(m_UseInterpolationGrid)
      {
      // landmark is within the interpolation grid
      if( this->m_InterpolationGrid->TransformPhysicalPointToIndex(point, index) )
        {
        Element::ConstPointer e = this->m_InterpolationGrid->GetPixel(index);

        // landmark is inside the mesh
        if(e.IsNotNull())
          {
          landmark->SetContainedElement(const_cast<Element*>(e.GetPointer()));

          unsigned int NumberOfDimensions = e->GetNumberOfSpatialDimensions();
          VectorType localPos(NumberOfDimensions);

          e->GetLocalFromGlobalCoordinates(globalPosition, localPos);
          landmark->SetShape(e->ShapeFunctions(localPos));
          landmark->SetIsOutOfMesh(false);
          }
        else
          {
          // remove the landmark
          landmark->SetIsOutOfMesh(true);
          }

        }

      }
    else
      {
      landmark->AssignToElement(this->m_FEMObject->GetElementContainer() );
      Element::ConstPointer ep = landmark->GetElement(0);
      VectorType localPos = landmark->GetPoint();
      landmark->SetShape(ep->ShapeFunctions(localPos));
      }
    }

    // remove landmarks outside of the mesh
    this->DeleteLandmarksOutOfMesh();
}

/**
 * Run the solver and produce a warped FEM object
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::GenerateData()
{
  // initialize matrix, vector, solution, interpolation grid, and landmark.
  this->Initialization();

  // LS solver, which can be a VNL solver or a PETSc solver
  this->RunSolver();

  // copy the input to the output and add the
  // displacements to update the nodal coordinates
  FEMObjectType *femObject = this->GetOutput();
  femObject->DeepCopy(this->GetInput());
  this->UpdateDisplacements();
}

/**
 * Solve the displacement vector U
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::RunSolver()
{
  this->AssembleMeshStiffnessMatrix();

  this->ComputeLandmarkTensor();

  this->AssembleLandmarkStiffnessMatrix();

  this->AssembleGlobalMatrixFromLandmarksAndMeshMatrices();

  this->AssembleF();

  if(m_OutlierRejectionSteps != 0)
    {
    this->IncrementalSolverWithOutlierRejection(m_OutlierRejectionSteps, m_ApproximationSteps);
    }
  else
    {
    // do "interpolation" only (no points discarded)
    this->IncrementalSolverWithoutOutlierRejection(m_ApproximationSteps);
    }
}

/**
 * Solve the displacement vector U with outlier rejection
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::IncrementalSolverWithOutlierRejection(unsigned int OutlierRejectionSteps,
                                        unsigned int ApproximationSteps)
{
  unsigned int numberOfLoads = this->m_FEMObject->GetNumberOfLoads();
  double rejectionRate = m_FractionErrorRejected / OutlierRejectionSteps;
  unsigned int numberOfRejectedLandmarksPerStep = (unsigned int)floor(numberOfLoads * rejectionRate);

  itkDebugMacro("numberOfRejectedLandmarksPerStep " << numberOfRejectedLandmarksPerStep);

  unsigned int step = 0;

  for (unsigned int iteration = 0; iteration < OutlierRejectionSteps; ++iteration)
    {
    step++;

    // get scaling parameter before outlier rejection
    const double oldPointTensorPonderation = this->GetLandmarkTensorPonderation();

    this->AddExternalForcesToSetMeshZeroEnergy();
    itkDebugMacro("external force added");

    // solve linear system of equations
    // solver to find possible outliers
    this->SolveSystem();
    itkDebugMacro("System solved");

    // compute simulated error for sorting
    this->ComputeLandmarkSimulatedDisplacementAndWeightedError(m_ToleranceToLargestDisplacement);

    // sort the points in the *decreasing* order of error norm
    this->Nth_element_WRTDisplacementError(numberOfRejectedLandmarksPerStep);

    // set first n to "unselected", and decrease
    // numberOfSelectedBlocks accordingly
    this->UnselectLandmarks(numberOfRejectedLandmarksPerStep);

    this->RemoveUnselectedLandmarkContributionInPointStiffnessMatrix();
    itkDebugMacro("unselected points' contribution removed");

    this->DeleteFromLandmarkBeginning(numberOfRejectedLandmarksPerStep);
    itkDebugMacro("unselected points removed from the list");

    // rescale the point stiffness matrix with the new pointTensorPonderation
    this->RescaleLandmarkStiffnessMatrix(oldPointTensorPonderation);
    itkDebugMacro("matrix rescaled");

    this->AssembleGlobalMatrixFromLandmarksAndMeshMatrices();
    itkDebugMacro("matrix reassembled");

    this->AssembleF();
    itkDebugMacro("vector rebuilt");

    this->AddExternalForcesToSetMeshZeroEnergy();
    itkDebugMacro("external force added");

    // solver when some outliers are rejected
    this->SolveSystem();
    itkDebugMacro("system resolved");

    this->CalculateExternalForces();
    itkDebugMacro("approximation step with outlier rejection " << step);

    }

  for (unsigned int iteration = 0; iteration < ApproximationSteps; ++iteration)
    {
    step++;

    this->AddExternalForcesToSetMeshZeroEnergy();
    itkDebugMacro("external force added");

    this->SolveSystem();

    this->CalculateExternalForces();

    itkDebugMacro("approximation step without outlier rejection " << step);
    }
}

/**
 * Solve the displacement vector U without outlier rejection
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::IncrementalSolverWithoutOutlierRejection(unsigned int ApproximationSteps)
{
  for(unsigned int iteration = 0; iteration < ApproximationSteps; ++iteration)
    {
    this->AddExternalForcesToSetMeshZeroEnergy();
    itkDebugMacro("external force added");

    this->SolveSystem();

    this->CalculateExternalForces();
    itkDebugMacro("Approximation step without outlier rejection " << iteration);
    }

}

/**
 * Compute the approximation error for each landmark for subsequent outlier
 * rejection
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::ComputeLandmarkSimulatedDisplacementAndWeightedError(double displacementPonderation)
{
  double lambda = displacementPonderation;
  double displacementNorm = 0.0;
  VectorType shape;

  LoadContainerType *container = this->m_FEMObject->GetLoadContainer();

  if(!container)
    {
    itkExceptionMacro("no container");
    }

  LoadContainerIterator it = container->Begin();

  while(it != container->End())
    {
    Load::Pointer load = it.Value();

    LoadNoisyLandmark *landmark = dynamic_cast<LoadNoisyLandmark*>(load.GetPointer());

    if(landmark == NULL)
      {
      itkExceptionMacro("Encounter landmark that is not a LoadNoisyLandmark");
      }

    if(!landmark->IsOutlier())
      {
      shape = landmark->GetShape();

      Element::Pointer ep = landmark->GetContainedElement();

      const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
      const unsigned int Nnodes = ep->GetNumberOfNodes();

      VectorType error(NnDOF, 0.0);
      VectorType nodeSolution(NnDOF);

      for(unsigned int nodeId = 0; nodeId < Nnodes; nodeId++)
        {
        for(unsigned int dofId = 0; dofId < NnDOF; ++dofId)
          {
          const int degreeOfFreedom = ep->GetDegreeOfFreedom(nodeId * NnDOF + dofId);
          nodeSolution[dofId] = this->m_ls->GetSolutionValue(degreeOfFreedom, m_SolutionIndex);
          }

        error += shape[nodeId] * nodeSolution;

        }

      landmark->SetSimulatedDisplacement(error);
      displacementNorm = error.two_norm();
      error = landmark->GetRealDisplacement() - error;

      if(landmark->HasStructureTensor())
        {
        MatrixType structureTensor = landmark->GetStructureTensor();
        double confidence = landmark->GetConfidence();
        VectorType weightedError = error / ((1-lambda) * displacementNorm + lambda);
        VectorType structureTensorPonderatedError = structureTensor * confidence * weightedError;
        landmark->SetErrorNorm(structureTensorPonderatedError.two_norm());
        }
      else
        {
        double confidence = landmark->GetConfidence();
        VectorType weightedError = error / ((1-lambda) * displacementNorm + lambda);
        VectorType nonStructureTensorponderatedError = confidence * weightedError;
        landmark->SetErrorNorm(nonStructureTensorponderatedError.two_norm());
        }

    }

    ++it;
    }
}

/**
 * Compute landmark tensor weighted by a structure tensor if exists
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::ComputeLandmarkTensor()
{
  VectorType shape;

  LoadContainerType *container = this->m_FEMObject->GetLoadContainer();

  if(!container)
    {
    itkExceptionMacro("Missing container");
    }

  LoadContainerIterator it = container->Begin();

  for(; it != container->End(); ++it)
    {
    Load::Pointer load = it.Value();

    LoadNoisyLandmark *landmark = dynamic_cast<LoadNoisyLandmark*>(load.GetPointer());

    if(landmark == NULL)
      {
      itkExceptionMacro("Encounter landmark that is not a LoadNoisyLandmark");
      }

    if(!landmark->IsOutlier())
      {
      shape = landmark->GetShape();

      Element::Pointer ep = landmark->GetContainedElement();

      const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
      const unsigned int Nnodes = ep->GetNumberOfNodes();

      MatrixType nodeTensor(NnDOF, NnDOF);
      MatrixType landmarkTensor(NnDOF, NnDOF);

      landmarkTensor.fill(0.0);

      for(unsigned int nodeId = 0; nodeId < Nnodes; ++nodeId)
        {
        for(unsigned int dofXId = 0; dofXId < NnDOF; dofXId++)
          {
          for(unsigned int dofYId = 0; dofYId < NnDOF; dofYId++)
            {
            unsigned nx = ep->GetDegreeOfFreedom(nodeId * NnDOF + dofXId);
            unsigned ny = ep->GetDegreeOfFreedom(nodeId * NnDOF + dofYId);
            nodeTensor[dofXId][dofYId] = this->m_ls->GetMatrixValue(nx, ny, m_MeshStiffnessMatrixIndex);
            }
          }

        landmarkTensor += nodeTensor*shape[nodeId];
        }

      if(landmark->HasStructureTensor())
        {
        landmarkTensor *= landmark->GetStructureTensor();
        }

      landmark->SetLandmarkTensor(landmarkTensor);

      }
    }
}

/**
 * Sort the landmarks according to the error norm
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::Nth_element_WRTDisplacementError(unsigned int nthPoint)
{
  if(nthPoint == 0)
    {
      return;
    }

  typedef std::vector<Load::Pointer> LoadVectorType;

  LoadContainerType *container = this->m_FEMObject->GetLoadContainer();

  if(!container)
    {
    itkExceptionMacro("Missing container");
    }

  LoadVectorType &loadVector = container->CastToSTLContainer();

  LoadVectorType::iterator it = loadVector.begin();
  std::advance(it, nthPoint - 1);
  LoadVectorType::iterator nth = it;
  std::nth_element(loadVector.begin(), nth ,loadVector.end(), CompareLandmarkDisplacementError());

}

/**
 * Unselect landmarks
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::UnselectLandmarks(unsigned int nUnselected)
{
  if(nUnselected == 0)
    {
      return;
    }

  typedef std::vector<Load::Pointer> LoadVectorType;

  LoadContainerType *container = this->m_FEMObject->GetLoadContainer();

  if(!container)
    {
    itkExceptionMacro("Missing container");
    }

  LoadVectorType &loadVector = container->CastToSTLContainer();

  LoadVectorType::iterator it;
  it = loadVector.begin();
  std::advance(it, nUnselected - 1);
  LoadVectorType::iterator nth = it;

  for(it = loadVector.begin(); it <= nth; it++)
    {
    LoadNoisyLandmark * landmark = dynamic_cast<LoadNoisyLandmark*>((*it).GetPointer());

    if(landmark == NULL)
      {
      itkExceptionMacro("Encounter landmark that is not a LoadNoisyLandmark");
      }

    landmark->SetOutlier(true);
    }

}

/**
 * Delete outliers from the begining
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::DeleteFromLandmarkBeginning(unsigned int nDeleted)
{
  if(nDeleted == 0)
    {
    return;
    }

  typedef std::vector<Load::Pointer> LoadVectorType;

  LoadContainerType *container = this->m_FEMObject->GetLoadContainer();

  if(!container)
    {
    itkExceptionMacro("Missing container");
    }

  LoadVectorType &loadVector = container->CastToSTLContainer();

  LoadVectorType::iterator it;
  it = loadVector.begin();
  std::advance(it, nDeleted);
  LoadVectorType::iterator nth = it;
  loadVector.erase(loadVector.begin(), nth);

}

/**
 * Delete landmarks out of the mesh
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::DeleteLandmarksOutOfMesh()
{
  typedef itk::VectorContainer<typename FEMObjectType::LoadIdentifier, itk::fem::Load::Pointer> VectorContainerType;
  typename VectorContainerType::Pointer toRemoveLoadContainer = VectorContainerType::New();
  typename VectorContainerType::Pointer newLoadContainer = VectorContainerType::New();

  unsigned int count = 0;
  unsigned int numToRemoveLoads = 0;

  LoadContainerType *container = this->m_FEMObject->GetLoadContainer();

  if(!container)
    {
    itkExceptionMacro("Missing container");
    }

  unsigned int initialNumLoads = container->Size();

  for(LoadContainerIterator it = container->Begin(); it != container->End(); ++it)
    {
    count++;

    Load::Pointer load = it.Value();

    LoadNoisyLandmark *landmark = dynamic_cast<LoadNoisyLandmark*>(load.GetPointer());

    if(landmark == NULL)
      {
      itkExceptionMacro("Encounter landmark that is not a LoadNoisyLandmark");
      }

    if(landmark->IsOutOfMesh())
      {
      toRemoveLoadContainer->push_back(landmark);
      numToRemoveLoads++;
      }
    else
      {
      newLoadContainer->push_back(landmark);
      }
    }

  // no removals
  if(numToRemoveLoads == 0)
    {
    newLoadContainer->clear();
    return;
    }

  unsigned int newNumLoads = newLoadContainer->Size();

  if(initialNumLoads == newNumLoads)
    {
    itkExceptionMacro("Initial Num Loads equals New Num Loads");
    }

  // clear the containers
  container->clear();
  toRemoveLoadContainer->clear();

  // add the new loads to Load container
  for(LoadContainerIterator it = newLoadContainer->Begin(); it != newLoadContainer->End(); ++it)
    {
    this->m_FEMObject->AddNextLoad(it.Value());
    }

}

/**
 * PointTensorPonderation is changing throughout outlier rejection.
 * This function scales the point stiffness matrix by the updated
 * pointTensorPonderation
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::RescaleLandmarkStiffnessMatrix(double oldPointTensorPonderation)
{
  double pointTensorPonderation;

  pointTensorPonderation = this->GetLandmarkTensorPonderation() / oldPointTensorPonderation;

  this->m_ls->ScaleMatrix(pointTensorPonderation, m_LandmarkStiffnessMatrixIndex);

}

/**
 * Assemble the mechanical stiffness matrix from the mesh
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::AssembleMeshStiffnessMatrix()
{
  // if no DOFs exist in a system, we have nothing to do
  if( this->m_NGFN <= 0 )
    {
    return;
    }

  // assemble the mechanical matrix by stepping over all elements
  unsigned int numberOfElements = this->m_FEMObject->GetNumberOfElements();
  for( unsigned int i = 0; i < numberOfElements; i++)
    {
    // call the function that actually moves the element matrix to the master matrix.
    Element::Pointer e = this->m_FEMObject->GetElement(i);

    this->AssembleElementMatrixWithID(e, m_MeshStiffnessMatrixIndex);
    }

}

/**
 * Assemble the element stiffness matrix
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::AssembleElementMatrixWithID(Element::Pointer e, unsigned int matrixIndex)
{
  // copy the element stiffness matrix for faster access.
  Element::MatrixType Ke;

  e->GetStiffnessMatrix(Ke);

  // same for number of DOF
  const unsigned int Ne = e->GetNumberOfDegreesOfFreedom();

  // step over all rows in element matrix
  for( unsigned int j = 0; j < Ne; j++ )
    {
    // step over all columns in element matrix
    for( unsigned int k = 0; k < Ne; k++ )
      {
      // error checking. all GFN should be >= 0 and < NGFN
      const int dofj = e->GetDegreeOfFreedom(j);
      const int dofk = e->GetDegreeOfFreedom(k);
      if( dofj >= this->m_NGFN || dofk >= this->m_NGFN )
        {
        throw FEMExceptionSolution(__FILE__, __LINE__, "Solver::AssembleElementMatrix()", "Illegal GFN!");
        }

      if( Ke[j][k] != Float(0.0) )
        {
        this->m_ls->AddMatrixValue(dofj, dofk, Ke[j][k], matrixIndex);
        }
      }
    }
}

/**
 * Assemble the contribution matrix of the landmarks
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::AssembleLandmarkStiffnessMatrix()
{
  double pointTensorPonderation = this->GetLandmarkTensorPonderation();

  LoadContainerType *container = this->m_FEMObject->GetLoadContainer();

  if(!container)
    {
    itkExceptionMacro("Missing container");
    }

  LoadContainerIterator it = container->Begin();

  for(;it != container->End(); ++it)
    {
    Load::Pointer load = it.Value();

    LoadNoisyLandmark *landmark = dynamic_cast<LoadNoisyLandmark*>(load.GetPointer());

    if(landmark == NULL)
      {
      itkExceptionMacro("Encounter landmark that is not a LoadNoisyLandmark");
      }

    if(!landmark->IsOutlier())
      {
      double confidence = landmark->GetConfidence();

      const VectorType & realDisplacement = landmark->GetRealDisplacement();

      const MatrixType & tens = landmark->GetLandmarkTensor();

      const VectorType & shape = landmark->GetShape();

      Element::Pointer ep = landmark->GetContainedElement();

      const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
      const unsigned int Nnodes = ep->GetNumberOfNodes();

      // fill the diagonal matrices
      for(unsigned int k = 0; k < Nnodes; ++k)
        {
        double barCoor = shape[k] * shape[k];

        for(unsigned int n = 0; n < NnDOF; n++)
          {
          for(unsigned int m = 0; m < NnDOF; m++)
            {
            const int dofn = ep->GetDegreeOfFreedom(k * NnDOF + n);
            const int dofm = ep->GetDegreeOfFreedom(k * NnDOF + m);
            const float value = barCoor * m_TradeOffImageMeshEnergy * pointTensorPonderation * (tens(n,m)) * confidence;

            this->m_ls->AddMatrixValue(dofn, dofm, value, m_LandmarkStiffnessMatrixIndex);
            }
          }
        }

      // fill the extradiagonal matrices
      for(unsigned int i = 0; i < Nnodes - 1; i++)
        {
        for(unsigned int j = i + 1; j < Nnodes; j++)
          {
          double barCoor = shape[i] * shape[j];

          for(unsigned int n = 0; n < NnDOF; n++)
            {
            for(unsigned int m = 0; m < NnDOF; m++)
              {
              const int dofn = ep->GetDegreeOfFreedom(i * NnDOF + n);
              const int dofm = ep->GetDegreeOfFreedom(j * NnDOF + m);
              const float value = barCoor * m_TradeOffImageMeshEnergy * pointTensorPonderation * (tens(n, m)) * confidence;

              this->m_ls->AddMatrixValue(dofn, dofm, value, m_LandmarkStiffnessMatrixIndex);
              this->m_ls->AddMatrixValue(dofm, dofn, value, m_LandmarkStiffnessMatrixIndex);

              }
            }
          }
        }
      }
    }
}

/**
 * Remove the contribution of the unselected landmarks from the landmark
 * stiffness matrix
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::RemoveUnselectedLandmarkContributionInPointStiffnessMatrix()
{
  const double pointTensorPonderation = GetLandmarkTensorPonderation();

  itkDebugMacro("Removing unselected blocks contribution, " << "pointTensorPonderation is " << pointTensorPonderation);

  LoadContainerType *container = this->m_FEMObject->GetLoadContainer();

  if(!container)
    {
    itkExceptionMacro("Missing container");
    }

  LoadContainerIterator it = container->Begin();

  for(;it != container->End(); ++it)
    {
    Load::Pointer load = it.Value();

    LoadNoisyLandmark *landmark = dynamic_cast<LoadNoisyLandmark*>(load.GetPointer());

    if(landmark == NULL)
      {
      itkExceptionMacro("Encounter landmark that is not a LoadNoisyLandmark");
      }

    if(landmark->IsOutlier())
      {
      const float confidence = landmark->GetConfidence();

      const MatrixType & tens = landmark->GetLandmarkTensor();

      const VectorType & shape = landmark->GetShape();

      Element::ConstPointer ep = landmark->GetElement(0);

      const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
      const unsigned int Nnodes = ep->GetNumberOfNodes();

      for(unsigned int k = 0; k < Nnodes; ++k)
        {
        double barCoor = shape[k] * shape[k];

        for(unsigned int n = 0; n < NnDOF; n++)
          {
          for(unsigned int m = 0; m < NnDOF; m++)
            {
            const int dofn = ep->GetDegreeOfFreedom(k * NnDOF + n);
            const int dofm = ep->GetDegreeOfFreedom(k * NnDOF + m);
            const float value = -barCoor * m_TradeOffImageMeshEnergy * pointTensorPonderation * (tens(n, m)) * confidence;

            this->m_ls->AddMatrixValue(dofn, dofm, value, m_LandmarkStiffnessMatrixIndex);

            }
          }
        }

      for(unsigned int i = 0; i < Nnodes - 1; i++)
        {
        for(unsigned int j = i + 1; j < Nnodes; j++)
          {
          double barCoor = shape[i] * shape[j];

          for(unsigned int n = 0; n < NnDOF; n++)
            {
            for(unsigned int m = 0; m < NnDOF; m++)
              {
              const int dofn = ep->GetDegreeOfFreedom(i * NnDOF + n);
              const int dofm = ep->GetDegreeOfFreedom(j * NnDOF + m);
              const float value = -barCoor * m_TradeOffImageMeshEnergy * pointTensorPonderation * (tens(n,m)) * confidence;

              this->m_ls->AddMatrixValue(dofn, dofm, value, m_LandmarkStiffnessMatrixIndex);
              this->m_ls->AddMatrixValue(dofm, dofn, value, m_LandmarkStiffnessMatrixIndex);

              }
            }
          }
        }
      }
    }
}

template <unsigned int VDimension>
float
RobustSolver<VDimension>
::GetLandmarkTensorPonderation(void) const
{
  const LoadContainerType * loadContainer = this->m_FEMObject->GetLoadContainer();

  if(!loadContainer)
    {
    itkExceptionMacro("Missing load container");
    }

  const NodeContainerType * nodeContainer = this->m_FEMObject->GetNodeContainer();

  if(!nodeContainer)
    {
    itkExceptionMacro("Missing node container");
    }

  const float ponderation = (float)nodeContainer->Size() / loadContainer->Size();

  return ponderation;
}

template <unsigned int VDimension>
void
RobustSolver<VDimension>
::AssembleGlobalMatrixFromLandmarksAndMeshMatrices()
{
  this->m_ls->CopyMatrix(m_MeshStiffnessMatrixIndex, m_StiffnessMatrixIndex);
  this->m_ls->AddMatrixMatrix(m_StiffnessMatrixIndex, m_LandmarkStiffnessMatrixIndex);
}

template <unsigned int VDimension>
void
RobustSolver<VDimension>
::AssembleF()
{
  double pointTensorPonderation = GetLandmarkTensorPonderation();

  this->m_ls->InitializeVector(m_LandmarkForceIndex);

  LoadContainerType *container = this->m_FEMObject->GetLoadContainer();

  if(!container)
    {
    itkExceptionMacro("Missing container");
    }

  LoadContainerIterator it = container->Begin();

  for(;it != container->End(); ++it)
    {
    Load::Pointer load = it.Value();

    LoadNoisyLandmark *landmark = dynamic_cast<LoadNoisyLandmark*>(load.GetPointer());

    if(landmark == NULL)
      {
      itkExceptionMacro("Encounter landmark that is not a LoadNoisyLandmark");
      }

    if(!landmark->IsOutlier())
      {
      const double confidence = landmark->GetConfidence();

      const VectorType & realDisplacement = landmark->GetRealDisplacement();

      const MatrixType & tens = landmark->GetLandmarkTensor();

      const VectorType & shape = landmark->GetShape();

      Element::Pointer ep = landmark->GetContainedElement();

      const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
      const unsigned int Nnodes = ep->GetNumberOfNodes();

      for(unsigned int m = 0;m < Nnodes; ++m)
        {
        double barCoor = shape[m];

        const VectorType weightedRealDisplacement = confidence * barCoor * pointTensorPonderation * m_TradeOffImageMeshEnergy * ((tens) * realDisplacement);

        for(unsigned int j = 0; j < NnDOF; ++j)
          {
          const int index = ep->GetDegreeOfFreedom(m * NnDOF + j);
          this->m_ls->AddVectorValue(index, weightedRealDisplacement[j], m_LandmarkForceIndex);
          }
        }
      }
    }
}

template <unsigned int VDimension>
void
RobustSolver<VDimension>
::CalculateExternalForces()
{
  this->m_ls->MultiplyMatrixSolution(m_ExternalForceIndex, m_MeshStiffnessMatrixIndex, m_SolutionIndex);
}

/**
 * Add exteranl force to set the mesh energy to be zero, which
 * is equivalent to starting FEM solver from the deformed mesh
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::AddExternalForcesToSetMeshZeroEnergy()
{
  this->m_ls->CopyVector(m_LandmarkForceIndex, m_ForceIndex);
  this->m_ls->AddVectorVector(m_ForceIndex, m_ExternalForceIndex);
}

/**
 * Solve linear system of equations
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::SolveSystem()
{
  // note that VNL LS solver uses the matrix and vector with index zero to construct the LS
  this->m_ls->Solve();
}

/**
 * Initialize the interpolation grid
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::InitializeInterpolationGrid(const InterpolationGridRegionType& region,
                              const InterpolationGridPointType& origin,
                              const InterpolationGridSpacingType& spacing,
                              const InterpolationGridDirectionType& direction)
{
  InterpolationGridSizeType size = region.GetSize();
  for( unsigned int i = 0; i < this->FEMDimension; i++ )
    {
    if( size[i] == 0 )
      {
      itkExceptionMacro("Size must be specified.");
      }
    }

  this->m_InterpolationGrid = InterpolationGridType::New();
  this->m_InterpolationGrid->SetOrigin( origin );
  this->m_InterpolationGrid->SetSpacing( spacing );
  this->m_InterpolationGrid->SetDirection( direction );
  this->m_InterpolationGrid->SetRegions( region );
  this->m_InterpolationGrid->Allocate();

   // initialize all pointers in interpolation grid image to 0
  this->m_InterpolationGrid->FillBuffer(0);

  // fill the interpolation grid with proper pointers to elements
  unsigned int numberOfElements = this->m_FEMObject->GetNumberOfElements();
  for( unsigned int index = 0; index < numberOfElements; index++ )
    {
    Element::Pointer e = this->m_FEMObject->GetElement( index );
    // get square boundary box of an element
    VectorType v1 = e->GetNodeCoordinates(0);      // lower left corner
    VectorType v2 = v1;                            // upper right corner

    const unsigned int NumberOfDimensions = e->GetNumberOfSpatialDimensions();
    for( unsigned int n = 1; n < e->GetNumberOfNodes(); n++ )
      {
      const VectorType & v = e->GetNodeCoordinates(n);
      for( unsigned int d = 0; d < NumberOfDimensions; d++ )
        {
        if( v[d] < v1[d] )
          {
          v1[d] = v[d];
          }
        if( v[d] > v2[d] )
          {
          v2[d] = v[d];
          }
        }
      }

    // convert boundary box corner points into discrete image indexes.
    InterpolationGridIndexType vi1;
    InterpolationGridIndexType vi2;

    typedef Point<Float, FEMDimension> PointType;
    PointType vp1;
    PointType vp2;
    PointType pt;
    for( unsigned int i = 0; i < FEMDimension; i++ )
      {
      vp1[i] = v1[i];
      vp2[i] = v2[i];
      }

    // obtain the Index of BB corner and check whether it is within image.
    // if it is not, we ignore the entire element.
    if( !this->m_InterpolationGrid->TransformPhysicalPointToIndex(vp1, vi1) )
      {
      continue;
      }
    if( !this->m_InterpolationGrid->TransformPhysicalPointToIndex(vp2, vi2) )
      {
      continue;
      }

    InterpolationGridSizeType region_size;
    for( unsigned int i = 0; i < FEMDimension; i++ )
      {
      region_size[i] = vi2[i] - vi1[i] + 1;
      }

    InterpolationGridRegionType interRegion(vi1, region_size);

    // initialize the iterator that will step over all grid points within
    // element boundary box.
    ImageRegionIterator<InterpolationGridType> iter(this->m_InterpolationGrid, interRegion);

    // update the element pointers in the points defined within the region.
    VectorType global_point(NumberOfDimensions);

    VectorType local_point(NumberOfDimensions);

    // step over all points within the region
    for( iter.GoToBegin(); !iter.IsAtEnd(); ++iter )
      {
      // note: Iteratior is guarantied to be within image, since the
      // elements with BB outside are skipped before.
      this->m_InterpolationGrid->TransformIndexToPhysicalPoint(iter.GetIndex(), pt);
      for( unsigned int d = 0; d < NumberOfDimensions; d++ )
        {
        global_point[d] = pt[d];
        }

      // if the point is within the element, we update the pointer at
      // this point in the interpolation grid image.
      if( e->GetLocalFromGlobalCoordinates(global_point, local_point) )
        {
        iter.Set(e.GetPointer());
        }
      } // next point in region
    }   // next element
}

}
}  // end namespace itk::fem
#endif // __itkFEMRobustSolver_hxx
