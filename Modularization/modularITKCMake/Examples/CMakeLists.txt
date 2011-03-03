cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ITK-Examples)
find_package(itk-base 4.0 EXACT REQUIRED)

itk_module_impl()

set(ITK_LIBRARIES ${ITK-Examples_LIBRARIES})
