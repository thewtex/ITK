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

#include "itkSpatialObjectFactory.h"
#include "itkMetaMeshConverter.hpp"
#include "itkMetaImageConverter.hpp"
#include "itkMetaBlobConverter.hpp"
#include "itkMetaVesselTubeConverter.hpp"
#include "itkSurfaceSpatialObjectPoint.hpp"
#include "itkDTITubeSpatialObject.hpp"
#include "itkPolygonGroupSpatialObject.hpp"
#include "itkCylinderSpatialObject.h"
#include "itkPlaneSpatialObject.hpp"
#include "itkLineSpatialObject.hpp"
#include "itkMetaGaussianConverter.hpp"
#include "itkPointBasedSpatialObject.hpp"
#include "itkSpatialObjectToPointSetFilter.h"
#include "itkSpatialObjectToImageFilter.hpp"
#include "itkMetaGroupConverter.hpp"
#include "itkSpatialObjectToImageStatisticsCalculator.h"
#include "itkBlobSpatialObject.h"
#include "itkMetaLandmarkConverter.hpp"
#include "itkMetaEvent.h"
#include "itkEllipseSpatialObject.h"
#include "itkAffineGeometryFrame.h"
#include "itkSpatialObjectTreeContainer.h"
#include "itkMetaDTITubeConverter.hpp"
#include "itkMetaSurfaceConverter.hpp"
#include "itkSceneSpatialObject.hpp"
#include "itkMeshSpatialObject.hpp"
#include "itkSpatialObjectToImageStatisticsCalculator.hpp"
#include "itkTubeSpatialObjectPoint.hpp"
#include "itkSpatialObjectProperty.hpp"
#include "itkSpatialObject.hpp"
#include "itkSpatialObjectTreeNode.hpp"
#include "itkMetaEllipseConverter.h"
#include "itkMetaContourConverter.h"
#include "itkLandmarkSpatialObject.hpp"
#include "itkSpatialObjectPoint.h"
#include "itkSceneSpatialObject.h"
#include "itkLineSpatialObjectPoint.hpp"
#include "itkContourSpatialObject.hpp"
#include "itkVesselTubeSpatialObject.hpp"
#include "itkPolygonSpatialObject.h"
#include "itkSurfaceSpatialObject.hpp"
#include "itkImageSpatialObject.hpp"
#include "itkSpatialObjectTreeContainer.hpp"
#include "itkMetaSceneConverter.hpp"
#include "itkEllipseSpatialObject.hpp"
#include "itkDTITubeSpatialObjectPoint.hpp"
#include "itkSpatialObjectDuplicator.h"
#include "itkBoxSpatialObject.hpp"
#include "itkPolygonSpatialObject.hpp"
#include "itkSpatialObjectFactoryBase.h"
#include "itkContourSpatialObjectPoint.hpp"
#include "itkGaussianSpatialObject.hpp"
#include "itkSpatialObjectToPointSetFilter.hpp"
#include "itkImageMaskSpatialObject.hpp"
#include "itkSpatialObjectDuplicator.hpp"
#include "itkSpatialObjectProperty.h"
#include "itkGroupSpatialObject.hpp"
#include "itkArrowSpatialObject.hpp"
#include "itkMetaContourConverter.hpp"
#include "itkMetaTubeConverter.hpp"
#include "itkBlobSpatialObject.hpp"
#include "itkTubeSpatialObject.hpp"
#include "itkMetaEllipseConverter.hpp"
#include "itkMetaArrowConverter.hpp"
#include "itkSpatialObjectPoint.hpp"
#include "itkMetaLineConverter.hpp"
#include "itkAffineGeometryFrame.hpp"
#include "itkVesselTubeSpatialObjectPoint.hpp"



int itkSpatialObjectsHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
