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
#include "itkVideoDummyCameraFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkVideoDummyCamera.h"
#include "itkVersion.h"

namespace itk
{
VideoDummyCameraFactory::VideoDummyCameraFactory()
{
  this->RegisterOverride( "itkVideoIOBase",
                          "itkVideoDummyCamera",
                          "Video Dummy Camera IO",
                          1,
                          CreateObjectFunction< VideoDummyCamera >::New() );
}

VideoDummyCameraFactory::~VideoDummyCameraFactory()
{}

const char *
VideoDummyCameraFactory::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

const char *
VideoDummyCameraFactory::GetDescription() const
{
  return "Video Dummy Camera IO Factory (camera://dummy/...) acts as a camera \
          grabber but synthetically generates all the images it returns.";
}

// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.

static bool VideoDummyCameraFactoryHasBeenRegistered;

void VideoDummyCameraFactoryRegister__Private(void)
{
  if( ! VideoDummyCameraFactoryHasBeenRegistered )
    {
    VideoDummyCameraFactoryHasBeenRegistered = true;
    VideoDummyCameraFactory::RegisterOneFactory();
    }
}

} // end namespace itk
