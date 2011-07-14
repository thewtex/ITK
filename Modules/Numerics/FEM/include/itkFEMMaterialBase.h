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
#ifndef __itkFEMMaterialBase_h
#define __itkFEMMaterialBase_h

#include "itkFEMLightObject.h"
#include "itkFEMPArray.h"

namespace itk {
namespace fem {

/**
 * \class Material
 * \brief Base class for storing all the implicit material and other properties
          required to fully define the element class.
 *
 * When specifying materials for particular element, you should use
 * MaterialStandard class or derive your own class (using Material
 * or MaterialStandard as a base class) if your Element requires
 * special properties or constants.
 *
 * Material base class doesn't define any data member.
 * Everything usefull is stored in derived clases. This class
 * is here just to group all material classes together and access
 * them via this base class.
 * \ingroup ITKFEM
 */
class Material : public FEMLightObject
{
  FEM_ABSTRACT_CLASS(Material,FEMLightObject)
public:
  /**
   * Array class that holds special pointers to objects of all Material classes
   */
  typedef FEMPArray<Self> ArrayType;

};

}} // end namespace itk::fem

#endif // #ifndef __itkFEMMaterialBase_h
