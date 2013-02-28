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
#ifndef __VNLSparseSymmetricEigensystemTraits_h
#define __VNLSparseSymmetricEigensystemTraits_h

#include "vnl/vnl_vector.h"
#include "vnl/vnl_sparse_matrix.h"
#include "vnl/algo/vnl_sparse_symmetric_eigensystem.h"

/** \class VNLSparseSymmetricEigensystemTraits
 * \brief Generic interface for finding the eigenvalues of a sparse symmetric matrix
 *
 * This generic interface allows interchange of sparse symmetric eigensystem calculators
 * See itk::ParameterizationQuadEdgeMeshFilter for reference.
 *
 * It internally uses the VNL library to represent and deal with vectors
 * (vnl_vector) and sparse matrices (vnl_sparse_matrix).
 * see vnl_sparse_symmetric_eigensystem for more details on the method used.
 *
 * \ingroup ITKCommon
 *
 * \sa VNLSparseSymmetricEigensystemTraits
 */
template <typename T = double>
class VNLSparseSymmetricEigensystemTraits
{
public:
  typedef T                                ValueType;
  typedef vnl_sparse_matrix<ValueType>     MatrixType;
  typedef vnl_matrix<ValueType>            EigenvectorMatrixType;
  typedef vnl_vector<ValueType>            VectorType;
  typedef vnl_sparse_symmetric_eigensystem EigenSystemType;

  /** \brief initialize a symmetric sparse matrix of size iN x iN */
  static MatrixType InitializeSparseMatrix(const unsigned int & iN)
  {
    return MatrixType(iN, iN);
  }

  /** \brief initialize a vector of size iN */
  static VectorType InitializeVector(const unsigned int & iN)
  {
    return VectorType(iN);
  }

  /** \brief iA[iR][iC] = iV */
  static void FillMatrix(MatrixType & iA, const unsigned int & iR, const unsigned int & iC, const ValueType & iV)
  {
    iA(iR, iC) = iV;
  }

  /** \brief Find n eigenvalue/eigenvectors of the eigenproblem A * x = lambda * x \f$ */
  static bool Calculate(const MatrixType & iA, unsigned int nEig,
                        EigenvectorMatrixType& oEigVec, VectorType & oEigVals,
                        bool smallest = true, long nFigures = 10)
  {
    EigenSystemType eSystem;

    typedef vnl_sparse_matrix<double> DoubleMatrixType;
    typedef vnl_vector<double>        DoubleVectorType;

    // vnl_sparse_symmetric_eigensystem not templated - always double...
    DoubleMatrixType myA(iA.rows(), iA.cols() );
    myA.reset();
    iA.reset();
    while( iA.next() )
      {
      myA(iA.getrow(), iA.getcolumn() ) = iA.value();
      }

    if( eSystem.CalculateNPairs(myA, nEig, smallest, nFigures) != 0 )
      {
      return false;
      }

    // retrieve results
    oEigVec.set_size(iA.rows(), nEig);
    oEigVals.set_size(nEig);
    for( unsigned int evIx = 0; evIx < nEig; evIx++ )
      {
      oEigVals(evIx) = eSystem.get_eigenvalue(evIx);
      DoubleVectorType anEigenVector = eSystem.get_eigenvector(evIx);
      for( unsigned int pntIx = 0; pntIx < iA.rows(); pntIx++ )
        {
        oEigVec(pntIx, evIx) = anEigenVector(pntIx);
        }
      }

    return true;
  }

  /** \brief Find n eigenvalue/eigenvectors of the general eigenproblem A * x = lambda * B * x \f$ */
  static bool Calculate(const MatrixType & iA, const MatrixType & iB, unsigned int nEig,
                        EigenvectorMatrixType& oEigVec, VectorType & oEigVals,
                        bool smallest = true, bool magnitude = true,
                        double tolerance = 1.0e-10, unsigned int maxIterations = 0,
                        double sigma = 0.0)
  {
    EigenSystemType eSystem;

    typedef vnl_sparse_matrix<double> DoubleMatrixType;
    typedef vnl_vector<double>        DoubleVectorType;

    // vnl_sparse_symmetric_eigensystem not templated - always double...
    DoubleMatrixType myA(iA.rows(), iA.cols() );
    DoubleMatrixType myB(iB.rows(), iB.cols() );
    myA.reset();
    iA.reset();
    while( iA.next() )
      {
      myA(iA.getrow(), iA.getcolumn() ) = iA.value();
      }

    myB.reset();
    iB.reset();
    while( iB.next() )
      {
      myB(iB.getrow(), iB.getcolumn() ) = iB.value();
      }

    if( eSystem.CalculateNPairs(myA, myB, nEig,
                                tolerance, 0,
                                smallest, magnitude,
                                maxIterations, sigma) != 0 )
      {
      return false;
      }

    // retrieve results
    oEigVec.set_size(iA.rows(), nEig);
    oEigVals.set_size(nEig);
    for( unsigned int evIx = 0; evIx < nEig; evIx++ )
      {
      oEigVals(evIx) = eSystem.get_eigenvalue(evIx);
      DoubleVectorType anEigenVector = eSystem.get_eigenvector(evIx);
      for( unsigned int pntIx = 0; pntIx < iA.rows(); pntIx++ )
        {
        oEigVec(pntIx, evIx) = anEigenVector(pntIx);
        }
      }

    return true;
  }

};

#endif
