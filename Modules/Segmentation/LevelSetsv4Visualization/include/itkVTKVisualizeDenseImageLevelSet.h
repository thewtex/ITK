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
#ifndef __itkVTKVisualizeDenseImageLevelSet_h
#define __itkVTKVisualizeDenseImageLevelSet_h

#include "itkVTKVisualizeImageLevelSet.h"

#include "itkImageToVTKImageFilter.h"
#include "itkLevelSetImageBaseTovtkImageData.h"

#include "vtkCornerAnnotation.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkMarchingSquares.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkScalarBarActor.h"
#include "vtkProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageShiftScale.h"

namespace itk
{

template< class TInputPixel, unsigned int VInputImageDimension, class TLevelSetImage >
class VTKVisualizeDenseImageLevelSet
{};

template< class TInputImage, class TLevelSetImage >
class VTKVisualizeDenseImageLevelSet< typename TInputImage::PixelType, 2, TLevelSetImage >
  : public VTKVisualizeImageLevelSet< TInputImage, ImageToVTKImageFilter< TInputImage > >
{
public:
  typedef VTKVisualizeDenseImageLevelSet                                                 Self;
  typedef VTKVisualizeImageLevelSet< TInputImage, ImageToVTKImageFilter< TInputImage > > Superclass;
  typedef SmartPointer< Self >                                                           Pointer;
  typedef SmartPointer< const Self >                                                     ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VTKVisualizeDenseImageLevelSet, VTKVisualizeImageLevelSet);

  typedef Superclass::InputImageType     InputImageType;

  typedef TLevelSetImage                 LevelSetImageType;

  typedef LevelSetDenseImageBase< LevelSetImageType > LevelSetType;

  typedef LevelSetImageBaseTovtkImageData< LevelSetImageType > LevelSetConverterType;
  typedef typename LevelSetConverterType::Pointer              LevelSetConverterPointer;

  void SetLevelSet( const LevelSetType * levelSet );

  void SetNumberOfLevels( const iLevel )
    {
    if( iLevel > 0 )
      {
      m_NumberOfLevels = iLevel;
      }
    }

  void SetLevelLimit( double iLimit )
    {
    if( iLimit > 0. )
      {
      m_LevelLimit = iLimit;
      }
    }

  void Update()
    {
    try
      {
      m_LevelSetConverter->Update();
      }
    catch( itk::ExceptionObject& e )
      {
      std::cout << e << std::endl;
      return;
      }
    if( m_Count % m_Period == 0 )
      {

    vtkSmartPointer< vtkMarchingSquares > contours =
      vtkSmartPointer< vtkMarchingSquares >::New();
    contours->SetInput( m_LevelSetConverter->GetOutput() );
    contours->GenerateValues( m_NumberOfLevels, - m_LevelLimit, m_LevelLimit );
    contours->Update();

    vtkSmartPointer< vtkPolyDataMapper > mapper =
        vtkSmartPointer< vtkPolyDataMapper >::New();
    mapper->SetInputConnection( contours->GetOutputPort() );
    mapper->SetScalarRange( - m_LevelLimit, m_LevelLimit );

    vtkSmartPointer< vtkActor > ContourActor =
        vtkSmartPointer< vtkActor >::New();
    ContourActor->SetMapper( mapper );
    ContourActor->GetProperty()->SetLineWidth( 2. );
    ContourActor->GetProperty()->SetColor( 1, 0, 0 );
    //ContourActor->GetProperty()->SetOpacity( 1.0 );

    vtkSmartPointer< vtkImageShiftScale > shift =
        vtkSmartPointer< vtkImageShiftScale >::New();
    shift->SetInput( m_ImageConverter->GetOutput() );
    shift->SetOutputScalarTypeToUnsignedChar();
    shift->Update();

    vtkSmartPointer< vtkImageActor > input_Actor =
        vtkSmartPointer< vtkImageActor >::New();
    input_Actor->SetInput( shift->GetOutput() );

    vtkSmartPointer< vtkScalarBarActor > scalarbar =
        vtkSmartPointer< vtkScalarBarActor >::New();
    scalarbar->SetLookupTable( mapper->GetLookupTable() );
    scalarbar->SetTitle( "Level Set Values" );
    scalarbar->SetNumberOfLabels( m_NumberOfLevels );

    m_Renderer->AddActor ( input_Actor );
    m_Renderer->AddActor ( ContourActor );
    m_Renderer->AddActor2D( scalarbar );

    std::stringstream counter;
    counter << m_Count;

    m_Annotation->SetText( 0, counter.str().c_str() );

    m_Renderer->AddActor2D( input_Actor );
    //      m_Ren->AddActor2D( scalarbar );

    m_RenWin->Render();

    if( m_ScreenCapture )
      {
      std::string filename;
      std::stringstream yo;
      yo << "snapshot_dense_" << std::setfill( '0' ) << std::setw( 5 ) << m_Count;
      filename = yo.str();
      filename.append ( ".png" );

      vtkCaptureScreen< vtkPNGWriter > capture ( m_RenWin );
      // begin mouse interaction
      capture( filename );
      }
    }
  ++m_Count;
  }

protected:
  vtkVisualize2DLevelSetImageBase() : Superclass(),
    {

    m_Renderer = vtkSmartPointer< vtkRenderer >::New();
    m_Renderer->SetBackground( 0.5, 0.5, 0.5 );

    m_Annotation = vtkSmartPointer< vtkCornerAnnotation >::New();
    m_Renderer->AddActor2D( m_Annotation );

    m_Iren = vtkSmartPointer< vtkRenderWindowInteractor >::New();

    m_RenWin = vtkSmartPointer< vtkRenderWindow >::New();

    m_RenWin->AddRenderer( m_Renderer );
    m_Iren->SetRenderWindow( m_RenWin );
    }

  virtual ~vtkVisualize2DLevelSetImageBase();

private:
  VTKVisualizeDenseImageLevelSet( const Self& ); // purposely not implemented
  void operator = ( const Self& ); // purposely not implemented

  ImageConverterPointer     m_ImageConverter;
  LevelSetConverterPointer  m_LevelSetConverter;

  vtkSmartPointer< vtkCornerAnnotation >        m_Annotation;
  vtkSmartPointer< vtkRenderer >                m_Renderer;
  vtkSmartPointer< vtkRenderWindow >            m_RenWin;
  vtkSmartPointer< vtkRenderWindowInteractor >  m_Iren;

  itk::IdentifierType m_Count;
  itk::IdentifierType m_Period;
  unsigned int        m_NumberOfLevels;
  double              m_LevelLimit;
  bool                m_ScreenCapture;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVTKVisualizeDenseImageLevelSet.hxx"
#endif

#endif
