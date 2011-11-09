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
#include "itkOneHiddenLayerBackPropagationNeuralNetwork.h"
#include "itkBatchSupervisedTrainingFunction.h"
#include "itkListSample.h"

#include <fstream>

#define ROUND(x) (floor(x+0.5))

int
XORTest1(int argc, char* argv[])
{
  if (argc < 2)
    {
    std::cout << "Usage: " << argv[0]
              << " InputTrainingFile(.txt)" << std::endl ;
    return EXIT_FAILURE;
    }

  char* dataFileName = argv[1] ;


  unsigned int num_input_nodes = 2;
  unsigned int num_hidden_nodes = 5;
  unsigned int num_output_nodes = 1;

  typedef itk::Array<double> MeasurementVectorType;
  typedef itk::Array<double> TargetVectorType;

  typedef itk::Statistics::ListSample<MeasurementVectorType> SampleType;
  typedef itk::Statistics::ListSample<TargetVectorType> TargetType;
  typedef itk::Statistics::BatchSupervisedTrainingFunction<SampleType, TargetType, double> TrainingFcnType;

  MeasurementVectorType mv;
  mv.SetSize(num_input_nodes);

  TargetVectorType tv;
  tv.SetSize(num_output_nodes);

  SampleType::Pointer sample = SampleType::New();
  TargetType::Pointer targets = TargetType::New();
  sample->SetMeasurementVectorSize( num_input_nodes);
  targets->SetMeasurementVectorSize( num_output_nodes);

  std::ifstream infile1;
  infile1.open(dataFileName, std::ios::in);
  if (infile1.fail())
    {
    std::cout << argv[0] << " Cannot open file for reading: "
              << dataFileName << std::endl;
    return EXIT_FAILURE;
    }

  infile1 >> mv[0] >> mv[1]>> tv[0];

  while (!infile1.eof())
    {
    std::cout << "Input =" << mv << std::endl;
    std::cout << "target =" << tv << std::endl;
    sample->PushBack(mv);
    targets->PushBack(tv);
    infile1 >> mv[0] >> mv[1]>> tv[0];
   }
  infile1.close();

  std::cout << sample->Size() << std::endl;

  typedef itk::Statistics::OneHiddenLayerBackPropagationNeuralNetwork<MeasurementVectorType, TargetVectorType> OneHiddenLayerBackPropagationNeuralNetworkType;
  OneHiddenLayerBackPropagationNeuralNetworkType::Pointer net1 = OneHiddenLayerBackPropagationNeuralNetworkType::New();
  net1->SetNumOfInputNodes(num_input_nodes);
  net1->SetNumOfFirstHiddenNodes(num_hidden_nodes);
  net1->SetNumOfOutputNodes(num_output_nodes);

  net1->SetFirstHiddenLayerBias(1.0);
  net1->SetOutputLayerBias(1.0);

  net1->Initialize();
  net1->InitializeWeights();
  net1->SetLearningRate(0.05);

  TrainingFcnType::Pointer trainingfcn = TrainingFcnType::New();
  trainingfcn->SetIterations(200);

  trainingfcn->SetThreshold(0.001);
  trainingfcn->Train(net1, sample, targets);

  //Network Simulation
  std::cout << sample->Size() << std::endl;
  std::cout << "Network Simulation" << std::endl;
  TargetVectorType ov;
  ov.SetSize(num_output_nodes);

  SampleType::ConstIterator iter1 = sample->Begin();
  TargetType::ConstIterator iter2 = targets->Begin();
  unsigned int error1 = 0 ;
  unsigned int error2 = 0 ;
  int flag;
  std::ofstream outfile;
  outfile.open("out1.txt",std::ios::out);
  if (outfile.fail())
    {
    std::cout << argv[0] << " Cannot open file for writing: "
              << "out1.txt" << std::endl;
    return EXIT_FAILURE;
    }

  while (iter1 != sample->End())
    {
    mv = iter1.GetMeasurementVector();
    tv = iter2.GetMeasurementVector();
    ov=net1->GenerateOutput(mv);
    flag = 0;
    if (vcl_fabs(tv[0]-ov[0])>0.3)
      {
      flag = 1;
      }
    if (flag == 1 && ROUND(tv[0]) == 1)
      {
      ++error1;
      }
    else if (flag == 1 && ROUND(tv[0]) == -1)
      {
      ++error2;
      }

    outfile << mv[0] << " " << mv[1] << " "
            << tv[0] << " " << ov[0] << std::endl;

    std::cout << "Network Input = " << mv << std::endl;
    std::cout << "Network Output = " << ov << std::endl;
    std::cout << "Target = " << tv << std::endl;
    ++iter1;
    ++iter2;
    }

  std::cout << "Among 4 measurement vectors, " << error1 + error2
            << " vectors are misclassified." << std::endl ;
  std::cout<<"Network Weights and Biases after Training= "<<std::endl;
  std::cout << net1 << std::endl;

  if ((error1 + error2) > 2)
    {
    std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}
