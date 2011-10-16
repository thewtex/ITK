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

#ifndef __itkFEMRobustSolver_h
#define __itkFEMRobustSolver_h

#include "itkFEMSolver.h"
#include "itkFEMElementBase.h"
#include "itkFEMMaterialBase.h"
#include "itkFEMLoadBase.h"
#include "itkFEMLoadNoisyLandmark.h"
#include "itkFEMLinearSystemWrapperVNL.h"

#include "vnl/vnl_sparse_matrix.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_cholesky.h"
#include <vnl/vnl_sparse_matrix_linear_system.h>
#include <math.h>

namespace itk
{
namespace fem
{
/**
 * \class FEMRobustSolver
 * \brief A FEM solver characterized by accommodating outliers or
 * noises in landmarks and advancing approximation to interpolation.
 * This solver takes a FEMObject as input and outputs a deformed FEMObject.
 *
 * \code
 *       typedef itk::fem::FEMObject<3>    FEMObjectType;
 *       FEMObjectObjectType::Pointer fem = FEMObjectObjectType::New();
 *       ...
 *       typedef itk::fem::RobustSolver<3>    FEMSolverType;
 *       FEMSolverType::Pointer solver = FEMSolverType::New();
 *
 *       solver->SetInput( fem );
 *       solver->Update( );
 *       FEMSolverType::Pointer defem = solver->GetOutput( );
 *   ...
 * \endcode
 *
 * For the purpose of easy-of-use, a FEMScatteredDataPointSetToImageFilter is
 * developed to facilitate the use of this solver by hiding the details about
 * the FEMObject.
 * FEMScatteredDataPointSetToImageFilter takes a mesh and a feature point set as
 * inputs and converts them into a FEMObject, then calls this solver to find the
 * solution. Based on the solution and the user specified grid, a deformation
 * field is generated.
 *
 * \author Yixun Liu
 *
 * \par REFERENCE
 * O. Clatz, H. Delingette, I.-F. Talos, A. Golby, R. Kikinis, F. Jolesz,
 * N. Ayache, and S. Warfield, "Robust non-rigid registration to capture
 * brain shift from intra-operative MRI", IEEE Trans. Med. Imag., vol. 24,
 * no. 11, pp. 1417¨C1427, 2005.
 *
 *
 * \par REFERENCE
 * Andriy Fedorov, Ron Kikinis and Nikos Chrisochoides, "Real-time Non-rigid
 * Registration of Medical Images on a Cooperative Parallel Architecture",
 * IEEE International Conference on Bioinformatics & Biomedicine, pp. 401- 4,
 * November 2009.
 *
 *
 * \ingroup ITKFEM
 */

template <unsigned int VDimension = 3>
class RobustSolver : public Solver<VDimension>
{
public:
  /** Standard class typedefs. */
  typedef RobustSolver              Self;
  typedef Solver<VDimension>        Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro(RobustSolver, Solver<VDimension> );

  /** Inherit some types from the superclass. */
  typedef typename Superclass::VectorType       VectorType;
  typedef typename Superclass::Float            Float;
  typedef typename Superclass::
      InterpolationGridType                     InterpolationGridType;
  typedef typename Superclass::
      InterpolationGridPointerType              InterpolationGridPointerType;
  typedef typename Superclass::
      InterpolationGridSizeType                 InterpolationGridSizeType;
  typedef typename Superclass::
      InterpolationGridRegionType               InterpolationGridRegionType;
  typedef typename Superclass::
      InterpolationGridPointType                InterpolationGridPointType;
  typedef typename Superclass::
      InterpolationGridSpacingType              InterpolationGridSpacingType;
  typedef typename Superclass::
      InterpolationGridIndexType                InterpolationGridIndexType;
  typedef typename Superclass::
      InterpolationGridType                     InterpolationGridType;
  typedef typename InterpolationGridType::
      DirectionType                             InterpolationGridDirectionType;

  /** Some convenient types*/
  typedef typename Element::MatrixType                     MatrixType;
  typedef typename FEMObjectType::LoadContainerIterator    LoadContainerIterator;

  /**
   * Get the number of iterations run for the solver
   */
  itkGetConstMacro(Iterations, unsigned int);

  itkSetMacro(ApproximationSteps, unsigned int);
  itkGetMacro(ApproximationSteps, unsigned int);

  itkSetMacro(InterpolationSteps, unsigned int);
  itkGetMacro(InterpolationSteps, unsigned int);

  /*
  * Get/Set meta information for the interpolation grid
  */
  itkSetMacro(Origin, InterpolationGridPointType);
  itkGetMacro(Origin, InterpolationGridPointType);

  itkSetMacro(Spacing, InterpolationGridSpacingType);
  itkGetMacro(Spacing, InterpolationGridSpacingType);

  itkSetMacro(Region, InterpolationGridRegionType);
  itkGetMacro(Region, InterpolationGridRegionType);

  itkSetMacro(Direction, InterpolationGridDirectionType);
  itkGetMacro(Direction, InterpolationGridDirectionType);

  itkSetMacro(UseInterpolationGrid, bool);
  itkGetMacro(UseInterpolationGrid, bool);

  void PrintDisplacements();

  void PrintForce();

  itkSetMacro(TradeOffImageMeshEnergy, float);
  itkGetMacro(TradeOffImageMeshEnergy, float);

protected:

  RobustSolver();
  ~RobustSolver() { }

  /** Method invoked by the pipeline in order to trigger the computation of
   * the registration. */
  void  GenerateData();

  /**
   * Run the solver and produce a warped FEM object
   */
  virtual void RunSolver(void);

  /**
   * Initialize matrix, vector, solution, interpolation grid, and landmark.
   */
  void Initialization();

  /**
  * Initialize the interpolation grid, which will be used to accelerate the
  * locating of the element containing the image pixels or landmarks.
  */
  void InitializeInterpolationGrid(const InterpolationGridRegionType & region,
                              const InterpolationGridPointType & origin,
                              const InterpolationGridSpacingType & spacing,
                              const InterpolationGridDirectionType & direction);

  /**
  * Record the element, in which the landmark is located, and its local
  * coordinate.
  */
  void InitializeLandmark();

  /**
   * Assemble the global mechanical stiffness matrix from the mesh contained in
   * the FEMObject
   */
  void AssembleMeshStiffnessMatrix();

    /**
   * Assemble element stiffness matrix, which will be used to assemble the
   * global stiffness matrix
   */
  virtual void AssembleElementMatrix(Element::Pointer e,
      unsigned int matrixIndex);

  /**
  * Simulate the landmark as a physical point and
  * assemble its contribution matrix
  */
  void AssembleLandmarkStiffnessMatrix();

  /**
  * Add global stiffness matrix with landmark stiffness matrix
  */
  void AssembleGlobalMatrixFromLandmarksAndMeshMatrices();

  /**
  * Assemble right side F vector based on the landmarks
  */
  void AssembleF();

  /*
  * Solve iteratively, with outlier rejection,
  * from approximation to interpolation
  */
  void IncrementalSolver(unsigned int ApproximationSteps,
      unsigned int InterpolationSteps);

  /*
  * Solve iteratively, without outlier rejection,
  * from approximation to interpolation
  */
  void IncrementalApproximationSolver(unsigned int InterpolationSteps);

  /*
  * Solve LS
  */
  void SolveSystem(double epsilon);

  /*
  * Compute the approximation error for each landmark
  * for subsequent outlier rejection
  */
  void ComputeLandmarkSimulatedDisplacementAndWeightedError(
                              double displacementPonderation);

  /*
  * Compute the tensor associated with the landmark. The tensor is structural
  * weighted if a structural tensor point set is available
  */
  void ComputeLandmarkTensor();

  /*
  * Get scaling factor
  */
  float GetLandmarkTensorPonderation();

  /*
  * Sort the points in the decreasing order of error norm
  */
  void Nth_element_WRTDisplacementError(
       unsigned int numberOfRejectedBlocksPerStep);

  /*
  * Unselect landmark from landmark array
  */
  void UnselectLandmarks(unsigned int numberOfRejectedBlocksPerStep);

  /*
  * Remove the contribution of the unselected landmarks
  * from the landmark stiffness matrix
  */
  void RemoveUnselectedLandmarkContributionInPointStiffnessMatrix();

  /*
  * Delete outliers
  */
  void DeleteFromLandmarkBeginning(
      unsigned int numberOfRejectedLandmarksPerStep);

  /*
  * Delete landmarks out of the mesh.
  */
  void DeleteLandmarksOutOfMesh();

  /*
  * Adjust the landmark stiffness matrix based on
  * the change of the number of the landmarks
  */
  void RescaleLandmarkStiffnessMatrix(double oldPointTensorPonderation);

  /*
  * Add KU on the righ hand side to reach the effect of zeroing mesh energy
  */
  void AddExternalForcesToSetMeshZeroEnergy();

  unsigned int m_Iterations;

  unsigned int m_ApproximationSteps;
  unsigned int m_InterpolationSteps;

  unsigned int m_ForceIndex;
  unsigned int m_ExternalForceIndex;
  unsigned int m_SolutionIndex;
  unsigned int m_MeshStiffnessMatrixIndex;
  unsigned int m_LandmarkStiffnessMatrixIndex;
  unsigned int m_StiffnessMatrixIndex;

  float m_TradeOffImageMeshEnergy;

  float         m_ToleranceToLargeDispl;
  double        m_CgPrecision;
  double        m_FracErrorRejected;

  /*
  * for interpolation grid
  */
  InterpolationGridRegionType       m_Region;
  InterpolationGridPointType        m_Origin;
  InterpolationGridSpacingType      m_Spacing;
  InterpolationGridDirectionType    m_Direction;

  /*
  * Use interpolation grid to initialize the landmarks or not.
  * If use the grid, make sure the landmark is the grid point.
  * The landmarks (feature points) are usually the grid points if these landmarks
  * come from a feature point detection algorithm applied on an image.
  */
  bool m_UseInterpolationGrid;
};

/**
 * \class CompareLandmarkDisplacementError
 *  Comparison function object for sorting landmarks.
*/
class CompareLandmarkDisplacementError :
    public std::binary_function<Load::Pointer, Load::Pointer, bool>
{
public:
    bool operator()(Load::Pointer L1 , Load::Pointer L2)
    {
        LoadNoisyLandmark * l1 = dynamic_cast<LoadNoisyLandmark*>(L1.GetPointer());
        LoadNoisyLandmark * l2 = dynamic_cast<LoadNoisyLandmark*>(L2.GetPointer());

        return l1->GetErrorNorm() > l2->GetErrorNorm();
    }
};

/**
 * \class IsOutsideMesh

 * Unary predicate for removing landmarks, which are out of the mesh
*/
class IsOutsideMesh : public std::unary_function<Load::Pointer, bool>
{
public:
    bool operator()(Load::Pointer l)
    {
        LoadNoisyLandmark *nl = dynamic_cast<LoadNoisyLandmark*>(l.GetPointer());
        if(nl != NULL)
            return nl->IsOutlier();
        return false;
    }
};

}

}  // end namespace itk::fem

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFEMRobustSolver.hxx"
#endif

#endif // #ifndef __itkFEMRobustSolver_h
