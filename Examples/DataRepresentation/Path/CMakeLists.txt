project(PathExamples)
include_regular_expression("^.*$")

add_example(PolyLineParametricPath1)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
