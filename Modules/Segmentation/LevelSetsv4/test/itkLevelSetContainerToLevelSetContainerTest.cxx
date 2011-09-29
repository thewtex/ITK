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

#include "itkImage.h"
#include "itkImageFileReader.h"

#include "itkLevelSetDenseImageBase.h"
#include "itkWhitakerSparseLevelSetImage.h"
#include "itkShiSparseLevelSetImage.h"
#include "itkMalcolmSparseLevelSetImage.h"

#include "itkBinaryImageToSparseLevelSetImageAdaptor.h"
#include "itkLevelSetContainerToLevelSetContainerFilter.h"

template< class TInputImage, class TLevelSet >
int TestLevelSetContainerToSparseLevelSetContainer( char* argv[] )
{
  typedef TInputImage                             InputImageType;
  typedef typename InputImageType::Pointer        InputImagePointer;

  typedef itk::ImageFileReader< InputImageType >  InputReaderType;
  typedef typename InputReaderType::Pointer       InputReaderPointer;

  InputReaderPointer reader = InputReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    return EXIT_FAILURE;
    }
  InputImagePointer input = reader->GetOutput();

  typedef TLevelSet LevelSetType;

  typedef itk::BinaryImageToSparseLevelSetImageAdaptor< InputImageType,
      LevelSetType > BinaryToSparseAdaptorType;

  typename BinaryToSparseAdaptorType::Pointer adaptor = BinaryToSparseAdaptorType::New();
  adaptor->SetInputImage( input );
  adaptor->Initialize();

  typename LevelSetType::Pointer LevelSet = adaptor->GetLevelSet();

  typedef itk::LevelSetContainer< unsigned int, LevelSetType > LevelSetContainerType;

  typename LevelSetContainerType::Pointer lscontainer = LevelSetContainerType::New();
  lscontainer->AddLevelSet( 0, LevelSet, false );

  typedef itk::LevelSetContainerToLevelSetContainerFilter< LevelSetContainerType >  FilterType;

  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput( lscontainer );
  filter->Update();

  typename LevelSetContainerType::Pointer outputLevelSet = filter->GetOutput();
  (void) outputLevelSet;

  return EXIT_SUCCESS;
}

int itkLevelSetContainerToLevelSetContainerTest( int argc, char* argv[] )
{
  if( argc < 2 )
    {
    std::cerr << "Missing Arguments" <<std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 2;
  typedef unsigned short  InputPixelType;
  typedef double          LevelSetOutputType;

  typedef itk::Image< InputPixelType, Dimension >     InputImageType;
  typedef itk::Image< LevelSetOutputType, Dimension > ImageType;

  typedef itk::LevelSetDenseImageBase< ImageType >  DenseLevelSetType;

  int output = EXIT_SUCCESS;//TestLevelSetContainerToLevelSetContainer< DenseLevelSetType >();

  if( output == EXIT_FAILURE )
    {
    std::cerr << "DenseLevelSetType: FAILURE" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::WhitakerSparseLevelSetImage< LevelSetOutputType, Dimension >  WhitakerLevelSetType;

  output = TestLevelSetContainerToSparseLevelSetContainer< InputImageType, WhitakerLevelSetType >( argv );

  if( output == EXIT_FAILURE )
    {
    std::cerr << "WhitakerLevelSetType: FAILURE" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::ShiSparseLevelSetImage< Dimension >  ShiLevelSetType;

  output = TestLevelSetContainerToSparseLevelSetContainer< InputImageType, WhitakerLevelSetType >( argv );

  if( output == EXIT_FAILURE )
    {
    std::cerr << "ShiLevelSetType: FAILURE" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::MalcolmSparseLevelSetImage< Dimension > MalcolmLevelSetType;

  output = TestLevelSetContainerToSparseLevelSetContainer< InputImageType, MalcolmLevelSetType >( argv );

  if( output == EXIT_FAILURE )
    {
    std::cerr << "MalcolmLevelSetType: FAILURE" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
