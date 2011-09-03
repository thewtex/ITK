if (ITKV3_COMPATIBILITY)
set(DOCUMENTATION "Classes that provide ITKv3 compatibility. The CMake
variable ITKV3_COMPATIBILITY must be ON to use these classes.")

itk_module(ITKV3Compatibility
  DEPENDS
    ITKDisplacementField
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
endif()
