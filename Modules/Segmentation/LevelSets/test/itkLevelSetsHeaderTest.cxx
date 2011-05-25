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

#include <iostream>

#include "itkGeodesicActiveContourShapePriorLevelSetFunction.txx"
#include "itkShapePriorSegmentationLevelSetFunction.txx"
#include "itkBinaryMaskToNarrowBandPointSetFilter.txx"
#include "itkVectorThresholdSegmentationLevelSetFunction.txx"
#include "itkParallelSparseFieldLevelSetImageFilter.txx"
#include "itkSegmentationLevelSetImageFilter.txx"
#include "itkThresholdSegmentationLevelSetFunction.txx"
#include "itkImplicitManifoldNormalVectorFilter.txx"
#include "itkCurvesLevelSetImageFilter.txx"
#include "itkShapeDetectionLevelSetImageFilter.h"
#include "itkVectorThresholdSegmentationLevelSetImageFilter.txx"
#include "itkShapeDetectionLevelSetFunction.h"
#include "itkSparseFieldFourthOrderLevelSetImageFilter.txx"
#include "itkCurvesLevelSetFunction.txx"
#include "itkLevelSetNode.h"
#include "itkLevelSetFunction.txx"
#include "itkNarrowBandLevelSetImageFilter.txx"
#include "itkIsotropicFourthOrderLevelSetImageFilter.h"
#include "itkAnisotropicFourthOrderLevelSetImageFilter.txx"
#include "itkCannySegmentationLevelSetFunction.h"
#include "itkCannySegmentationLevelSetFunction.txx"
#include "itkCannySegmentationLevelSetImageFilter.txx"
#include "itkCollidingFrontsImageFilter.txx"
#include "itkExtensionVelocitiesImageFilter.txx"
#include "itkGeodesicActiveContourLevelSetFunction.txx"
#include "itkGeodesicActiveContourLevelSetImageFilter.txx"
#include "itkGeodesicActiveContourShapePriorLevelSetImageFilter.txx"
#include "itkImplicitManifoldNormalVectorFilter.h"
#include "itkIsotropicFourthOrderLevelSetImageFilter.txx"
#include "itkLaplacianSegmentationLevelSetFunction.txx"
#include "itkLaplacianSegmentationLevelSetImageFilter.txx"
#include "itkLevelSet.h"
#include "itkLevelSetFunctionWithRefitTerm.txx"
#include "itkLevelSetNeighborhoodExtractor.txx"
#include "itkLevelSetVelocityNeighborhoodExtractor.txx"
#include "itkNarrowBandCurvesLevelSetImageFilter.txx"
#include "itkNarrowBandThresholdSegmentationLevelSetImageFilter.txx"
#include "itkNormalVectorDiffusionFunction.txx"
#include "itkNormalVectorFunctionBase.h"
#include "itkNormalVectorFunctionBase.txx"
#include "itkReinitializeLevelSetImageFilter.txx"
#include "itkSegmentationLevelSetFunction.txx"
#include "itkShapeDetectionLevelSetFunction.txx"
#include "itkShapeDetectionLevelSetImageFilter.txx"
#include "itkShapePriorMAPCostFunction.txx"
#include "itkShapePriorMAPCostFunctionBase.txx"
#include "itkShapePriorSegmentationLevelSetImageFilter.txx"
#include "itkSparseFieldLevelSetImageFilter.h"
#include "itkSparseFieldLevelSetImageFilter.txx"
#include "itkThresholdSegmentationLevelSetImageFilter.txx"
#include "itkUnsharpMaskLevelSetImageFilter.txx"


int itkLevelSetsHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
