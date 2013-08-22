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
#include "itkCastImageFilter.h"
#include "itkTimeProbe.h"

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
  std::cout << "Defines: " << defines.str() << std::endl;

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
TimeProbe clock, clockGPU, clockKernel;
clock.Start();

  typename Superclass::FixedImageConstPointer fixedImage = this->GetFixedImage();
  typename Superclass::MovingImageConstPointer movingImage = this->GetMovingImage();
  typename Superclass::ImageSizeType imageSize = fixedImage->GetLargestPossibleRegion().GetSize();

  // 1D images for passing feature points and retrieving displacements
  typedef GPUImage< cl_uint, 1 >  IndexImage;
  typedef GPUImage< typename Superclass::SimilaritiesValue, 1 >  SimilaritiesImage;

  typename Superclass::FeaturePointsConstPointer featurePoints = this->GetFeaturePoints();
  cl_uint numberOfPoints = featurePoints->GetNumberOfPoints();

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


  typedef typename Superclass::FeaturePointsType::PointsContainer::ConstIterator FeaturestIteratorType;
  FeaturestIteratorType pointItr = featurePoints->GetPoints()->Begin();
  FeaturestIteratorType pointEnd = featurePoints->GetPoints()->End();

  ImageRegionIterator<IndexImage> iter( features, regionX );

  // copy feature points index coordinates into an image (1D, 3 * numberOfPoints pixels)
  while ( pointItr != pointEnd )
    {
    typename Superclass::ImageIndexType voxelIndex;
    fixedImage->TransformPhysicalPointToIndex( pointItr.Value(), voxelIndex );

    iter.Set( voxelIndex[0] ); ++iter; // X
    iter.Set( voxelIndex[1] ); ++iter; // Y
    iter.Set( voxelIndex[2] ); ++iter; // Z

    pointItr++;
    }

  // cast Image to GPUImage, dynamic_cast gives null FIXME
  typedef typename itk::GPUTraits< TFixedImage >::Type GPUFixedImage;
  typedef CastImageFilter< typename Superclass::FixedImageType, GPUFixedImage > FixedCasterType;
  typename FixedCasterType::Pointer fixedCaster = FixedCasterType::New();
  fixedCaster->SetInput( fixedImage );
  fixedCaster->Update();
  typename GPUFixedImage::Pointer fixedGPUImage = fixedCaster->GetOutput();

  typedef typename itk::GPUTraits< TMovingImage >::Type GPUMovingImage;
  typedef CastImageFilter< typename Superclass::MovingImageType, GPUMovingImage > MovingCasterType;
  typename MovingCasterType::Pointer movingCaster = MovingCasterType::New();
  movingCaster->SetInput( movingImage );
  movingCaster->Update();
  typename GPUFixedImage::Pointer movingGPUImage = movingCaster->GetOutput();

clockGPU.Start();

  // pass images to kernel
  int argidx = 0;

  this->m_GPUKernelManager->SetKernelArgWithImage(m_GPUKernelHandle, argidx++, fixedGPUImage->GetGPUDataManager() );
  this->m_GPUKernelManager->SetKernelArgWithImage(m_GPUKernelHandle, argidx++, movingGPUImage->GetGPUDataManager() );
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

  size_t globalSize, localSize;
  localSize = this->m_WorkGroupSize; //OpenCLGetLocalBlockSize( 1 );
  globalSize = localSize * numberOfPoints;

std::cout << "numberOfPoints: " << numberOfPoints
          << ", globalSize: "   << globalSize
          << ", localSize: " << localSize << std::endl;

  // allocate local workgroup memory
  size_t windowVoxelCount = ( ( clSearchRadius.s[0] + clBlockRadius.s[0] ) * 2 + 1 )
                          * ( ( clSearchRadius.s[1] + clBlockRadius.s[1] ) * 2 + 1 )
                          * ( ( clSearchRadius.s[2] + clBlockRadius.s[2] ) * 2 + 1 );
  size_t blockVoxelCount = ( clBlockRadius.s[0] * 2 + 1 )
                         * ( clBlockRadius.s[1] * 2 + 1 )
                         * ( clBlockRadius.s[2] * 2 + 1 );

  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( typename TFixedImage::PixelType ) * windowVoxelCount, NULL ); // FIXEDTYPE* windowBuffer
  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( typename TMovingImage::PixelType ) * blockVoxelCount, NULL ); // MOVINGTYPE* blockBuffer
  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( cl_uint ) * 3 * localSize, NULL ); // uint *localDisplacements,
  this->m_GPUKernelManager->SetKernelArg(m_GPUKernelHandle, argidx++, sizeof( typename Superclass::SimilaritiesValue ) * localSize, NULL ); // SIMTYPE *localSimilarities

clockKernel.Start();

  // launch kernel
  this->m_GPUKernelManager->LaunchKernel( m_GPUKernelHandle, 1, &globalSize, &localSize );

clockKernel.Stop();

  // copy displacements and similarities to the output
  displacements->UpdateBuffers();
  similarities->UpdateBuffers();

clockGPU.Stop();

  ImageRegionIterator<IndexImage> dispIter( displacements, regionX );
  ImageRegionIterator<SimilaritiesImage> similaritiesIter( similarities, region );

  const typename Superclass::FeaturePointsType::PointsContainer *points = featurePoints->GetPoints();

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

clock.Stop();
std::cout << "GPUGenerateData: " <<
  clockKernel.GetTotal() << "s kernel, " <<
  clockGPU.GetTotal() << "s ( +" << ( clockGPU.GetTotal() - clockKernel.GetTotal() ) << "s ) with GPU-Host data transfers, "<<
  clock.GetTotal() << "s ( +" << ( clock.GetTotal() - clockGPU.GetTotal() ) << "s ) total" << std::endl;

}

} // end namespace itk

#endif
