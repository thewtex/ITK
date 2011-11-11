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
#include "itkBinaryImageToLevelSetImageAdaptor.h"
#include "itkImageFileReader.h"
#include "itkLevelSetIterationUpdateCommand.h"
#include "itkLevelSetContainer.h"
#include "itkLevelSetEquationChanAndVeseInternalTerm.h"
#include "itkLevelSetEquationChanAndVeseExternalTerm.h"
#include "itkLevelSetEquationContainerBase.h"
#include "itkLevelSetEquationTermContainerBase.h"
#include "itkLevelSetEvolution.h"
#include "itkLevelSetEvolutionNumberOfIterationsStoppingCriterion.h"
#include "itkLevelSetDenseImageBase.h"
#include "itkMultiThreader.h"
#include "itkVTKVisualizeDenseImageLevelSet.h"
#include "vtkRenderWindow.h"
#include "itkSinRegularizedHeavisideStepFunction.h"

const unsigned int Dimension = 2;
typedef unsigned char                                    InputPixelType;
typedef itk::Image< InputPixelType, Dimension >          InputImageType;
typedef float                                            LevelSetPixelType;
typedef itk::Image< LevelSetPixelType, Dimension >       LevelSetImageType;
typedef itk::LevelSetDenseImageBase< LevelSetImageType > LevelSetType;

template< class TInputImage, class TLevelSetType >
void
visualizeLevelSet( TInputImage * inputImage, const int numberOfIterations, vtkRenderWindow * renderWindow )
{
  // Basic typedefs
  typedef TInputImage InputImageType;

  typedef typename LevelSetType::OutputType                LevelSetOutputType;
  typedef typename LevelSetType::OutputRealType            LevelSetRealType;

  // Generate a binary mask that will be used as initialization for the level
  // set evolution.
  typedef typename itk::Image< LevelSetOutputType, InputImageType::ImageDimension > BinaryImageType;
  typename BinaryImageType::Pointer binary = BinaryImageType::New();
  binary->SetRegions( inputImage->GetLargestPossibleRegion() );
  binary->CopyInformation( inputImage );
  binary->Allocate();
  binary->FillBuffer( itk::NumericTraits< LevelSetOutputType >::Zero );

  typename BinaryImageType::RegionType region;
  typename BinaryImageType::IndexType  index;
  typename BinaryImageType::SizeType   size;

  index.Fill( 5 );
  size.Fill( 120 );

  region.SetIndex( index );
  region.SetSize( size );

  typedef itk::ImageRegionIteratorWithIndex< BinaryImageType > InputIteratorType;
  InputIteratorType iIt( binary, region );
  iIt.GoToBegin();
  while( !iIt.IsAtEnd() )
    {
    iIt.Set( itk::NumericTraits< LevelSetOutputType >::One );
    ++iIt;
    }

  typedef itk::BinaryImageToLevelSetImageAdaptor< BinaryImageType,
    LevelSetType > BinaryImageToLevelSetType;

  typename BinaryImageToLevelSetType::Pointer adaptor = BinaryImageToLevelSetType::New();
  adaptor->SetInputImage( binary );
  adaptor->Initialize();
  typename LevelSetType::Pointer levelSet = adaptor->GetLevelSet();
  std::cout << "Finished converting to sparse format" << std::endl;

  // The Heaviside function
  typedef typename itk::SinRegularizedHeavisideStepFunction< LevelSetRealType, LevelSetRealType > HeavisideFunctionType;
  typename HeavisideFunctionType::Pointer heaviside = HeavisideFunctionType::New();
  heaviside->SetEpsilon( 1.5 );
  std::cout << "Heaviside function created" << std::endl;

  // Create the level set container
  typedef typename itk::LevelSetContainer< itk::IdentifierType, LevelSetType > LevelSetContainerType;
  typename LevelSetContainerType::Pointer levelSetContainer = LevelSetContainerType::New();
  levelSetContainer->SetHeaviside( heaviside );
  levelSetContainer->AddLevelSet( 0, levelSet );
  std::cout << "LevelSetContainer created" << std::endl;

  // Create the terms.
  //
  // // Chan and Vese internal term
  typedef itk::LevelSetEquationChanAndVeseInternalTerm< InputImageType, LevelSetContainerType > ChanAndVeseInternalTermType;
  typename ChanAndVeseInternalTermType::Pointer cvInternalTerm = ChanAndVeseInternalTermType::New();
  cvInternalTerm->SetInput( inputImage );
  cvInternalTerm->SetCoefficient( 0.5 );
  std::cout << "Chan and Vese internal term created" << std::endl;

  // // Chan and Vese external term
  typedef typename itk::LevelSetEquationChanAndVeseExternalTerm< InputImageType, LevelSetContainerType > ChanAndVeseExternalTermType;
  typename ChanAndVeseExternalTermType::Pointer cvExternalTerm = ChanAndVeseExternalTermType::New();
  cvExternalTerm->SetInput( inputImage );
  std::cout << "Chan and Vese external term created" << std::endl;

  // Create term container (equation rhs)
  typedef typename itk::LevelSetEquationTermContainerBase< InputImageType, LevelSetContainerType > TermContainerType;
  typename TermContainerType::Pointer termContainer = TermContainerType::New();
  termContainer->SetLevelSetContainer( levelSetContainer );
  termContainer->SetInput( inputImage );
  termContainer->AddTerm( 0, cvInternalTerm );
  termContainer->AddTerm( 1, cvExternalTerm );
  std::cout << "Term container created" << std::endl;

  // Create equation container
  typedef typename itk::LevelSetEquationContainerBase< TermContainerType > EquationContainerType;
  typename EquationContainerType::Pointer equationContainer = EquationContainerType::New();
  equationContainer->SetLevelSetContainer( levelSetContainer );
  equationContainer->AddEquation( 0, termContainer );
  std::cout << "Equation container created" << std::endl;

  // Create stopping criteria
  typedef typename itk::LevelSetEvolutionNumberOfIterationsStoppingCriterion< LevelSetContainerType > StoppingCriterionType;
  typename StoppingCriterionType::Pointer criterion = StoppingCriterionType::New();
  criterion->SetNumberOfIterations( numberOfIterations );
  std::cout << "Stopping criteria created" << std::endl;

  // Create the visualizer
  typedef itk::VTKVisualizeDenseImageLevelSet< InputPixelType, Dimension, LevelSetImageType > VisualizationType;
  typename VisualizationType::Pointer visualizer = VisualizationType::New();
  //! \todo the visualizer should get the input image from the level set
  visualizer->SetInputImage( inputImage );
  visualizer->SetLevelSet( levelSet );
  visualizer->SetRenderWindow( renderWindow );
  std::cout << "Visualizer created" << std::endl;

  // Create evolution class
  typedef typename itk::LevelSetEvolution< EquationContainerType, LevelSetType > LevelSetEvolutionType;
  typename LevelSetEvolutionType::Pointer evolution = LevelSetEvolutionType::New();
  evolution->SetEquationContainer( equationContainer );
  evolution->SetStoppingCriterion( criterion );
  evolution->SetLevelSetContainer( levelSetContainer );
  std::cout << "Evolution class created" << std::endl;

  typedef typename itk::LevelSetIterationUpdateCommand< LevelSetEvolutionType, VisualizationType > IterationUpdateCommandType;
  typename IterationUpdateCommandType::Pointer iterationUpdateCommand = IterationUpdateCommandType::New();
  iterationUpdateCommand->SetFilterToUpdate( visualizer );
  iterationUpdateCommand->SetUpdatePeriod( 1 );
  evolution->AddObserver( itk::IterationEvent(), iterationUpdateCommand );
  std::cout << "Visualization IterationUpdateCommand created" << std::endl;

  std::cout << "Evolving the level set..." << std::endl;
  evolution->Update();
}

struct VisualizationThreadData
{
  InputImageType *  m_InputImage;
  unsigned int      m_NumberOfIterations;
  vtkRenderWindow * m_RenderWindow;
};

ITK_THREAD_RETURN_TYPE visualizationThreadRunner( void * threadInfo )
{
  itk::MultiThreader::ThreadInfoStruct* info =
    static_cast<itk::MultiThreader::ThreadInfoStruct*>( threadInfo );

  VisualizationThreadData * visualizationThreadData = static_cast< VisualizationThreadData * >( info->UserData );
  visualizeLevelSet< InputImageType, LevelSetType >( visualizationThreadData->m_InputImage, visualizationThreadData->m_NumberOfIterations, visualizationThreadData->m_RenderWindow );

  return ITK_THREAD_RETURN_VALUE;
}

int itkVTKVisualizeLevelSetsInteractivePauseTest( int argc, char* argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Missing Arguments" << std::endl;
    std::cerr << argv[0] << std::endl;
    std::cerr << "1- Input Image" << std::endl;
    std::cerr << "2- Number of Iterations" << std::endl;

    return EXIT_FAILURE;
    }

  // Image Dimension

  // Read input image (to be processed).
  typedef itk::ImageFileReader< InputImageType >   ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();
  InputImageType::Pointer input = reader->GetOutput();
  std::cout << "Input image read" << std::endl;

  // Convert the binary mask into a level set function.
  // Here the output level-set will be a "Whitaker" sparse level-set;
  // i.e. only few layers {-2, -1, 0, +1, +2 } around the zero-set are
  // maintained, the rest of the domain is either -3 or +3.

  int numberOfIterations;
  std::istringstream istrm( argv[2] );
  istrm >> numberOfIterations;

  vtkSmartPointer< vtkRenderWindow > renderWindow = vtkSmartPointer< vtkRenderWindow >::New();
  renderWindow->Render();

  VisualizationThreadData visualizationThreadData;
  visualizationThreadData.m_InputImage = input.GetPointer();
  visualizationThreadData.m_NumberOfIterations = numberOfIterations;
  visualizationThreadData.m_RenderWindow = renderWindow.GetPointer();

  itk::MultiThreader::Pointer threader = itk::MultiThreader::New();
  try
    {
    //itk::ThreadIdType threadId = threader->SpawnThread( visualizationThreadRunner, &visualizationThreadData );
    //std::cout << "The spawned thread was: " << threadId << std::endl;
    //sleep(10);
    std::cout << "TerminatingThread..." << std::endl;
    //threader->TerminateThread( threadId );
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
