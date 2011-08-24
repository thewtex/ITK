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
#ifndef __itkVnlFFTNormalizedCrossCorrelationImageFilter_hxx
#define __itkVnlFFTNormalizedCrossCorrelationImageFilter_hxx

#include "itkVnlFFTNormalizedCrossCorrelationImageFilter.h"
#include "itkFFTNormalizedCrossCorrelationImageFilter.hxx"
#include "itkFFTCalculators.h"
#include "itkVnlBridge.h"

namespace itk
{
template< class TInputImage, class TOutputImage >
void
VnlFFTNormalizedCrossCorrelationImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  MatrixIteratorType matrixIterator;
  MatrixType fixedImage = VnlBridge::ImageToVnlMatrix<InputImageType,MatrixType>( this->GetFixedImageInput() );
  MatrixType rotatedMovingImage = VnlBridge::ImageToVnlMatrix<InputImageType,MatrixType>( this->GetMovingImageInput() );

  // Rotate the moving image by 180 degrees.
  rotatedMovingImage.flipud();
  rotatedMovingImage.fliplr();

  unsigned int fixedMatrixRows = fixedImage.rows();
  unsigned int fixedMatrixColumns = fixedImage.columns();
  unsigned int movingMatrixRows = rotatedMovingImage.rows();
  unsigned int movingMatrixColumns = rotatedMovingImage.columns();
  unsigned int combinedRows = fixedMatrixRows + movingMatrixRows - 1;
  unsigned int combinedColumns = fixedMatrixColumns + movingMatrixColumns - 1;

  // All values of the following two matrices are set to 1 since they
  // are used to calculate local sums using FFT operations.
  // We call them masks here out of convenience because of the analogy
  // of these calculations with the masked normalized cross
  // correlation algorithm.
  MatrixType fixedMask(fixedMatrixRows,fixedMatrixColumns,1);
  MatrixType rotatedMovingMask(movingMatrixRows,movingMatrixColumns,1);

  // Only 6 FFTs are needed.
  // We can calculate two forward FFTs at a time by using odd-even
  // separation.
  int FFTRows = fixedImage.rows() + rotatedMovingImage.rows() - 1;
  int FFTColumns = fixedImage.columns() + rotatedMovingImage.columns() - 1;
  ComplexMatrixType fixedFFT;
  ComplexMatrixType rotatedMovingFFT;
  FFTCalculators::CalculateCombinedForwardFFT<MatrixType,ComplexMatrixType>( fixedImage, rotatedMovingImage, fixedFFT, rotatedMovingFFT, FFTRows, FFTColumns );
  ComplexMatrixType fixedSquaredFFT;
  ComplexMatrixType rotatedMovingSquaredFFT;
  MatrixType fixedImageSquared = element_product(fixedImage,fixedImage);
  MatrixType rotatedMovingImageSquared = element_product(rotatedMovingImage,rotatedMovingImage);
  FFTCalculators::CalculateCombinedForwardFFT<MatrixType,ComplexMatrixType>(fixedImageSquared,rotatedMovingImageSquared,fixedSquaredFFT,rotatedMovingSquaredFFT, FFTRows, FFTColumns );
  ComplexMatrixType fixedMaskFFT;
  ComplexMatrixType rotatedMovingMaskFFT;
  FFTCalculators::CalculateCombinedForwardFFT<MatrixType,ComplexMatrixType>( fixedMask,rotatedMovingMask,fixedMaskFFT,rotatedMovingMaskFFT, FFTRows, FFTColumns );

  // Only 6 IFFTs are needed.
  // Compute and save these results rather than computing them multiple times.
  MatrixType numberOfOverlapPixels = FFTCalculators::CalculateRealInverseFFT<MatrixType,ComplexMatrixType>(element_product(rotatedMovingMaskFFT,fixedMaskFFT),combinedRows,combinedColumns);
  // Ensure that the result is positive.
  for( matrixIterator = numberOfOverlapPixels.begin(); matrixIterator != numberOfOverlapPixels.end(); ++matrixIterator )
    {
    *matrixIterator = vnl_math_rnd(*matrixIterator);
    //*matrixIterator = vnl_math_max(*matrixIterator,vnl_math::eps);
    *matrixIterator = vnl_math_max(*matrixIterator,(RealPixelType)0);
    }
  m_NumberOfOverlapPixels = VnlBridge::VnlMatrixToImage<OutputImageType,MatrixType>( numberOfOverlapPixels );

  MatrixType maskCorrelatedFixedFFT = FFTCalculators::CalculateRealInverseFFT<MatrixType,ComplexMatrixType>(element_product(rotatedMovingMaskFFT,fixedFFT),combinedRows,combinedColumns);
  MatrixType maskCorrelatedRotatedMovingFFT = FFTCalculators::CalculateRealInverseFFT<MatrixType,ComplexMatrixType>(element_product(fixedMaskFFT,rotatedMovingFFT),combinedRows,combinedColumns);

  MatrixType numerator = FFTCalculators::CalculateRealInverseFFT<MatrixType,ComplexMatrixType>(element_product(rotatedMovingFFT,fixedFFT),combinedRows,combinedColumns)
      - element_quotient(element_product(maskCorrelatedFixedFFT,maskCorrelatedRotatedMovingFFT),numberOfOverlapPixels);

  MatrixType fixedDenom = FFTCalculators::CalculateRealInverseFFT<MatrixType,ComplexMatrixType>(element_product(rotatedMovingMaskFFT,fixedSquaredFFT),combinedRows,combinedColumns)
      - element_quotient(element_product(maskCorrelatedFixedFFT,maskCorrelatedFixedFFT),numberOfOverlapPixels);
  // Ensure that the result is positive.
  for( matrixIterator = fixedDenom.begin(); matrixIterator != fixedDenom.end(); ++matrixIterator )
    {
    *matrixIterator = vnl_math_max(*matrixIterator,RealPixelType(0));
    }

  MatrixType movingDenom = FFTCalculators::CalculateRealInverseFFT<MatrixType,ComplexMatrixType>(element_product(fixedMaskFFT,rotatedMovingSquaredFFT),combinedRows,combinedColumns)
      - element_quotient(element_product(maskCorrelatedRotatedMovingFFT,maskCorrelatedRotatedMovingFFT),numberOfOverlapPixels);
  // Ensure that the result is positive.
  for( matrixIterator = movingDenom.begin(); matrixIterator != movingDenom.end(); ++matrixIterator )
    {
    *matrixIterator = vnl_math_max(*matrixIterator,RealPixelType(0));
    }

  MatrixType denominator = element_product(fixedDenom,movingDenom);
  denominator = denominator.apply(sqrt);

  MatrixType normalizedCorrelationMatrix = element_quotient(numerator,denominator);
  //numerator.clear();

  // The correlation must be between -1 and 1 by definition.  But
  // numerical errors can cause the values to be large values (for
  // example, when dividing by zero).  So, we loop through the matrix
  // and set to zero all values outside of this range.
  MatrixIteratorType denomIterator;
  MatrixIteratorType overlapIterator;
  for( matrixIterator = normalizedCorrelationMatrix.begin(), denomIterator = denominator.begin(), overlapIterator = numberOfOverlapPixels.begin();
      matrixIterator != normalizedCorrelationMatrix.end(); ++matrixIterator, ++denomIterator, ++overlapIterator )
    {
    if( *denomIterator == 0 || *matrixIterator < -1 || *matrixIterator > 1 || *overlapIterator == 0 )
      {
      *matrixIterator = 0;
      }
    }
  //denominator.clear();

  typename OutputImageType::Pointer outputImage = VnlBridge::VnlMatrixToImage<OutputImageType,MatrixType>( normalizedCorrelationMatrix );
  this->GetOutput()->Graft( outputImage );
}

template< class TInputImage, class TOutputImage >
void
VnlFFTNormalizedCrossCorrelationImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

} // end namespace itk

#endif
