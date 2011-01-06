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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkImage.h"
#include "itkScalarImageToHistogramGenerator.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkImageFileReader.h"

int itkImageToHistogramFilterTest4( int argc, char * argv [] )
{
  if( argc < 2 )
    {
    std::cerr << "Missing command line arguments" << std::endl;
    std::cerr << "Usage :  " << argv[0] << " outputHistogramFile.txt" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Writing " << argv[1] << std::endl;

  typedef float PixelType;
  const unsigned int Dimension = 3;
  const unsigned int MeasurementVectorSize = 1;
  typedef itk::Image< PixelType, Dimension > ScalarImageType;

  const float testHistMax = 32768;
  const float scaleMax=30000;


  ScalarImageType::Pointer image = ScalarImageType::New();

  float origin[Dimension] = { 0, 0, 0};
  float spacing[Dimension] = { 1.0, 1.0, 1.0};
  ScalarImageType::SizeType imageSize = {{ 256, 256, 256 }};

  ScalarImageType::RegionType region;
  region.SetSize(imageSize);
  image->SetRegions( region );
  image->SetOrigin(origin);
  image->SetSpacing(spacing);
  image->Allocate();
  image->FillBuffer(0);

  typedef itk::ImageRegionIterator< ScalarImageType > ImageIterator;


  ImageIterator itr( image, region );
  itr.GoToBegin();
  float count = 0.0;
  itr.GoToBegin();
  while ( ! itr.IsAtEnd() )
  {
    if (count <= scaleMax)
      itr.Value() = count;
    else
      itr.Value() = 0.0;
    count += 1.0;
    if (count >= testHistMax) count = 0.0;
    ++itr;
  }
  itk::MinimumMaximumImageFilter<ScalarImageType>::Pointer minmaxFilter
    = itk::MinimumMaximumImageFilter<ScalarImageType>::New();
  minmaxFilter->SetInput(image);
  minmaxFilter->Update();
  const ScalarImageType::PixelType imageMin = minmaxFilter->GetMinimum();
  const ScalarImageType::PixelType imageMax = minmaxFilter->GetMaximum();

  typedef itk::Statistics::ScalarImageToHistogramGenerator<ScalarImageType>
    HistogramGeneratorType;
  HistogramGeneratorType::Pointer histogramGenerator
    = HistogramGeneratorType::New();

  histogramGenerator->SetInput(image);

  histogramGenerator->SetNumberOfBins( 32768 );
  histogramGenerator->SetMarginalScale(1.0);
  // IF YOU SET THE MINIMUM AND MAXIMUM HISTOGRAM VALUES
  // THE OUTPUT IS AS EXPECTED
#if 0
  histogramGenerator->SetHistogramMin( imageMin );
  histogramGenerator->SetHistogramMax( imageMax );
#endif
  histogramGenerator->Compute();

  typedef HistogramGeneratorType::HistogramType  HistogramType;
  const HistogramType * histogram = histogramGenerator->GetOutput();

  std::ofstream outputFile;
  outputFile.open( argv[1] );

  const unsigned int histogramSize = histogram->Size();
  outputFile << "Histogram size " << histogramSize << std::endl;

  unsigned int channel = 0;  // red channel
  outputFile << "Histogram of the scalar component" << std::endl;
  for( unsigned int bin=0; bin < histogramSize; bin++ )
    {
    outputFile << "bin = " << bin << " frequency = ";
    outputFile << histogram->GetFrequency( bin, channel ) << std::endl;
    }
  outputFile.close();
  return EXIT_SUCCESS;
}
