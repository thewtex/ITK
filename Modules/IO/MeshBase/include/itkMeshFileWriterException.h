/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkMeshFileWriterException_h
#define itkMeshFileWriterException_h
#include "ITKIOMeshBaseExport.h"

#include "itkMacro.h"

namespace itk
{
/**
 * \class MeshFileWriterException.
 * \brief Base exception class for IO problems during writing.
 *
 * \ingroup ITKIOMeshBase
 */
class ITKIOMeshBase_EXPORT MeshFileWriterException : public ExceptionObject
{
public:
  ITK_DEFAULT_COPY_AND_MOVE(MeshFileWriterException);

  /** Has to have empty throw(). */
  ~MeshFileWriterException() noexcept override;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(MeshFileWriterException);

  /** Constructor. */
  MeshFileWriterException(std::string  file,
                          unsigned int line,
                          std::string  message = "Error in IO",
                          std::string  location = {});
};
} // end namespace itk

#endif
