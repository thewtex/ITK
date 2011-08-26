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
#ifndef __itkFFTCalculators_h
#define __itkFFTCalculators_h

namespace itk
{
class FFTCalculators
{
public:
  template< class MatrixType, class ComplexMatrixType >
  static bool CalculateCombinedForwardFFT(MatrixType & fixedMatrix, MatrixType & movingMatrix, ComplexMatrixType & fixedMatrixFFT,
    ComplexMatrixType & movingMatrixFFT, int FFTRows = -1, int FFTColumns = -1 );

  template< class MatrixType, class ComplexMatrixType >
  static MatrixType CalculateRealInverseFFT( ComplexMatrixType inputMatrix, unsigned int outputRows, unsigned int outputColumns );

private:
  static int FactorizeNumber( int n );
  static int FindClosestValidDimension(int n);
  template< class MatrixType, class ComplexMatrixType >
  static ComplexMatrixType FFTShift( ComplexMatrixType inputMatrix );
};
} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFFTCalculators.hxx"
#endif

#endif
