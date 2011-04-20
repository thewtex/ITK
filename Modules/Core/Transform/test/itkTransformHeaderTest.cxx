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

#include "itkAffineTransform.hpp"
#include "itkAzimuthElevationToCartesianTransform.hpp"
#include "itkBSplineDeformableTransform.hpp"
#include "itkCenteredAffineTransform.hpp"
#include "itkCenteredEuler3DTransform.hpp"
#include "itkCenteredRigid2DTransform.hpp"
#include "itkCenteredSimilarity2DTransform.hpp"
//BUG 11909
//#include "itkCenteredTransformInitializer.hpp"
//#include "itkCenteredVersorTransformInitializer.hpp"
#include "itkElasticBodyReciprocalSplineKernelTransform.hpp"
#include "itkElasticBodySplineKernelTransform.hpp"
#include "itkEuler2DTransform.hpp"
#include "itkEuler3DTransform.hpp"
#include "itkFixedCenterOfRotationAffineTransform.hpp"
#include "itkIdentityTransform.h"
#include "itkKernelTransform.hpp"
#include "itkLandmarkBasedTransformInitializer.hpp"
#include "itkMatlabTransformIO.h"
#include "itkMatlabTransformIOFactory.h"
#include "itkMatrixOffsetTransformBase.hpp"
#include "itkQuaternionRigidTransform.hpp"
#include "itkRigid2DTransform.hpp"
#include "itkRigid3DPerspectiveTransform.hpp"
#include "itkRigid3DTransform.hpp"
#include "itkScalableAffineTransform.hpp"
#include "itkScaleLogarithmicTransform.hpp"
#include "itkScaleSkewVersor3DTransform.hpp"
#include "itkScaleTransform.hpp"
#include "itkScaleVersor3DTransform.hpp"
#include "itkSimilarity2DTransform.hpp"
#include "itkSimilarity3DTransform.hpp"
#include "itkThinPlateR2LogRSplineKernelTransform.hpp"
#include "itkThinPlateSplineKernelTransform.hpp"
#include "itkTransform.hpp"
#include "itkTransformBase.h"
#include "itkTransformFactory.h"
#include "itkTransformFactoryBase.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkTransformIOBase.h"
#include "itkTransformIOFactory.h"
#include "itkTranslationTransform.hpp"
#include "itkTxtTransformIO.h"
#include "itkTxtTransformIOFactory.h"
#include "itkv3Rigid3DTransform.h"
#include "itkVersorRigid3DTransform.hpp"
#include "itkVersorTransform.hpp"
#include "itkVolumeSplineKernelTransform.hpp"



int itkTransformHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
