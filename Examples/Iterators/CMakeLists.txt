#
#  Examples on the use of Iterators.
#
project(IteratorExamples)
include_regular_expression("^.*$")

foreach(example
    NeighborhoodIterators1
    NeighborhoodIterators2
    NeighborhoodIterators3
    NeighborhoodIterators4
    NeighborhoodIterators5
    NeighborhoodIterators6
    ImageRandomConstIteratorWithIndex
    ImageRegionIterator
    ImageRegionIteratorWithIndex
    ImageLinearIteratorWithIndex
    ImageLinearIteratorWithIndex2
    ImageSliceIteratorWithIndex
    ShapedNeighborhoodIterators1
    ShapedNeighborhoodIterators2
    )
  add_example(${example})
endforeach()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
