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

#include "itkWhitakerSparseLevelSetImage.h"

int
itkWhitakerSparseLevelSetImageTest( int , char* [] )
{
  typedef double OutputType;
  const unsigned int Dimension = 2;
  typedef itk::WhitakerSparseLevelSetImage< OutputType, Dimension >
    SparseLevelSetType;

  typedef SparseLevelSetType::LabelMapType LabelMapType;
  typedef LabelMapType::Pointer            LabelMapPointer;
  typedef LabelMapType::IndexType          IndexType;

  IndexType index;
  index.Fill( 3 );

  LabelMapType::Pointer labelMap = LabelMapType::New();
  labelMap->SetBackgroundValue( 3 );

  for( int i = 0; i < 4; i++ )
    {
    ++index[1];
    labelMap->SetPixel( index, -3 );
    }

  SparseLevelSetType::Pointer phi = SparseLevelSetType::New();
  phi->SetLabelMap( labelMap );

  index[0] = 3;
  index[1] = 3;

  if( phi->Evaluate( index ) != 3 )
    {
    std::cout << index << ' ' << phi->Evaluate( index ) << " != 3" << std::endl;
    return EXIT_FAILURE;
    }

  index[0] = 3;
  index[1] = 4;
  if( phi->Evaluate( index ) != -3 )
    {
    std::cout << index << ' ' << phi->Evaluate( index ) << " != -3" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
