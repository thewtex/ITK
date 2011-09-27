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

#ifndef __itkFEMLinearSystemWrapperPetSc_h
#define __itkFEMLinearSystemWrapperPetSc_h
#include "itkFEMLinearSystemWrapper.h"

#include "petscksp.h"
#include <vector>

namespace itk
{
namespace fem
{
/**
 * \class LinearSystemWrapperPetSc
 * \brief LinearSystemWrapper class that uses PetSc numeric library functions
 *        to define a sparse linear system of equations.
 * \sa LinearSystemWrapper
 * \ingroup ITKFEM
 */
class LinearSystemWrapperPetSc : public LinearSystemWrapper
{
public:

  /* values stored in matrices & vectors */
  typedef LinearSystemWrapper::Float Float;

  /* superclass */
  typedef LinearSystemWrapper SuperClass;

  /* avoid conflicting with other matrix classes */
  typedef Mat PetscMat;

  /* avoid conflicting with other vector classes */
  typedef Vec PetscVec;

  /* further describe what we mean by KSP */
  typedef KSP PetscKSP;

  /* matrix typedef */
  typedef PetscMat MatrixRepresentation;

  /* matrix holder typedef */
  typedef std::vector<MatrixRepresentation *> MatrixHolder;

  /* constructor & destructor */
  LinearSystemWrapperPetSc() : LinearSystemWrapper(), m_Matrices(0), m_Vectors(0), m_Solutions(0)
  {
  }
  virtual ~LinearSystemWrapperPetSc();

  /* memory management routines */
  virtual void  InitializeMatrix(unsigned int matrixIndex);

  virtual bool  IsMatrixInitialized(unsigned int matrixIndex);

  virtual void  DestroyMatrix(unsigned int matrixIndex);

  virtual void  InitializeVector(unsigned int vectorIndex);

  virtual bool  IsVectorInitialized(unsigned int vectorIndex);

  virtual void  DestroyVector(unsigned int vectorIndex);

  virtual void  InitializeSolution(unsigned int solutionIndex);

  virtual bool  IsSolutionInitialized(unsigned int solutionIndex);

  virtual void  DestroySolution(unsigned int solutionIndex);

  virtual void  SetMaximumNonZeroValuesInMatrix(unsigned int, unsigned int)
  {
  }

  /* assembly & solving routines */
  virtual Float GetMatrixValue(unsigned int i, unsigned int j,
                               unsigned int matrixIndex) const
  {
    PetscScalar result;
    MatGetValue(*( ( *m_Matrices )[matrixIndex] ), i, j, &result );

    return(result);
  }
  virtual void  SetMatrixValue(unsigned int i, unsigned int j, Float value,
                               unsigned int matrixIndex)
  {
    MatSetValue(*( ( *m_Matrices )[matrixIndex] ), i, j, value, INSERT_VALUES );
  }
  virtual void  AddMatrixValue(unsigned int i, unsigned int j, Float value,
                               unsigned int matrixIndex)
  {
    MatSetValue(*( ( *m_Matrices )[matrixIndex] ), i, j, value, ADD_VALUES );
  }
  virtual Float GetVectorValue(unsigned int i,
                               unsigned int vectorIndex) const
  {

    PetscScalar v[3];
    PetscInt Ix[3];

    for(int j=0;j<3;j++)
      {
      Ix[j]=3*vectorIndex+j;
      }

    VecGetValues(*( ( *m_Vectors )[0]) , 3, Ix, v);

    return(static_cast<Float>(v[i]));
  }
  virtual void  SetVectorValue(unsigned int i, Float value,
                               unsigned int vectorIndex)
  {
    VecSetValue(*( ( *m_Vectors )[vectorIndex] ), i, value, INSERT_VALUES );
  }
  virtual void  AddVectorValue(unsigned int i, Float value,
                               unsigned int vectorIndex)
  {
    VecSetValue(*( ( *m_Vectors )[vectorIndex] ), i, value, ADD_VALUES );
  }
  virtual Float GetSolutionValue(unsigned int i,
                               unsigned int vectorIndex) const
  {
    PetscScalar v[3];
    PetscInt Ix[3];

    for(int j=0;j<3;j++)
      {
      Ix[j]=3*vectorIndex+j;
      }

    VecGetValues(*( ( *m_Vectors )[0]) , 3, Ix, v);

    return(static_cast<Float>(v[i]));
  }
  virtual void  SetSolutionValue(unsigned int i, Float value,
                               unsigned int vectorIndex)
  {
    VecSetValue(*( ( *m_Solutions )[vectorIndex] ), i, value, INSERT_VALUES );
  }
  virtual void  AddSolutionValue(unsigned int i, Float value,
                               unsigned int vectorIndex)
  {
    VecSetValue(*( ( *m_Solutions )[vectorIndex] ), i, value, ADD_VALUES );
  }

  virtual void  Solve(void);

  /* matrix & vector manipulation routines */
  virtual void  ScaleMatrix(Float scale, unsigned int matrixIndex);

  virtual void  SwapMatrices(unsigned int matrixIndex1, unsigned int matrixIndex2);

  virtual void  SwapVectors(unsigned int vectorIndex1, unsigned int vectorIndex2);

  virtual void  SwapSolutions(unsigned int solutionIndex1, unsigned int solutionIndex2);

  virtual void  CopySolution2Vector(unsigned solutionIndex, unsigned int vectorIndex);

  virtual void  CopyVector2Solution(unsigned int vectorIndex, unsigned int solutionIndex);

  virtual void  MultiplyMatrixMatrix(unsigned int resultMatrixIndex, unsigned int leftMatrixIndex,
                                     unsigned int rightMatrixIndex);

  virtual void  MultiplyMatrixVector(unsigned int resultVectorIndex, unsigned int matrixIndex, unsigned int vectorIndex);

private:

  /** vector of pointers to PetSc sparse matrices */
  // std::vector< vnl_sparse_matrix<Float>* > *m_Matrices;
  MatrixHolder *m_Matrices;

  /** vector of pointers to PetSc vectors  */
  std::vector<PetscVec *> *m_Vectors;

  /** vector of pointers to PetSc vectors */
  std::vector<PetscVec *> *m_Solutions;


};
}
}  // end namespace itk::fem

#endif
