cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ITK-KWsys)
find_package(itk-base 4.0 EXACT REQUIRED)

set(KWSYS_HEADER_ROOT ${ITK-KWsys_BINARY_DIR}/src)
set(ITK-KWsys_INCLUDE_DIRS
  ${KWSYS_HEADER_ROOT}
  )
set(ITK-KWsys_LIBRARIES ITK-KWsys)
itk_module_impl()
