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
#ifndef __itkVnlFFTComplexConjugateToRealImageFilter_txx
#define __itkVnlFFTComplexConjugateToRealImageFilter_txx

#include "itkVnlFFTComplexConjugateToRealImageFilter.h"
#include "itkFFTComplexConjugateToRealImageFilter.txx"
#include <complex>
#include "vnl/algo/vnl_fft_1d.h"
#include "vnl/algo/vnl_fft_2d.h"
#include "vnl_fft_3d.h"
#include "itkProgressReporter.h"

namespace itk
{
template< class TInputImage, class TOutputImage >
bool VnlFFTComplexConjugateToRealImageFilter< TInputImage, TOutputImage >::Legaldim(int n)
{
  int ifac = 2;

  for ( int l = 1; l <= 3; l++ )
    {
    for (; n % ifac == 0; )
      {
      n /= ifac;
      }
    ifac += l;
    }
  return ( n == 1 ); // return false if decomposition failed
}

template< class TInputImage, class TOutputImage >
void
VnlFFTComplexConjugateToRealImageFilter< TInputImage, TOutputImage >::GenerateData()
{
  unsigned int i;

  // get pointers to the input and output
  typename InputImageType::ConstPointer inputPtr  = this->GetInput();
  typename OutputImageType::Pointer outputPtr = this->GetOutput();

  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // we don't have a nice progress to report, but at least this simple line
  // reports the begining and the end of the process
  ProgressReporter progress(this, 0, 1);

  const typename InputImageType::SizeType &   outputSize =
    outputPtr->GetLargestPossibleRegion().GetSize();
  unsigned int num_dims = outputPtr->GetImageDimension();

  if ( num_dims != outputPtr->GetImageDimension() )
    {
    return;
    }

  // allocate output buffer memory
  outputPtr->SetBufferedRegion( outputPtr->GetRequestedRegion() );
  outputPtr->Allocate();

  InputPixelType *in = const_cast< InputPixelType * >
                               ( inputPtr->GetBufferPointer() );

  unsigned int vec_size = 1;
  for ( i = 0; i < num_dims; i++ )
    {
    //#if 0
    if ( !this->Legaldim(outputSize[i]) )
      {
      ExceptionObject exception(__FILE__, __LINE__);
      exception.SetDescription("Illegal Array DIM for FFT");
      exception.SetLocation(ITK_LOCATION);
      throw exception;
      }
    //#endif
    vec_size *= outputSize[i];
    }

  vnl_vector< InputPixelType > signal(vec_size);
  for ( i = 0; i < vec_size; i++ )
    {
    signal[i] = in[i];
    }

  OutputPixelType *out = outputPtr->GetBufferPointer();

  // In the following, we use the VNL "fwd_transform" even though this
  // filter is actually taking the reverse transform.  This is done
  // because the VNL definitions are switched from the standard
  // definition.  The standard definition uses a negative exponent for
  // the forward transform and positive for the reverse transform.
  // VNL does the opposite.
  switch ( num_dims )
    {
    case 1:
      {
      vnl_fft_1d< OutputPixelType > v1d(vec_size);
      //v1d.fwd_transform(signal);
      v1d.vnl_fft_1d< OutputPixelType >::base::transform(signal.data_block(), +1);
      }
      break;
    case 2:
      {
      // The arguments are specified in the order rows, columns.
      vnl_fft_2d< OutputPixelType > v2d(outputSize[1], outputSize[0]);
      v2d.vnl_fft_2d< OutputPixelType >::base::transform(signal.data_block(), +1);
      }
      break;
    case 3:
      {
      vnl_fft_3d< OutputPixelType > v3d(outputSize[2], outputSize[1], outputSize[0]);
      v3d.vnl_fft_3d< OutputPixelType >::base::transform(signal.data_block(), +1);
      }
      break;
    default:
      break;
    }

  // Copy the VNL output back to the ITK image.
  // Extract the real part of the signal.
  // Ideally, the normalization by the number of elements
  // should have been accounted for by the VNL inverse Fourier transform,
  // but it is not.  So, we take care of it by dividing the signal by
  // the vec_size.
  for ( i = 0; i < vec_size; i++ )
    {
    out[i] = signal[i].real() / vec_size;
    }
}

template< class TInputImage, class TOutputImage >
bool
VnlFFTComplexConjugateToRealImageFilter< TInputImage, TOutputImage >::FullMatrix()
{
  return true;
}
}
#endif
