project(ITK_Netlib)
set(ITK_Netlib_THIRD_PARTY 1)

set(ITK_Netlib_INCLUDE_DIRS
   ${ITK_Netlib_BINARY_DIR})
set(ITK_Netlib_LIBRARIES itkNetlibSlatec)

itk_module_impl()
