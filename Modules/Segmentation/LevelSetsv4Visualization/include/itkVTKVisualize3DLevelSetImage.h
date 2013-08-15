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

#ifndef __itkVTKVisualize3DLevelSetImage_h
#define __itkVTKVisualize3DLevelSetImage_h

#include "itkImage.h"

#include "itkLevelSetImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkLevelSetTovtkImageData.h"

#include "vtkVersion.h"

#include "vtkImageData.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkImageMapper3D.h"
#include "vtkImageActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkImageViewer2.h"
#include "vtkImageShiftScale.h"

#include "vtkCaptureScreen.h"
#include "vtkPNGWriter.h"

#include "itkVTKVisualizeImageLevelSet.h"

namespace itk
{
/**
 * \class VTKVisualize3DLevelSetImage
 * \brief visualization for 3D Dense Level Set
 *
 * \tparam TInputImage Input Image Type
 * \tparam TLevelSetImage Level Set Image Type
 *
 * \ingroup ITKLevelSetsv4Visualization
 */
template< class TInputImage, class TLevelSet >
class VTKVisualize3DLevelSetImage : public VTKVisualizeImageLevelSet< TInputImage, ImageToVTKImageFilter< TInputImage > >
{
public:
  typedef ImageToVTKImageFilter< TInputImage >  ImageConverterType;
  typedef typename ImageConverterType::Pointer  ImageConverterPointer;


  typedef VTKVisualize3DLevelSetImage Self;
  typedef VTKVisualizeImageLevelSet< TInputImage, ImageConverterType > Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VTKVisualize3DLevelSetImage, VTKVisualizeImageLevelSet );

  typedef TInputImage     InputImageType;
  typedef TLevelSet       LevelSetType;

  void SetInputImage( const InputImageType * iImage )
    {
    this->m_InputImageConverter->SetInput( iImage );
    this->m_InputImageConverter->Update();

    vtkSmartPointer< vtkImageShiftScale > ImageShiftScale = vtkSmartPointer< vtkImageShiftScale >::New();
    ImageShiftScale->SetOutputScalarTypeToUnsignedChar();
  #if VTK_MAJOR_VERSION <= 5
    ImageShiftScale->SetInput( this->m_InputImageConverter->GetOutput() );
  #else
    this->m_InputImageConverter->Update();
    ImageShiftScale->SetInputData( this->m_InputImageConverter->GetOutput() );
  #endif
    ImageShiftScale->Update();

    int dimensions[3];
    this->m_InputImageConverter->GetOutput()->GetDimensions( dimensions );

    vtkSmartPointer< vtkImageActor > imageActor = vtkSmartPointer< vtkImageActor >::New();
    imageActor->InterpolateOff();
    imageActor->GetMapper()->SetInputConnection( ImageShiftScale->GetOutputPort() );
    imageActor->SetDisplayExtent( 0, dimensions[0], 0, dimensions[1], dimensions[2] / 2, dimensions[2] / 2 );

    this->m_Renderer->AddActor( imageActor );
    }

  void SetLevelSet( LevelSetType *f )
    {
    m_LevelSetConverter->SetInput( f );
    }

  /*
   *void Update()
    {


    m_ImageConverter->Update();

    vtkSmartPointer< vtkImageData > image = m_ImageConverter->GetOutput();

    int dimensions[3];
    image->GetDimensions( dimensions );

    vtkSmartPointer< vtkImageShiftScale > shift =
        vtkSmartPointer< vtkImageShiftScale >::New();
#if VTK_MAJOR_VERSION <= 5
    shift->SetInput( image );
#else
    shift->SetInputData( image );
#endif
    shift->SetOutputScalarTypeToUnsignedChar();
    shift->Update();

    vtkSmartPointer< vtkImageActor > imageActor =
        vtkSmartPointer< vtkImageActor >::New();

#if VTK_MAJOR_VERSION <= 5
    imageActor->SetInput( shift->GetOutput() );
#else
    imageActor->GetMapper()->SetInputConnection( shift->GetOutputPort() );
#endif
    imageActor->SetDisplayExtent( 0, dimensions[0], 0, dimensions[1], dimensions[2] / 2, dimensions[2] / 2 );

    vtkSmartPointer< vtkRenderer > ren =
        vtkSmartPointer< vtkRenderer >::New();
    ren->SetBackground( 0.5, 0.5, 0.5 );

    vtkSmartPointer< vtkRenderWindowInteractor > iren =
        vtkSmartPointer< vtkRenderWindowInteractor >::New();

    vtkSmartPointer< vtkRenderWindow > renWin =
        vtkSmartPointer< vtkRenderWindow >::New();

    ren->AddActor( imageActor );
    ren->AddActor( ContourActor );

    renWin->AddRenderer( ren );

    if( m_ScreenCapture )
      {
      iren->SetRenderWindow( renWin );

      renWin->Render();

      std::string filename;
      std::stringstream yo;
      yo << "snapshot_" << m_Count;
      filename = yo.str();
      filename.append ( ".png" );

      vtkCaptureScreen< vtkPNGWriter > capture ( renWin );
      // begin mouse interaction
      iren->Start();
      capture( filename );
      ++m_Count;
      }
    else
      {
      renWin->Render();
      }
    }*/

protected:
  VTKVisualize3DLevelSetImage() : Superclass()
    {
    m_LevelSetConverter = LevelSetConverterType::New();

    this->m_MeshMapper = vtkSmartPointer< vtkPolyDataMapper >::New();
    this->m_MeshMapper->ScalarVisibilityOff();

    this->m_MeshActor = vtkSmartPointer< vtkActor >::New();
    this->m_MeshActor->SetMapper( this->m_MeshMapper );
    this->m_MeshActor->GetProperty( )->SetLineWidth( 2. );
    this->m_MeshActor->GetProperty()->SetColor( 0, 0, 1 );
    this->m_MeshActor->GetProperty()->SetOpacity( 1.0 );

    this->m_Renderer->AddActor( this->m_MeshActor );
    }

  ~VTKVisualize3DLevelSetImage()
    {}

  virtual void PrepareVTKPipeline()
  {
    try
      {
      m_LevelSetConverter->Update();
      }
    catch( ExceptionObject& e )
      {
      std::cout << e << std::endl;
      return;
      }

    const int LevelSet_id = 0;
    vtkSmartPointer< vtkMarchingCubes > contours =
      vtkSmartPointer< vtkMarchingCubes >::New();
#if VTK_MAJOR_VERSION <= 5
    contours->SetInput( m_LevelSetConverter->GetOutput() );
#else
    contours->SetInputData( m_LevelSetConverter->GetOutput() );
#endif
    contours->SetValue( LevelSet_id, 0. );
    contours->ComputeNormalsOff();

    m_MeshMapper->SetInputConnection( contours->GetOutputPort() );
  }

private:
  VTKVisualize3DLevelSetImage ( const Self& );
  void operator = ( const Self& );

  typedef LevelSetTovtkImageData< LevelSetType >    LevelSetConverterType;
  typedef typename LevelSetConverterType::Pointer   LevelSetConverterPointer;

  LevelSetConverterPointer              m_LevelSetConverter;
  vtkSmartPointer< vtkPolyDataMapper >  m_MeshMapper;
  vtkSmartPointer< vtkActor >           m_MeshActor;
  };
}
#endif
