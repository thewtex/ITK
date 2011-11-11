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
#ifndef __itkVTKVisualizeDenseImageLevelSet_hxx
#define __itkVTKVisualizeDenseImageLevelSet_hxx

#include "itkVTKVisualizeDenseImageLevelSet.h"

namespace itk
{

template< class TInputImage, class TLevelSetImage >
VTKVisualizeDenseImageLevelSet< typename TInputImage::PixelType, 2, TLevelSetImage >
VTKVisualizeDenseImageLevelSet()
  m_Count( 0 ),
  m_Period( 20 ),
  m_NumberOfLevels( 1 ),
  m_LevelLimit( 0 ),
  m_ScreenCapture( false )
{
  m_LevelSetConverter = LevelSetConverterType::New();
}

template< class TInputImage, class TLevelSetImage >
VTKVisualizeDenseImageLevelSet< typename TInputImage::PixelType, 2, TLevelSetImage >
~VTKVisualizeDenseImageLevelSet()
{
}

template< class TInputImage, class TLevelSetImage >
void
VTKVisualizeDenseImageLevelSet< typename TInputImage::PixelType, 2, TLevelSetImage >
::SetLevelSet( const LevelSetType * levelSet )
{
  this->m_LevelSetConverter->SetInput( levelSet );
}

} // end namespace itk

#endif
