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

#include "itkConstantBoundaryCondition.h"
#include "itkWienerDeconvolutionImageFilter.h"
#include "itkFFTConvolutionImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int itkWienerDeconvolutionImageFilterTest(int argc, char * argv[])
{

  if ( argc < 4 )
    {
    std::cout << "Usage: " << argv[0]
      << " inputImage kernelImage outputImage [normalizeImage]" << std::endl;
    return EXIT_FAILURE;
    }

  const int ImageDimension = 2;

  typedef float                                  PixelType;
  typedef itk::Image<PixelType, ImageDimension>  ImageType;
  typedef itk::ImageFileReader<ImageType>        ReaderType;

  ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName( argv[1] );
  reader1->Update();

  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );
  reader2->Update();

  typedef itk::FFTConvolutionImageFilter<ImageType> ConvolutionFilterType;
  ConvolutionFilterType::Pointer convolutionFilter
    = ConvolutionFilterType::New();
  convolutionFilter->SetInput( reader1->GetOutput() );
  convolutionFilter->SetKernelImage( reader2->GetOutput() );

  bool normalize = false;
  if( argc >= 5 )
    {
    normalize = static_cast< bool >( atoi( argv[4] ) );
    }

  convolutionFilter->SetNormalize( normalize );

  typedef itk::WienerDeconvolutionImageFilter<ImageType> DeconvolutionFilterType;
  DeconvolutionFilterType::Pointer deconvolutionFilter = DeconvolutionFilterType::New();

  deconvolutionFilter->SetInput( convolutionFilter->GetOutput() );
  deconvolutionFilter->SetKernelImage( reader2->GetOutput() );
  deconvolutionFilter->SetNormalize( normalize );

  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( deconvolutionFilter->GetOutput() );


  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Tests for raising code coverage
  //
  deconvolutionFilter->Print( std::cout );

  ImageType::Pointer emtpyImage = ImageType::New();
  deconvolutionFilter->SetInput( emtpyImage );
  try
    {
    deconvolutionFilter->Update();
    std::cerr << "Failed to throw expected exception" << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << excp << std::endl;
    std::cout << "caught EXPECTED exception for empty image as input" << std::endl;
    }

  deconvolutionFilter->NormalizeOn();
  if( !deconvolutionFilter->GetNormalize() )
    {
    std::cerr << "Set/GetNormalize() error" << std::endl;
    return EXIT_FAILURE;
    }

  deconvolutionFilter->NormalizeOff();
  if( deconvolutionFilter->GetNormalize() )
    {
    std::cerr << "Set/GetNormalize() error" << std::endl;
    return EXIT_FAILURE;
    }

  deconvolutionFilter->SetNormalize( true );
  if( !deconvolutionFilter->GetNormalize() )
    {
    std::cerr << "Set/GetNormalize() error" << std::endl;
    return EXIT_FAILURE;
    }

  deconvolutionFilter->SetNormalize( false );
  if( deconvolutionFilter->GetNormalize() )
    {
    std::cerr << "Set/GetNormalize() error" << std::endl;
    return EXIT_FAILURE;
    }

  deconvolutionFilter->SetOutputRegionMode( ConvolutionFilterType::SAME );
  if ( deconvolutionFilter->GetOutputRegionMode() != ConvolutionFilterType::SAME )
    {
    std::cerr << "SetOutputRegionMode() error when argument is SAME" << std::endl;
    return EXIT_FAILURE;
    }

  deconvolutionFilter->SetOutputRegionMode( ConvolutionFilterType::VALID );
  if ( deconvolutionFilter->GetOutputRegionMode() != ConvolutionFilterType::VALID )
    {
    std::cerr << "SetOutputRegionMode() error when argument is VALID" << std::endl;
    return EXIT_FAILURE;
    }

  deconvolutionFilter->SetOutputRegionModeToSame();
  if ( deconvolutionFilter->GetOutputRegionMode() != ConvolutionFilterType::SAME )
    {
    std::cerr << "SetOutputRegionModeToSame() error" << std::endl;
    return EXIT_FAILURE;
    }

  deconvolutionFilter->SetOutputRegionModeToValid();
  if ( deconvolutionFilter->GetOutputRegionMode() != ConvolutionFilterType::VALID )
    {
    std::cerr << "SetOutputRegionModeToValid() error" << std::endl;
    return EXIT_FAILURE;
    }

  itk::ConstantBoundaryCondition< ImageType > constantBoundaryCondition;
  constantBoundaryCondition.SetConstant( 0.0 );
  deconvolutionFilter->SetBoundaryCondition( &constantBoundaryCondition );
  if ( deconvolutionFilter->GetBoundaryCondition() != &constantBoundaryCondition )
    {
    std::cerr << "SetBoundaryCondition() error" << std::endl;
    return EXIT_FAILURE;
    }


  // Test VALID output region mode with kernel that is larger than
  // the input image. Should result in a zero-size valid region.
  ImageType::Pointer largeKernel = ImageType::New();
  ImageType::RegionType kernelRegion( reader1->GetOutput()->GetLargestPossibleRegion().GetSize() );
  kernelRegion.PadByRadius( 5 );

  largeKernel->SetRegions( kernelRegion );
  largeKernel->Allocate();
  deconvolutionFilter->SetOutputRegionModeToValid();
  deconvolutionFilter->SetInput( reader1->GetOutput() );
  deconvolutionFilter->SetKernelImage( largeKernel );
  try
    {
    deconvolutionFilter->Update();
    std::cerr << "Failed to catch expected exception when kernel is larger than the input image."
              << std::endl;
    return EXIT_FAILURE;
    }
  catch ( itk::ExceptionObject e )
    {
    std::cout << "Caught expected exception when kernel is larger than the input image."
              << std::endl;
    std::cout << e << std::endl;
    }

  // Test for invalid request region.
  ImageType::IndexType invalidIndex;
  invalidIndex.Fill( 1000 );
  ImageType::SizeType invalidSize;
  invalidSize.Fill( 1000 );
  ImageType::RegionType invalidRequestRegion( invalidIndex, invalidSize );
  deconvolutionFilter->GetOutput()->SetRequestedRegion( invalidRequestRegion );
  try
    {
    deconvolutionFilter->Update();
    std::cerr << "Failed to catch expected exception when request region is outside the largest "
              << "possible region." << std::endl;
    return EXIT_FAILURE;
    }
  catch ( itk::ExceptionObject e )
    {
    std::cout << "Caught expected exception when request region is outside the largest "
              << "possible region." << std::endl;
    std::cout << e << std::endl;
    }

  return EXIT_SUCCESS;
}
