/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

#include "itkDifferenceImageFilter.hxx"
#include "itkPipelineMonitorImageFilter.hxx"
#include "itkRandomImageSource.hxx"
// These need to be commented to prevent double definitions from
// itkTestDriverInclude.h contents, but they still need to be here to pass the
// HeaderTestCheck test.
// #include "itkTestDriverIncludeBuiltInIOFactories.h"
// #include "itkTestDriverInclude.h"
// #include "itkTestDriverIncludeRequiredIOFactories.h"
#include "itkTestingComparisonImageFilter.hxx"
#include "itkTestingExtractSliceImageFilter.hxx"
#include "itkTestingMacros.h"
#include "itkTestingStretchIntensityImageFilter.hxx"

int itkTestKernelHeaderTest ( int, char * [] )
{

  return EXIT_SUCCESS;
}
