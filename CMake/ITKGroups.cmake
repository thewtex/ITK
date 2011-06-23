# Set a list of group names
set(group_list Core IO Filtering Registration Segmentation Numerics Utilities Bridge Nonunit)

#------------------------------------------------
# Set a module name list for each group
set(Core_module_list
ITK_Common
ITK_FiniteDifference
ITK_ImageAdaptors
ITK_ImageFunction
ITK_ImageGrid
ITK_ImageStatistics
ITK_Mesh
ITK_QuadEdgeMesh
ITK_SpatialObjects
ITK_TestKernel
ITK_Transform
)


set(IO_module_list
ITK_IO_Base
ITK_IO_BioRad
ITK_IO_BMP
ITK_IO_GDCM
ITK_IO_GE
ITK_IO_GIPL
ITK_IO_IPL
ITK_IO_JPEG
ITK_IO_LSM
ITK_IO_Meta
ITK_IO_NIFTI
ITK_IO_NRRD
ITK_IO_PNG
ITK_IO_RAW
ITK_IO_Siemens
ITK_IO_SpatialObjects
ITK_IO_Stimulate
ITK_IO_TIFF
ITK_IO_VTK
ITK_IO_XML
)

set(Filtering_module_list
ITK_AnisotropicSmoothing
ITK_AntiAlias
ITK_BiasCorrection
ITK_ConnectedComponents
ITK_CurvatureFlow
ITK_DeformationField
ITK_DiffusionTensorImage
ITK_DistanceMap
ITK_FFT
ITK_FastMarching
ITK_ImageCompare
ITK_ImageCompose
ITK_ImageFeature
ITK_ImageFilterBase
ITK_ImageGradient
ITK_ImageGrid
ITK_ImageIntensity
ITK_ImageLabel
ITK_ImageStatistics
ITK_LabelVoting
ITK_MathematicalMorphology
ITK_Path
ITK_QuadEdgeMeshFiltering
ITK_Smoothing
ITK_SpatialFunction
ITK_Thresholding
)

set(Registration_module_list
ITK_FEMRegistration
ITK_PDEDeformableRegistration
ITK_RegistrationCommon
)

set(Segmentation_module_list
ITK_BioCell
ITK_Blox
ITK_Classifiers
ITK_ConnectedComponents
ITK_DeformableMesh
ITK_KLMRegionGrowing
ITK_LabelVoting
ITK_LevelSets
ITK_MarkovRandomFieldsClassifiers
ITK_RegionGrowing
ITK_SignedDistanceFunction
ITK_Voronoi
ITK_Watersheds
)

set(Numerics_module_list
ITK_Eigen
ITK_FEM
ITK_NarrowBand
ITK_NeuralNetworks
ITK_Optimizers
ITK_Polynomials
ITK_Statistics
)


set(Bridge_module_list
ITK_VTK)

set(Utilities_module_list
ITK_KWSys
ITK_VNL
ITK_PNG
ITK_JPEG
ITK_Expat
ITK_NrrdIO
ITK_NIFTI
ITK_MetaIO
ITK_GDCM
ITK_OpenJPEG
ITK_ZLIB
ITK_VNLInstantiation
ITK_TIFF
)

set(Nonunit_module_list
ITK_IntegratedTest
ITK_Review
)
#------------------------------------------------
# Turn on the ITK_BUILD option for each group
if("$ENV{DASHBOARD_TEST_FROM_CTEST}" STREQUAL "")
  # developer build
  option(ITKGroup_Core "Request building core modules" ON)
endif()
foreach( group ${group_list})
    option(ITKGroup_${group} "Request building ${group} modules" OFF)
    if (ITKGroup_${group})
      foreach (itk-module ${${group}_module_list} )
         list(APPEND ITK_MODULE_${itk-module}_REQUEST_BY ITKGroup_${group})
      endforeach()
    endif()
    # Hide group options if building all modules anyway.
    if(ITK_BUILD_ALL_MODULES)
      set_property(CACHE ITKGroup_${group} PROPERTY TYPE INTERNAL)
    else()
      set_property(CACHE ITKGroup_${group} PROPERTY TYPE BOOL)
    endif()
endforeach()
