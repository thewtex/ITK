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

#include <string>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryImageToWhitakerSparseLevelSetAdaptor.h"
#include "itkImageRegionIteratorWithIndex.h"

int itkBinaryImageToWhitakerSparseLevelSetAdaptorTest( int argc, char* argv[] )
{
  const unsigned int Dimension = 2;

  typedef unsigned char InputPixelType;
  typedef double        OutputPixelType;

  typedef itk::Image< InputPixelType, Dimension >     InputImageType;

  typedef itk::ImageFileReader< InputImageType >      InputReaderType;
  InputReaderType::Pointer reader = InputReaderType::New();
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    }

  InputImageType::Pointer input = reader->GetOutput();
  std::cout << "Input image read" << std::endl;

  typedef itk::BinaryImageToWhitakerSparseLevelSetAdaptor< InputImageType,
      OutputPixelType > BinaryToSparseAdaptorType;

  BinaryToSparseAdaptorType::Pointer adaptor = BinaryToSparseAdaptorType::New();
  adaptor->SetInputImage( input );
  adaptor->Initialize();

  std::cout << "Finished converting to sparse format" << std::endl;

  typedef BinaryToSparseAdaptorType::LevelSetType     SparseLevelSetType;
  SparseLevelSetType::Pointer sparseLevelSet = adaptor->GetSparseLevelSet();


  typedef itk::Image< OutputPixelType, Dimension >    OutputImageType;
  OutputImageType::Pointer output = OutputImageType::New();
  output->SetRegions( input->GetLargestPossibleRegion() );
  output->CopyInformation( input );
  output->Allocate();
  output->FillBuffer( 0.0 );

  typedef itk::Image< char, Dimension >               StatusImageType;
  StatusImageType::Pointer status = StatusImageType::New();
  status->SetRegions( input->GetLargestPossibleRegion() );
  status->CopyInformation( input );
  status->Allocate();
  status->FillBuffer( 0 );

  typedef itk::ImageRegionIteratorWithIndex< OutputImageType > OutputIteratorType;
  OutputIteratorType oIt( output, output->GetLargestPossibleRegion() );
  oIt.GoToBegin();

  typedef itk::ImageRegionIteratorWithIndex< StatusImageType > StatusIteratorType;
  StatusIteratorType sIt( status, status->GetLargestPossibleRegion() );
  sIt.GoToBegin();

  StatusImageType::IndexType idx;

  while( !oIt.IsAtEnd() )
    {
    idx = oIt.GetIndex();
    oIt.Set( sparseLevelSet->Evaluate( idx ) );
    sIt.Set( sparseLevelSet->Status( idx ) );
    ++oIt;
    ++sIt;
    }

  typedef itk::ImageFileWriter< OutputImageType >     OutputWriterType;
  OutputWriterType::Pointer outputWriter = OutputWriterType::New();
  outputWriter->SetFileName( argv[2] );
  outputWriter->SetInput( output );

  try
    {
    outputWriter->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    }

  typedef itk::ImageFileWriter< StatusImageType >     StatusWriterType;
  StatusWriterType::Pointer statusWriter = StatusWriterType::New();
  statusWriter->SetFileName( argv[3] );
  statusWriter->SetInput( status );

  try
    {
    statusWriter->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    }

  for( char lyr = -2; lyr < 3; lyr++ )
  {
    SparseLevelSetType::LayerType layer = sparseLevelSet->GetLayer( lyr );
    SparseLevelSetType::LayerIterator lIt = layer.begin();

    std::cout << "*** " << static_cast< int >( lyr ) << " ***" <<std::endl;

    while( lIt != layer.end() )
    {
      std::cout << lIt->first << std::endl;
      ++lIt;
    }
    std::cout << std::endl;
  }

  typedef itk::LabelObject< unsigned long, 2 > LabelObjectType;
  LabelObjectType::Pointer labelObject = LabelObjectType::New();
  labelObject->CopyAllFrom( sparseLevelSet->GetAsLabelObject<unsigned long>() );
  labelObject->SetLabel( 1 );

  labelObject->Optimize();
  std::cout << labelObject->Size() << std::endl;

  return EXIT_SUCCESS;
}
