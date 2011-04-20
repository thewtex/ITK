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

#include "itkGeodesicActiveContourShapePriorLevelSetFunction.hpp"
#include "itkShapePriorSegmentationLevelSetFunction.hpp"
#include "itkBinaryMaskToNarrowBandPointSetFilter.hpp"
#include "itkVectorThresholdSegmentationLevelSetFunction.hpp"
#include "itkParallelSparseFieldLevelSetImageFilter.hpp"
#include "itkSegmentationLevelSetImageFilter.hpp"
#include "itkThresholdSegmentationLevelSetFunction.hpp"
#include "itkImplicitManifoldNormalVectorFilter.hpp"
#include "itkCurvesLevelSetImageFilter.hpp"
#include "itkShapeDetectionLevelSetImageFilter.h"
#include "itkVectorThresholdSegmentationLevelSetImageFilter.hpp"
#include "itkShapeDetectionLevelSetFunction.h"
#include "itkSparseFieldFourthOrderLevelSetImageFilter.hpp"
#include "itkCurvesLevelSetFunction.hpp"
#include "itkLevelSetNode.h"
#include "itkLevelSetFunction.hpp"
#include "itkNarrowBandLevelSetImageFilter.hpp"
#include "itkIsotropicFourthOrderLevelSetImageFilter.h"
#include "itkLaplacianSegmentationLevelSetImageFilter.hpp"
#include "itkShapePriorMAPCostFunctionBase.hpp"
#include "itkGeodesicActiveContourLevelSetImageFilter.hpp"
#include "itkExtensionVelocitiesImageFilter.hpp"
#include "itkGeodesicActiveContourLevelSetFunction.hpp"
#include "itkLevelSetNeighborhoodExtractor.hpp"
#include "itkNormalVectorFunctionBase.h"
#include "itkCannySegmentationLevelSetFunction.h"
#include "itkCollidingFrontsImageFilter.hpp"
#include "itkShapeDetectionLevelSetImageFilter.hpp"
#include "itkLaplacianSegmentationLevelSetFunction.hpp"
#include "itkAnisotropicFourthOrderLevelSetImageFilter.hpp"
#include "itkThresholdSegmentationLevelSetImageFilter.hpp"
#include "itkNormalVectorFunctionBase.hpp"
#include "itkLevelSet.h"
#include "itkSparseFieldLevelSetImageFilter.h"
#include "itkNormalVectorDiffusionFunction.hpp"
#include "itkGeodesicActiveContourShapePriorLevelSetImageFilter.hpp"
#include "itkSegmentationLevelSetFunction.hpp"
#include "itkShapePriorMAPCostFunction.hpp"
#include "itkCannySegmentationLevelSetFunction.hpp"
#include "itkLevelSetFunctionWithRefitTerm.hpp"
#include "itkReinitializeLevelSetImageFilter.hpp"
#include "itkIsotropicFourthOrderLevelSetImageFilter.hpp"
#include "itkNarrowBandThresholdSegmentationLevelSetImageFilter.hpp"
#include "itkShapePriorSegmentationLevelSetImageFilter.hpp"
#include "itkLevelSetVelocityNeighborhoodExtractor.hpp"
#include "itkSparseFieldLevelSetImageFilter.hpp"
#include "itkNarrowBandCurvesLevelSetImageFilter.hpp"
#include "itkShapeDetectionLevelSetFunction.hpp"
#include "itkCannySegmentationLevelSetImageFilter.hpp"
#include "itkImplicitManifoldNormalVectorFilter.h"
#include "itkUnsharpMaskLevelSetImageFilter.hpp"



int itkLevelSetsHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
