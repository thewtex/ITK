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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLevelSetContainer.h"
#include "itkLevelSetEquationChanAndVeseExternalTerm.h"
#include "itkLevelSetEquationTermContainer.h"
#include "itkLevelSetEquationContainer.h"
#include "itkSinRegularizedHeavisideStepFunction.h"
#include "itkLevelSetEvolution.h"
#include "itkBinaryImageToLevelSetImageAdaptor.h"
#include "itkLevelSetEvolutionNumberOfIterationsStoppingCriterion.h"
#include "itkLevelSetSegmentationImage.h"

int itkTwoLevelSetWhitakerImage2DTest( int argc, char* argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Missing Arguments" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 2;

  typedef unsigned short                                    InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >           InputImageType;
  typedef itk::ImageRegionIteratorWithIndex< InputImageType >
                                                            InputIteratorType;
  typedef itk::ImageFileReader< InputImageType >            ReaderType;

  typedef float                                             PixelType;

  typedef itk::WhitakerSparseLevelSetImage< PixelType, Dimension >
                                                            SparseLevelSetType;
  typedef itk::BinaryImageToLevelSetImageAdaptor< InputImageType, SparseLevelSetType >
                                                            BinaryToSparseAdaptorType;

  typedef itk::IdentifierType                               IdentifierType;

  typedef itk::LevelSetContainer< IdentifierType, SparseLevelSetType >
                                                            LevelSetContainerType;

  typedef std::list< IdentifierType >                       IdListType;
  typedef itk::Image< IdListType, Dimension >               IdListImageType;
  typedef itk::Image< short, Dimension >                    CacheImageType;
  typedef itk::LevelSetDomainMapImageFilter< IdListImageType, CacheImageType >
                                                            DomainMapImageFilterType;

  typedef itk::LevelSetEquationChanAndVeseInternalTerm< InputImageType, LevelSetContainerType >
                                                            ChanAndVeseInternalTermType;
  typedef itk::LevelSetEquationChanAndVeseExternalTerm< InputImageType, LevelSetContainerType >
                                                            ChanAndVeseExternalTermType;
  typedef itk::LevelSetEquationTermContainer< InputImageType, LevelSetContainerType >
                                                            TermContainerType;

  typedef itk::LevelSetEquationContainer< TermContainerType >
                                                            EquationContainerType;

  typedef itk::LevelSetEvolution< EquationContainerType, SparseLevelSetType >
                                                            LevelSetEvolutionType;

  typedef SparseLevelSetType::OutputRealType                      LevelSetOutputRealType;
  typedef itk::SinRegularizedHeavisideStepFunction< LevelSetOutputRealType, LevelSetOutputRealType >
                                                            HeavisideFunctionBaseType;
  typedef itk::ImageRegionIteratorWithIndex< InputImageType >     InputIteratorType;

  typedef itk::LevelSetSegmentationImage< LevelSetContainerType > LevelSetSegmentationImageType;
  typedef LevelSetSegmentationImageType::SegmentImageType         SegmentImageType;
  // load binary input for segmentation
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();
  InputImageType::Pointer input = reader->GetOutput();

  // Create a binary initialization
  InputImageType::Pointer binary = InputImageType::New();
  binary->SetRegions( input->GetLargestPossibleRegion() );
  binary->CopyInformation( input );
  binary->Allocate();
  binary->FillBuffer( itk::NumericTraits<InputPixelType>::Zero );

  InputImageType::RegionType region;
  InputImageType::IndexType index;
  InputImageType::SizeType size;

  index.Fill( 10 );
  size.Fill( 30 );

  region.SetIndex( index );
  region.SetSize( size );

  InputIteratorType iIt( binary, region );
  iIt.GoToBegin();
  while( !iIt.IsAtEnd() )
    {
    iIt.Set( itk::NumericTraits<InputPixelType>::One );
    ++iIt;
    }

  // Convert binary mask to sparse level set
  BinaryToSparseAdaptorType::Pointer adaptor0 = BinaryToSparseAdaptorType::New();
  adaptor0->SetInputImage( binary );
  adaptor0->Initialize();
  std::cout << "Finished converting to sparse format" << std::endl;

  SparseLevelSetType::Pointer levelSet0 = adaptor0->GetModifiableLevelSet();

  BinaryToSparseAdaptorType::Pointer adaptor1 = BinaryToSparseAdaptorType::New();
  adaptor1->SetInputImage( binary );
  adaptor1->Initialize();
  std::cout << "Finished converting to sparse format" << std::endl;

  SparseLevelSetType::Pointer levelSet1 = adaptor1->GetModifiableLevelSet();

  // Create a list image specifying both level set ids
  IdListType listIds;
  listIds.push_back( 1 );
  listIds.push_back( 2 );

  IdListImageType::Pointer idImage = IdListImageType::New();
  idImage->SetRegions( input->GetLargestPossibleRegion() );
  idImage->Allocate();
  idImage->FillBuffer( listIds );

  DomainMapImageFilterType::Pointer domainMapFilter = DomainMapImageFilterType::New();
  domainMapFilter->SetInput( idImage );
  domainMapFilter->Update();
  std::cout << "Domain map computed" << std::endl;

  // Define the Heaviside function
  HeavisideFunctionBaseType::Pointer heaviside = HeavisideFunctionBaseType::New();
  heaviside->SetEpsilon( 1.0 );

  // Insert the levelsets in a levelset container
  LevelSetContainerType::Pointer lscontainer = LevelSetContainerType::New();
  lscontainer->SetHeaviside( heaviside );
  lscontainer->SetDomainMapFilter( domainMapFilter );

  bool levelSetNotYetAdded = lscontainer->AddLevelSet( 0, levelSet0, false );
  if ( !levelSetNotYetAdded )
    {
    return EXIT_FAILURE;
    }

  levelSetNotYetAdded = lscontainer->AddLevelSet( 1, levelSet1, false );
  if ( !levelSetNotYetAdded )
    {
    return EXIT_FAILURE;
    }
  std::cout << "Level set container created" << std::endl;

  // **************** CREATE ALL TERMS ****************

  // -----------------------------
  // *** 1st Level Set phi ***

  // Create ChanAndVese internal term for phi_{1}
  ChanAndVeseInternalTermType::Pointer cvInternalTerm0 = ChanAndVeseInternalTermType::New();
  cvInternalTerm0->SetInput( input );
  cvInternalTerm0->SetCoefficient( 1.0 );
  std::cout << "LevelSet 1: CV internal term created" << std::endl;

  // Create ChanAndVese external term for phi_{1}
  ChanAndVeseExternalTermType::Pointer cvExternalTerm0 = ChanAndVeseExternalTermType::New();
  cvExternalTerm0->SetInput( input );
  cvExternalTerm0->SetCoefficient( 1.0 );
  std::cout << "LevelSet 1: CV external term created" << std::endl;

  // -----------------------------
  // *** 2nd Level Set phi ***

  ChanAndVeseInternalTermType::Pointer cvInternalTerm1 = ChanAndVeseInternalTermType::New();
  cvInternalTerm1->SetInput( input );
  cvInternalTerm1->SetCoefficient( 1.0 );
  std::cout << "LevelSet 2: CV internal term created" << std::endl;

  // Create ChanAndVese external term for phi_{1}
  ChanAndVeseExternalTermType::Pointer cvExternalTerm1 = ChanAndVeseExternalTermType::New();
  cvExternalTerm1->SetInput( input );
  cvExternalTerm1->SetCoefficient( 1.0 );
  std::cout << "LevelSet 2: CV external term created" << std::endl;

  // **************** CREATE ALL EQUATIONS ****************

  // Create Term Container
  TermContainerType::Pointer termContainer0 = TermContainerType::New();
  termContainer0->SetInput( input );
  termContainer0->SetCurrentLevelSetId( 0 );
  termContainer0->SetLevelSetContainer( lscontainer );

  termContainer0->AddTerm( 0, cvInternalTerm0 );
  termContainer0->AddTerm( 1, cvExternalTerm0 );
  std::cout << "Term container 0 created" << std::endl;

  // Create Term Container
  TermContainerType::Pointer termContainer1 = TermContainerType::New();
  termContainer1->SetInput( input );
  termContainer1->SetCurrentLevelSetId( 1 );
  termContainer1->SetLevelSetContainer( lscontainer );

  termContainer1->AddTerm( 0, cvInternalTerm1 );
  termContainer1->AddTerm( 1, cvExternalTerm1 );
  std::cout << "Term container 1 created" << std::endl;

  // Create equation container
  EquationContainerType::Pointer equationContainer = EquationContainerType::New();
  equationContainer->SetLevelSetContainer( lscontainer );
  equationContainer->AddEquation( 0, termContainer0 );
  equationContainer->AddEquation( 1, termContainer1 );

  typedef itk::LevelSetEvolutionNumberOfIterationsStoppingCriterion< LevelSetContainerType >
      StoppingCriterionType;
  StoppingCriterionType::Pointer criterion = StoppingCriterionType::New();
  criterion->SetNumberOfIterations( atoi(argv[2]) );

  LevelSetEvolutionType::Pointer evolution = LevelSetEvolutionType::New();
  evolution->SetEquationContainer( equationContainer );
  evolution->SetStoppingCriterion( criterion );
  evolution->SetLevelSetContainer( lscontainer );

  try
    {
    evolution->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image< char, Dimension > OutputImageType;
  OutputImageType::Pointer outputImage = OutputImageType::New();
  outputImage->SetRegions( input->GetLargestPossibleRegion() );
  outputImage->CopyInformation( input );
  outputImage->Allocate();
  outputImage->FillBuffer( 0 );

  typedef itk::ImageRegionIteratorWithIndex< OutputImageType > OutputIteratorType;
  OutputIteratorType oIt( outputImage, outputImage->GetLargestPossibleRegion() );
  oIt.GoToBegin();

  OutputImageType::IndexType idx;

  while( !oIt.IsAtEnd() )
    {
    idx = oIt.GetIndex();
    oIt.Set( levelSet1->GetLabelMap()->GetPixel(idx) );
    ++oIt;
    }

  typedef itk::ImageFileWriter< OutputImageType >     OutputWriterType;
  OutputWriterType::Pointer writer = OutputWriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( outputImage );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    }

  // Obtain the segmentation output
  LevelSetSegmentationImageType::Pointer segmentFilter = LevelSetSegmentationImageType::New();
  segmentFilter->SetLevelSetContainer( lscontainer );
  SegmentImageType::Pointer segmentationImage = segmentFilter->GetSegmentationImage( 0 );
  std::cout << "Segmentation using domain map created" << std::endl;


  // Create a level-set container without the domain map filter
  LevelSetContainerType::Pointer lscontainer2 = LevelSetContainerType::New();
  lscontainer2->SetHeaviside( heaviside );

  levelSetNotYetAdded = lscontainer2->AddLevelSet( 0, levelSet0, false );
  if ( !levelSetNotYetAdded )
    {
    return EXIT_FAILURE;
    }

  levelSetNotYetAdded = lscontainer2->AddLevelSet( 1, levelSet1, false );
  if ( !levelSetNotYetAdded )
    {
    return EXIT_FAILURE;
    }
  std::cout << "Level set container without domain map created" << std::endl;

  LevelSetSegmentationImageType::Pointer segmentFilter2 = LevelSetSegmentationImageType::New();
  segmentFilter2->SetLevelSetContainer( lscontainer2 );
  SegmentImageType::Pointer segmentationImage2 = segmentFilter2->GetSegmentationImage( 0 );
  std::cout << "Segmentation using level-set container created" << std::endl;

  return EXIT_SUCCESS;
}
