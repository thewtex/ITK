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

/**
 * Test program for itkGPUBlockMatchingImageFilter class
 */

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkGPUImage.h"
#include "itkGPUKernelManager.h"
#include "itkGPUContextManager.h"
#include "itkGPUImageToImageFilter.h"
#include "itkGPUBlockMatchingImageFilter.h"

#include "itkBlockMatchingImageFilter.h"
#include "itkTranslationTransform.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include <cmath>


int itkGPUBlockMatchingImageFilterTest(int argc, char *argv[])
{
  if(!itk::IsGPUAvailable())
  {
    std::cerr << "OpenCL-enabled GPU is not present." << std::endl;
    return EXIT_FAILURE;
  }

  if( argc < 2 )
  {
    std::cerr << "Error: missing image argument" << std::endl;
    return EXIT_FAILURE;
  }

  typedef float  InputPixelType;

  static const unsigned int Dimension = 3;

  typedef itk::Image< InputPixelType,  Dimension >  InputImageType;
  typedef itk::GPUImage< InputPixelType,  Dimension >  GPUInputImageType;

  typedef InputImageType::SizeType RadiusType;
  RadiusType blockRadius;
  blockRadius.Fill( 1 );

  RadiusType searchRadius;
  searchRadius.Fill( 2 );

  typedef itk::ImageFileReader< InputImageType >  ReaderType;

  //Set up the reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Error in reading the input image: " << e << std::endl;
    return EXIT_FAILURE;
    }

  // GPU BM
  typedef itk::GPUBlockMatchingImageFilter< GPUInputImageType >  GPUBlockMatchingFilterType;
  GPUBlockMatchingFilterType::Pointer GPUBlockMatchingFilter = GPUBlockMatchingFilterType::New();
  GPUBlockMatchingFilter->SetWorkGroupSize( 1 );

  // CPU BM
  typedef itk::BlockMatchingImageFilter< GPUInputImageType >  BlockMatchingFilterType;
  BlockMatchingFilterType::Pointer CPUBlockMatchingFilter = BlockMatchingFilterType::New();
  CPUBlockMatchingFilter->SetNumberOfThreads( 1 );

  // Feature Selection -- use pre-computed points
  typedef BlockMatchingFilterType::FeaturePointsType FeaturePointsType;
  FeaturePointsType::Pointer pointSet = FeaturePointsType::New();
  FeaturePointsType::PointsContainer::Pointer pointsContainer = FeaturePointsType::PointsContainer::New();
  typedef FeaturePointsType::PointType PointType;
  PointType point;
  point[ 0 ] = 134.0;
  point[ 1 ] = 109.0;
  point[ 2 ] = 58.0;
  pointsContainer->push_back( point );
  point[ 0 ] = 134.0;
  point[ 1 ] = 109.0;
  point[ 2 ] = 60.0;
  pointsContainer->push_back( point );
  point[ 0 ] = 125.0;
  point[ 1 ] = 135.0;
  point[ 2 ] = 80.0;
  pointsContainer->push_back( point );
  point[ 0 ] = 126.0;
  point[ 1 ] = 134.0;
  point[ 2 ] = 69.0;
  pointsContainer->push_back( point );
  point[ 0 ] = 136.0;
  point[ 1 ] = 108.0;
  point[ 2 ] = 58.0;
  pointsContainer->push_back( point );
  point[ 0 ] = 134.0;
  point[ 1 ] = 109.0;
  point[ 2 ] = 56.0;
  pointsContainer->push_back( point );
  point[ 0 ] = 125.0;
  point[ 1 ] = 133.0;
  point[ 2 ] = 80.0;
  pointsContainer->push_back( point );
  point[ 0 ] = 124.0;
  point[ 1 ] = 135.0;
  point[ 2 ] = 78.0;
  pointsContainer->push_back( point );
  point[ 0 ] = 124.0;
  point[ 1 ] = 135.0;
  point[ 2 ] = 76.0;
  pointsContainer->push_back( point );
  point[ 0 ] = 126.0;
  point[ 1 ] = 134.0;
  point[ 2 ] = 67.0;
  pointsContainer->push_back( point );
  pointSet->SetPoints( pointsContainer );

  // Create transformed image from input to match with
  typedef itk::TranslationTransform< double, Dimension > TranslationTransformType;
  TranslationTransformType::Pointer transform = TranslationTransformType::New();
  TranslationTransformType::OutputVectorType translation;
  // move each pixel in input image 3 pixels along first(0) dimension
  translation[0] = 3.0;
  translation[1] = 0.0;
  translation[2] = 0.0;
  transform->Translate(translation);

  typedef itk::ResampleImageFilter< InputImageType, InputImageType > ResampleImageFilterType;
  ResampleImageFilterType::Pointer resampleFilter = ResampleImageFilterType::New();
  resampleFilter->SetTransform( transform.GetPointer() );
  resampleFilter->SetInput( reader->GetOutput() );
  resampleFilter->SetReferenceImage( reader->GetOutput() );
  resampleFilter->UseReferenceImageOn();

  // cast InputImageType to GPUInputImageType
  typedef itk::CastImageFilter< InputImageType, GPUInputImageType >  CasterType;
  typename CasterType::Pointer fixedCaster = CasterType::New();
  fixedCaster->SetInput( resampleFilter->GetOutput() );
  fixedCaster->Update();
  typename CasterType::Pointer movingCaster = CasterType::New();
  movingCaster->SetInput( reader->GetOutput() );
  movingCaster->Update();

  // inputs (all required)
  GPUBlockMatchingFilter->SetFixedImage( fixedCaster->GetOutput() );
  GPUBlockMatchingFilter->SetMovingImage( movingCaster->GetOutput() );
  GPUBlockMatchingFilter->SetFeaturePoints( pointSet );
  CPUBlockMatchingFilter->SetFixedImage( fixedCaster->GetOutput() );
  CPUBlockMatchingFilter->SetMovingImage( movingCaster->GetOutput() );
  CPUBlockMatchingFilter->SetFeaturePoints( pointSet );

  // parameters (all optional)
  GPUBlockMatchingFilter->SetBlockRadius( blockRadius );
  GPUBlockMatchingFilter->SetSearchRadius( searchRadius );
  CPUBlockMatchingFilter->SetBlockRadius( blockRadius );
  CPUBlockMatchingFilter->SetSearchRadius( searchRadius );

  try
    {
    GPUBlockMatchingFilter->Update();
    }
  catch ( itk::ExceptionObject &err )
    {
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }
  BlockMatchingFilterType::DisplacementsType::PointDataContainer::ConstIterator
    dispsItrGPU = GPUBlockMatchingFilter->GetDisplacements()->GetPointData()->Begin();
  BlockMatchingFilterType::SimilaritiesType::PointDataContainer::ConstIterator
    simsItrGPU = GPUBlockMatchingFilter->GetSimilarities()->GetPointData()->Begin();

  // CPU BM
  try
    {
    CPUBlockMatchingFilter->Update();
    }
  catch ( itk::ExceptionObject &err )
    {
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }
  BlockMatchingFilterType::DisplacementsType::PointDataContainer::ConstIterator
    dispsItrCPU = CPUBlockMatchingFilter->GetDisplacements()->GetPointData()->Begin();
  BlockMatchingFilterType::SimilaritiesType::PointDataContainer::ConstIterator
    simsItrCPU = CPUBlockMatchingFilter->GetSimilarities()->GetPointData()->Begin();

  FeaturePointsType::PointsContainerConstIterator pointsItr = pointSet->GetPoints()->Begin();
  FeaturePointsType::PointsContainerConstIterator pointsEnd = pointSet->GetPoints()->End();

  // compare CPU and GPU results
  while ( pointsItr != pointsEnd )
    {
    double e = 1e-6;
    if ( std::abs( simsItrCPU.Value() - simsItrGPU.Value() ) > e ) return EXIT_FAILURE;
    if ( std::abs( dispsItrCPU.Value()[0] - dispsItrGPU.Value()[0] ) > e ) return EXIT_FAILURE;
    if ( std::abs( dispsItrCPU.Value()[1] - dispsItrGPU.Value()[1] ) > e ) return EXIT_FAILURE;
    if ( std::abs( dispsItrCPU.Value()[2] - dispsItrGPU.Value()[2] ) > e ) return EXIT_FAILURE;

    simsItrGPU++;
    dispsItrGPU++;
    simsItrCPU++;
    dispsItrCPU++;
    pointsItr++;
    }

  return EXIT_SUCCESS;
}
