/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkMath.h"
#include "itkNumericTraits.h"
#include "itkLevelSetContainer.h"
#include "itkLevelSetEquationOverlapPenaltyTerm.h"
#include "itkSinRegularizedHeavisideStepFunction.h"
#include "itkBinaryImageToLevelSetImageAdaptor.h"
#include "itkTestingMacros.h"

int
itkLevelSetEquationOverlapPenaltyTermTest(int, char *[])
{
  constexpr unsigned int Dimension = 2;

  using InputPixelType = unsigned short;
  using InputImageType = itk::Image<InputPixelType, Dimension>;
  using IdentifierType = itk::IdentifierType;

  using InputPixelType = unsigned short;
  using InputImageType = itk::Image<InputPixelType, Dimension>;

  using PixelType = float;
  using SparseLevelSetType = itk::WhitakerSparseLevelSetImage<PixelType, Dimension>;
  using BinaryToSparseAdaptorType = itk::BinaryImageToLevelSetImageAdaptor<InputImageType, SparseLevelSetType>;

  using LevelSetContainerType = itk::LevelSetContainer<IdentifierType, SparseLevelSetType>;
  using OverlapPenaltyTermType = itk::LevelSetEquationOverlapPenaltyTerm<InputImageType, LevelSetContainerType>;

  using IdListType = std::list<IdentifierType>;
  using IdListImageType = itk::Image<IdListType, Dimension>;
  using CacheImageType = itk::Image<short, Dimension>;
  using DomainMapImageFilterType = itk::LevelSetDomainMapImageFilter<IdListImageType, CacheImageType>;

  using LevelSetOutputRealType = SparseLevelSetType::OutputRealType;
  using HeavisideFunctionBaseType =
    itk::SinRegularizedHeavisideStepFunction<LevelSetOutputRealType, LevelSetOutputRealType>;
  using InputImageIteratorType = itk::ImageRegionIteratorWithIndex<InputImageType>;

  // load binary mask
  auto size = InputImageType::SizeType::Filled(50);

  InputImageType::PointType origin;
  origin[0] = 0.0;
  origin[1] = 0.0;

  InputImageType::SpacingType spacing;
  spacing[0] = 1.0;
  spacing[1] = 1.0;

  InputImageType::IndexType index{};

  InputImageType::RegionType region{ index, size };

  // Binary initialization
  auto binary = InputImageType::New();
  binary->SetRegions(region);
  binary->SetSpacing(spacing);
  binary->SetOrigin(origin);
  binary->Allocate();
  binary->FillBuffer(InputPixelType{});

  index.Fill(10);
  size.Fill(30);

  region.SetIndex(index);
  region.SetSize(size);

  InputImageIteratorType iIt(binary, region);
  iIt.GoToBegin();
  while (!iIt.IsAtEnd())
  {
    iIt.Set(itk::NumericTraits<InputPixelType>::OneValue());
    ++iIt;
  }

  // Convert binary mask to sparse level set
  auto adaptor1 = BinaryToSparseAdaptorType::New();
  adaptor1->SetInputImage(binary);
  adaptor1->Initialize();
  std::cout << "Finished converting levelset1 to sparse format" << std::endl;

  // Convert binary mask to sparse level set
  auto adaptor2 = BinaryToSparseAdaptorType::New();
  adaptor2->SetInputImage(binary);
  adaptor2->Initialize();
  std::cout << "Finished converting levelset2 to sparse format" << std::endl;

  const SparseLevelSetType::Pointer level_set1 = adaptor1->GetModifiableLevelSet();
  const SparseLevelSetType::Pointer level_set2 = adaptor2->GetModifiableLevelSet();

  IdListType list_ids;
  list_ids.push_back(1);
  list_ids.push_back(2);

  auto id_image = IdListImageType::New();
  id_image->SetRegions(binary->GetLargestPossibleRegion());
  id_image->Allocate();
  id_image->FillBuffer(list_ids);

  auto domainMapFilter = DomainMapImageFilterType::New();
  domainMapFilter->SetInput(id_image);
  domainMapFilter->Update();
  std::cout << "Domain map computed" << std::endl;

  // Define the Heaviside function
  auto heaviside = HeavisideFunctionBaseType::New();
  heaviside->SetEpsilon(2.0);

  // Insert the levelsets in a levelset container
  auto lscontainer = LevelSetContainerType::New();
  lscontainer->SetHeaviside(heaviside);
  lscontainer->SetDomainMapFilter(domainMapFilter);

  bool LevelSetNotYetAdded = lscontainer->AddLevelSet(0, level_set1, false);
  if (!LevelSetNotYetAdded)
  {
    return EXIT_FAILURE;
  }

  LevelSetNotYetAdded = lscontainer->AddLevelSet(1, level_set2, false);
  if (!LevelSetNotYetAdded)
  {
    return EXIT_FAILURE;
  }


  // Create overlap penalty term
  auto penaltyTerm0 = OverlapPenaltyTermType::New();
  penaltyTerm0->SetInput(binary);
  penaltyTerm0->SetCoefficient(1000.0);
  penaltyTerm0->SetCurrentLevelSetId(0);
  penaltyTerm0->SetLevelSetContainer(lscontainer);

  auto penaltyTerm1 = OverlapPenaltyTermType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(penaltyTerm1, LevelSetEquationOverlapPenaltyTerm, LevelSetEquationTermBase);


  penaltyTerm1->SetInput(binary);
  ITK_TEST_SET_GET_VALUE(binary, penaltyTerm1->GetInput());

  constexpr typename OverlapPenaltyTermType::LevelSetOutputRealType coefficient = 1000.0;
  penaltyTerm1->SetCoefficient(coefficient);
  ITK_TEST_SET_GET_VALUE(coefficient, penaltyTerm1->GetCoefficient());

  constexpr typename OverlapPenaltyTermType::LevelSetIdentifierType currentLevelSetId = 1;
  penaltyTerm1->SetCurrentLevelSetId(currentLevelSetId);
  ITK_TEST_SET_GET_VALUE(currentLevelSetId, penaltyTerm1->GetCurrentLevelSetId());

  penaltyTerm1->SetLevelSetContainer(lscontainer);
  ITK_TEST_SET_GET_VALUE(lscontainer, penaltyTerm1->GetLevelSetContainer());

  const std::string termName = "Overlap term";
  penaltyTerm1->SetTermName(termName);
  ITK_TEST_SET_GET_VALUE(termName, penaltyTerm1->GetTermName());

  std::cout << "Penalty terms created" << std::endl;

  // Initialize the ChanAndVese term here
  penaltyTerm0->InitializeParameters();
  penaltyTerm1->InitializeParameters();

  InputImageIteratorType it(binary, binary->GetLargestPossibleRegion());
  it.GoToBegin();

  while (!it.IsAtEnd())
  {
    penaltyTerm0->Initialize(it.GetIndex());
    penaltyTerm1->Initialize(it.GetIndex());
    ++it;
  }

  penaltyTerm0->Update();
  penaltyTerm1->Update();

  index[0] = 5;
  index[1] = 5;

  std::cout << penaltyTerm0->Evaluate(index) << std::endl;
  if (itk::Math::NotAlmostEquals(penaltyTerm0->Evaluate(index), OverlapPenaltyTermType::LevelSetOutputRealType{}))
  {
    return EXIT_FAILURE;
  }

  index[0] = 20;
  index[1] = 20;

  std::cout << penaltyTerm0->Evaluate(index) << std::endl;
  if (itk::Math::NotAlmostEquals(penaltyTerm0->Evaluate(index), 1000))
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
