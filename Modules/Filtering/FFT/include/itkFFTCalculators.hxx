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

#ifndef __itkFFTCalculators_hxx
#define __itkFFTCalculators_hxx

#include "itkFFTCalculators.h"
#include "vnl/algo/vnl_fft_2d.h"

namespace itk
{
// This function factorizes the image size uses factors of 2, 3, and
// 5.  After this factorization, if there are any remaining values,
// the function returns this value.
int FFTCalculators::FactorizeNumber( int n )
{
  int ifac = 2;
  // This loop is just a convenient way of ensuring that ifac assumes
  // values of 2, 3, and 5 and then quits.  These are the only factors
  // that are valid for the FFT calculation.
  for (int offset = 1; offset <= 3; offset++)
    {
    // Using the given factor, factor the image continuously until it
    // can no longer be factored with this value.
    for(; n % ifac == 0;)
      {
      n /= ifac;
      }
    ifac += offset;
    }
  return n;
}

// Find the closest valid dimension above the desired dimension.  This
// will be a combination of 2s, 3s, and 5s.
int FFTCalculators::FindClosestValidDimension(int n)
{
  // Incrementally add 1 to the size until
  // we reach a size that can be properly factored.
  int newNumber = n;
  int result = 0;
  newNumber -= 1;
  while( result!=1 )
    {
    newNumber += 1;
    result = FactorizeNumber(newNumber);
    }
  return newNumber;
}

template< class MatrixType, class ComplexMatrixType >
bool FFTCalculators::CalculateCombinedForwardFFT(MatrixType & fixedMatrix, MatrixType & movingMatrix, ComplexMatrixType & fixedMatrixFFT,
    ComplexMatrixType & movingMatrixFFT, int FFTRows, int FFTColumns )
{
  typedef typename MatrixType::element_type         PixelType;
  typedef typename ComplexMatrixType::element_type  ComplexType;
  typedef typename MatrixType::iterator             MatrixIteratorType;
  typedef typename ComplexMatrixType::iterator      ComplexMatrixIteratorType;

  // If the number of FFT rows or columns is not set, we set them to the
  // size of the image.
  if( FFTRows == -1 )
    {
    // Check that the sizes of the two input matrices are the same.
    // Otherwise, give an error message.
    if( fixedMatrix.rows() != movingMatrix.rows() )
      {
      std::cerr << "ERROR: CalculateCombinedForwardFFT - The input matrices must have the same size!" << std::endl;
      return false;
      }

    FFTRows = fixedMatrix.rows();
    }

  if( FFTColumns == -1 )
    {
    // Check that the sizes of the two input matrices are the same.
    // Otherwise, give an error message.
    if( fixedMatrix.columns() != movingMatrix.columns() )
      {
      std::cerr << "ERROR: CalculateCombinedForwardFFT - The input matrices must have the same size!" << std::endl;
      return false;
      }

    FFTColumns = fixedMatrix.columns();
    }

  // This is an algorithm for finding the closest valid dimension for
  // the rows and columns.  The dimension must be divisible by a
  // combination of 2, 3, and 5.
  FFTRows = FindClosestValidDimension( FFTRows );
  FFTColumns = FindClosestValidDimension( FFTColumns );

  typedef vnl_fft_2d < PixelType > FFTCalculatorType;
  FFTCalculatorType FFTCalculator( FFTRows, FFTColumns );

  // Pad the fixed and moving images with zeros on the right and
  // bottom so that the resulting matrices have the size given by
  // (FFTRows,FFTColumns).
  MatrixType paddedFixedMatrix( FFTRows, FFTColumns, 0 );
  paddedFixedMatrix.update(fixedMatrix,0,0);
  //fixedMatrix.clear();
  MatrixType paddedMovingMatrix( FFTRows, FFTColumns, 0 );
  paddedMovingMatrix.update(movingMatrix,0,0);
  //movingMatrix.clear();

  // Create a complex image that embeds the fixed image as the real
  // component and the moving image as the imaginary component.
  ComplexMatrixType combinedMatrixFFT( FFTRows, FFTColumns, 0 );
  MatrixIteratorType fixedMatrixIterator = paddedFixedMatrix.begin();
  MatrixIteratorType movingMatrixIterator = paddedMovingMatrix.begin();
  ComplexMatrixIteratorType combinedMatrixIterator = combinedMatrixFFT.begin();
  for( combinedMatrixIterator = combinedMatrixFFT.begin(), fixedMatrixIterator = paddedFixedMatrix.begin(), movingMatrixIterator = paddedMovingMatrix.begin();
     combinedMatrixIterator != combinedMatrixFFT.end(); ++combinedMatrixIterator, ++fixedMatrixIterator, ++movingMatrixIterator )
    {
    *combinedMatrixIterator = ComplexType( *fixedMatrixIterator, *movingMatrixIterator );
    }

  FFTCalculator.fwd_transform( combinedMatrixFFT );

  ComplexMatrixType combinedMatrixFFTFlipped( FFTRows+1, FFTColumns+1, 0 );
  combinedMatrixFFTFlipped.update(combinedMatrixFFT, 0, 0);
  // Before flipping, copy the first row/column to the ends of the matrix.
  combinedMatrixFFTFlipped.set_column( FFTColumns, combinedMatrixFFTFlipped.get_column(0) );
  combinedMatrixFFTFlipped.set_row( FFTRows, combinedMatrixFFTFlipped.get_row(0) );
  // Flip the matrix.
  combinedMatrixFFTFlipped.flipud();
  combinedMatrixFFTFlipped.fliplr();
  // Delete the last row and column.
  combinedMatrixFFTFlipped = combinedMatrixFFTFlipped.extract(FFTRows,FFTColumns,0,0);

  fixedMatrixFFT.set_size( FFTRows, FFTColumns );
  fixedMatrixFFT.fill( 0.0 );
  movingMatrixFFT.set_size( FFTRows, FFTColumns );
  movingMatrixFFT.fill( 0.0 );

  // Separate the FFTs of the fixed and moving images using odd-even
  // separation.
  ComplexMatrixIteratorType fixedMatrixFFTIterator = fixedMatrixFFT.begin();
  ComplexMatrixIteratorType movingMatrixFFTIterator = movingMatrixFFT.begin();
  ComplexMatrixIteratorType combinedMatrixFFTIterator = combinedMatrixFFT.begin();
  ComplexMatrixIteratorType combinedMatrixFFTFlippedIterator = combinedMatrixFFTFlipped.begin();
  while( fixedMatrixFFTIterator != fixedMatrixFFT.end() )
    {
    *fixedMatrixFFTIterator = ComplexType( 0.5 * ((*combinedMatrixFFTIterator).real() + (*combinedMatrixFFTFlippedIterator).real()),
      0.5 * ((*combinedMatrixFFTIterator).imag() - (*combinedMatrixFFTFlippedIterator).imag()) );

    // Here, the real and imaginary parts are flipped as required by
    // the odd-even separation.
    *movingMatrixFFTIterator = ComplexType( 0.5 * ((*combinedMatrixFFTFlippedIterator).imag() + (*combinedMatrixFFTIterator).imag()),
      0.5 * ((*combinedMatrixFFTFlippedIterator).real() - (*combinedMatrixFFTIterator).real()) );

    ++fixedMatrixFFTIterator;
    ++movingMatrixFFTIterator;
    ++combinedMatrixFFTIterator;
    ++combinedMatrixFFTFlippedIterator;
    }

  combinedMatrixFFT.clear();
  combinedMatrixFFTFlipped.clear();

return true;
}

template< class MatrixType, class ComplexMatrixType >
MatrixType FFTCalculators::CalculateRealInverseFFT( ComplexMatrixType inputMatrix, unsigned int outputRows, unsigned int outputColumns )
{
  typedef typename MatrixType::element_type     PixelType;
  typedef typename MatrixType::iterator         MatrixIteratorType;
  typedef typename ComplexMatrixType::iterator  ComplexMatrixIteratorType;

  MatrixIteratorType matrixIterator;
  ComplexMatrixIteratorType complexMatrixIterator;

  int FFTRows = inputMatrix.rows();
  int FFTColumns = inputMatrix.columns();

  typedef vnl_fft_2d < PixelType > FFTCalculatorType;
  FFTCalculatorType IFFTCalculator( FFTRows, FFTColumns );

  IFFTCalculator.bwd_transform(inputMatrix);
  // The normalization by the number of pixels in the Fourier image
  // should have been accounted for by the inverse Fourier transform,
  // but it was not.
  unsigned int numberOfFourierPixels = (FFTRows)*(FFTColumns);

  // Extract the relevant part out of the image.
  // The input FFT image may be bigger than the desired output image
  // because specific sizes are required for the FFT calculation.
  inputMatrix = inputMatrix.extract(outputRows,outputColumns,0,0);

  // Convert the image from complex (with small imaginary values since
  // the input to the original FFTs was real) to real.
  // In this loop we also divide by the number of pixels in the
  // Fourier spectrum.
  MatrixType outputMatrix( outputRows, outputColumns );
  for( complexMatrixIterator = inputMatrix.begin(), matrixIterator = outputMatrix.begin(); complexMatrixIterator != inputMatrix.end(); ++complexMatrixIterator, ++matrixIterator )
    {
    *matrixIterator = (*complexMatrixIterator).real() / numberOfFourierPixels;
    }

  return outputMatrix;
}
template< class MatrixType, class ComplexMatrixType >
ComplexMatrixType FFTCalculators::FFTShift( ComplexMatrixType inputMatrix )
{
  int m = inputMatrix.rows();
  int n = inputMatrix.columns();
  // Find the ceil of half of the size.
  int p = (m+1)/2;
  int q = (n+1)/2;

  ComplexMatrixType outputMatrix(m,n,0);
  // Switch top-left and bottom-right blocks.
  outputMatrix.update(inputMatrix.extract(m-p,n-q,p,q),0,0);
  outputMatrix.update(inputMatrix.extract(p,q,0,0),p-1,q-1);

  // Switch the bottom-left and top-right blocks.
  outputMatrix.update(inputMatrix.extract(m-p,q,p,0),0,q-1);
  outputMatrix.update(inputMatrix.extract(p,n-q,0,q),p-1,0);

  // Check that the size of the output matrix is the same as the input
  // matrix.
  if( inputMatrix.rows() != outputMatrix.rows() || inputMatrix.columns() != outputMatrix.columns() )
    {
    std::cerr << "ERROR: FFTShift: inputMatrix and outputMatrix must be the same size!" << std::endl;
    }

  return outputMatrix;
}
} // end namespace

#endif
