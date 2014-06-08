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
#ifndef __itkIosStateSave_h
#define __itkIosStateSave_h

#include <iostream>
#include <string>

namespace itk
{
/** \class IosStateSave
 *
 * \brief Save a stream's format state and restore it upon destruction
 *
 * An RAII class to provide an exception safe mechanism to restore the
 * format state of a stream. The class holds a resource (the stream's
 * format state) and resets the resource to a default state upon destruction.
 *
 * Typical usage:
 * #include "itkIosStateSave.h"
 *   itk::IosStateSave coutState(std::cout);
 *   std::cout.precision(20);
 *   std::cout.hex();
 *   ...
 *   return;
 * \ingroup ITKCommon
 */

class IosStateSave
{
public:
  explicit IosStateSave(std::ostream& os) :
    m_Ios(os),
    m_State(NULL)
  {
    m_State.copyfmt(os);
  }
  ~IosStateSave()
  {
    m_Ios.copyfmt(m_State);
  }

private:
  IosStateSave(const IosStateSave &);   //purposely not implemented
  void operator=(const IosStateSave &); //purposely not implemented

  std::ostream& m_Ios;
  std::ios      m_State;
};
}

#endif
