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
#ifndef __itkWin32Header_h
#define __itkWin32Header_h

#include "itkConfigure.h"

// As of MSVS++ 7.1 and greater, typename is supported in templates
#define ITK_TYPENAME typename

// When a class definition has ITK_EXPORT, the class will be
// checked automatically, by Utilities/Dart/PrintSelfCheck.tcl
#define ITK_EXPORT

#if ( defined( _WIN32 ) || defined( WIN32 ) ) && !defined( ITKSTATIC )
#ifdef ITKCommon_EXPORTS
#define ITKCommon_EXPORT __declspec(dllexport)
#else
#define ITKCommon_EXPORT __declspec(dllimport)
#endif  /* ITKCommon_EXPORTS */

#else
/* unix needs nothing */
#define ITKCommon_EXPORT
#endif

#endif
