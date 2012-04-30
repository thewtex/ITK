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
#ifndef __itkPhysicsBasedNonRigidRegistrationFilter_hxx
#define __itkPhysicsBasedNonRigidRegistrationFilter_hxx

#include <ctime>
#include <iostream>

#include "itkPhysicsBasedNonRigidRegistrationFilter.h"

namespace itk
{

namespace fem
{

template <class TFixedImage, class TMovingImage, class TMaskImage, class TMesh, class TDeformationField>
PhysicsBasedNonRigidRegistrationFilter<TFixedImage, TMovingImage, TMaskImage, TMesh, TDeformationField>
::~PhysicsBasedNonRigidRegistrationFilter()
{
}

template <class TFixedImage, class TMovingImage, class TMaskImage, class TMesh, class TDeformationField>
PhysicsBasedNonRigidRegistrationFilter<TFixedImage, TMovingImage, TMaskImage, TMesh, TDeformationField>
::PhysicsBasedNonRigidRegistrationFilter()
{
  // defaults
  m_NonConnectivity = 0; // VERTEX_CONNECTIVITY
  m_SelectFraction = 0.1;
  m_BlockRadius.Fill( 2 );
  m_SearchRadius.Fill( 5 );
  m_BlockStep.Fill( 1 );

  // all inputs are required
  this->RemoveRequiredInputName("Primary");
  this->AddRequiredInputName("FixedImage");
  this->AddRequiredInputName("MovingImage");
  this->AddRequiredInputName("MaskImage");
  this->AddRequiredInputName("Mesh");
}

template <class TFixedImage, class TMovingImage, class TMaskImage, class TMesh, class TDeformationField>
void
PhysicsBasedNonRigidRegistrationFilter<TFixedImage, TMovingImage, TMaskImage, TMesh, TDeformationField>
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "m_BlockRadius: " << m_BlockRadius << std::endl
     << indent << "m_SearchRadius: " << m_SearchRadius << std::endl
     << indent << "m_BlockStep: " << m_BlockStep << std::endl;
}

template <class TFixedImage, class TMovingImage, class TMaskImage, class TMesh, class TDeformationField>
void
PhysicsBasedNonRigidRegistrationFilter<TFixedImage, TMovingImage, TMaskImage, TMesh, TDeformationField>
::GenerateData()
{
  // feature selection
  typedef MaskFeaturePointSelectionFilter< MovingImageType, MaskImageType >  FeatureSelectionFilterType;
  typename FeatureSelectionFilterType::Pointer featureSelectionFilter = FeatureSelectionFilterType::New();

  featureSelectionFilter->SetInput( this->GetFixedImage() );
  featureSelectionFilter->SetMaskImage( this->GetMaskImage() );
  featureSelectionFilter->SetSelectFraction( this->m_SelectFraction );
  featureSelectionFilter->SetNonConnectivity( this->m_NonConnectivity );
  featureSelectionFilter->SetBlockRadius( this->m_BlockRadius );
  featureSelectionFilter->ComputeStructureTensorsOn();

std::cout << "FS: ";
const clock_t t1 = std::clock();

  featureSelectionFilter->Update();

const clock_t t2 = std::clock();
std::cout << ( t2 - t1 ) /  CLOCKS_PER_SEC << "sec" << std::endl;

  // copy output since using it as input causes repeated calls up the pipeline -- there must be a better way
  unsigned pointsCount = featureSelectionFilter->GetOutput()->GetNumberOfPoints();
  typename FeatureSelectionFilterType::FeaturePointsPointer featurePoints = FeatureSelectionFilterType::FeaturePointsType::New();
  typename FeatureSelectionFilterType::FeaturePointsType::PointsContainer::Pointer featurePointsContainer = FeatureSelectionFilterType::FeaturePointsType::PointsContainer::New();
  typename FeatureSelectionFilterType::FeaturePointsType::PointDataContainer::Pointer featurePointDataContainer = FeatureSelectionFilterType::FeaturePointsType::PointDataContainer::New();

  for ( unsigned i = 0; i < pointsCount; i++ )
    {
      featurePointsContainer->InsertElement( i, featureSelectionFilter->GetOutput()->GetPoint( i ) );

      typename FeatureSelectionFilterType::FeaturePointsType::PixelType p;
      featureSelectionFilter->GetOutput()->GetPointData( i, &p );
      featurePointDataContainer->InsertElement( i, p );
    }
  featurePoints->SetPoints( featurePointsContainer );
  featurePoints->SetPointData( featurePointDataContainer );


  // block matching
  typedef BlockMatchingImageFilter< FixedImageType, MovingImageType >  BlockMatchingFilterType;
  typename BlockMatchingFilterType::Pointer blockMatchingFilter = BlockMatchingFilterType::New();

  blockMatchingFilter->SetFixedImage( this->GetFixedImage() );
  blockMatchingFilter->SetMovingImage( this->GetMovingImage() );
  blockMatchingFilter->SetFeaturePoints( featurePoints );
  blockMatchingFilter->SetBlockRadius( m_BlockRadius );
  blockMatchingFilter->SetSearchRadius( m_SearchRadius );
  blockMatchingFilter->SetBlockStep( m_BlockStep );

std::cout << "BM: ";
const clock_t t3 = std::clock();

  blockMatchingFilter->Update();

const clock_t t4 = std::clock();
std::cout << ( t4 - t3 ) /  CLOCKS_PER_SEC << "sec" << std::endl;

  // copy outputs...
  typename BlockMatchingFilterType::DisplacementsPointer displacements = BlockMatchingFilterType::DisplacementsType::New();
  typename BlockMatchingFilterType::DisplacementsType::PointsContainer::Pointer displacementsContainer = BlockMatchingFilterType::DisplacementsType::PointsContainer::New();
  typename BlockMatchingFilterType::DisplacementsType::PointDataContainer::Pointer displacementsDataContainer = BlockMatchingFilterType::DisplacementsType::PointDataContainer::New();

  typename BlockMatchingFilterType::SimilaritiesPointer similarities = BlockMatchingFilterType::SimilaritiesType::New();
  typename BlockMatchingFilterType::SimilaritiesType::PointsContainer::Pointer similaritiesContainer = BlockMatchingFilterType::SimilaritiesType::PointsContainer::New();
  typename BlockMatchingFilterType::SimilaritiesType::PointDataContainer::Pointer similaritiesDataContainer = BlockMatchingFilterType::SimilaritiesType::PointDataContainer::New();

  for ( unsigned i = 0; i < pointsCount; i++ )
    {
      displacementsContainer->InsertElement( i, blockMatchingFilter->GetDisplacements()->GetPoint( i ) );

      typename BlockMatchingFilterType::DisplacementsType::PixelType pd;
      blockMatchingFilter->GetDisplacements()->GetPointData( i, &pd );
      displacementsDataContainer->InsertElement( i, pd );

      similaritiesContainer->InsertElement( i, blockMatchingFilter->GetSimilarities()->GetPoint( i ) );

      typename BlockMatchingFilterType::SimilaritiesType::PixelType ps;
      blockMatchingFilter->GetSimilarities()->GetPointData( i, &ps );
      similaritiesDataContainer->InsertElement( i, ps );
    }
  displacements->SetPoints( displacementsContainer );
  displacements->SetPointData( displacementsDataContainer );
  similarities->SetPoints( similaritiesContainer );
  similarities->SetPointData( similaritiesDataContainer );


  // assembly and solver
  typedef FEMScatteredDataPointSetToImageFilter<
    typename BlockMatchingFilterType::DisplacementsType,
    MeshType,
    DeformationFieldType,
    typename BlockMatchingFilterType::SimilaritiesType,
    typename FeatureSelectionFilterType::FeaturePointsType // tensors are optional pixel values of feature points pointset
  >  FilterType;

  typename FilterType::Pointer filter = FilterType::New();

  filter->SetInput( displacements );
  filter->SetConfidencePointSet( similarities );
  filter->SetTensorPointSet( featurePoints );
  filter->SetMesh( const_cast< MeshType * >( this->GetMesh() ) ); // TODO: make const_cast unnecessary

  filter->SetSpacing( GetFixedImage()->GetSpacing() );
  filter->SetOrigin( GetFixedImage()->GetOrigin() );
  filter->SetSize( GetFixedImage()->GetLargestPossibleRegion().GetSize() );

std::cout << "Solver: ";
const clock_t t5 = std::clock();

  filter->Update();

const clock_t t6 = std::clock();
std::cout << ( t6 - t5 ) /  CLOCKS_PER_SEC << "sec" << std::endl;

  this->SetNthOutput( 0, filter->GetOutput() );
}

}
}  // end namespace itk::fem

#endif
