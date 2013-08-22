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
#ifndef __itkGPUBlockMatchingImageFilter_hxx
#define __itkGPUBlockMatchingImageFilter_hxx

#include "itkGPUBlockMatchingImageFilter.h"

namespace itk
{

template< class TFixedImage, class TMovingImage, class TFeatures, class TDisplacements, class TSimilarities >
GPUBlockMatchingImageFilter< TFixedImage, TMovingImage, TFeatures, TDisplacements, TSimilarities >
::GPUBlockMatchingImageFilter()
{
  m_GPUEnabled = true;
  m_GPUKernelManager = GPUKernelManager::New();
  m_WorkGroupSize = 32;

  std::ostringstream defines;

  defines << "#define FIXEDTYPE ";
  GetTypenameInString( typeid ( typename TFixedImage::PixelType ), defines );

  defines << "#define MOVINGTYPE ";
  GetTypenameInString( typeid ( typename TMovingImage::PixelType ), defines );

  defines << "#define SIMTYPE ";
  GetTypenameInString( typeid ( typename Superclass::SimilaritiesValue ), defines );

  // load and build program
  const char* GPUSource = GPUBlockMatchingImageFilter::GetOpenCLSource();
  m_GPUKernelManager->LoadProgramFromString( GPUSource, defines.str().c_str() );

  // create kernel
  m_GPUKernelHandle = this->m_GPUKernelManager->CreateKernel("BlockMatchingFilter");
}

template< class TFixedImage, class TMovingImage, class TFeatures, class TDisplacements, class TSimilarities >
GPUBlockMatchingImageFilter< TFixedImage, TMovingImage, TFeatures, TDisplacements, TSimilarities >
::~GPUBlockMatchingImageFilter()
{
}

template< class TFixedImage, class TMovingImage, class TFeatures, class TDisplacements, class TSimilarities >
void
GPUBlockMatchingImageFilter< TFixedImage, TMovingImage, TFeatures, TDisplacements, TSimilarities >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "GPU: " << ( m_GPUEnabled ? "Enabled" : "Disabled" ) << std::endl;
}


template< class TFixedImage, class TMovingImage, class TFeatures, class TDisplacements, class TSimilarities >
void
GPUBlockMatchingImageFilter< TFixedImage, TMovingImage, TFeatures, TDisplacements, TSimilarities >
::GenerateData()
{
if( !m_GPUEnabled ) // call CPU update function
  {
  Superclass::GenerateData();
  }
else // call GPU update function
  {
  // Call a method to allocate memory for the filter's outputs
  //  this->AllocateOutputs();
  GPUGenerateData();
  }
}


template< class TFixedImage, class TMovingImage, class TFeatures, class TDisplacements, class TSimilarities >
void
GPUBlockMatchingImageFilter< TFixedImage, TMovingImage, TFeatures, TDisplacements, TSimilarities >
::GPUGenerateData()
{
  // 1D images for passing feature points and retrieving displacements/similarities
  typedef GPUImage< cl_uint, 1 >  IndexImage;
  typedef GPUImage< typename Superclass::SimilaritiesValue, 1 >  SimilaritiesImage;

  typename Superclass::FeaturePointsConstPointer featurePoints = this->GetFeaturePoints();
  cl_uint numberOfPoints = featurePoints->GetNumberOfPoints();

  typename Superclass::FixedImageConstPointer fixedImage = this->GetFixedImage();
  typename Superclass::MovingImageConstPointer movingImage = this->GetMovingImage();
  typename Superclass::ImageSizeType imageSize = fixedImage->GetLargestPossibleRegion().GetSize();

  IndexImage::IndexType start;
  start[0] = 0;

  IndexImage::SizeType size;
  size[0] = numberOfPoints;

  IndexImage::RegionType region;
  region.SetIndex( start );
  region.SetSize( size );

  typename SimilaritiesImage::Pointer similarities = SimilaritiesImage::New();
  similarities->SetRegions( region );
  similarities->Allocate();

  IndexImage::RegionType regionX;
  regionX.SetIndex( start );
  size[0] = numberOfPoints * Superclass::ImageDimension; // X, Y and Z indices
  regionX.SetSize( size );

  IndexImage::Pointer features = IndexImage::New();
  features->SetRegions( regionX );
  features->Allocate();

  IndexImage::Pointer displacements = IndexImage::New();
  displacements->SetRegions( regionX );
  displacements->Allocate();


  typedef typename Superclass::FeaturePointsType::PointsContainerConstIterator  FeaturestIteratorType;
  const typename Superclass::FeaturePointsType::PointsContainer  *points = featurePoints->GetPoints();
  FeaturestIteratorType  pointsItr = points->Begin();
  FeaturestIteratorType  pointsEnd = points->End();
  ImageRegionIterator<IndexImage> featuresItr( features, regionX );

  // copy feature points index coordinates into an image (1D, 3 * numberOfPoints pixels)
  while ( pointsItr != pointsEnd )
    {
    typename Superclass::ImageIndexType voxelIndex;
    fixedImage->TransformPhysicalPointToIndex( pointsItr.Value(), voxelIndex );

    featuresItr.Set( voxelIndex[0] ); ++featuresItr; // X
    featuresItr.Set( voxelIndex[1] ); ++featuresItr; // Y
    featuresItr.Set( voxelIndex[2] ); ++featuresItr; // Z

    pointsItr++;
    }

  // pass images to kernel
  int argidx = 0;

  this->m_GPUKernelManager->SetKernelArgWithImage(m_GPUKernelHandle, argidx++, fixedImage->GetGPUDataManager() );
  this->m_GPUKernelManager->SetKernelArgWithImage(m_GPUKernelHandle, argidx++, movingImage->GetGPUDataManager() );
  this->m_GPUKernelManager->SetKernelArgWithImage(m_GPUKernelHandle, argidx++, features->GetGPUDataManager() );
  this->m_GPUKernelManager->SetKernelArgWithImage(m_GPUKernelHandle, argidx++, displacements->GetGPUDataManager() );
  this->m_GPUKernelManager->SetKernelArgWithImage(m_GPUKernelHandle, argidx++, similarities->GetGPUDataManager() );

  // imageSize
  cl_uint4 clImageSize;
  clImageSize.s[0] = imageSize[0];
  clImageSize.s[1] = imageSize[1];
  clImageSize.s[2] = imageSize[2];
  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( clImageSize ), &clImageSize );


  // block radius
  cl_uint4 clBlockRadius;
  clBlockRadius.s[0] = this->GetBlockRadius()[0];
  clBlockRadius.s[1] = this->GetBlockRadius()[1];
  clBlockRadius.s[2] = this->GetBlockRadius()[2];
  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( clBlockRadius ), &clBlockRadius );

  // search radius
  cl_uint4 clSearchRadius;
  clSearchRadius.s[0]= this->GetSearchRadius()[0];
  clSearchRadius.s[1] = this->GetSearchRadius()[1];
  clSearchRadius.s[2] = this->GetSearchRadius()[2];
  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( clSearchRadius ), &clSearchRadius );

  // m_WorkGroupSize must be a power of 2
  if ( m_WorkGroupSize < 1 || ( m_WorkGroupSize & ( m_WorkGroupSize - 1 ) ) )
    {
    itkExceptionMacro( "WorkGroupSize cannot be " << m_WorkGroupSize
      << ", expected 2^n value" );
    }
  size_t globalSize, localSize;
  localSize = this->m_WorkGroupSize;
  globalSize = localSize * numberOfPoints;

  // allocate local workgroup memory
  size_t windowVoxelCount = ( ( clSearchRadius.s[0] + clBlockRadius.s[0] ) * 2 + 1 )
                          * ( ( clSearchRadius.s[1] + clBlockRadius.s[1] ) * 2 + 1 )
                          * ( ( clSearchRadius.s[2] + clBlockRadius.s[2] ) * 2 + 1 );
  size_t blockVoxelCount = ( clBlockRadius.s[0] * 2 + 1 )
                         * ( clBlockRadius.s[1] * 2 + 1 )
                         * ( clBlockRadius.s[2] * 2 + 1 );

  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( typename TFixedImage::PixelType ) * windowVoxelCount, NULL );
  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( typename TMovingImage::PixelType ) * blockVoxelCount, NULL );
  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( cl_uint ) * 3 * localSize, NULL );
  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( typename Superclass::SimilaritiesValue ) * localSize, NULL );

  // launch kernel
  this->m_GPUKernelManager->LaunchKernel( m_GPUKernelHandle, 1, &globalSize, &localSize );

  // copy displacements and similarities to the output
  displacements->GetGPUDataManager()->UpdateCPUBuffer();
  similarities->GetGPUDataManager()->UpdateCPUBuffer();

  ImageRegionIterator<IndexImage> dispIter( displacements, regionX );
  ImageRegionIterator<SimilaritiesImage> similaritiesIter( similarities, region );

  typename Superclass::DisplacementsPointer displacementsPointSet = Superclass::GetDisplacements();

  typedef typename Superclass::DisplacementsType::PointsContainerPointer  DisplacementsPointsContainerPointerType;
  typedef typename Superclass::DisplacementsType::PointsContainer         DisplacementsPointsContainerType;
  DisplacementsPointsContainerPointerType displacementsPoints = DisplacementsPointsContainerType::New();

  typedef typename Superclass::DisplacementsType::PointDataContainerPointer  DisplacementsPointDataContainerPointerType;
  typedef typename Superclass::DisplacementsType::PointDataContainer         DisplacementsPointDataContainerType;
  DisplacementsPointDataContainerPointerType displacementsData = DisplacementsPointDataContainerType::New();

  typename Superclass::SimilaritiesPointer similaritiesPointSet = Superclass::GetSimilarities();

  typedef typename Superclass::SimilaritiesType::PointsContainerPointer  SimilaritiesPointsContainerPointerType;
  typedef typename Superclass::SimilaritiesType::PointsContainer         SimilaritiesPointsContainerType;
  SimilaritiesPointsContainerPointerType similaritiesPoints = SimilaritiesPointsContainerType::New();

  typedef typename Superclass::SimilaritiesType::PointDataContainerPointer  SimilaritiesPointDataContainerPointerType;
  typedef typename Superclass::SimilaritiesType::PointDataContainer         SimilaritiesPointDataContainerType;
  SimilaritiesPointDataContainerPointerType similaritiesData = SimilaritiesPointDataContainerType::New();

  SizeValueType i = 0;
  while ( !similaritiesIter.IsAtEnd() )
    {
    typename Superclass::FeaturePointsPhysicalCoordinates originalLocation = points->GetElement( i );
    typename Superclass::DisplacementsVector displacementVector;

    typename Superclass::ImageIndexType newLocationIdx;
    newLocationIdx[0] = dispIter.Value(); ++dispIter;
    newLocationIdx[1] = dispIter.Value(); ++dispIter;
    newLocationIdx[2] = dispIter.Value(); ++dispIter;

    typename Superclass::FeaturePointsPhysicalCoordinates newLocation;
    fixedImage->TransformIndexToPhysicalPoint( newLocationIdx, newLocation );
    displacementVector = newLocation - originalLocation;

    displacementsPoints->InsertElement( i, points->GetElement( i ) );
    similaritiesPoints->InsertElement( i, points->GetElement( i ) );
    displacementsData->InsertElement( i, displacementVector );
    similaritiesData->InsertElement( i, similaritiesIter.Value() );

    ++i;
    ++similaritiesIter;
    }

    displacementsPointSet->SetPoints( displacementsPoints );
    displacementsPointSet->SetPointData( displacementsData );
    similaritiesPointSet->SetPoints( similaritiesPoints );
    similaritiesPointSet->SetPointData( similaritiesData );
}

} // end namespace itk

#endif
