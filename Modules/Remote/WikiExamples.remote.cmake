#
# ITK WikiExamples
#  This remote module require a VTK build
#  The following CMake variable must be set for the ITK build
#
# Check for requires cmake variables

itk_fetch_module(WikiExamples
  "A collection of examples that illustrate how to use ITK."
  GIT_REPOSITORY https://github.com/InsightSoftwareConsortium/ITKWikiExamples.git
# June 13, 2014
  GIT_TAG 7fad9fc1f96a7866f4b826595a5c5d8ef7a0e27c
  )
