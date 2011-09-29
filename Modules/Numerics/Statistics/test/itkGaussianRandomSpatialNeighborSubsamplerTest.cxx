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

#include "itkWin32Header.h"

#include <fstream>

#include "itkImageToNeighborhoodSampleAdaptor.h"
#include "itkSubsample.h"
#include "itkImageFileWriter.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"

#include "itkGaussianRandomSpatialNeighborSubsampler.h"

int itkGaussianRandomSpatialNeighborSubsamplerTest(int argc, char* argv[] )
{
  std::cout << "GaussianRandomSpatialNeighborSubsampler Test \n \n";

  std::string outFile = "";
  if (argc > 1)
  {
    outFile = argv[1];
  }

  typedef itk::Image< float, 2 > FloatImage;
  typedef FloatImage::RegionType RegionType;
  typedef FloatImage::IndexType  IndexType;
  typedef FloatImage::SizeType   SizeType;
  typedef itk::ZeroFluxNeumannBoundaryCondition< FloatImage > BoundaryCondition;
  typedef itk::Statistics::ImageToNeighborhoodSampleAdaptor< FloatImage, BoundaryCondition >
    AdaptorType;
  typedef itk::Statistics::GaussianRandomSpatialNeighborSubsampler< AdaptorType, RegionType >
    SamplerType;
  typedef itk::ImageFileWriter< FloatImage > WriterType;

  FloatImage::Pointer inImage = FloatImage::New();
  SizeType sz;
  sz.Fill(35);
  IndexType idx;
  idx.Fill(0);
  RegionType region;
  region.SetSize(sz);
  region.SetIndex(idx);

  inImage->SetRegions(region);
  inImage->Allocate();
  inImage->FillBuffer(0);

  AdaptorType::Pointer sample = AdaptorType::New();
  sample->SetImage(inImage);

  SamplerType::Pointer sampler = SamplerType::New();
  sampler->SetSample(sample);
  sampler->SetSampleRegion(region);
  sampler->SetRadius(20);
  sampler->SetNumberOfResultsRequested(50);
  sampler->SetVariance(25);
  sampler->SetSeed(100);
  sampler->CanSelectQueryOff();

  SamplerType::SubsamplePointer subsample = SamplerType::SubsampleType::New();
  sampler->Search(612, subsample);

  for (SamplerType::SubsampleConstIterator sIt = subsample->Begin();
       sIt != subsample->End();
       ++sIt)
  {
    IndexType index;
    index = sIt.GetMeasurementVector()[0].GetIndex();
    inImage->SetPixel(index, 255);
  }

  if (outFile != "")
  {
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outFile );
    writer->SetInput( inImage );
    try
    {
      writer->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << excp << std::endl;
    }
  }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}
