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

#include "itk_alloc.h"
#include "itk_hash_map.h"
#include "itk_hash_set.h"
#include "itkAnnulusOperator.txx"
#include "itkArray.txx"
#include "itkArray2D.txx"
#include "itkAutoPointer.h"
#include "itkAutoPointerDataObjectDecorator.txx"
#include "itkBackwardDifferenceOperator.txx"
#include "itkBarrier.h"
#include "itkBinaryThresholdSpatialFunction.txx"
#include "itkBoundingBox.txx"
#include "itkBresenhamLine.txx"
#include "itkBSplineDerivativeKernelFunction.h"
#include "itkBSplineInterpolationWeightFunction.txx"
#include "itkBSplineKernelFunction.h"
#include "itkByteSwapper.txx"
#include "itkCellInterface.txx"
#include "itkCellInterfaceVisitor.h"
#include "itkChildTreeIterator.txx"
#include "itkColorTable.txx"
#include "itkCommand.h"
#include "itkConceptChecking.h"
#include "itkConditionalConstIterator.txx"
#include "itkConditionVariable.h"
#include "itkConicShellInteriorExteriorSpatialFunction.txx"
#include "itkConstantBoundaryCondition.h"
#include "itkConstNeighborhoodIterator.txx"
#include "itkConstShapedNeighborhoodIterator.txx"
#include "itkConstSliceIterator.h"
#include "itkContinuousIndex.h"
#include "itkCorrespondenceDataStructureIterator.txx"
#include "itkCovariantVector.txx"
#include "itkCreateObjectFunction.h"
#include "itkCrossHelper.h"
#include "itkDataObject.h"
#include "itkDataObjectDecorator.txx"
#include "itkDefaultDynamicMeshTraits.h"
#include "itkDefaultPixelAccessor.h"
#include "itkDefaultPixelAccessorFunctor.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkDefaultVectorPixelAccessor.h"
#include "itkDefaultVectorPixelAccessorFunctor.h"
#include "itkDerivativeOperator.txx"
#include "itkDiffusionTensor3D.txx"
#include "itkDirectory.h"
#include "itkDynamicLoader.h"
#include "itkEllipsoidInteriorExteriorSpatialFunction.txx"
#include "itkEquivalencyTable.h"
#include "itkEventObject.h"
#include "itkExceptionObject.h"
#include "itkExtractImageFilter.txx"
#include "itkExtractImageFilterRegionCopier.h"
#include "itkFactoryTestLib.h"
#include "itkFastMutexLock.h"
#include "itkFileOutputWindow.h"
#include "itkFiniteCylinderSpatialFunction.txx"
#include "itkFixedArray.txx"
#include "itkFloatingPointExceptions.h"
#include "itkFloodFilledFunctionConditionalConstIterator.txx"
#include "itkFloodFilledImageFunctionConditionalConstIterator.txx"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkFloodFilledSpatialFunctionConditionalConstIterator.txx"
#include "itkFloodFilledSpatialFunctionConditionalIterator.h"
#include "itkForwardDifferenceOperator.txx"
#include "itkFrustumSpatialFunction.txx"
#include "itkFunctionBase.h"
#include "itkGaussianDerivativeSpatialFunction.txx"
#include "itkGaussianKernelFunction.h"
#include "itkGaussianOperator.txx"
#include "itkGaussianSpatialFunction.txx"
#include "itkHexahedronCell.txx"
#include "itkHexahedronCellTopology.h"
#include "itkImage.txx"
#include "itkImageBase.txx"
#include "itkImageBoundaryCondition.h"
#include "itkImageConstIterator.txx"
#include "itkImageConstIteratorWithIndex.txx"
#include "itkImageContainerInterface.h"
#include "itkImageDuplicator.txx"
#include "itkImageHelper.h"
#include "itkImageIterator.txx"
#include "itkImageIteratorWithIndex.txx"
#include "itkImageLinearConstIteratorWithIndex.txx"
#include "itkImageLinearIteratorWithIndex.txx"
//BUG 11903
//#include "itkImageRandomConstIteratorWithIndex.txx"
//#include "itkImageRandomIteratorWithIndex.txx"
//#include "itkImageRandomNonRepeatingConstIteratorWithIndex.txx"
//#include "itkImageRandomNonRepeatingIteratorWithIndex.txx"
#include "itkImageRegion.txx"
#include "itkImageRegionConstIterator.txx"
#include "itkImageRegionConstIteratorWithIndex.txx"
#include "itkImageRegionExclusionConstIteratorWithIndex.txx"
#include "itkImageRegionExclusionIteratorWithIndex.txx"
#include "itkImageRegionIterator.txx"
#include "itkImageRegionIteratorWithIndex.txx"
#include "itkImageRegionMultidimensionalSplitter.txx"
#include "itkImageRegionReverseConstIterator.txx"
#include "itkImageRegionReverseIterator.txx"
#include "itkImageRegionSplitter.txx"
#include "itkImageReverseConstIterator.txx"
#include "itkImageReverseIterator.txx"
#include "itkImageSliceConstIteratorWithIndex.txx"
#include "itkImageSliceIteratorWithIndex.txx"
#include "itkImageSource.txx"
#include "itkImageToImageFilter.txx"
#include "itkImageToImageFilterDetail.h"
#include "itkImageTransformHelper.h"
#include "itkImportImageContainer.txx"
#include "itkImportImageFilter.txx"
#include "itkIndent.h"
#include "itkIndex.h"
#include "itkIndexedContainerInterface.h"
#include "itkInOrderTreeIterator.h"
#include "itkInPlaceImageFilter.txx"
#include "itkInteriorExteriorSpatialFunction.txx"
#include "itkIntTypes.h"
#include "itkIterationReporter.h"
#include "itkKernelFunction.h"
#include "itkLaplacianOperator.txx"
#include "itkLeafTreeIterator.h"
#include "itkLevelOrderTreeIterator.txx"
#include "itkLightObject.h"
#include "itkLightProcessObject.h"
#include "itkLineCell.txx"
#include "itkLineConstIterator.txx"
#include "itkLineIterator.txx"
#include "itkLogger.h"
#include "itkLoggerBase.h"
#include "itkLoggerManager.h"
#include "itkLoggerOutput.h"
#include "itkLoggerThreadWrapper.txx"
#include "itkLogOutput.h"
#include "itkMacro.h"
#include "itkMapContainer.txx"
#include "itkMath.h"
#include "itkMathDetail.h"
#include "itkMatrix.txx"
#include "itkMatrixResizeableDataObject.txx"
#include "itkMemoryProbe.h"
#include "itkMemoryProbesCollectorBase.h"
#include "itkMemoryUsageObserver.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.txx"
#include "itkMetaDataObjectBase.h"
#include "itkMinimumMaximumImageCalculator.txx"
#include "itkMultipleLogOutput.h"
#include "itkMultiThreader.h"
#include "itkMutexLock.h"
#include "itkMutexLockHolder.h"
#include "itkNeighborhood.txx"
#include "itkNeighborhoodAccessorFunctor.h"
#include "itkNeighborhoodAlgorithm.txx"
#include "itkNeighborhoodAllocator.h"
#include "itkNeighborhoodInnerProduct.txx"
#include "itkNeighborhoodIterator.txx"
#include "itkNeighborhoodIteratorTestCommon.txx"
#include "itkNeighborhoodOperator.txx"
#include "itkNumericTraits.h"
#include "itkNumericTraitsArrayPixel.h"
#include "itkNumericTraitsCovariantVectorPixel.h"
#include "itkNumericTraitsDiffusionTensor3DPixel.h"
#include "itkNumericTraitsFixedArrayPixel.h"
#include "itkNumericTraitsPointPixel.h"
#include "itkNumericTraitsRGBAPixel.h"
#include "itkNumericTraitsRGBPixel.h"
#include "itkNumericTraitsStdVector.h"
#include "itkNumericTraitsTensorPixel.h"
#include "itkNumericTraitsVariableLengthVectorPixel.h"
#include "itkNumericTraitsVectorPixel.h"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkObjectFactoryBase.h"
#include "itkObjectStore.txx"
#include "itkOctree.txx"
#include "itkOctreeNode.h"
#include "itkOffset.h"
#include "itkOrientationAdapterBase.h"
#include "itkOutputWindow.h"
#include "itkPeriodicBoundaryCondition.txx"
#include "itkPhasedArray3DSpecialCoordinatesImage.txx"
#include "itkPixelTraits.h"
#include "itkPoint.txx"
#include "itkPointSet.txx"
#include "itkPointSetToImageFilter.txx"
#include "itkPolygonCell.txx"
#include "itkPostOrderTreeIterator.h"
#include "itkPreOrderTreeIterator.h"
#include "itkPriorityQueueContainer.txx"
#include "itkProcessObject.h"
#include "itkProgressAccumulator.h"
#include "itkProgressReporter.h"
#include "itkQuadraticEdgeCell.txx"
#include "itkQuadraticTriangleCell.txx"
#include "itkQuadraticTriangleCellTopology.h"
#include "itkQuadrilateralCell.txx"
#include "itkQuadrilateralCellTopology.h"
#include "itkRealTimeClock.h"
#include "itkRealTimeInterval.h"
#include "itkRealTimeStamp.h"
#include "itkRegion.h"
#include "itkResourceProbe.txx"
#include "itkResourceProbesCollectorBase.txx"
#include "itkRGBAPixel.txx"
#include "itkRGBPixel.txx"
#include "itkRootTreeIterator.h"
#include "itkScalarToRGBPixelFunctor.txx"
#include "itkShapedNeighborhoodIterator.txx"
#include "itkSimpleDataObjectDecorator.txx"
#include "itkSimpleFastMutexLock.h"
#include "itkSimpleFilterWatcher.h"
#include "itkSize.h"
#include "itkSliceIterator.h"
#include "itkSmapsFileParser.txx"
#include "itkSmartPointer.h"
#include "itkSmartPointerForwardReference.txx"
#include "itkSobelOperator.txx"
#include "itkSparseFieldLayer.txx"
#include "itkSparseImage.txx"
#include "itkSpatialFunction.txx"
#include "itkSpatialOrientation.h"
#include "itkSpatialOrientationAdapter.h"
#include "itkSpecialCoordinatesImage.txx"
#include "itkSphereSpatialFunction.txx"
#include "itkStdStreamLogOutput.h"
#include "itkSTLConstContainerAdaptor.h"
#include "itkSTLContainerAdaptor.h"
#include "itkStreamingImageFilter.txx"
#include "itkStructHashFunction.h"
#include "itkSymmetricEigenAnalysis.txx"
#include "itkSymmetricEllipsoidInteriorExteriorSpatialFunction.txx"
#include "itkSymmetricSecondRankTensor.txx"
#include "itkTestingMacros.h"
#include "itkTetrahedronCell.txx"
#include "itkTetrahedronCellTopology.h"
#include "itkTextOutput.h"
#include "itkThreadLogger.h"
#include "itkThreadSupport.h"
#include "itkTimeProbe.h"
#include "itkTimeProbesCollectorBase.h"
#include "itkTimeStamp.h"
#include "itkTorusInteriorExteriorSpatialFunction.txx"
#include "itkTreeChangeEvent.h"
#include "itkTreeContainer.txx"
#include "itkTreeContainerBase.h"
#include "itkTreeIteratorBase.txx"
#include "itkTreeIteratorClone.h"
#include "itkTreeNode.txx"
#include "itkTriangleCell.txx"
#include "itkTriangleCellTopology.h"
#include "itkTriangleHelper.txx"
#include "itkUnaryCorrespondenceMatrix.txx"
#include "itkUnaryFunctorImageFilter.txx"
#include "itkValarrayImageContainer.h"
#include "itkVariableLengthVector.txx"
#include "itkVariableSizeMatrix.txx"
#include "itkVector.txx"
#include "itkVectorContainer.txx"
#include "itkVectorImage.txx"
#include "itkVectorImageNeighborhoodAccessorFunctor.h"
#include "itkVectorNeighborhoodInnerProduct.txx"
#include "itkVersion.h"
#include "itkVersor.txx"
#include "itkVertexCell.txx"
#include "itkWeakPointer.h"
#include "itkXMLFileOutputWindow.h"
#include "itkXMLFilterWatcher.h"
#include "itkZeroFluxNeumannBoundaryCondition.txx"
#include "vcl_alloc.h"
#include "VNLIterativeSparseSolverTraits.h"
#include "itkAtanRegularizedHeavisideStepFunction.h"
#include "itkAtanRegularizedHeavisideStepFunction.txx"
#include "itkHeavisideStepFunction.h"
#include "itkHeavisideStepFunction.txx"
#include "itkHeavisideStepFunctionBase.h"
#include "itkRegularizedHeavisideStepFunction.h"
#include "itkRegularizedHeavisideStepFunction.txx"
#include "itkSinRegularizedHeavisideStepFunction.h"
#include "itkSinRegularizedHeavisideStepFunction.txx"

int itkCommonHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
