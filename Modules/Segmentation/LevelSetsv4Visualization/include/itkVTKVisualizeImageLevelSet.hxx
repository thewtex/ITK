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
#ifndef __itkVTKVisualizeImageLevelSet_hxx
#define __itkVTKVisualizeImageLevelSet_hxx

namespace itk
{

template < class TInputImage, class TInputImageConverter >
VTKVisualizeImageLevelSet< TInputImage, TInputImageConverter >
VTKVisualizeImageLevelSet()
{
  this->m_InputImageConverter = InputImageConverterType::New();

  m_Renderer = vtkSmartPointer< vtkRenderer >::New();
  m_Renderer->SetBackground( 0.5, 0.5, 0.5 );
}

template < class TInputImage, class TInputImageConverter >
VTKVisualizeImageLevelSet< TInputImage, TInputImageConverter >
~VTKVisualizeImageLevelSet()
{
}

template < class TInputImage, class TInputImageConverter >
void
VTKVisualizeImageLevelSet< TInputImage, TInputImageConverter >
::SetInput( const InputImageType * inputImage )
{
  this->m_InputImageConverter->SetInput( inputImage );
  this->m_InputImageConverter->Update();
}

template < class TInputImage, class TInputImageConverter >
void
VTKVisualizeImageLevelSet< TInputImage, TInputImageConverter >
::SetScreenCapture( const bool iCapture )
{
  this->m_ScreenCapture = iCapture;
}

template < class TInputImage, class TInputImageConverter >
bool
VTKVisualizeImageLevelSet< TInputImage, TInputImageConverter >
::GetScreenCapture() const
{
  return this->m_ScreenCapture;
}

} // end namespace itk

#endif
