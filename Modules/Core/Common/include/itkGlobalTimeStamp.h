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
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#ifndef itkGlobalTimeStamp_h
#define itkGlobalTimeStamp_h

#include "itkIntTypes.h"
#include "itkAtomicInt.h"

namespace itk
{
/**
 * This is the global time stamp, an integer, used by all ITK objects in their
 * ModifiedTime. It is used to determine whether pipeline updates are
 * necessary.
 * \ingroup ITKCommon
 */
extern AtomicInt<ModifiedTimeType> GlobalTimeStamp;

} // end namespace itk

#endif
