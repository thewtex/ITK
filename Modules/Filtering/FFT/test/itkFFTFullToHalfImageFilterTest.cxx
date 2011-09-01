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

#include <complex>
#include <cstdlib>
#include <iostream>

#include "itkFFTHalfToFullImageFilter.h"
#include "itkFFTFullToHalfImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageRegion.h"
#include "itkImageRegionConstIterator.h"
#include "itkRandomImageSource.h"
#include "itkForwardFFTImageFilter.h"

int itkFFTFullToHalfImageFilterTest(int argc, char *argv[])
{
  // Print usage information.
  if ( argc < 3 )
    {
    std::cerr << "Usage: " << argv[0] << " <test image x size> <test image y size>"
              << std::endl;
    return EXIT_FAILURE;
    }

  // Read in image.
  typedef itk::Image< float, 2 >                 ImageType;
  typedef itk::Image< std::complex< float >, 2 > ComplexImageType;
  typedef itk::RandomImageSource< ImageType >    RandomSourceType;

  RandomSourceType::Pointer source = RandomSourceType::New();
  RandomSourceType::SizeType size;
  size[0] = atoi( argv[1] );
  size[1] = atoi( argv[2] );
  source->SetMin( 0.0f );
  source->SetMax( 1.0f );
  source->SetSize( size );
  source->Update();

  // Compute frequency image, yielding the non-redundant half of the
  // full complex image.
  typedef itk::ForwardFFTImageFilter< ImageType, ComplexImageType > FFTFilter;
  FFTFilter::Pointer fft = FFTFilter::New();
  fft->SetInput( source->GetOutput() );
  fft->Update();

  ComplexImageType::SizeType fftSize = fft->GetOutput()->GetLargestPossibleRegion().GetSize();

  // Expand the non-redundant half to the full complex image.
  typedef itk::FFTHalfToFullImageFilter< ComplexImageType > HalfToFullFilterType;
  HalfToFullFilterType::Pointer halfToFullFilter = HalfToFullFilterType::New();
  bool xDimensionIsOdd = size[0] % 2 == 1;
  halfToFullFilter->SetActualXDimensionIsOdd( xDimensionIsOdd  );
  halfToFullFilter->SetInput( fft->GetOutput() );

  typedef itk::FFTFullToHalfImageFilter< ComplexImageType > FullToHalfFilterType;
  FullToHalfFilterType::Pointer fullToHalfFilter = FullToHalfFilterType::New();
  fullToHalfFilter->SetInput( halfToFullFilter->GetOutput() );
  fullToHalfFilter->Update();

  // Check that the output of the full-to-half filter has the same
  // size as the output of the FFT filter.
  if ( fullToHalfFilter->GetOutput()->GetLargestPossibleRegion().GetSize() != fftSize )
    {
    std::cerr << "Output size of full-to-half filter is not the same as the output size "
              << "of the FFT filter." << std::endl;
    return EXIT_FAILURE;
    }

  // Check that the output of the full-to-half filter is equal to the
  // output of the FFT filter.
  ComplexImageType::RegionType comparisonRegion( fftSize );
  typedef itk::ImageRegionConstIterator< ComplexImageType > IteratorType;
  IteratorType fftIt( fft->GetOutput(), comparisonRegion );
  IteratorType f2hIt( fullToHalfFilter->GetOutput(), comparisonRegion );

  for (fftIt.GoToBegin(), f2hIt.GoToBegin(); !fftIt.IsAtEnd(); ++fftIt, ++f2hIt)
    {
    if ( fftIt.Get() != f2hIt.Get() )
      {
      std::cerr << "Pixel at index " << fftIt.GetIndex() << " does not match!" << std::endl;
      std::cerr << "FFT output: " << fftIt.Get() << ", full-to-half output: " << f2hIt.Get()
                << std::endl;
      }
    }

  return EXIT_SUCCESS;
}
