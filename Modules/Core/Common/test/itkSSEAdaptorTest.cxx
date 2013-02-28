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
 /*
   This tests the adaptor class VNLSparseSymmetricEigensystemTraits.h
   by comparing the eigensystem output to the output of the vnl class it wraps
 */


#include <iostream>

#include "VNLSparseSymmetricEigensystemTraits.h"
#include "vnl/algo/vnl_sparse_symmetric_eigensystem.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_math.h"
#include "vnl/vnl_diag_matrix.h"


int itkSSEAdaptorTest(int , char* [] )
{
  // some test data for the standard case
  //     A * x = lambda * x
  // and the generalized case of CalculateNPairs
  //     A * x = lambda * B * x

  const int matOrd = 6;
  double aData[matOrd*matOrd] = {
    -1,  2,  2,  2,  2,  2,
    2,  0,  0,  0,  0,  2,
    2,  0,  -1,  0,  0,  2,
    2,  0,  0,  -1,  0,  2,
    2,  0,  0,  0, -1,  0,
    2,  2,  2,  2,  0,  -1,
  };

  double bData[matOrd*matOrd] = {
    30.0000,   -3.4273,   13.9254,   13.7049,   -2.4446,   20.2380,
    -3.4273,   13.7049,   -2.4446,    1.3659,    3.6702,   -0.2282,
    13.9254,   -2.4446,   20.2380,    3.6702,   -0.2282,   28.6779,
    13.7049,    1.3659,    3.6702,   12.5273,   -1.6045,    3.9419,
    -2.4446,    3.6702,   -0.2282,   -1.6045,    3.9419,    2.5821,
    20.2380,   -0.2282,   28.6779,    3.9419,    2.5821,   44.0636,
  };

  vnl_matrix<double> aDense(aData, matOrd, matOrd);
  vnl_matrix<double> bDense(bData, matOrd, matOrd);

  // set up sparse matrices via vnl and adaptor
  vnl_sparse_matrix<double> vnlSparseA(matOrd, matOrd);
  vnl_sparse_matrix<double> vnlSparseB(matOrd, matOrd);

  typedef VNLSparseSymmetricEigensystemTraits<double> SSEType;
  SSEType::MatrixType A = SSEType::InitializeSparseMatrix(matOrd);
  SSEType::MatrixType B = SSEType::InitializeSparseMatrix(matOrd);

  // set values
  for (int i = 0; i < matOrd; i++)
  {
    for (int j = 0; j < matOrd; j++)
    {
      if (aDense(i, j) != 0.0)
      {
        vnlSparseA(i, j) = aDense(i, j);
        SSEType::FillMatrix(A, i, j, aDense(i, j));
      }
      if (bDense(i, j) != 0.0)
      {
        vnlSparseB(i, j) = bDense(i, j);
        SSEType::FillMatrix(B, i, j, bDense(i, j));
      }
    }
  }

  double precision = 0.0000001;
  bool testFailed = false;

  // can't get all eigenvals because 0 < evCount < numLanzcosVectors < matOrd
  int evCount = matOrd - 1;

  vnl_vector<double> eigenvalues1(evCount);
  for (int n = 1; n <= 5; n++)
  {
    bool smallest;
    bool magnitude;
    double sigma;
    switch (n)
    {
      case 1:
        smallest = true;
        magnitude = false;
        sigma = 0.0;
        break;
      case 2:
        smallest = true;
        magnitude = false;
        sigma = 0.0;
        break;
      case 3:
        smallest = false;
        magnitude = false;
        sigma = 0.0;
        break;
      case 4:
        smallest = false;
        magnitude = true;
        sigma = 0.0;
        break;
      case 5:
        smallest = false;
        magnitude = true;
        sigma = 1.0;
        break;
      default:
        std::cout << "unhandled test case:  " << n << std::endl;
        return EXIT_FAILURE;
        break;
    }

    std::cout << "run " << n << std::endl;
    std::cout << "smallest:  " << (smallest ? "true" : "false");
    std::cout << ", magnitude:  " << (magnitude ? "true" : "false");
    std::cout << ", sigma:  " << sigma << std::endl;

    vnl_sparse_symmetric_eigensystem sse;
    int result = sse.CalculateNPairs(vnlSparseA, vnlSparseB, evCount, 1.0e-14, 0, true, false);
    if (result != 0)
    {
        std::cout << "sse failed, result:  " << result << std::endl;
        return EXIT_FAILURE;
    }

    // validate output:
    // A * x = lambda * B * x
    // for each eigenvalue lambda and eigenvector x
    // lambda is its corresponding eigenvalue
    for (int valIx = 0; valIx < evCount; valIx++)
    {
      eigenvalues1(valIx) = sse.get_eigenvalue(valIx);
      vnl_vector<double> leftSide;
      vnlSparseA.mult(sse.get_eigenvector(valIx), leftSide);
      vnl_vector<double> rightSide;
      vnlSparseB.mult(sse.get_eigenvector(valIx) * sse.get_eigenvalue(valIx), rightSide);
      if ((leftSide - rightSide).inf_norm() > precision)
      {
        std::cout << "*** - Error:  Max diff value great than precision: "  << precision << "  ****" << std::endl;
        testFailed = true;
      }
    }

    // try it with the Adaptor, compare the results
    SSEType::EigenvectorMatrixType eigenvectors2;
    SSEType::VectorType            eigenvalues2;
    if (!SSEType::Calculate(A, B, evCount,
                            eigenvectors2, eigenvalues2,
                            true, false, 1.0e-14, 10000))
    {
      // failed to compute eigenvalues
      std::cout << "Adaptor sse call failed."
                << std::endl;
      return EXIT_FAILURE;
    }

    // validate output:
    // A * x = lambda * B * x
    // x is an eigenvector
    // lambda is its corresponding eigenvalue
    double eigenvecDiffPerRun = 0.0;
    double eigenvalDiffPerRun = 0.0;
    for (int valIx = 0; valIx < evCount; valIx++)
    {
      vnl_vector<double> leftSide;
      vnlSparseA.mult(eigenvectors2.get_column(valIx), leftSide);
      vnl_vector<double> rightSide;
      vnlSparseB.mult(eigenvectors2.get_column(valIx) * eigenvalues2(valIx), rightSide);
      if ((leftSide - rightSide).inf_norm() > precision)
      {
        std::cout << "Max diff value is: "  << (leftSide - rightSide).inf_norm() << std::endl;
        std::cout << "*** - Error:  Max diff value greater than precision: "  << precision << "  ****" << std::endl;
        testFailed = true;
      }
      eigenvecDiffPerRun += (eigenvectors2.get_column(valIx) - sse.get_eigenvector(valIx)).one_norm();
      if (eigenvecDiffPerRun > precision)
      {
          std::cout << "*** - Error:  Eigenvector [" << valIx << "] differs too much from last run:  "  << eigenvecDiffPerRun << " > " << precision << "  ****" << std::endl;
        testFailed = true;
      }
      eigenvalDiffPerRun += fabs(eigenvalues2[valIx] - sse.get_eigenvalue(valIx));
      if (eigenvalDiffPerRun > precision)
      {
          std::cout << "*** - Error:  Eigenvalue [" << valIx << "] differs too much from last run:  "  << eigenvalDiffPerRun << " > " << precision << "  ****" << std::endl;
        testFailed = true;
      }
    }
  }

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;

  return (testFailed ? EXIT_FAILURE : EXIT_SUCCESS);
}
