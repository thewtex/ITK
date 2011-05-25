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

#include "itkAffineGeometryFrame.h"
#include "itkAffineGeometryFrame.txx"
#include "itkArrowSpatialObject.txx"
#include "itkBlobSpatialObject.h"
#include "itkBlobSpatialObject.txx"
#include "itkBoxSpatialObject.txx"
#include "itkContourSpatialObject.txx"
#include "itkContourSpatialObjectPoint.txx"
#include "itkCylinderSpatialObject.h"
#include "itkDTITubeSpatialObject.txx"
#include "itkDTITubeSpatialObjectPoint.txx"
#include "itkEllipseSpatialObject.h"
#include "itkEllipseSpatialObject.txx"
#include "itkGaussianSpatialObject.txx"
#include "itkGroupSpatialObject.txx"
#include "itkImageMaskSpatialObject.txx"
#include "itkImageSpatialObject.txx"
#include "itkLandmarkSpatialObject.txx"
#include "itkLineSpatialObject.txx"
#include "itkLineSpatialObjectPoint.txx"
#include "itkMeshSpatialObject.txx"
#include "itkMetaArrowConverter.txx"
#include "itkMetaBlobConverter.txx"
#include "itkMetaContourConverter.h"
#include "itkMetaContourConverter.txx"
#include "itkMetaDTITubeConverter.txx"
#include "itkMetaEllipseConverter.h"
#include "itkMetaEllipseConverter.txx"
#include "itkMetaEvent.h"
#include "itkMetaGaussianConverter.txx"
#include "itkMetaGroupConverter.txx"
#include "itkMetaImageConverter.txx"
#include "itkMetaLandmarkConverter.txx"
#include "itkMetaLineConverter.txx"
#include "itkMetaMeshConverter.txx"
#include "itkMetaSceneConverter.txx"
#include "itkMetaSurfaceConverter.txx"
#include "itkMetaTubeConverter.txx"
#include "itkMetaVesselTubeConverter.txx"
#include "itkPlaneSpatialObject.txx"
#include "itkPointBasedSpatialObject.txx"
#include "itkPolygonGroupSpatialObject.txx"
#include "itkPolygonSpatialObject.h"
#include "itkPolygonSpatialObject.txx"
#include "itkSceneSpatialObject.h"
#include "itkSceneSpatialObject.txx"
#include "itkSpatialObject.txx"
#include "itkSpatialObjectDuplicator.h"
#include "itkSpatialObjectDuplicator.txx"
#include "itkSpatialObjectFactory.h"
#include "itkSpatialObjectFactoryBase.h"
#include "itkSpatialObjectPoint.h"
#include "itkSpatialObjectPoint.txx"
#include "itkSpatialObjectProperty.h"
#include "itkSpatialObjectProperty.txx"
#include "itkSpatialObjectToImageFilter.txx"
#include "itkSpatialObjectToImageStatisticsCalculator.h"
#include "itkSpatialObjectToImageStatisticsCalculator.txx"
#include "itkSpatialObjectToPointSetFilter.h"
#include "itkSpatialObjectToPointSetFilter.txx"
#include "itkSpatialObjectTreeContainer.h"
#include "itkSpatialObjectTreeContainer.txx"
#include "itkSpatialObjectTreeNode.txx"
#include "itkSurfaceSpatialObject.txx"
#include "itkSurfaceSpatialObjectPoint.txx"
#include "itkTubeSpatialObject.txx"
#include "itkTubeSpatialObjectPoint.txx"
#include "itkVesselTubeSpatialObject.txx"
#include "itkVesselTubeSpatialObjectPoint.txx"


int itkSpatialObjectsHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
