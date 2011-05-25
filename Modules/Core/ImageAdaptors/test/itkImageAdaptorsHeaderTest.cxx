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

#include "itkAbsImageAdaptor.h"
#include "itkAcosImageAdaptor.h"
#include "itkAddImageAdaptor.h"
#include "itkAddPixelAccessor.h"
#include "itkAsinImageAdaptor.h"
#include "itkAtanImageAdaptor.h"
#include "itkBluePixelAccessor.h"
#include "itkComplexToImaginaryImageAdaptor.h"
#include "itkComplexToModulusImageAdaptor.h"
#include "itkComplexToPhaseImageAdaptor.h"
#include "itkComplexToRealImageAdaptor.h"
#include "itkCosImageAdaptor.h"
#include "itkExpImageAdaptor.h"
#include "itkExpNegativeImageAdaptor.h"
#include "itkGreenPixelAccessor.h"
#include "itkImageAdaptor.txx"
#include "itkLog10ImageAdaptor.h"
#include "itkLogImageAdaptor.h"
#include "itkNthElementImageAdaptor.h"
#include "itkNthElementPixelAccessor.h"
#include "itkPixelAccessor.h"
#include "itkRGBToLuminanceImageAdaptor.h"
#include "itkRGBToVectorImageAdaptor.h"
#include "itkRGBToVectorPixelAccessor.h"
#include "itkRedPixelAccessor.h"
#include "itkSinImageAdaptor.h"
#include "itkSqrtImageAdaptor.h"
#include "itkTanImageAdaptor.h"
#include "itkVectorImageToImageAdaptor.h"
#include "itkVectorToRGBImageAdaptor.h"
#include "itkVectorToRGBPixelAccessor.h"


int itkImageAdaptorsHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
