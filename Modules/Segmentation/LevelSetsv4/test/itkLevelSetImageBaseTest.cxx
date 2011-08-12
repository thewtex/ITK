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
#include "itkLevelSetImageBase.h"
#include "itkImageRegionIteratorWithIndex.h"

int itkLevelSetImageBaseTest( int , char* [] )
{
  const unsigned int Dimension = 2;

  typedef float PixelType;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::LevelSetImageBase< ImageType > LevelSetType;

  ImageType::IndexType index;
  index[0] = 0;
  index[1] = 0;

  ImageType::SizeType size;
  size[0] = 10;
  size[1] = 10;

  ImageType::RegionType region;
  region.SetIndex( index );
  region.SetSize( size );

  PixelType value = 0.;

  ImageType::Pointer input = ImageType::New();
  input->SetRegions( region );
  input->Allocate();
  input->FillBuffer( value );

  itk::ImageRegionIteratorWithIndex< ImageType > it( input,
                                              input->GetLargestPossibleRegion() );

  it.GoToBegin();

  ImageType::IndexType idx;

  while( !it.IsAtEnd() )
    {
    idx = it.GetIndex();

    it.Set( vcl_sqrt(
             static_cast< float> ( ( idx[0] - 5 ) * ( idx[0] - 5 ) +
                                   ( idx[1] - 5 ) * ( idx[1] - 5 ) ) ) );
    ++it;
    }

  LevelSetType::Pointer level_set = LevelSetType::New();
  level_set->SetImage( input );

  itk::ImageRegionConstIteratorWithIndex< ImageType > o_it( input,
                                                     input->GetLargestPossibleRegion() );

  o_it.GoToBegin();

  while( !o_it.IsAtEnd() )
    {
    idx = o_it.GetIndex();

    std::cout << idx << " " << level_set->Evaluate( idx ) <<std::endl;
    ++o_it;
    }

  return EXIT_SUCCESS;
}
