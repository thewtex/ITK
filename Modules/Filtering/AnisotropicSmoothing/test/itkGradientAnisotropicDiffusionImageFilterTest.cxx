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

#include <iostream>
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkNullImageToImageFilterDriver.hxx"

/**
 * This program tests the FilterImageAnisotropicDiffusion object by driving it
 * with a null input and output.  Returns 0 on success and 1 on failure.
 */
int
itkGradientAnisotropicDiffusionImageFilterTest(int itkNotUsed(argc), char *itkNotUsed(argv) [] )
{
  try
    {
    typedef itk::Image<float, 2> ImageType;

    // Set up filter
    itk::GradientAnisotropicDiffusionImageFilter<ImageType, ImageType>
    ::Pointer filter
      = itk::GradientAnisotropicDiffusionImageFilter<ImageType, ImageType>
        ::New();
    filter->SetNumberOfIterations(1);
    filter->SetConductanceParameter(3.0f);
    filter->SetTimeStep(0.125f);

    // The following lines are only for increased code coverage in testing.
    filter->GetTimeStep();
    filter->GetNumberOfIterations();
    filter->GetConductanceParameter();
    filter->SetConductanceScalingParameter(filter->GetConductanceScalingParameter() );
    filter->GetFixedAverageGradientMagnitude();

    // Run Test
    itk::Size<2> sz;
    sz[0] = 250;
    sz[1] = 250;
    itk::NullImageToImageFilterDriver< ImageType,ImageType > test1;
    test1.SetImageSize(sz);
    test1.SetFilter(filter.GetPointer() );
    test1.Execute();
    }
  catch(itk::ExceptionObject &err)
    {
    (&err)->Print(std::cerr);
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
