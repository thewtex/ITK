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

#include "itkAmoebaOptimizer.h"
#include "itkConjugateGradientOptimizer.h"
#include "itkCostFunction.h"
#include "itkCumulativeGaussianCostFunction.h"
#include "itkCumulativeGaussianOptimizer.h"
#include "itkExhaustiveOptimizer.h"
#include "itkFRPROptimizer.h"
#include "itkGradientDescentOptimizer.h"
#include "itkLBFGSBOptimizer.h"
#include "itkLBFGSOptimizer.h"
#include "itkLevenbergMarquardtOptimizer.h"
#include "itkMultipleValuedCostFunction.h"
#include "itkMultipleValuedNonLinearOptimizer.h"
#include "itkMultipleValuedNonLinearVnlOptimizer.h"
#include "itkMultipleValuedVnlCostFunctionAdaptor.h"
#include "itkNonLinearOptimizer.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkOptimizer.h"
#include "itkPowellOptimizer.h"
#include "itkQuaternionRigidTransformGradientDescentOptimizer.h"
#include "itkRegularStepGradientDescentBaseOptimizer.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkSPSAOptimizer.h"
#include "itkSingleValuedCostFunction.h"
#include "itkSingleValuedNonLinearOptimizer.h"
#include "itkSingleValuedNonLinearVnlOptimizer.h"
#include "itkSingleValuedVnlCostFunctionAdaptor.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkVersorTransformOptimizer.h"


int itkOptimizersHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
