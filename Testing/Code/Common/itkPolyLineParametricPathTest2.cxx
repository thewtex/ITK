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
#include "itkImage.h"
#include "itkPolyLineParametricPath.h"
#include "itkPathConstIterator.h"

int itkPolyLineParametricPathTest2(int, char* [])
{
  typedef itk::Image<unsigned short, 3>  ImageType;
  typedef itk::PolyLineParametricPath<3> PathType;

  typedef itk::PathConstIterator< ImageType, PathType > PathConstIteratorType;

  bool passed = true;

  ImageType::Pointer image = ImageType::New();
  ImageType::SizeType imageSize = {{10,10,2}};
  image->SetRegions( imageSize );
  image->Allocate();

  PathType::Pointer path = PathType::New();
  double vertices[][3] =
  {
    { 3.5396, 4.5323, 0.42581 },
    { 4.0457, 5.1173, 0.47410 },
    { 4.4586, 5.6914, 0.52771 },
    { 4.6246, 5.9129, 0.60156 },
    { 4.6535, 5.9860, 0.67940 },
    { 4.6541, 6.0289, 0.75745 },
    { 4.6705, 6.0968, 0.83538 },
    { 4.7229, 6.2189, 0.91279 },
    { 4.8351, 6.4354, 0.98848 },
    { 5.0322, 6.7869, 1.0598  },
    { 5.1955, 7.2585, 1.1254  },
    { 4.9012, 7.5336, 1.1968  }
  };
  for( unsigned i = 0; i < 12; i++ ) path->AddVertex( vertices[i] );

  try
    {
    std::cout << "Iterate path: ";
    PathConstIteratorType tractIt( image, path );
    for( tractIt.GoToBegin(); ! tractIt.IsAtEnd(); ++tractIt )
      {
      std::cout << tractIt.GetPathPosition() << " -> " << std::flush;
      }
    std::cout << "done." << std::endl;
    }
  catch( itk::ExceptionObject & ex )
    {
    std::cerr << "Exception thrown.";
    std::cerr << ex;
    std::cerr <<
      "itkPolyLineParametricPathTest2: Iterate path [FAILED]" << std::endl;
    passed = false;
    }

  if( ! passed )
    {
    std::cerr << "itkPolyLineParametricPathTest2 [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cout << "itkPolyLineParametricPathTest2 [PASSED]" << std::endl;
    return EXIT_SUCCESS;
    }
}
