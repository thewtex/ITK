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

#include "itkBYUMeshIOFactory.h"
#include "itkBYUMeshIO.h"
#include "itkConvertArrayPixelBuffer.h"
#include "itkConvertVariableLengthVectorPixelBuffer.h"
#include "itkFreeSurferAsciiMeshIOFactory.h"
#include "itkFreeSurferAsciiMeshIO.h"
#include "itkFreeSurferBinaryMeshIOFactory.h"
#include "itkFreeSurferBinaryMeshIO.h"
#include "itkGiftiMeshIOFactory.h"
#include "itkGiftiMeshIO.h"
#include "itkMeshConvertPixelTraits.h"
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"
#include "itkMeshIOBase.h"
#include "itkMeshIOFactory.h"
#include "itkOBJMeshIOFactory.h"
#include "itkOBJMeshIO.h"
#include "itkOFFMeshIOFactory.h"
#include "itkOFFMeshIO.h"
#include "itkVTKPolyDataMeshIOFactory.h"
#include "itkVTKPolyDataMeshIO.h"

int itkIOMeshHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
