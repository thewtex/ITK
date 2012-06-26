set(DOCUMENTATION "This is a collection of classes that are intended to be
removed from the toolkit.")

if(NOT ITK_LEGACY_REMOVE AND ITKV3_COMPATIBILITY)
  # If ITK_LEGACY_REMOVE:BOOL=ON AND ITKV3_COMPATIBILITY:BOOL=OFF
  # then exclude the deprecated code from being built.
  set(EXCLUDE_DEPRECATED_LEGACY_CODE "EXCLUDE_FROM_ALL")
else()
  set(EXCLUDE_DEPRECATED_LEGACY_CODE "")
endif()
itk_module(ITKDeprecated
  DEPENDS
    ITKAnisotropicSmoothing
    ITKAntiAlias
    ITKBiasCorrection
    ITKBioCell
    ITKClassifiers
    ITKCommon
    ITKConnectedComponents
    ITKCurvatureFlow
    ITKDeformableMesh
    ITKDisplacementField
    ITKDiffusionTensorImage
    ITKDistanceMap
    ITKDICOMParser
    ITKEigen
    ITKFEM
    ITKFEMRegistration
    ITKFFT
    ITKFiniteDifference
    ITKImageAdaptors
    ITKImageCompare
    ITKImageCompose
    ITKImageFeature
    ITKImageFilterBase
    ITKImageFunction
    ITKImageGradient
    ITKImageGrid
    ITKImageIntensity
    ITKImageLabel
    ITKImageStatistics
    ITKIOImageBase
    ITKIOBioRad
    ITKIOBMP
    ITKIOGDCM
    ITKIOGE
    ITKIOGIPL
    ITKIOIPL
    ITKIOJPEG
    ITKIOLSM
    ITKIOMeta
    ITKIONIFTI
    ITKIONRRD
    ITKIOPNG
    ITKIORAW
    ITKIOSiemens
    ITKIOSpatialObjects
    ITKIOStimulate
    ITKIOTIFF
    ITKIOVTK
    ITKIOXML
    ITKKLMRegionGrowing
    ITKLabelVoting
    ITKLevelSets
    ITKMarkovRandomFieldsClassifiers
    ITKMathematicalMorphology
    ITKMesh
    ITKNarrowBand
    ITKNeuralNetworks
    ITKOptimizers
    ITKPath
    ITKPDEDeformableRegistration
    ITKPolynomials
    ITKQuadEdgeMesh
    ITKQuadEdgeMeshFiltering
    ITKRegionGrowing
    ITKRegistrationCommon
    ITKSignedDistanceFunction
    ITKSmoothing
    ITKSpatialFunction
    ITKSpatialObjects
    ITKStatistics
    ITKTestKernel
    ITKThresholding
    ITKTransform
    ITKVoronoi
    ITKVTK
    ITKWatersheds
  TEST_DEPENDS
    ITKTestKernel
    ${EXCLUDE_DEPRECATED_LEGACY_CODE}
  DESCRIPTION
    "${DOCUMENTATION}"
)
