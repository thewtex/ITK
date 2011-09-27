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
  //The VNL LS solver is the dafault LS solver.
  m_ForceIndex = 1;
  m_ExternalForceIndex = 0;
  m_SolutionIndex = 0;
  m_MeshStiffnessMatrixIndex = 1;
  m_LandmarkStiffnessMatrixIndex = 2;
  m_StiffnessMatrixIndex = 0;

  m_Iterations = 0;

  m_ApproximationSteps = 10;
  m_InterpolationSteps = 0;

  m_ToleranceToLargeDispl = .5;
  m_CgPrecision = 1e-3;
  m_FracErrorRejected=.25;

  m_TradeOffImageMeshEnergy = 1.0;

  //Initialize parameters for the interpolation grid
  InterpolationGridSizeType size;
  size.Fill(0);
  m_Region.SetSize(size);
  m_Direction.SetIdentity();
  m_Origin.Fill(0.0);
  m_Spacing.Fill(1.0);

}

template <unsigned int VDimension>
void
RobustSolver<VDimension>
::Initialization()
{
  //the NGFN is finalized once the FEMObject is set as input.
  this->m_ls->SetSystemOrder(m_NGFN);
  this->m_ls->SetNumberOfVectors(2);
  this->m_ls->SetNumberOfSolutions(1);
  this->m_ls->SetNumberOfMatrices(3);
  this->m_ls->InitializeMatrix(m_MeshStiffnessMatrixIndex);
  this->m_ls->InitializeMatrix(m_LandmarkStiffnessMatrixIndex);
  this->m_ls->InitializeMatrix(m_StiffnessMatrixIndex);
  this->m_ls->InitializeVector(m_ForceIndex);
  this->m_ls->InitializeVector(m_ExternalForceIndex);
  this->m_ls->InitializeSolution(m_SolutionIndex);

  this->InitializeInterpolationGrid(m_Region,m_Origin, m_Spacing, m_Direction);

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
    InterpolationGridIndexType ind;
    VectorType localPos;
    VectorType globalPos;
    InterpolationGridPointType point;
    LoadContainerIterator it = m_FEMObject->GetLoadContainer()->Begin();

    for(; it != m_FEMObject->GetLoadContainer()->End(); ++it)
    {
        NoisyLoadLandmark * landmark = dynamic_cast<NoisyLoadLandmark*>
            ((it.Value()).GetPointer());

        globalPos = landmark->GetSource();
    for( unsigned int i = 0; i < FEMDimension; i++ )
      {
        point[i] = globalPos[i];
      }

        if(m_UseInterpolationGrid)
        {
            //landmark is within the interpolation grid
            if( m_InterpolationGrid->TransformPhysicalPointToIndex(point,
                ind) )
            {
                Element::ConstPointer e = m_InterpolationGrid->GetPixel(ind).
                    GetPointer();

                //landmark is inside the mesh
                if(e.IsNotNull())
                {
                    // TODO: loadmark should provide a interface to set element
                    landmark->SetContainedElement(e);
                    e->GetLocalFromGlobalCoordinates(globalPos, localPos);
                        landmark->SetShape(e->ShapeFunctions(localPos));

                }
                else
                {
                    //remove the landmark
                    landmark->SetIsOutOfMesh(true);

                }

            }

        }
        else
        {
            //TODO: return a flag rather than signaling an exception
            landmark->AssignToElement(m_FEMObject->GetElementContainer() );
            // dynamic_cast< LoadLandmark * >( &( *( *l2 ) ) ) )
            Element::ConstPointer ep = landmark->GetElement(0).GetPointer();
            localPos = landmark->GetPoint();
            landmark->SetShape(ep->ShapeFunctions(localPos));
        }
    }
    //remove landmarks outside of the mesh
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
  //Initialize matrix, vector, solution, interpolation grid, and landmark.
  this->Initialization();

  //LS solver, which can be a VNL solver or a PETSc solver
  this->RunSolver();

  // copy the input to the output and add the
  //displacements to update the nodal co-ordinates
  this->GetOutput()->DeepCopy(this->GetInput() );
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

      if(m_ApproximationSteps != 0)
        this->IncrementalSolver(m_ApproximationSteps, m_InterpolationSteps);
      else// do "interpolation" only (no points discarded)
        this->IncrementalApproximationSolver(m_InterpolationSteps);
}

/**
 * Solve the displacement vector U with outlier rejection
 */
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::IncrementalSolver(unsigned int ApproximationSteps,
                    unsigned int InterpolationSteps)
{
  unsigned int numberOfRejectedLandmarksPerStep =
      (unsigned int) floor (double(m_FEMObject->GetNumberOfNodes()) *
      m_FracErrorRejected / (double(ApproximationSteps)));

  unsigned int j = 0;
  itkDebugMacro("numberOfRejectedLandmarksPerStep " <<
      numberOfRejectedLandmarksPerStep);

  double oldPointTensorPonderation;
  for (unsigned int i = 0; i < ApproximationSteps; ++i)
  {
    j++;
    //get scaling parameter before outlier rejection
    oldPointTensorPonderation = this->GetLandmarkTensorPonderation();

    //solve linear system of equations
    this->SolveSystem(m_CgPrecision);
    itkDebugMacro("System solved");

    //Compute simulated error for sorting
    this->ComputeLandmarkSimulatedDisplacementAndWeightedError(
        m_ToleranceToLargeDispl);

    // sort the points in the *decreasing* order of error norm
    this->Nth_element_WRTDisplacementError(numberOfRejectedLandmarksPerStep);

    // set first n to "unselected", and decrease
    //numberOfSelectedBlocks accordingly
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

    this->SolveSystem(m_CgPrecision);
    itkDebugMacro("system resolved");

    this->AddExternalForcesToSetMeshZeroEnergy();
    itkDebugMacro("approximation step " << j) ;

  }

  for (unsigned int i = 0; i < InterpolationSteps; ++i)
  {
    j++;

    this->SolveSystem(m_CgPrecision);

    this->AddExternalForcesToSetMeshZeroEnergy();
    itkDebugMacro("interpolation step " << j) ;
  }
}

/**
* Solve the displacement vector U without outlier rejection
*/
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::IncrementalApproximationSolver(unsigned int InterpolationSteps)
{
    unsigned int i;

    for (i = 0; i < InterpolationSteps; ++i)
    {
        this->SolveSystem(m_CgPrecision);

        this->AddExternalForcesToSetMeshZeroEnergy();
        itkDebugMacro("Interpolation step " << i);
    }

}

/**
* Compute the approximation error for each landmark for subsequent outlier
* rejection
*/
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::ComputeLandmarkSimulatedDisplacementAndWeightedError(
    double displacementPonderation)
{
    double lambda = displacementPonderation;
    double displacementNorm;
    VectorType structureTensorPonderatedError;
    VectorType shape;

    LoadContainerIterator it = m_FEMObject->GetLoadContainer()->Begin();

    for(; it != m_FEMObject->GetLoadContainer()->End(); ++it)
    {
        NoisyLoadLandmark * landmark = dynamic_cast<NoisyLoadLandmark*>
            ((it.Value()).GetPointer());

        if(!landmark->IsOutlier())
        {
            shape = landmark->GetShape();

            Element::ConstPointer ep = landmark->GetElement(0).GetPointer();

            const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
            const unsigned int Nnodes = ep->GetNumberOfNodes();

            VectorType error(NnDOF, 0.0);
            VectorType nodeSolution(NnDOF);

            for(unsigned int m = 0; m < Nnodes; m++)
            {
                for(unsigned int j = 0; j < NnDOF; ++j)
                    nodeSolution[j] = m_ls->GetSolutionValue(
                    ep->GetDegreeOfFreedom(m*NnDOF+j),m_SolutionIndex);

                error += shape[m] * nodeSolution;

            }

            landmark->SetSimulatedDisplacement(error);
            displacementNorm = error.two_norm();
            error = landmark->GetRealDisplacement() - error;

            if(landmark->HasStructureTensor())
                structureTensorPonderatedError =
                  landmark->GetStructureTensor() * landmark->GetConfidence() *
                  (error / ((1-lambda) * displacementNorm + lambda));
            else
                structureTensorPonderatedError = (error / ((1-lambda) *
                displacementNorm + lambda)) * landmark->GetConfidence();

            landmark->SetErrorNorm(structureTensorPonderatedError.two_norm());
        }
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

    LoadContainerIterator it = this->m_FEMObject->GetLoadContainer()->Begin();

    for(; it != m_FEMObject->GetLoadContainer()->End(); ++it)
    {
        NoisyLoadLandmark * landmark = dynamic_cast<NoisyLoadLandmark*>
            ((it.Value()).GetPointer());

        if(!landmark->IsOutlier())
        {
            shape = landmark->GetShape();

            Element::ConstPointer ep = landmark->GetElement(0).GetPointer();

            const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
            const unsigned int Nnodes = ep->GetNumberOfNodes();

            MatrixType nodeTensor(NnDOF, NnDOF);
            MatrixType landmarkTensor(NnDOF, NnDOF);

            landmarkTensor.fill(0.0);

            for(unsigned int m = 0; m < Nnodes; ++m)
            {
                for(unsigned int i=0; i<NnDOF; i++)
                    for(unsigned int j=0; j<NnDOF; j++)
                    {
                        nodeTensor[i][j] = m_ls->GetMatrixValue(
                        ep->GetDegreeOfFreedom(m*NnDOF+i),
                        ep->GetDegreeOfFreedom(m*NnDOF+j),
                        m_MeshStiffnessMatrixIndex);
                    }

                landmarkTensor += nodeTensor*shape[m];
            }

            if(landmark->HasStructureTensor())
                landmark->SetLandmarkTensor(landmarkTensor *
                    landmark->GetStructureTensor());
            else
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
    typedef std::vector<Load::Pointer> LoadVectorType;
    LoadVectorType loadVector = m_FEMObject->GetLoadContainer()
        ->CastToSTLContainer();
    LoadVectorType::iterator it;
    it = loadVector.begin();
    std::advance(it, nthPoint - 1);
    LoadVectorType::iterator nth = it;
    std::nth_element(loadVector.begin(), nth ,loadVector.end(),
    CompareLandmarkDisplacementError());

}

/**
* Unselect landmarks
*/
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::UnselectLandmarks(unsigned int nUnselected)
{
    typedef std::vector<Load::Pointer> LoadVectorType;
    LoadVectorType loadVector = m_FEMObject->GetLoadContainer()
        ->CastToSTLContainer();
    LoadVectorType::iterator it;
    it = loadVector.begin();
    std::advance(it, nUnselected - 1);
    LoadVectorType::iterator nth = it;

    for(it = loadVector.begin(); it != loadVector.end(); it++)
    {
        NoisyLoadLandmark * landmark = dynamic_cast<NoisyLoadLandmark*>
            ((*it).GetPointer());
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

    typedef std::vector<Load::Pointer> LoadVectorType;
    LoadVectorType loadVector = m_FEMObject->GetLoadContainer()
        ->CastToSTLContainer();
    LoadVectorType::iterator it;
    it = loadVector.begin();
    std::advance(it, nDeleted - 1);
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
    typedef std::vector<Load::Pointer> LoadVectorType;
    LoadVectorType loadVector = m_FEMObject->GetLoadContainer()
        ->CastToSTLContainer();
    std::remove_if(loadVector.begin(), loadVector.end(),IsOutsideMesh());
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

    pointTensorPonderation = this->GetLandmarkTensorPonderation() /
        oldPointTensorPonderation;

    m_ls->ScaleMatrix(pointTensorPonderation, m_LandmarkStiffnessMatrixIndex);

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
        return;

    // Assemble the mechanical matrix by stepping over all elements
    unsigned int numberOfElements = m_FEMObject->GetNumberOfElements();
    for( unsigned int i = 0; i < numberOfElements; i++)
    {
        // Call the function that actually moves the element matrix
        // to the master matrix.
        Element::Pointer e = m_FEMObject->GetElement(i);

        this->AssembleElementMatrix(e, m_MeshStiffnessMatrixIndex);
    }

}

/**
* Assemble the element stiffness matrix
*/
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::AssembleElementMatrix(Element::Pointer e, unsigned int matrixIndex)
{
  // Copy the element stiffness matrix for faster access.
  Element::MatrixType Ke;

  e->GetStiffnessMatrix(Ke);

  // ... same for number of DOF
  int Ne = e->GetNumberOfDegreesOfFreedom();
  // step over all rows in element matrix
  for( unsigned int j = 0; j < Ne; j++ )
    {
    // step over all columns in element matrix
    for( unsigned int k = 0; k < Ne; k++ )
      {
      // error checking. all GFN should be =>0 and <NGFN
      if( e->GetDegreeOfFreedom(j) >= this->m_NGFN
          || e->GetDegreeOfFreedom(k) >= this->m_NGFN  )
        {
        throw FEMExceptionSolution(__FILE__, __LINE__,
            "Solver::AssembleElementMatrix()", "Illegal GFN!");
        }

      if( Ke[j][k] != Float(0.0) )
        {
        this->m_ls->AddMatrixValue(e->GetDegreeOfFreedom(j),
            e->GetDegreeOfFreedom(k), Ke[j][k], matrixIndex);
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
    double barCoor;
    MatrixType tens;
    VectorType shape;
    VectorType realDisplacement;
    double confidence;

    double pointTensorPonderation = this->GetLandmarkTensorPonderation();

    LoadContainerIterator it = m_FEMObject->GetLoadContainer()->Begin();

    for(;it != m_FEMObject->GetLoadContainer()->End(); ++it)
    {
        NoisyLoadLandmark * landmark = dynamic_cast<NoisyLoadLandmark*>
            ((it.Value()).GetPointer());
        if(!landmark->IsOutlier())
        {
            confidence = landmark->GetConfidence();

            realDisplacement = landmark->GetRealDisplacement();

            tens = landmark->GetLandmarkTensor();

            shape = landmark->GetShape();

            Element::ConstPointer ep = landmark->GetElement(0).GetPointer();

            const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
            const unsigned int Nnodes = ep->GetNumberOfNodes();

            //fill the diagonal matrices
            for(unsigned int k = 0; k < Nnodes; ++k)
            {
                //i = (*it)->getAssociatedVertice(m)->getRef();
                barCoor = shape[k] * shape[k];

                for(unsigned int n = 0; n < NnDOF; n++)
                    for(unsigned int m = 0; m < NnDOF; m++)
                    {
                        m_ls->AddMatrixValue(ep->GetDegreeOfFreedom(k * NnDOF + n),
                            ep->GetDegreeOfFreedom(k * NnDOF + m),
                            barCoor * m_TradeOffImageMeshEnergy *
                            pointTensorPonderation * (tens(n,m)) * confidence,
                            m_LandmarkStiffnessMatrixIndex);
                    }
            }

            //fill the extradiagonal matrices
            for(unsigned int i = 0; i < Nnodes - 1; i++)
                for(unsigned int j = i + 1; j < Nnodes; j++)
                {
                    barCoor = shape[i] * shape[j];

                    for(unsigned int n = 0; n < NnDOF; n++)
                        for(unsigned int m = 0; m < NnDOF; m++)
                        {
                            m_ls->AddMatrixValue(
                                ep->GetDegreeOfFreedom(i * NnDOF + n),
                                ep->GetDegreeOfFreedom(j * NnDOF + m),
                                barCoor * m_TradeOffImageMeshEnergy *
                                pointTensorPonderation * (tens(n, m)) *
                                confidence, m_LandmarkStiffnessMatrixIndex);

                            m_ls->AddMatrixValue(ep->GetDegreeOfFreedom(
                                j * NnDOF + m),
                                ep->GetDegreeOfFreedom(i * NnDOF + n),
                                barCoor * m_TradeOffImageMeshEnergy *
                                pointTensorPonderation * (tens(n, m)) *
                                confidence, m_LandmarkStiffnessMatrixIndex);

                        }

                }

        }

    }

}

/*
* Remove the contribution of the unselected landmarks from the landmark
* stiffness matrix
*/
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::RemoveUnselectedLandmarkContributionInPointStiffnessMatrix()
{
    double barCoor;
    MatrixType tens;
    VectorType realDisplacement;
    VectorType shape;
    double pointTensorPonderation = GetLandmarkTensorPonderation();
    float confidence;

    itkDebugMacro("Removing unselected blocks contribution, " <<
        "pointTensorPonderation is " <<
        pointTensorPonderation);

    LoadContainerIterator it = m_FEMObject->GetLoadContainer()->Begin();

    for(;it != m_FEMObject->GetLoadContainer()->End(); ++it)
    {
        NoisyLoadLandmark * landmark = dynamic_cast<NoisyLoadLandmark*>
            ((it.Value()).GetPointer());

        if(!landmark->IsOutlier())
        {
            confidence = landmark->GetConfidence();

            realDisplacement = landmark->GetRealDisplacement();

            tens = landmark->GetLandmarkTensor();

            shape = landmark->GetShape();

            Element::ConstPointer ep = landmark->GetElement(0).GetPointer();

            const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
            const unsigned int Nnodes = ep->GetNumberOfNodes();

            for(unsigned int k = 0; k < Nnodes; ++k)
            {
                barCoor = shape[k] * shape[k];

                for(unsigned int n = 0; n < NnDOF; n++)
                    for(unsigned int m = 0; m < NnDOF; m++)
                    {
                        m_ls->AddMatrixValue(
                            ep->GetDegreeOfFreedom(k * NnDOF + n),
                            ep->GetDegreeOfFreedom(k * NnDOF + m),
                            -barCoor * m_TradeOffImageMeshEnergy *
                            pointTensorPonderation * (tens(n, m)) *
                            confidence, m_LandmarkStiffnessMatrixIndex);

                    }
            }

            for(unsigned int i = 0; i < Nnodes - 1; i++)
                for(unsigned int j = i + 1; j < Nnodes; j++)
                {
                    barCoor = shape[i] * shape[j];

                    for(unsigned int n = 0; n < NnDOF; n++)
                        for(unsigned int m = 0; m < NnDOF; m++)
                        {
                            m_ls->AddMatrixValue(
                                ep->GetDegreeOfFreedom(i * NnDOF + n),
                                ep->GetDegreeOfFreedom(j * NnDOF + m),
                                -barCoor * m_TradeOffImageMeshEnergy *
                                pointTensorPonderation * (tens(n,m)) *
                                confidence, m_LandmarkStiffnessMatrixIndex);

                            m_ls->AddMatrixValue(
                                ep->GetDegreeOfFreedom(j * NnDOF + m),
                                ep->GetDegreeOfFreedom(i * NnDOF + n),
                                -barCoor * m_TradeOffImageMeshEnergy *
                                pointTensorPonderation * (tens(n,m)) *
                                confidence, m_LandmarkStiffnessMatrixIndex);

                        }
                }

        }

    }

}

template <unsigned int VDimension>
float
RobustSolver<VDimension>
::GetLandmarkTensorPonderation(void)
{
  double ptp = (double)this->m_FEMObject->GetNodeContainer()->Size() /
      (double)this->m_FEMObject->GetLoadContainer()->Size();

  return ptp;
}

template <unsigned int VDimension>
void
RobustSolver<VDimension>
::AssembleGlobalMatrixFromLandmarksAndMeshMatrices()
{
    m_ls->CopyMatrix(m_MeshStiffnessMatrixIndex, m_StiffnessMatrixIndex);
    m_ls->AddMatrixMatrix(m_StiffnessMatrixIndex, m_LandmarkStiffnessMatrixIndex);
}

template <unsigned int VDimension>
void
RobustSolver<VDimension>
::AssembleF()
{
    double barCoor;
    VectorType realDisplacement;
    VectorType weightedRealDisplacement;
    VectorType realUpdatedCenterBlockPosition;
    VectorType realClosestPosition;
    VectorType shape;
    MatrixType tens;
    double pointTensorPonderation = GetLandmarkTensorPonderation();
    double confidence;

    m_ls->InitializeVector(m_ForceIndex);

    LoadContainerIterator it = m_FEMObject->GetLoadContainer()->Begin();

    for(;it != m_FEMObject->GetLoadContainer()->End(); ++it)
    {
        NoisyLoadLandmark * landmark = dynamic_cast<NoisyLoadLandmark*>
            ((it.Value()).GetPointer());

        if(!landmark->IsOutlier())
        {
            confidence = landmark->GetConfidence();

            realDisplacement = landmark->GetRealDisplacement();

            tens = landmark->GetLandmarkTensor();

            shape = landmark->GetShape();

            Element::ConstPointer ep = landmark->GetElement(0).GetPointer();

            const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
            const unsigned int Nnodes = ep->GetNumberOfNodes();

            for(unsigned int m = 0;m < Nnodes; ++m)
            {
                barCoor = shape[m];

                weightedRealDisplacement = confidence * barCoor *
                    pointTensorPonderation * m_TradeOffImageMeshEnergy *
                    ((tens) * realDisplacement);

                for(unsigned int j = 0; j < NnDOF; ++j)
                    m_ls->AddVectorValue(ep->GetDegreeOfFreedom(m * NnDOF + j),
                    weightedRealDisplacement[j], m_ForceIndex);
            }

        }
    }

}

/**
* MultiplyMatrixSolution is not implemented now.
*/
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::AddExternalForcesToSetMeshZeroEnergy()
{
    //TODO: support the following function in the LinearSystemWrapper.
    //m_ls->MultiplyMatrixSolution(m_ExternalForceIndex, m_MeshStiffnessMatrixIndex,
    //	m_SolutionIndex);
}

/*
* Solve linear system of equations
*/
template <unsigned int VDimension>
void
RobustSolver<VDimension>
::SolveSystem(double epsilon)
{
    m_ls->AddVectorVector(m_ExternalForceIndex,m_ForceIndex);

    //Note that VNL LS solver uses the matrix and vector with index zero to
    //constitute the LS
    m_ls->Solve();

    m_ls->InitializeVector(m_ExternalForceIndex);

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
  for( unsigned int i = 0; i < FEMDimension; i++ )
    {
    if( size[i] == 0 )
      {
      itkExceptionMacro("Size must be specified.");
      }
    }

  // Discard any old image object an create a new one
  m_InterpolationGrid = InterpolationGridType::New();
  m_InterpolationGrid->SetOrigin( origin );
  m_InterpolationGrid->SetSpacing( spacing );
  m_InterpolationGrid->SetDirection( direction );
  m_InterpolationGrid->SetRegions( region );
  m_InterpolationGrid->Allocate();

   // Initialize all pointers in interpolation grid image to 0
  m_InterpolationGrid->FillBuffer(0);

  VectorType v1, v2;

  // Fill the interpolation grid with proper pointers to elements
  unsigned int numberOfElements = m_FEMObject->GetNumberOfElements();
  for( unsigned int index = 0; index <= numberOfElements; index++ )
    {
    Element::Pointer e = m_FEMObject->GetElement( index );
    // Get square boundary box of an element
    v1 = e->GetNodeCoordinates(0);      // lower left corner
    v2 = v1;                            // upper right corner

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

    // Convert boundary box corner points into discrete image indexes.
    InterpolationGridIndexType vi1, vi2;

    Point<Float, FEMDimension> vp1, vp2, pt;
    for( unsigned int i = 0; i < FEMDimension; i++ )
      {
      vp1[i] = v1[i];
      vp2[i] = v2[i];
      }

    // Obtain the Index of BB corner and check whether it is within image.
    // If it is not, we ignore the entire element.
    if( !m_InterpolationGrid->TransformPhysicalPointToIndex(vp1, vi1) )
      {
      continue;
      }
    if( !m_InterpolationGrid->TransformPhysicalPointToIndex(vp2, vi2) )
      {
      continue;
      }

    InterpolationGridSizeType region_size;
    for( unsigned int i = 0; i < FEMDimension; i++ )
      {
      region_size[i] = vi2[i] - vi1[i] + 1;
      }
    InterpolationGridRegionType region(vi1, region_size);

    // Initialize the iterator that will step over all grid points within
    // element boundary box.
    ImageRegionIterator<InterpolationGridType> iter(m_InterpolationGrid, region);

    // Update the element pointers in the points defined within the region.
    VectorType global_point(NumberOfDimensions);

    VectorType local_point;

    // Step over all points within the region
    for( iter.GoToBegin(); !iter.IsAtEnd(); ++iter )
      {
      // Note: Iteratior is guarantied to be within image, since the
      //       elements with BB outside are skipped before.
      m_InterpolationGrid->TransformIndexToPhysicalPoint(iter.GetIndex(), pt);
      for( unsigned int d = 0; d < NumberOfDimensions; d++ )
        {
        global_point[d] = pt[d];
        }

      // If the point is within the element, we update the pointer at
      // this point in the interpolation grid image.
      if( e->GetLocalFromGlobalCoordinates(global_point, local_point) )
        {
            iter.Set(e.GetPointer());
        }
      } // next point in region
    }   // next element
}

template <unsigned int VDimension>
void
RobustSolver<VDimension>
::PrintDisplacements()
{
  std::cout <<  " printing current displacements " << std::endl;
  for( unsigned int i = 0; i < this->m_NGFN; i++ )
    {
    itkDebugMacro(this->m_ls->GetSolutionValue(i, m_TotalSolutionIndex));
    }
}

template <unsigned int VDimension>
void
RobustSolver<VDimension>
::PrintForce()
{
  std::cout <<  " printing current forces " << std::endl;
  for( unsigned int i = 0; i < this->m_NGFN; i++ )
    {
    itkDebugMacro(this->m_ls->GetVectorValue(i, m_ForceTIndex));
    }
}

}
}  // end namespace itk::fem
#endif // __itkFEMRobustSolver_hxx
