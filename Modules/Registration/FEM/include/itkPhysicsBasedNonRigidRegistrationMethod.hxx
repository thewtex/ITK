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
#ifndef __itkPhysicsBasedNonRigidRegistrationMethod_hxx
#define __itkPhysicsBasedNonRigidRegistrationMethod_hxx

#include <iostream>
#include "itkTimeProbe.h"
#include "itkPhysicsBasedNonRigidRegistrationMethod.h"

// \todo remove debug code
//#include "itkPipelineMonitorImageFilter.h"

namespace itk
{

namespace fem
{

template <class TFixedImage, class TMovingImage, class TMaskImage, class TMesh, class TDeformationField>
PhysicsBasedNonRigidRegistrationMethod<TFixedImage, TMovingImage, TMaskImage, TMesh, TDeformationField>
::~PhysicsBasedNonRigidRegistrationMethod()
{
}

template <class TFixedImage, class TMovingImage, class TMaskImage, class TMesh, class TDeformationField>
PhysicsBasedNonRigidRegistrationMethod<TFixedImage, TMovingImage, TMaskImage, TMesh, TDeformationField>
::PhysicsBasedNonRigidRegistrationMethod()
{
  // defaults
  this->m_NonConnectivity = 0; // VERTEX_CONNECTIVITY
  this->m_SelectFraction = 0.1;
  this->m_BlockRadius.Fill( 2 );
  this->m_SearchRadius.Fill( 5 );
  this->m_ApproximationSteps = 10;
  this->m_OutlierRejectionSteps = 10;

  // all inputs are required
  this->AddRequiredInputName("FixedImage");
  this->AddRequiredInputName("MovingImage");
  this->AddRequiredInputName("MaskImage");
  this->AddRequiredInputName("Mesh");
}

template <class TFixedImage, class TMovingImage, class TMaskImage, class TMesh, class TDeformationField>
void
PhysicsBasedNonRigidRegistrationMethod<TFixedImage, TMovingImage, TMaskImage, TMesh, TDeformationField>
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "m_BlockRadius: " << m_BlockRadius << std::endl
     << indent << "m_SearchRadius: " << m_SearchRadius << std::endl
     << indent << "m_SelectFraction: " << m_SelectFraction << std::endl
     << indent << "m_NonConnectivity: " << m_NonConnectivity << std::endl
     << indent << "m_ApproximationSteps: " << m_ApproximationSteps << std::endl
     << indent << "m_OutlierRejectionSteps: " << m_OutlierRejectionSteps << std::endl;
}

template <class TFixedImage, class TMovingImage, class TMaskImage, class TMesh, class TDeformationField>
void
PhysicsBasedNonRigidRegistrationMethod<TFixedImage, TMovingImage, TMaskImage, TMesh, TDeformationField>
::GenerateData()
{
  // feature selection
  typedef MaskFeaturePointSelectionFilter< MovingImageType, MaskImageType >  FeatureSelectionFilterType;
  typename FeatureSelectionFilterType::Pointer featureSelectionFilter = FeatureSelectionFilterType::New();

  featureSelectionFilter->SetInput( this->GetMovingImage() );
  featureSelectionFilter->SetMaskImage( this->GetMaskImage() );
  featureSelectionFilter->SetSelectFraction( this->m_SelectFraction );
  featureSelectionFilter->SetNonConnectivity( this->m_NonConnectivity );
  featureSelectionFilter->SetBlockRadius( this->m_BlockRadius );
  featureSelectionFilter->ComputeStructureTensorsOn();

  //typedef PipelineMonitorImageFilter< FixedImageType > PipelineMonitorImageFilterType;
  //typename PipelineMonitorImageFilterType::Pointer pipelineMonitorImageFilter = PipelineMonitorImageFilterType::New();
  //pipelineMonitorImageFilter->SetInput( this->GetFixedImage() );
  //pipelineMonitorImageFilter->DebugOn();


  // block matching
  typedef BlockMatchingImageFilter< FixedImageType, MovingImageType >  BlockMatchingFilterType;
  typename BlockMatchingFilterType::Pointer blockMatchingFilter = BlockMatchingFilterType::New();

  //blockMatchingFilter->SetFixedImage( pipelineMonitorImageFilter->GetOutput() );
  blockMatchingFilter->SetMovingImage( this->GetMovingImage() );
  blockMatchingFilter->SetFeaturePoints( featureSelectionFilter->GetOutput() ); //  blockMatchingFilter->SetFeaturePoints( featureSelectionFilter->GetOutput() );
  blockMatchingFilter->SetBlockRadius( m_BlockRadius );
  blockMatchingFilter->SetSearchRadius( m_SearchRadius );

  itk::TimeProbe bmtimer;
  bmtimer.Start();

  blockMatchingFilter->Update();

  bmtimer.Stop();
  std::cout << "Block matching took " << bmtimer.GetMeanTime() << " seconds with "
            << blockMatchingFilter->GetNumberOfThreads() << " threads" << std::endl;


  // assembly and solver
  typedef FEMScatteredDataPointSetToImageFilter<
    typename BlockMatchingFilterType::DisplacementsType,
    MeshType,
    DeformationFieldType,
    typename BlockMatchingFilterType::SimilaritiesType,
    typename FeatureSelectionFilterType::FeaturePointsType // tensors are optional pixel values of feature points pointset
  >  FilterType;

  typename FilterType::Pointer filter = FilterType::New();

  filter->SetInput( blockMatchingFilter->GetDisplacements() ); //  filter->SetInput( blockMatchingFilter->GetDisplacements() );
  filter->SetConfidencePointSet( blockMatchingFilter->GetSimilarities() ); //  filter->SetConfidencePointSet( blockMatchingFilter->GetSimilarities() );
  filter->SetTensorPointSet( featureSelectionFilter->GetOutput() ); //  filter->SetTensorPointSet( featureSelectionFilter->GetOutput() );
  filter->SetMesh( const_cast< MeshType * >( this->GetMesh() ) );

  filter->SetSpacing( GetFixedImage()->GetSpacing() );
  filter->SetOrigin( GetFixedImage()->GetOrigin() );
  filter->SetSize( GetFixedImage()->GetLargestPossibleRegion().GetSize() );

  filter->GetFEMSolver()->SetApproximationSteps( m_ApproximationSteps );
  filter->GetFEMSolver()->SetOutlierRejectionSteps( m_OutlierRejectionSteps );

  // graft our output to the filter to force the proper regions to be generated
  filter->GraftOutput( this->GetOutput() );

  itk::TimeProbe astimer;
  astimer.Start();

  filter->Update();

  astimer.Stop();
  std::cout << "Assembly and solver took " << astimer.GetMeanTime() << " seconds" << std::endl;

  // graft the output of the subtract filter back onto this filter's output
  // this is needed to get the appropriate regions passed back
  this->GraftOutput( filter->GetOutput() );
}

}
}  // end namespace itk::fem

#endif
