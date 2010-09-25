/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    wrap_itkSobelEdgeDetectionImageFilter.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkSobelEdgeDetectionImageFilter.h"

#ifdef CABLE_CONFIGURATION
#include "itkCSwigMacros.h"
#include "itkCSwigImages.h"

namespace _cable_
{
  const char* const group = ITK_WRAP_GROUP(itkSobelEdgeDetectionImageFilter);
  namespace wrappers
  {
    ITK_WRAP_OBJECT2(SobelEdgeDetectionImageFilter, image::F2, image::F2,
                     itkSobelEdgeDetectionImageFilterF2F2);
    ITK_WRAP_OBJECT2(SobelEdgeDetectionImageFilter, image::F3, image::F3,
                     itkSobelEdgeDetectionImageFilterF3F3);
  }
}
#endif
