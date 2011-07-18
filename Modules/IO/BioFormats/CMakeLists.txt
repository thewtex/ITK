project(ITKIOBioFormats)
set(ITKIOBioFormats_LIBRARIES ITKIOBioFormats)
itk_module_impl()

find_package(Java REQUIRED)
file(DOWNLOAD http://www.loci.wisc.edu/files/software/loci_tools.jar
              ${CMAKE_CURRENT_BINARY_DIR}/loci_tools.jar
              EXPECTED_MD5 e036527726a1685318a0c60c6a42d91e
              SHOW_PROGRESS)
