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


#include "itkAddConstantToImageFilter.h"
#include "itkAggregateLabelMapFilter.h"
#include "itkAggregateLabelMapFilter.txx"
#include "itkAreaClosingImageFilter.h"
#include "itkAreaOpeningImageFilter.h"
#include "itkAtanRegularizedHeavisideStepFunction.h"
#include "itkAttributeKeepNObjectsLabelMapFilter.h"
#include "itkAttributeKeepNObjectsLabelMapFilter.txx"
#include "itkAttributeLabelObject.h"
#include "itkAttributeMorphologyBaseImageFilter.h"
#include "itkAttributeMorphologyBaseImageFilter.txx"
#include "itkAttributeOpeningLabelMapFilter.h"
#include "itkAttributeOpeningLabelMapFilter.txx"
#include "itkAttributePositionLabelMapFilter.h"
#include "itkAttributePositionLabelMapFilter.txx"
#include "itkAttributeRelabelLabelMapFilter.h"
#include "itkAttributeRelabelLabelMapFilter.txx"
#include "itkAttributeSelectionLabelMapFilter.h"
#include "itkAttributeSelectionLabelMapFilter.txx"
#include "itkAttributeUniqueLabelMapFilter.h"
#include "itkAttributeUniqueLabelMapFilter.txx"
#include "itkAutoCropLabelMapFilter.h"
#include "itkAutoCropLabelMapFilter.txx"
#include "itkAutumnColormapFunction.h"
#include "itkAutumnColormapFunction.txx"
#include "itkBinaryClosingByReconstructionImageFilter.h"
#include "itkBinaryClosingByReconstructionImageFilter.txx"
#include "itkBinaryContourImageFilter.h"
#include "itkBinaryContourImageFilter.txx"
#include "itkBinaryFillholeImageFilter.h"
#include "itkBinaryFillholeImageFilter.txx"
#include "itkBinaryGrindPeakImageFilter.h"
#include "itkBinaryGrindPeakImageFilter.txx"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkBinaryImageToLabelMapFilter.txx"
#include "itkBinaryImageToShapeLabelMapFilter.h"
#include "itkBinaryImageToShapeLabelMapFilter.txx"
#include "itkBinaryImageToStatisticsLabelMapFilter.h"
#include "itkBinaryImageToStatisticsLabelMapFilter.txx"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryMorphologicalClosingImageFilter.txx"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryMorphologicalOpeningImageFilter.txx"
#include "itkBinaryNotImageFilter.h"
#include "itkBinaryOpeningByReconstructionImageFilter.h"
#include "itkBinaryOpeningByReconstructionImageFilter.txx"
#include "itkBinaryReconstructionByDilationImageFilter.h"
#include "itkBinaryReconstructionByDilationImageFilter.txx"
#include "itkBinaryReconstructionByErosionImageFilter.h"
#include "itkBinaryReconstructionByErosionImageFilter.txx"
#include "itkBinaryReconstructionLabelMapFilter.h"
#include "itkBinaryReconstructionLabelMapFilter.txx"
#include "itkBinaryShapeKeepNObjectsImageFilter.h"
#include "itkBinaryShapeKeepNObjectsImageFilter.txx"
#include "itkBinaryShapeOpeningImageFilter.h"
#include "itkBinaryShapeOpeningImageFilter.txx"
#include "itkBinaryStatisticsKeepNObjectsImageFilter.h"
#include "itkBinaryStatisticsKeepNObjectsImageFilter.txx"
#include "itkBinaryStatisticsOpeningImageFilter.h"
#include "itkBinaryStatisticsOpeningImageFilter.txx"
#include "itkBlueColormapFunction.h"
#include "itkBlueColormapFunction.txx"
#include "itkBoxMeanImageFilter.h"
#include "itkBoxMeanImageFilter.txx"
#include "itkBoxSigmaImageFilter.h"
#include "itkBoxSigmaImageFilter.txx"
#include "itkBoxUtilities.h"
#include "itkBruker2DSEQImageIOFactory.h"
#include "itkBruker2DSEQImageIO.h"
#include "itkBSplineControlPointImageFilter.h"
#include "itkBSplineControlPointImageFilter.txx"
#include "itkBSplineControlPointImageFunction.h"
#include "itkBSplineControlPointImageFunction.txx"
#include "itkBSplineDeformableTransformInitializer.h"
#include "itkBSplineDeformableTransformInitializer.txx"
#include "itkBSplineScatteredDataPointSetToImageFilter.h"
#include "itkBSplineScatteredDataPointSetToImageFilter.txx"
#include "itkChangeLabelLabelMapFilter.h"
#include "itkChangeLabelLabelMapFilter.txx"
#include "itkChangeRegionLabelMapFilter.h"
#include "itkChangeRegionLabelMapFilter.txx"
#include "itkColormapFunction.h"
#include "itkComplexBSplineInterpolateImageFunction.h"
#include "itkComplexBSplineInterpolateImageFunction.txx"
#include "itkComposeRGBAImageFilter.h"
#include "itkCompositeTransform.h"
#include "itkCompositeTransform.txx"
#include "itkConformalFlatteningMeshFilter.h"
#include "itkConformalFlatteningMeshFilter.txx"
#include "itkConstrainedRegionBasedLevelSetFunctionSharedData.h"
#include "itkContourExtractor2DImageFilter.h"
#include "itkContourExtractor2DImageFilter.txx"
#include "itkConvolutionImageFilter.h"
#include "itkConvolutionImageFilter.txx"
#include "itkCoolColormapFunction.h"
#include "itkCoolColormapFunction.txx"
#include "itkCopperColormapFunction.h"
#include "itkCopperColormapFunction.txx"
#include "itkCoxDeBoorBSplineKernelFunction.h"
#include "itkCoxDeBoorBSplineKernelFunction.txx"
#include "itkCropLabelMapFilter.h"
#include "itkCropLabelMapFilter.txx"
#include "itkCustomColormapFunction.h"
#include "itkCustomColormapFunction.txx"
#include "itkDeformationFieldTransform.h"
#include "itkDeformationFieldTransform.txx"
#include "itkDiffeomorphicDemonsRegistrationFilter.h"
#include "itkDiffeomorphicDemonsRegistrationFilter.txx"
#include "itkDirectFourierReconstructionImageToImageFilter.h"
#include "itkDirectFourierReconstructionImageToImageFilter.txx"
#include "itkDiscreteGaussianDerivativeImageFilter.h"
#include "itkDiscreteGaussianDerivativeImageFilter.txx"
#include "itkDiscreteGaussianDerivativeImageFunction.h"
#include "itkDiscreteGaussianDerivativeImageFunction.txx"
#include "itkDiscreteGradientMagnitudeGaussianImageFunction.h"
#include "itkDiscreteGradientMagnitudeGaussianImageFunction.txx"
#include "itkDiscreteHessianGaussianImageFunction.h"
#include "itkDiscreteHessianGaussianImageFunction.txx"
#include "itkDivideByConstantImageFilter.h"
#include "itkESMDemonsRegistrationFunction.h"
#include "itkESMDemonsRegistrationFunction.txx"
#include "itkExponentialDeformationFieldImageFilter.h"
#include "itkExponentialDeformationFieldImageFilter.txx"
#include "itkFastApproximateRankImageFilter.h"
#include "itkFastSymmetricForcesDemonsRegistrationFilter.h"
#include "itkFastSymmetricForcesDemonsRegistrationFilter.txx"
#include "itkFFTComplexToComplexImageFilter.h"
#include "itkFFTComplexToComplexImageFilter.txx"
#include "itkFFTShiftImageFilter.h"
#include "itkFFTShiftImageFilter.txx"
#include "itkFFTWComplexToComplexImageFilter.h"
#include "itkFFTWComplexToComplexImageFilter.txx"
#include "itkGaborImageSource.h"
#include "itkGaborImageSource.txx"
#include "itkGaborKernelFunction.h"
#include "itkGaussianDerivativeOperator.h"
#include "itkGaussianDerivativeOperator.txx"
#include "itkGenericUtilities.h"
#include "itkGeometryUtilities.h"
#include "itkGreenColormapFunction.h"
#include "itkGreenColormapFunction.txx"
#include "itkGreyColormapFunction.h"
#include "itkGreyColormapFunction.txx"
#include "itkGridForwardWarpImageFilter.h"
#include "itkGridForwardWarpImageFilter.txx"
#include "itkGridImageSource.h"
#include "itkGridImageSource.txx"
#include "itkHeavisideStepFunctionBase.h"
#include "itkHeavisideStepFunction.h"
#include "itkHessianToObjectnessMeasureImageFilter.h"
#include "itkHessianToObjectnessMeasureImageFilter.txx"
#include "itkHotColormapFunction.h"
#include "itkHotColormapFunction.txx"
#include "itkHSVColormapFunction.h"
#include "itkHSVColormapFunction.txx"
#include "itkImageKernelOperator.h"
#include "itkImageKernelOperator.txx"
#include "itkImageToPathFilter.h"
#include "itkImageToPathFilter.txx"
#include "itkInPlaceLabelMapFilter.h"
#include "itkInPlaceLabelMapFilter.txx"
#include "itkJetColormapFunction.h"
#include "itkJetColormapFunction.txx"
#include "itkJPEG2000ImageIOFactory.h"
#include "itkJPEG2000ImageIO.h"
#include "itkKappaSigmaThresholdImageCalculator.h"
#include "itkKappaSigmaThresholdImageCalculator.txx"
#include "itkKappaSigmaThresholdImageFilter.h"
#include "itkKappaSigmaThresholdImageFilter.txx"
#include "itkLabelContourImageFilter.h"
#include "itkLabelContourImageFilter.txx"
#include "itkLabelGeometryImageFilter.h"
#include "itkLabelGeometryImageFilter.txx"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelImageToLabelMapFilter.txx"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkLabelImageToShapeLabelMapFilter.txx"
#include "itkLabelImageToStatisticsLabelMapFilter.h"
#include "itkLabelImageToStatisticsLabelMapFilter.txx"
#include "itkLabelMapContourOverlayImageFilter.h"
#include "itkLabelMapContourOverlayImageFilter.txx"
#include "itkLabelMapFilter.h"
#include "itkLabelMapFilter.txx"
#include "itkLabelMap.h"
#include "itkLabelMapMaskImageFilter.h"
#include "itkLabelMapMaskImageFilter.txx"
#include "itkLabelMapOverlayImageFilter.h"
#include "itkLabelMapOverlayImageFilter.txx"
#include "itkLabelMapToAttributeImageFilter.h"
#include "itkLabelMapToAttributeImageFilter.txx"
#include "itkLabelMapToBinaryImageFilter.h"
#include "itkLabelMapToBinaryImageFilter.txx"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkLabelMapToLabelImageFilter.txx"
#include "itkLabelMapToRGBImageFilter.h"
#include "itkLabelMapToRGBImageFilter.txx"
#include "itkLabelMap.txx"
#include "itkLabelMapUtilities.h"
#include "itkLabelObjectAccessors.h"
#include "itkLabelObject.h"
#include "itkLabelObjectLineComparator.h"
#include "itkLabelObjectLine.h"
#include "itkLabelObjectLine.txx"
#include "itkLabelObject.txx"
#include "itkLabelOverlayFunctor.h"
#include "itkLabelOverlayImageFilter.h"
#include "itkLabelOverlayImageFilter.txx"
#include "itkLabelSelectionLabelMapFilter.h"
#include "itkLabelShapeKeepNObjectsImageFilter.h"
#include "itkLabelShapeKeepNObjectsImageFilter.txx"
#include "itkLabelShapeOpeningImageFilter.h"
#include "itkLabelShapeOpeningImageFilter.txx"
#include "itkLabelStatisticsKeepNObjectsImageFilter.h"
#include "itkLabelStatisticsKeepNObjectsImageFilter.txx"
#include "itkLabelStatisticsOpeningImageFilter.h"
#include "itkLabelStatisticsOpeningImageFilter.txx"
#include "itkLabelToRGBFunctor.h"
#include "itkLabelToRGBImageFilter.h"
#include "itkLabelToRGBImageFilter.txx"
#include "itkLabelUniqueLabelMapFilter.h"
#include "itkMagnitudeAndPhaseToComplexImageFilter.h"
#include "itkMaskedMovingHistogramImageFilter.h"
#include "itkMaskedMovingHistogramImageFilter.txx"
#include "itkMaskedRankImageFilter.h"
#include "itkMaskedRankImageFilter.txx"
#include "itkMergeLabelMapFilter.h"
#include "itkMergeLabelMapFilter.txx"
#include "itkMINC2ImageIOFactory.h"
#include "itkMINC2ImageIO.h"
#include "itkMiniPipelineSeparableImageFilter.h"
#include "itkMiniPipelineSeparableImageFilter.txx"
#include "itkMorphologicalWatershedFromMarkersImageFilter.h"
#include "itkMorphologicalWatershedFromMarkersImageFilter.txx"
#include "itkMorphologicalWatershedImageFilter.h"
#include "itkMorphologicalWatershedImageFilter.txx"
#include "itkMRCHeaderObject.h"
#include "itkMRCImageIOFactory.h"
#include "itkMRCImageIO.h"
#include "itkMultiphaseDenseFiniteDifferenceImageFilter.h"
#include "itkMultiphaseDenseFiniteDifferenceImageFilter.txx"
#include "itkMultiphaseFiniteDifferenceImageFilter.h"
#include "itkMultiphaseFiniteDifferenceImageFilter.txx"
#include "itkMultiphaseSparseFiniteDifferenceImageFilter.h"
#include "itkMultiphaseSparseFiniteDifferenceImageFilter.txx"
#include "itkMultiplyByConstantImageFilter.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.txx"
#include "itkN4MRIBiasFieldCorrectionImageFilter.h"
#include "itkN4MRIBiasFieldCorrectionImageFilter.txx"
#include "itkNeuralNetworkFileReader.h"
#include "itkNeuralNetworkFileReader.txx"
#include "itkNeuralNetworkFileWriter.h"
#include "itkNeuralNetworkFileWriter.txx"
#include "itkObjectByObjectLabelMapFilter.h"
#include "itkObjectByObjectLabelMapFilter.txx"
#include "itkObjectToObjectMetric.h"
#include "itkObjectToObjectMetric.txx"
#include "itkOptImageToImageMetricsTest2.h"
#include "itkOptImageToImageMetricsTest.h"
#include "itkOverUnderColormapFunction.h"
#include "itkOverUnderColormapFunction.txx"
#include "itkPadLabelMapFilter.h"
#include "itkPadLabelMapFilter.txx"
#include "itkPhilipsPAR.h"
#include "itkPhilipsRECImageIOFactory.h"
#include "itkPhilipsRECImageIO.h"
#include "itkRankHistogram.h"
#include "itkRankImageFilter.h"
#include "itkRankImageFilter.txx"
#include "itkRealAndImaginaryToComplexImageFilter.h"
#include "itkRedColormapFunction.h"
#include "itkRedColormapFunction.txx"
#include "itkRegionalMaximaImageFilter.h"
#include "itkRegionalMaximaImageFilter.txx"
#include "itkRegionalMinimaImageFilter.h"
#include "itkRegionalMinimaImageFilter.txx"
#include "itkRegionBasedLevelSetFunctionData.h"
#include "itkRegionBasedLevelSetFunctionData.txx"
#include "itkRegionBasedLevelSetFunction.h"
#include "itkRegionBasedLevelSetFunctionSharedData.h"
#include "itkRegionBasedLevelSetFunction.txx"
#include "itkRegionFromReferenceLabelMapFilter.h"
#include "itkRegionFromReferenceLabelMapFilter.txx"
#include "itkRegularizedHeavisideStepFunction.h"
#include "itkRelabelLabelMapFilter.h"
#include "itkRobustAutomaticThresholdCalculator.h"
#include "itkRobustAutomaticThresholdCalculator.txx"
#include "itkRobustAutomaticThresholdImageFilter.h"
#include "itkRobustAutomaticThresholdImageFilter.txx"
#include "itkScalarChanAndVeseDenseLevelSetImageFilter.h"
#include "itkScalarChanAndVeseDenseLevelSetImageFilter.txx"
#include "itkScalarChanAndVeseLevelSetFunctionData.h"
#include "itkScalarChanAndVeseLevelSetFunction.h"
#include "itkScalarChanAndVeseLevelSetFunction.txx"
#include "itkScalarChanAndVeseSparseLevelSetImageFilter.h"
#include "itkScalarChanAndVeseSparseLevelSetImageFilter.txx"
#include "itkScalarRegionBasedLevelSetFunction.h"
#include "itkScalarRegionBasedLevelSetFunction.txx"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.txx"
#include "itkShapedFloodFilledFunctionConditionalConstIterator.h"
#include "itkShapedFloodFilledFunctionConditionalConstIterator.txx"
#include "itkShapedFloodFilledImageFunctionConditionalConstIterator.h"
#include "itkShapedFloodFilledImageFunctionConditionalConstIterator.txx"
#include "itkShapedFloodFilledImageFunctionConditionalIterator.h"
#include "itkShapeKeepNObjectsLabelMapFilter.h"
#include "itkShapeKeepNObjectsLabelMapFilter.txx"
#include "itkShapeLabelMapFilter.h"
#include "itkShapeLabelMapFilter.txx"
#include "itkShapeLabelObjectAccessors.h"
#include "itkShapeLabelObject.h"
#include "itkShapeOpeningLabelMapFilter.h"
#include "itkShapeOpeningLabelMapFilter.txx"
#include "itkShapePositionLabelMapFilter.h"
#include "itkShapePositionLabelMapFilter.txx"
#include "itkShapeRelabelImageFilter.h"
#include "itkShapeRelabelImageFilter.txx"
#include "itkShapeRelabelLabelMapFilter.h"
#include "itkShapeRelabelLabelMapFilter.txx"
#include "itkShapeUniqueLabelMapFilter.h"
#include "itkShapeUniqueLabelMapFilter.txx"
#include "itkShiftScaleLabelMapFilter.h"
#include "itkShiftScaleLabelMapFilter.txx"
#include "itkSinRegularizedHeavisideStepFunction.h"
#include "itkSliceBySliceImageFilter.h"
#include "itkSliceBySliceImageFilter.txx"
#include "itkSpringColormapFunction.h"
#include "itkSpringColormapFunction.txx"
#include "itkStatisticsKeepNObjectsLabelMapFilter.h"
#include "itkStatisticsKeepNObjectsLabelMapFilter.txx"
#include "itkStatisticsLabelMapFilter.h"
#include "itkStatisticsLabelMapFilter.txx"
#include "itkStatisticsLabelObjectAccessors.h"
#include "itkStatisticsLabelObject.h"
#include "itkStatisticsOpeningLabelMapFilter.h"
#include "itkStatisticsOpeningLabelMapFilter.txx"
#include "itkStatisticsPositionLabelMapFilter.h"
#include "itkStatisticsPositionLabelMapFilter.txx"
#include "itkStatisticsRelabelImageFilter.h"
#include "itkStatisticsRelabelImageFilter.txx"
#include "itkStatisticsRelabelLabelMapFilter.h"
#include "itkStatisticsRelabelLabelMapFilter.txx"
#include "itkStatisticsUniqueLabelMapFilter.h"
#include "itkStatisticsUniqueLabelMapFilter.txx"
#include "itkStochasticFractalDimensionImageFilter.h"
#include "itkStochasticFractalDimensionImageFilter.txx"
#include "itkSubtractConstantFromImageFilter.h"
#include "itkSummerColormapFunction.h"
#include "itkSummerColormapFunction.txx"
#include "itkTransformToDeformationFieldSource.h"
#include "itkTransformToDeformationFieldSource.txx"
#include "itkUnconstrainedRegionBasedLevelSetFunctionSharedData.h"
#include "itkValuedRegionalExtremaImageFilter.h"
#include "itkValuedRegionalExtremaImageFilter.txx"
#include "itkValuedRegionalMaximaImageFilter.h"
#include "itkValuedRegionalMinimaImageFilter.h"
#include "itkVectorCentralDifferenceImageFunction.h"
#include "itkVectorCentralDifferenceImageFunction.txx"
#include "itkVectorLinearInterpolateNearestNeighborExtrapolateImageFunction.h"
#include "itkVectorLinearInterpolateNearestNeighborExtrapolateImageFunction.txx"
#include "itkVoxBoCUBImageIOFactory.h"
#include "itkVoxBoCUBImageIO.h"
#include "itkWarpHarmonicEnergyCalculator.h"
#include "itkWarpHarmonicEnergyCalculator.txx"
#include "itkWinterColormapFunction.h"
#include "itkWinterColormapFunction.txx"

int itkReviewHeaderTest ( int , char ** )
{

  return EXIT_SUCCESS;
}
