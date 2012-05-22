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
#ifndef __VNLSparseLUSolverTraits_h
#define __VNLSparseLUSolverTraits_h

#include "vnl/vnl_vector.h"
#include "vnl/vnl_sparse_matrix.h"
#include "vnl/algo/vnl_sparse_lu.h"

template< typename T = double >
class VNLSparseLUSolverTraits
{
public:
  typedef T                               ValueType;
  typedef vnl_sparse_matrix< ValueType >  MatrixType;
  typedef vnl_vector< ValueType >         VectorType;
  typedef vnl_sparse_lu                   SolverType;

  VNLSparseLUSolverTraits() {}
  ~VNLSparseLUSolverTraits() {}

  bool IsDirectSolver() const
  {
    return true;
  }

  MatrixType InitializeSparseMatrix(const unsigned int & iN) const
  {
    return MatrixType(iN, iN);
  }

  MatrixType InitializeSparseMatrix(const unsigned int & iRow, const unsigned int& iCol) const
  {
    return MatrixType(iRow, iCol);
  }

  VectorType InitializeVector(const unsigned int & iN) const
  {
    return VectorType(iN);
  }

  void FillMatrix(MatrixType & iA, const unsigned int & iR, const unsigned int & iC, const ValueType & iV) const
  {
    iA(iR, iC) = iV;
  }

  void AddToMatrix(MatrixType & iA, const unsigned int & iR, const unsigned int & iC, const ValueType & iV) const
  {
    iA(iR, iC) += iV;
  }

  bool Solve(const MatrixType & iA, const VectorType & iB, VectorType & oX) const
  {
    SolverType solver( iA );
    oX = solver.solve( iB );

    return true;
  }

  bool Solve(const MatrixType & iA,
             const VectorType & iBx, const VectorType & iBy, const VectorType & iBz,
             VectorType & oX, VectorType & oY, VectorType & oZ ) const
  {
    SolverType solver( iA );
    oX = solver.solve( iBx );
    oY = solver.solve( iBy );
    oZ = solver.solve( iBz );

    return true;
  }

  // no interest to use this method...
  bool Solve(const MatrixType & iA,
             const VectorType & iBx, const VectorType & iBy,
             VectorType & oX, VectorType & oY) const
  {
    SolverType solver( iA );
    oX = solver.solve( iBx );
    oY = solver.solve( iBy );

    return true;
  }

};

#endif
