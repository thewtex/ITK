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
// This file has been generated by BuildHeaderTest.tcl
// Test to include each header file for Insight

#include "itkAbsImageAdaptor.h"
#include "itkAcosImageAdaptor.h"
#include "itkAddImageAdaptor.h"
#include "itkAffineTransform.txx"
#include "itkAnnulusOperator.txx"
#include "itkArray.txx"
#include "itkArray2D.txx"
#include "itkAsinImageAdaptor.h"
#include "itkAtanImageAdaptor.h"
#include "itkAutoPointer.h"
#include "itkAutoPointerDataObjectDecorator.txx"
#include "itkAzimuthElevationToCartesianTransform.txx"
#include "itkBSplineDeformableTransform.txx"
#include "itkBSplineDerivativeKernelFunction.h"
#include "itkBSplineInterpolationWeightFunction.txx"
#include "itkBackwardDifferenceOperator.txx"
#include "itkBarrier.h"
#include "itkBinaryBallStructuringElement.txx"
#include "itkBinaryCrossStructuringElement.txx"
#include "itkBinaryThresholdImageFunction.txx"
#include "itkBinaryThresholdSpatialFunction.txx"
#include "itkBloxBoundaryPointImage.txx"
#include "itkBloxBoundaryPointItem.txx"
#include "itkBloxBoundaryPointPixel.txx"
#include "itkBloxBoundaryProfileImage.txx"
#include "itkBloxBoundaryProfileItem.txx"
#include "itkBloxBoundaryProfilePixel.txx"
#include "itkBloxCoreAtomImage.txx"
#include "itkBloxCoreAtomItem.txx"
#include "itkBloxCoreAtomPixel.txx"
#include "itkBloxImage.txx"
#include "itkBloxItem.h"
#include "itkBloxPixel.txx"
#include "itkBluePixelAccessor.h"
#include "itkBoundingBox.txx"
#include "itkByteSwapper.txx"
#include "itkCellInterface.txx"
#include "itkCellInterfaceVisitor.h"
#include "itkCenteredAffineTransform.txx"
#include "itkCenteredEuler3DTransform.txx"
#include "itkCenteredRigid2DTransform.txx"
#include "itkCenteredSimilarity2DTransform.txx"
#include "itkCenteredTransformInitializer.txx"
#include "itkCenteredVersorTransformInitializer.txx"
#include "itkCentralDifferenceImageFunction.txx"
#include "itkChainCodePath.txx"
#include "itkChainCodePath2D.h"
#include "itkChildTreeIterator.h"
#include "itkColorTable.txx"
#include "itkComplexToImaginaryImageAdaptor.h"
#include "itkComplexToModulusImageAdaptor.h"
#include "itkComplexToPhaseImageAdaptor.h"
#include "itkComplexToRealImageAdaptor.h"
#include "itkConditionalConstIterator.txx"
#include "itkConicShellInteriorExteriorSpatialFunction.txx"
#include "itkConstNeighborhoodIterator.txx"
#include "itkConstShapedNeighborhoodIterator.txx"
#include "itkConstSliceIterator.h"
#include "itkConstantBoundaryCondition.h"
#include "itkContinuousIndex.h"
#include "itkCoreAtomImageToDistanceMatrixProcess.txx"
#include "itkCorrespondenceDataStructure.txx"
#include "itkCorrespondenceDataStructureIterator.txx"
#include "itkCorrespondingList.txx"
#include "itkCorrespondingMedialNodeClique.txx"
#include "itkCosImageAdaptor.h"
#include "itkCovarianceImageFunction.txx"
#include "itkCovariantVector.txx"
#include "itkDataObjectDecorator.txx"
#include "itkDefaultDynamicMeshTraits.h"
#include "itkDefaultImageTraits.h"
#include "itkDefaultPixelAccessor.h"
#include "itkDefaultPixelAccessorFunctor.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkDefaultVectorPixelAccessor.h"
#include "itkDefaultVectorPixelAccessorFunctor.h"
#include "itkDenseFiniteDifferenceImageFilter.txx"
#include "itkDerivativeOperator.txx"
#include "itkDifferenceImageFilter.txx"
#include "itkDiffusionTensor3D.txx"
#include "itkDirectory.h"
#include "itkDynamicLoader.h"
#include "itkElasticBodyReciprocalSplineKernelTransform.txx"
#include "itkElasticBodySplineKernelTransform.txx"
#include "itkEllipsoidInteriorExteriorSpatialFunction.txx"
#include "itkEquivalencyTable.h"
#include "itkEuler2DTransform.txx"
#include "itkEuler3DTransform.txx"
#include "itkExpImageAdaptor.h"
#include "itkExpNegativeImageAdaptor.h"
#include "itkFastMutexLock.h"
#include "itkFiniteCylinderSpatialFunction.txx"
#include "itkFiniteDifferenceFunction.txx"
#include "itkFiniteDifferenceImageFilter.txx"
#include "itkFiniteDifferenceSparseImageFilter.txx"
#include "itkFiniteDifferenceSparseImageFunction.txx"
#include "itkFixedArray.txx"
#include "itkFixedCenterOfRotationAffineTransform.txx"
#include "itkFloodFilledFunctionConditionalConstIterator.txx"
#include "itkFloodFilledImageFunctionConditionalConstIterator.txx"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkFloodFilledSpatialFunctionConditionalConstIterator.txx"
#include "itkFloodFilledSpatialFunctionConditionalIterator.h"
#include "itkForwardDifferenceOperator.txx"
#include "itkFourierSeriesPath.txx"
#include "itkFrustumSpatialFunction.txx"
#include "itkGaussianBlurImageFunction.txx"
#include "itkGaussianDerivativeImageFunction.txx"
#include "itkGaussianDerivativeSpatialFunction.txx"
#include "itkGaussianKernelFunction.h"
#include "itkGaussianOperator.txx"
#include "itkGaussianSpatialFunction.txx"
#include "itkGreenPixelAccessor.h"
#include "itkHexahedronCell.txx"
#include "itkHexahedronCellTopology.h"
#include "itkIdentityTransform.h"
#include "itkImage.txx"
#include "itkImageAdaptor.txx"
#include "itkImageAndPathToImageFilter.txx"
#include "itkImageBase.txx"
#include "itkImageConstIterator.txx"
#include "itkImageConstIteratorWithIndex.txx"
#include "itkImageContainerInterface.h"
#include "itkImageDuplicator.txx"
#include "itkImageFunction.txx"
#include "itkImageHelper.h"
#include "itkImageIterator.txx"
#include "itkImageIteratorWithIndex.txx"
#include "itkImageLinearConstIteratorWithIndex.txx"
#include "itkImageLinearIteratorWithIndex.txx"
#include "itkImageRandomConstIteratorWithIndex.txx"
#include "itkImageRandomIteratorWithIndex.txx"
#include "itkImageRandomNonRepeatingConstIteratorWithIndex.txx"
#include "itkImageRandomNonRepeatingIteratorWithIndex.txx"
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
#include "itkInOrderTreeIterator.h"
#include "itkInPlaceImageFilter.txx"
#include "itkIndexedContainerInterface.h"
#include "itkInteriorExteriorSpatialFunction.txx"
#include "itkIterationReporter.h"
#include "itkKLMSegmentationRegion.h"
#include "itkKernelTransform.txx"
#include "itkLandmarkBasedTransformInitializer.txx"
#include "itkLaplacianOperator.txx"
#include "itkLeafTreeIterator.h"
#include "itkLevelOrderTreeIterator.h"
#include "itkLevelSet.h"
#include "itkLevelSetFunction.txx"
#include "itkLightProcessObject.h"
#include "itkLineCell.txx"
#include "itkLineConstIterator.txx"
#include "itkLineIterator.txx"
#include "itkLinearInterpolateImageFunction.txx"
#include "itkLog10ImageAdaptor.h"
#include "itkLogImageAdaptor.h"
#include "itkLoggerManager.h"
#include "itkLoggerOutput.h"
#include "itkLoggerThreadWrapper.txx"
#include "itkMahalanobisDistanceThresholdImageFunction.txx"
#include "itkMapContainer.txx"
#include "itkMatrix.txx"
#include "itkMatrixOffsetTransformBase.txx"
#include "itkMatrixResizeableDataObject.txx"
#include "itkMaximumDecisionRule.h"
#include "itkMaximumRatioDecisionRule.h"
#include "itkMeanImageFunction.txx"
#include "itkMedianImageFunction.txx"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkMesh.txx"
#include "itkMeshRegion.h"
#include "itkMeshSource.txx"
#include "itkMeshToMeshFilter.txx"
#include "itkMetaDataObject.txx"
#include "itkMetaDataObjectBase.h"
#include "itkMinimumDecisionRule.h"
#include "itkMultipleLogOutput.h"
#include "itkMutexLockHolder.h"
#include "itkNearestNeighborExtrapolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkNeighborhood.txx"
#include "itkNeighborhoodAccessorFunctor.h"
#include "itkNeighborhoodAlgorithm.txx"
#include "itkNeighborhoodAllocator.h"
#include "itkNeighborhoodBinaryThresholdImageFunction.txx"
#include "itkNeighborhoodInnerProduct.txx"
#include "itkNeighborhoodIterator.txx"
#include "itkNeighborhoodOperator.txx"
#include "itkNeighborhoodOperatorImageFunction.txx"
#include "itkNodeList.txx"
#include "itkNonUniformBSpline.txx"
#include "itkNthElementImageAdaptor.h"
#include "itkNumericTraitsCovariantVectorPixel.h"
#include "itkNumericTraitsRGBPixel.h"
#include "itkNumericTraitsTensorPixel.h"
#include "itkNumericTraitsVariableLengthVectorPixel.h"
#include "itkNumericTraitsVectorPixel.h"
#include "itkObjectStore.txx"
#include "itkOctree.txx"
#include "itkOctreeNode.h"
#include "itkOneWayEquivalencyTable.h"
#include "itkOrthogonallyCorrected2DParametricPath.h"
#include "itkPCAShapeSignedDistanceFunction.txx"
#include "itkParametricPath.txx"
#include "itkPath.txx"
#include "itkPathAndImageToPathFilter.txx"
#include "itkPathConstIterator.txx"
#include "itkPathFunctions.h"
#include "itkPathIterator.txx"
#include "itkPathSource.txx"
#include "itkPathToPathFilter.txx"
#include "itkPeriodicBoundaryCondition.txx"
#include "itkPhasedArray3DSpecialCoordinatesImage.txx"
#include "itkPixelAccessor.h"
#include "itkPixelTraits.h"
#include "itkPoint.txx"
#include "itkPointSet.txx"
#include "itkPolyLineParametricPath.txx"
#include "itkPolygonCell.txx"
#include "itkPostOrderTreeIterator.h"
#include "itkProgressAccumulator.h"
#include "itkProgressReporter.h"
#include "itkQuadraticEdgeCell.txx"
#include "itkQuadraticTriangleCell.txx"
#include "itkQuadraticTriangleCellTopology.h"
#include "itkQuadrilateralCell.txx"
#include "itkQuadrilateralCellTopology.h"
#include "itkQuaternionOrientationAdapter.h"
#include "itkQuaternionRigidTransform.txx"
#include "itkRGBAPixel.txx"
#include "itkRGBPixel.txx"
#include "itkRGBToLuminanceImageAdaptor.h"
#include "itkRGBToVectorImageAdaptor.h"
#include "itkRedPixelAccessor.h"
#include "itkRigid2DTransform.txx"
#include "itkRigid3DPerspectiveTransform.txx"
#include "itkRigid3DTransform.txx"
#include "itkv3Rigid3DTransform.h"
#include "itkRootTreeIterator.h"
#include "itkSTLConstContainerAdaptor.h"
#include "itkSTLContainerAdaptor.h"
#include "itkScalableAffineTransform.txx"
#include "itkScalarToRGBPixelFunctor.txx"
#include "itkScalarVector.h"
#include "itkScaleLogarithmicTransform.txx"
#include "itkScaleSkewVersor3DTransform.txx"
#include "itkScaleTransform.txx"
#include "itkScatterMatrixImageFunction.txx"
#include "itkSecondaryNodeList.txx"
#include "itkSegmentationBorder.h"
#include "itkSegmentationLevelSetFunction.txx"
#include "itkSemaphore.h"
#include "itkShapeSignedDistanceFunction.h"
#include "itkShapedNeighborhoodIterator.txx"
#include "itkSimilarity2DTransform.txx"
#include "itkSimilarity3DTransform.txx"
#include "itkSimpleDataObjectDecorator.txx"
#include "itkSimplexMesh.txx"
#include "itkSimplexMeshGeometry.h"
#include "itkSinImageAdaptor.h"
#include "itkSize.h"
#include "itkSliceIterator.h"
#include "itkSmartPointerForwardReference.txx"
#include "itkSobelOperator.txx"
#include "itkSparseImage.txx"
#include "itkSpatialFunction.txx"
#include "itkSphereSignedDistanceFunction.txx"
#include "itkSpecialCoordinatesImage.txx"
#include "itkSpatialOrientationAdapter.h"
#include "itkSphereSpatialFunction.txx"
#include "itkSqrtImageAdaptor.h"
#include "itkStdStreamLogOutput.h"
#include "itkSumOfSquaresImageFunction.txx"
#include "itkSymmetricEigenAnalysis.txx"
#include "itkSymmetricEllipsoidInteriorExteriorSpatialFunction.txx"
#include "itkSymmetricSecondRankTensor.txx"
#include "itkTanImageAdaptor.h"
#include "itkTetrahedronCell.txx"
#include "itkTetrahedronCellTopology.h"
#include "itkTextOutput.h"
#include "itkThinPlateR2LogRSplineKernelTransform.txx"
#include "itkThinPlateSplineKernelTransform.txx"
#include "itkTimeProbesCollectorBase.h"
#include "itkTimeStamp.h"
#include "itkTorusInteriorExteriorSpatialFunction.txx"
#include "itkTransform.txx"
#include "itkTransformBase.h"
#include "itkTransformFactory.h"
#include "itkTranslationTransform.txx"
#include "itkTreeChangeEvent.h"
#include "itkTreeContainer.txx"
#include "itkTreeContainerBase.h"
#include "itkTreeIteratorBase.txx"
#include "itkTreeIteratorClone.h"
#include "itkTreeNode.txx"
#include "itkTriangleCell.txx"
#include "itkTriangleCellTopology.h"
#include "itkUnaryCorrespondenceMatrix.txx"
#include "itkVariableLengthVector.txx"
#include "itkVariableSizeMatrix.txx"
#include "itkVarianceImageFunction.txx"
#include "itkVector.txx"
#include "itkVectorContainer.txx"
#include "itkVectorImage.txx"
#include "itkVectorImageNeighborhoodAccessorFunctor.h"
#include "itkVectorImageToImageAdaptor.h"
#include "itkVectorLinearInterpolateImageFunction.txx"
#include "itkVectorMeanImageFunction.txx"
#include "itkVectorNearestNeighborInterpolateImageFunction.h"
#include "itkVectorNeighborhoodInnerProduct.txx"
#include "itkVectorToRGBImageAdaptor.h"
#include "itkVersion.h"
#include "itkVersor.txx"
#include "itkVersorRigid3DTransform.txx"
#include "itkVersorTransform.txx"
#include "itkVertexCell.txx"
#include "itkVolumeSplineKernelTransform.txx"
#include "itkWeakPointer.h"
#include "itkWindowedSincInterpolateImageFunction.txx"
#include "itkXMLFileOutputWindow.h"
#include "itkXMLFilterWatcher.h"
#include "itkZeroFluxNeumannBoundaryCondition.txx"
#include "itk_hash_set.h"
#include "vcl_alloc.h"
#include "vnl_complex_traits+char-.h"

int main ( int , char ** )
{

  return EXIT_SUCCESS;
}
