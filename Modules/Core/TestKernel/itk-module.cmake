set(DOCUMENTATION "This Module contains helper classes used for the Testing
infrastructure of ITK. As an application developer you should not normally need
any of these classes. However, they are essential for the quality control
system supporting ITK.")

itk_module(ITK_TestKernel
  DEPENDS
    ITK_IO_PNG
    ITK_IO_Meta
    ITK_IO_TIFF
    ITK_IO_BMP
    ITK_IO_VTK
    ITK_IO_JPEG
    ITK_IO_NRRD
    ITK_IO_GDCM
    ITK_IO_GIPL
    ITK_IO_NIFTI
    ITK_KWSys
  DESCRIPTION
    "${DOCUMENTATION}"
)
