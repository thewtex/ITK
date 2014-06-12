#
# ITK WikiExamples
#  This remote module require a VTK build
#  The following CMake variable must be set for the ITK build
#
# Check for requires cmake variables

itk_fetch_module(WikiExamples
  "A collection of examples that illustrate how to use ITK."
  GIT_REPOSITORY https://github.com/InsightSoftwareConsortium/ITKWikiExamples.git
# June 12, 2014
  GIT_TAG 8d290e5c438e0298e2fc7b9d70124f010e5e76b9
  )
