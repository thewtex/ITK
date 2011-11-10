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
#ifndef __itkVTKVisualizeImageLevelSet_h
#define __itkVTKVisualizeImageLevelSet_h

#include "itkLightObject.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCaptureScreen.h"
#include "vtkPNGWriter.h"

namespace itk
{

/** \class VTKVisualizeImageLevelSet
 * \brief Base class for visualizing level sets defined on an image with VTK.
 *
 * \tparam TInputImage Input image the level set evolves on.
 * \tparam TInputImageConverter ITK filter to convert the input image to a VTK
 * object.
 *
 * \ingroup ITKLevelSetsv4Visualization
 */
template< class TInputImage, class TInputImageConverter >
class VTKVisualizeImageLevelSet: public LightObject
{
public:
  typedef VTKVisualizeImageLevelSet  Self;
  typedef LightObject                Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( VTKVisualizeImageLevelSet, LightObject );

  /** Type of the input image the level set evolves on. */
  typedef TInputImage     InputImageType;

  /** Type of the ITK filter to convert the input image to a VTK object. */
  typedef TInputImageConverter InputImageConverterType;

  virtual void SetInput( const InputImageType * inputImage );

  /** Do a screen capture at every iteration. */
  void SetScreenCapture( const bool iCapture );
  bool GetScreenCapture() const;

  /** Set up the VTK pipeline and render. */
  virtual void Update() = 0;

protected:
  VTKVisualizeImageLevelSet();
  virtual ~VTKVisualizeImageLevelSet();

  typename InputImageConverterType::Pointer m_InputImageConverter;

private:
  VTKVisualizeImageLevelSet( const Self & ); // purposely not implemented
  void operator=( const Self & ); // purposely not implemented

  bool           m_ScreenCapture;

  vtkSmarPointer< vtkRenderer >  m_Renderer;
    m_Renderer->SetBackground( 0.5, 0.5, 0.5 );
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVTKVisualizeImageLevelSet.hxx"
#endif
