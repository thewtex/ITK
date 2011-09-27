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

#include "itkMacro.h"
#include "itkFEMLinearSystemWrapperPetSc.h"
#include "vxl_version.h"
#include <iostream>

namespace itk
{
namespace fem
{
void LinearSystemWrapperPetSc::InitializeMatrix(unsigned int matrixIndex)
{

  // allocate if necessary
  if( m_Matrices == 0 )
    {
    m_Matrices = new MatrixHolder(m_NumberOfMatrices);
    if( m_Matrices == NULL )
      {
      itkGenericExceptionMacro(<< "LinearSystemWrapperPetSc::InitializeMatrix(): m_Matrices allocation failed.");
      }
    }

  // out with old, in with new
  if( ( *m_Matrices )[matrixIndex] != 0 )
    {
    delete ( *m_Matrices )[matrixIndex];
    }

  ( *m_Matrices )[matrixIndex] = new MatrixRepresentation( this->GetSystemOrder(), this->GetSystemOrder() );
  if( ( *m_Matrices )[matrixIndex] == NULL )
    {
    itkGenericExceptionMacro(
      << "LinearSystemWrapperPetSc::InitializeMatrix(): allocation of (*m_Matrices)[" << matrixIndex << "] failed.");
    }

  return;
}

bool LinearSystemWrapperPetSc::IsMatrixInitialized(unsigned int matrixIndex)
{
  if( !m_Matrices )
    {
    return false;
    }
  if( !( ( *m_Matrices )[matrixIndex] ) )
    {
    return false;
    }

  return true;
}

void LinearSystemWrapperPetSc::DestroyMatrix(unsigned int matrixIndex)
{
  if( m_Matrices == 0 )
    {
    return;
    }
  if( ( *m_Matrices )[matrixIndex] == 0 )
    {
    return;
    }
  delete ( *m_Matrices )[matrixIndex];
  ( *m_Matrices )[matrixIndex] = 0;
}

void LinearSystemWrapperPetSc::InitializeVector(unsigned int vectorIndex)
{
  // allocate if necessary
  if( m_Vectors == 0 )
    {
    m_Vectors = new std::vector<PetscVec *>(m_NumberOfVectors);
    if( m_Vectors == NULL )
      {
      itkGenericExceptionMacro(<< "InitializeVector(): m_Vectors memory allocation failed.");
      }
    }

  // out with old, in with new
  if( ( *m_Vectors )[vectorIndex] != 0 )
    {
    delete ( *m_Vectors )[vectorIndex];
    }

  ( *m_Vectors )[vectorIndex] = new PetScVec( this->GetSystemOrder() );
  if( ( *m_Vectors )[vectorIndex] == NULL )
    {
    itkGenericExceptionMacro(<< "InitializeVector(): allocation of (*m_Vectors)[" << vectorIndex << "] failed.");
    }
  ( *m_Vectors )[vectorIndex]->fill(0.0);

  return;
}

bool LinearSystemWrapperPetSc::IsVectorInitialized(unsigned int vectorIndex)
{
  if( !m_Vectors )
    {
    return false;
    }
  if( !( *m_Vectors )[vectorIndex] )
    {
    return false;
    }

  return true;
}

void LinearSystemWrapperPetSc::DestroyVector(unsigned int vectorIndex)
{
  if( m_Vectors == 0 )
    {
    return;
    }
  if( ( *m_Vectors )[vectorIndex] == 0 )
    {
    return;
    }
  delete ( *m_Vectors )[vectorIndex];
  ( *m_Vectors )[vectorIndex] = 0;
}

void LinearSystemWrapperPetSc::InitializeSolution(unsigned int solutionIndex)
{
  // allocate if necessary
  if( m_Solutions == 0 )
    {
    m_Solutions = new std::vector<PetscVec *>(m_NumberOfSolutions);
    if( m_Solutions == NULL )
      {
      itkGenericExceptionMacro(<< "InitializeSolution(): m_Solutions memory allocation failed.");
      }
    }

  // out with old, in with new
  if( ( *m_Solutions )[solutionIndex] != 0 )
    {
    delete ( *m_Solutions )[solutionIndex];
    }

  ( *m_Solutions )[solutionIndex] = new PetscVec( this->GetSystemOrder() );
  if( ( *m_Solutions )[solutionIndex] == NULL )
    {
    itkGenericExceptionMacro(<< "InitializeSolution(): allocation of (*m_olutions)[" << solutionIndex << "] failed.");
    }
  ( *m_Solutions )[solutionIndex]->fill(0.0);

  return;
}

bool LinearSystemWrapperPetSc::IsSolutionInitialized(unsigned int solutionIndex)
{
  if( !m_Solutions )
    {
    return false;
    }
  if( !( *m_Solutions )[solutionIndex] )
    {
    return false;
    }

  return true;
}

void LinearSystemWrapperPetSc::DestroySolution(unsigned int solutionIndex)
{
  if( m_Solutions == 0 )
    {
    return;
    }
  if( ( *m_Solutions )[solutionIndex] == 0 )
    {
    return;
    }
  delete ( *m_Solutions )[solutionIndex];
  ( *m_Solutions )[solutionIndex] = 0;
}

LinearSystemWrapperPetSc::Float LinearSystemWrapperPetSc::GetSolutionValue(unsigned int i,
                                                                       unsigned int SolutionIndex) const
{
  if( m_Solutions == 0 )
    {
    return 0.0;
    }
  if( ( ( *m_Solutions )[SolutionIndex] )->size() <= i )
    {
    return 0.0;
    }
  else
    {
    return ( *( ( *m_Solutions )[SolutionIndex] ) )(i);
    }
}

void LinearSystemWrapperPetSc::Solve(void)
{
  if( ( m_Matrices->size() == 0 ) || ( m_Vectors->size() == 0 ) || ( m_Solutions->size() == 0 ) )
    {
    itkGenericExceptionMacro(
      << "LinearSystemWrapperPetSc::Solve(): m_Matrices, m_Vectors and m_Solutions size's are all zero.");
    }

  /*
   * Solve the sparse system of linear equation and store the result in m_Solutions(0).
   * Here we use the iterative least squares solver.
   */
  //std::cout<<"Creating the vnl_sparse_matrix_linear_system object\n";
  //vnl_sparse_matrix_linear_system<Float> ls( ( *( ( *m_Matrices )[0] ) ), ( *( ( *m_Vectors )[0] ) ) );
  //std::cout<<"Creating the vnl_lsqr object\n";
  //vnl_lsqr lsq(ls);

  //FIXME
#if 0
  VecAXPBY(externalForcesVector, 1, 1, weightedDisplacementVector);
  KSPSetOperators(ksp,sparseMechanicalStiffnessMatrix,sparseMechanicalStiffnessMatrix,DIFFERENT_NONZERO_PATTERN);
  KSPSetTolerances(ksp,epsilon,1.e-50,PETSC_DEFAULT, PETSC_DEFAULT);
  KSPSetFromOptions(ksp);
  KSPSetType(ksp, KSPCG);
  KSPSolve(ksp,externalForcesVector,solutionDisplacementVector);
#endif

  /*
   * Set max number of iterations to 3*size of the K matrix.
   * FIXME: There should be a better way to determine the number of iterations needed.
   */
  //lsq.set_max_iterations( 3 * this->GetSystemOrder() );
  //  std::cout<<"Performing lsq.minimize\n";
  //lsq.minimize( *( ( *m_Solutions )[0] ) );
}

void LinearSystemWrapperPetSc::SwapMatrices(unsigned int MatrixIndex1, unsigned int MatrixIndex2)
{
  vnl_sparse_matrix<Float> *tmp;
  tmp = ( *m_Matrices )[MatrixIndex1];
  ( *m_Matrices )[MatrixIndex1] = ( *m_Matrices )[MatrixIndex2];
  ( *m_Matrices )[MatrixIndex2] = tmp;
}

void LinearSystemWrapperPetSc::SwapVectors(unsigned int VectorIndex1, unsigned int VectorIndex2)
{
  PetscVec *tmp;
  tmp = ( *m_Vectors )[VectorIndex1];
  ( *m_Vectors )[VectorIndex1] = ( *m_Vectors )[VectorIndex2];
  ( *m_Vectors )[VectorIndex2] = tmp;
}

void LinearSystemWrapperPetSc::SwapSolutions(unsigned int SolutionIndex1, unsigned int SolutionIndex2)
{
  PetscVec *tmp;
  tmp = ( *m_Solutions )[SolutionIndex1];
  ( *m_Solutions )[SolutionIndex1] = ( *m_Solutions )[SolutionIndex2];
  ( *m_Solutions )[SolutionIndex2] = tmp;
}

void LinearSystemWrapperPetSc::CopySolution2Vector(unsigned int SolutionIndex, unsigned int VectorIndex)
{
  delete ( *m_Vectors )[VectorIndex];
  ( *m_Vectors )[VectorIndex] = new PetscVec( *( ( *m_Solutions )[SolutionIndex] ) );
}

void LinearSystemWrapperPetSc::CopyVector2Solution(unsigned int VectorIndex, unsigned int SolutionIndex)
{
  delete ( *m_Solutions )[SolutionIndex];
  ( *m_Solutions )[SolutionIndex] = new PetscVec( *( ( *m_Vectors )[VectorIndex] ) );
}

void LinearSystemWrapperPetSc::MultiplyMatrixMatrix(unsigned int ResultMatrixIndex,
                                                  unsigned int LeftMatrixIndex,
                                                  unsigned int RightMatrixIndex)
{
  delete ( *m_Matrices )[ResultMatrixIndex];

  MatMatMult( ( *m_Matrices )[LeftMatrixIndex] ), ( *m_Matrices )[RightMatrixIndex] ),
              MAT_INITIAL_MATRIX , PETSC_DEFAULT , *( ( *m_Matrices )[ResultMatrixIndex] ) );
}

void LinearSystemWrapperPetSc::MultiplyMatrixVector(unsigned int ResultVectorIndex,
                                                  unsigned int MatrixIndex,
                                                  unsigned int VectorIndex)
{
  delete ( *m_Vectors )[ResultVectorIndex];
  MatMult( ( *m_Matrices )[MatrixIndex] ),  *( ( *m_Vectors )[VectorIndex] ), *( ( *m_Vectors )[ResultVectorIndex] ));

}

void LinearSystemWrapperPetSc::ScaleMatrix(Float scale, unsigned int matrixIndex)
{
  for( ( ( *m_Matrices )[matrixIndex] )->reset(); ( ( *m_Matrices )[matrixIndex] )->next(); )
    {
    ( *( ( *m_Matrices )[matrixIndex] ) )( ( ( *m_Matrices )[matrixIndex] )->getrow(),
                                           ( ( *m_Matrices )[matrixIndex] )->getcolumn() ) =
      scale
      * ( *( ( *m_Matrices )[matrixIndex] ) )( ( ( *m_Matrices )[matrixIndex] )->getrow(),
                                               ( ( *m_Matrices )[matrixIndex] )->getcolumn() );
    }
}

LinearSystemWrapperPetSc::~LinearSystemWrapperPetSc()
{
  unsigned int i;
  for( i = 0; i < m_NumberOfMatrices; i++ )
    {
    this->DestroyMatrix(i);
    }
  for( i = 0; i < m_NumberOfVectors; i++ )
    {
    this->DestroyVector(i);
    }
  for( i = 0; i < m_NumberOfSolutions; i++ )
    {
    this->DestroySolution(i);
    }

  delete m_Matrices;
  delete m_Vectors;
  delete m_Solutions;
}

}
}  // end namespace itk::fem
