set(ITK_IO_NIFTI_SRC
itkNiftiImageIOFactory.cxx
itkNiftiImageIO.cxx
)

add_library(ITK_IO_NIFTI ${ITK_IO_NIFTI_SRC})
target_link_libraries(ITK_IO_NIFTI  ${ITK_NIFTI_LIBRARIES} ${ITK_IO_Base_LIBRARIES} ${ITK_Transform_LIBRARIES})
itk_module_target(ITK_IO_NIFTI)
