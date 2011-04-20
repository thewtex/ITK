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

#include "itkBackPropagationLayer.hpp"
#include "itkBatchSupervisedTrainingFunction.hpp"
#include "itkCompletelyConnectedWeightSet.hpp"
#include "itkErrorBackPropagationLearningFunctionBase.hpp"
#include "itkErrorBackPropagationLearningWithMomentum.hpp"
#include "itkErrorFunctionBase.h"
#include "itkGaussianRadialBasisFunction.hpp"
#include "itkGaussianTransferFunction.hpp"
#include "itkHardLimitTransferFunction.hpp"
#include "itkIdentityTransferFunction.hpp"
#include "itkInputFunctionBase.h"
#include "itkIterativeSupervisedTrainingFunction.hpp"
#include "itkLayerBase.hpp"
#include "itkLogSigmoidTransferFunction.hpp"
#include "itkMeanSquaredErrorFunction.hpp"
#include "itkMultilayerNeuralNetworkBase.hpp"
#include "itkMultiquadricRadialBasisFunction.hpp"
#include "itkNeuralNetworkObject.hpp"
#include "itkNNetDistanceMetricBase.h"
#include "itkOneHiddenLayerBackPropagationNeuralNetwork.hpp"
#include "itkProductInputFunction.hpp"
#include "itkQuickPropLearningRule.hpp"
#include "itkRBFBackPropagationLearningFunction.hpp"
#include "itkRBFLayer.hpp"
#include "itkRBFNetwork.hpp"
#include "itkSigmoidTransferFunction.hpp"
#include "itkSignedHardLimitTransferFunction.hpp"
#include "itkSquaredDifferenceErrorFunction.hpp"
#include "itkSumInputFunction.hpp"
#include "itkSymmetricSigmoidTransferFunction.hpp"
#include "itkTanHTransferFunction.hpp"
#include "itkTanSigmoidTransferFunction.hpp"
#include "itkTrainingFunctionBase.hpp"
#include "itkTransferFunctionBase.h"
#include "itkTwoHiddenLayerBackPropagationNeuralNetwork.hpp"
#include "itkWeightSetBase.hpp"

int itkNeuralNetworksHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
