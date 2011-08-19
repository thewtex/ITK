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
#include "itkBinaryImageToMalcolmSparseLevelSetAdaptor.h"
#include "itkImageRegionIterator.h"

int itkBinaryImageToMalcolmSparseLevelSetAdaptorTest( int argc, char* argv[] )
{
  const unsigned int Dimension = 2;

  typedef unsigned char InputPixelType;

  typedef itk::Image< InputPixelType, Dimension > InputImageType;
  typedef itk::ImageFileReader< InputImageType >  InputReaderType;

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

  typedef itk::BinaryImageToMalcolmSparseLevelSetAdaptor< InputImageType >
    BinaryToSparseAdaptorType;

  BinaryToSparseAdaptorType::Pointer adaptor = BinaryToSparseAdaptorType::New();
  adaptor->SetInputImage( input );
  adaptor->Initialize();
  std::cout << "Finished converting to sparse format" << std::endl;

  typedef BinaryToSparseAdaptorType::LevelSetType     SparseLevelSetType;
  SparseLevelSetType::Pointer sparseLevelSet = adaptor->GetSparseLevelSet();

  typedef BinaryToSparseAdaptorType::LevelSetOutputType LevelSetOutputType;

  typedef itk::Image< LevelSetOutputType, Dimension >   StatusImageType;
  StatusImageType::Pointer status = StatusImageType::New();
  status->SetRegions( input->GetLargestPossibleRegion() );
  status->CopyInformation( input );
  status->Allocate();
  status->FillBuffer( 0 );

  typedef itk::ImageRegionIteratorWithIndex< StatusImageType > StatusIteratorType;
  StatusIteratorType sIt( status, status->GetLargestPossibleRegion() );
  sIt.GoToBegin();

  StatusImageType::IndexType idx;

  while( !sIt.IsAtEnd() )
    {
    idx = sIt.GetIndex();
    sIt.Set( sparseLevelSet->Evaluate( idx ) );
    ++sIt;
    }

  typedef itk::ImageFileWriter< StatusImageType >     StatusWriterType;
  StatusWriterType::Pointer writer = StatusWriterType::New();
  writer->SetFileName( argv[2] );
  writer->SetInput( status );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    }

  SparseLevelSetType::LayerType layer = sparseLevelSet->GetLayer( 0 );
  SparseLevelSetType::LayerIterator lIt = layer.begin();

  while( lIt != layer.end() )
    {
    std::cout << lIt->first << std::endl;
    ++lIt;
    }

  typedef itk::LabelObject< unsigned long, 2 > LabelObjectType;
  LabelObjectType::Pointer labelObject = LabelObjectType::New();
  labelObject->CopyAllFrom( sparseLevelSet->GetAsLabelObject<unsigned long>() );
  labelObject->SetLabel( 1 );

  labelObject->Optimize();
  std::cout << labelObject->Size() << std::endl;

  return EXIT_SUCCESS;
}
