1/*=========================================================================
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

namespace itk
{
namespace fem
{


template <class TMovingImage, class TFixedImage, class TMesh, class TDeformationField>
PhysicsBasedNonRigidRegistrationFilter<TMovingImage, TFixedImage, TMesh, TDeformationField>
::~PhysicsBasedNonRigidRegistrationFilter()
{
}

template <class TMovingImage, class TFixedImage, class TMesh, class TDeformationField>
PhysicsBasedNonRigidRegistrationFilter<TMovingImage, TFixedImage, TMesh, TDeformationField>
::PhysicsBasedNonRigidRegistrationFilter()
{
}

template <class TMovingImage, class TFixedImage, class TMesh, class TDeformationField>
void PhysicsBasedNonRigidRegistrationFilter<TMovingImage, TFixedImage, TMesh, TDeformationField>
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "m_BlockRadius: " << m_BlockRadius << std::endl
     << indent << "m_SearchRadius: " << m_SearchRadius << std::endl
     << indent << "m_BlockStep: " << m_BlockStep << std::endl;
}

template <class TMovingImage, class TFixedImage, class TMesh, class TDeformationField>
void PhysicsBasedNonRigidRegistrationFilter<TMovingImage, TFixedImage, TMesh, TDeformationField>
::GenerateData()
{
  // feature selection
  typedef itk::MaskFeaturePointSelectionFilter< InputImageType >  FeatureSelectionFilterType;
  FeatureSelectionFilterType::Pointer featureSelectionFilter = FeatureSelectionFilterType::New();

  featureSelectionFilter->SetInput( this->GetFixedImage() );
  featureSelectionFilter->SetSelectFraction( this->m_SelectFraction );
  featureSelectionFilter->SetBlockRadius( this->m_BlockRadius );
  featureSelectionFilter->ComputeStructureTensorsOn();

  featureSelectionFilter->Update();

  // block matching
  typedef itk::BlockMatchingImageFilter< InputImageType >  BlockMatchingFilterType;
  BlockMatchingFilterType::Pointer blockMatchingFilter = BlockMatchingFilterType::New();

  blockMatchingFilter->SetFixedImage( this->GetFixedImage() );
  blockMatchingFilter->SetMovingImage( this->GetMovingImage() );
  blockMatchingFilter->SetFeaturePoints( featureSelectionFilter->GetOutput() );
  blockMatchingFilter->SetBlockRadius( m_BlockRadius );
  blockMatchingFilter->SetSearchRadius( m_SearchRadius );
  blockMatchingFilter->SetBlockStep( m_BlockStep );

  blockMatchingFilter->Update();

  // assembly and solver
  typedef itk::fem::FEMScatteredDataPointSetToImageFilter<
    BlockMatchingFilterType::DisplacementsType,
    MeshType,
    DeformationFieldType,
    BlockMatchingFilterType::SimilaritiesType,
    FeatureSelectionFilterType::FeaturePointsType // tensors are optional pixel values of feature points pointset
  >  FilterType;

  FilterType::Pointer filter = FilterType::New();

  filter->SetInput( blockMatchingFilter->GetDisplacements() ); // GetOutput() returns the same
  filter->SetConfidencePointSet( blockMatchingFilter->GetSimilarities() );
  filter->SetTensorPointSet( featureSelectionFilter->GetOutput() );
  filter->SetMesh( const_cast< MeshType * >( this->GetMesh() ) ); // TODO: make const_cast unnecessary

  filter->SetSpacing( GetFixedImage()->GetSpacing() );
  filter->SetOrigin( GetFixedImage()->GetOrigin() );
  filter->SetSize( GetFixedImage()->GetRequestedRegion().GetSize() );

  filter->Update();
  this->SetNthOutput( 0, filter->GetOutput() );
}

}
}  // end namespace itk::fem

#endif
