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

#ifndef __vtkVisualize2DLevelSetAsElevationMapBase_h
#define __vtkVisualize2DLevelSetAsElevationMapBase_h

#include "itkProcessObject.h"
#include "itkImageToVTKImageFilter.h"

#include "vnl/vnl_math.h"

#include "vtkCornerAnnotation.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkMarchingSquares.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkScalarBarActor.h"
#include "vtkProperty.h"
#include "vtkDoubleArray.h"
#include "vtkTextProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkImageShiftScale.h"
#include "vtkPointData.h"

#include "vtkCaptureScreen.h"
#include "vtkPNGWriter.h"


template< class TImage, class TLevelSet >
class vtkVisualize2DLevelSetAsElevationMapBase : public itk::ProcessObject
{
public:
  typedef vtkVisualize2DLevelSetAsElevationMapBase  Self;
  typedef itk::ProcessObject                        Superclass;
  typedef itk::SmartPointer< Self >                 Pointer;
  typedef itk::SmartPointer< const Self >           ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(vtkVisualize2DLevelSetAsElevationMapBase,
               ProcessObject);

  typedef TImage                            ImageType;
  typedef typename ImageType::Pointer       ImagePointer;
  typedef typename ImageType::SizeType      ImageSizeType;
  typedef typename ImageType::SizeValueType ImageSizeValueType;

  typedef TLevelSet                         LevelSetType;
  typedef typename LevelSetType::Pointer    LevelSetPointer;

  void SetInputImage( const ImageType * iImage )
    {
    m_ImageConverter->SetInput( iImage );
    try
      {
      m_ImageConverter->Update();
      }
    catch( itk::ExceptionObject& e )
      {
      std::cout << e << std::endl;
      return;
      }
    }

  void SetLevelSet( LevelSetType* f )
    {
    if( !f )
      {
      itkGenericExceptionMacro( << "Input Level Set is NULL" );
      }
    m_LevelSet = f;
    }

  void SetScreenCapture( const bool& iCapture )
    {
    m_ScreenCapture = iCapture;
    }

  void SetPeriod( const itk::IdentifierType& iPeriod )
    {
    m_Period = iPeriod;
    }

  void Update()
    {
    this->GenerateData();
    }

protected:
  vtkVisualize2DLevelSetAsElevationMapBase()
    {
    m_ColorValue = true;
    m_MinValue = itk::NumericTraits< double >::max( );
    m_MaxValue = itk::NumericTraits< double >::min( );
    m_Constant = 0.1;

    m_ImageConverter = ImageConverterType::New();

    m_Renderer = vtkSmartPointer< vtkRenderer >::New();
    m_Renderer->SetBackground( 0.5, 0.5, 0.5 );

    m_Annotation = vtkSmartPointer< vtkCornerAnnotation >::New();
    m_Renderer->AddActor2D( m_Annotation );

    m_Iren = vtkSmartPointer< vtkRenderWindowInteractor >::New();

    m_RenWin = vtkSmartPointer< vtkRenderWindow >::New();

    m_RenWin->AddRenderer( m_Renderer );
    m_Iren->SetRenderWindow( m_RenWin );
    }

  ~vtkVisualize2DLevelSetAsElevationMapBase() {}

  void GenerateData()
    {
    vtkSmartPointer< vtkImageShiftScale > shift =
        vtkSmartPointer< vtkImageShiftScale >::New();
    shift->SetInput( m_ImageConverter->GetOutput() );
    shift->SetOutputScalarTypeToUnsignedChar();
    shift->Update();

    vtkSmartPointer< vtkImageActor > input_Actor =
        vtkSmartPointer< vtkImageActor >::New();
    input_Actor->SetInput( shift->GetOutput() );

    this->GenerateElevationMap();

    vtkPolyDataMapper* meshmapper = vtkPolyDataMapper::New( );
    meshmapper->SetInput( m_Mesh );

    if( !m_ColorValue )
      {
      meshmapper->ScalarVisibilityOff( );
      }
    else
      {
      meshmapper->SetScalarRange( m_MinValue, m_MaxValue );
      }

    vtkActor *SurfaceActor = vtkActor::New( );
    SurfaceActor->SetMapper( meshmapper );
    SurfaceActor->GetProperty( )->SetColor( 0.7, 0.7, 0.7 );

    if( m_ColorValue )
      {
      vtkSmartPointer< vtkScalarBarActor > scalarBar =
          vtkSmartPointer< vtkScalarBarActor >::New( );
      scalarBar->SetLookupTable( meshmapper->GetLookupTable( )  );
      scalarBar->SetTitle( "Level Set" );
      scalarBar->GetPositionCoordinate( )->SetCoordinateSystemToNormalizedViewport( );
      scalarBar->GetPositionCoordinate( )->SetValue( 0.1, 0.01 );
      scalarBar->GetTitleTextProperty( )->SetColor( 0., 0., 0. );
      scalarBar->GetLabelTextProperty( )->SetColor( 0., 0., 0. );
      scalarBar->SetOrientationToHorizontal( );
      scalarBar->SetWidth( 0.8 );
      scalarBar->SetHeight( 0.17 );

      m_Renderer->AddActor2D( scalarBar );
      }

    m_Renderer->AddActor ( input_Actor );
    m_Renderer->AddActor ( SurfaceActor );

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

  ++m_Count;
  }

  void GenerateElevationMap()
    {
    ImagePointer image = m_ImageConverter->GetInput();

    typename ImageType::RegionType region = image->GetLargestPossibleRegion();

    typedef typename ImageType::IndexType      IndexType;
    typedef typename ImageType::IndexValueType IndexValueType;
    typedef typename ImageType::PointType      PointType;

    IndexType start = region.GetIndex();
    PointType itkPoint, itkPoint2;

    ImageSizeType   size =  region.GetSize();

    m_NumberOfSamples[0] = size[0] / 2;
    m_NumberOfSamples[1] = size[1] / 2;

    IndexType dx;
    dx[0] = static_cast< IndexValueType >( size[0] / m_NumberOfSamples[0] );
    dx[1] = static_cast< IndexValueType >( size[1] / m_NumberOfSamples[1] );

    vtkSmartPointer< vtkPoints > vtkpoints = vtkSmartPointer< vtkPoints >::New( );
    vtkSmartPointer< vtkDoubleArray > vtkpointdata = vtkSmartPointer< vtkDoubleArray >::New( );

    ImageSizeValueType k = 0;

    IndexType index;
    double p[3];

    for( ImageSizeValueType i = 0; i < m_NumberOfSamples[0]; i++ )
      {
      index[0] = start[0] + i * dx[0];

      for( ImageSizeValueType j = 0; j < m_NumberOfSamples[1]; j++ )
        {
        index[1] = start[1] + j * dx[1];

        image->TransformIndexToPhysicalPoint( index, &itkPoint );

        p[0] = itkPoint[0];
        p[1] = itkPoint[1];
        p[2] = static_cast< double >( this->m_LevelSet->Evaluate( index ) );

        vtkpointdata->InsertNextTuple1( p[2] );

        if( p[2] < m_MinValue )
          {
          m_MinValue = p[2];
          }
        if( p[2] > m_MaxValue )
          {
          m_MaxValue = p[2];
          }

        vtkpoints->InsertPoint( k++, p );
        }
      }

    double den = vnl_math_max( vnl_math_abs( m_MinValue ),
                               vnl_math_abs( m_MaxValue ) );

    image->TransformPhysicalPoint( start, &itkPoint );

    index = start;

    index[0] += size[0] - 1;
    index[1] += size[1] - 1;

    image->TransformPhysicalPoint( index, &itkPoint2 );

    double ratio = m_Constant * itkPoint.EuclideanDistanceTo( itkPoint2 );

    if( den != 0. )
      {
      ratio /= den;
      }

    for( vtkIdType i = 0; i < k; i++ )
      {
      vtkpoints->GetPoint( i, p );
      p[2] *= ratio;
      vtkpoints->SetPoint( i, p );
      }

    vtkSmartPointer< vtkCellArray > cells = vtkSmartPointer< vtkCellArray >::New( );

    m_Mesh->SetPoints( vtkpoints );
    m_Mesh->GetPointData( )->SetScalars( vtkpointdata );
    m_Mesh->SetPolys( cells );

    vtkIdType vtkId[3];

    for( ImageSizeValueType j = 0; j < ( m_NumberOfSamples[1] - 1 ); j++ )
      {
      for( ImageSizeValueType i = 0; i < ( m_NumberOfSamples[0] - 1 ); i++ )
        {
        vtkId[0] = j * m_NumberOfSamples[0] + i;
        vtkId[1] = vtkId[0] + 1;
        vtkId[2] = vtkId[1] + m_NumberOfSamples[0];

        m_Mesh->InsertNextCell( VTK_TRIANGLE, 3, static_cast< vtkIdType* >( vtkId ) );

        vtkId[1] = vtkId[2];
        vtkId[2] = vtkId[1] - 1;
        m_Mesh->InsertNextCell( VTK_TRIANGLE, 3, static_cast< vtkIdType* >( vtkId ) );
        }
      }
    }

private:
  typedef itk::ImageToVTKImageFilter< ImageType > ImageConverterType;
  typedef typename ImageConverterType::Pointer    ImageConverterPointer;

  ImageConverterPointer     m_ImageConverter;
  LevelSetPointer           m_LevelSet;

  vtkSmartPointer< vtkPolyData >                m_Mesh;
  vtkSmartPointer< vtkCornerAnnotation >        m_Annotation;
  vtkSmartPointer< vtkRenderer >                m_Renderer;
  vtkSmartPointer< vtkRenderWindow >            m_RenWin;
  vtkSmartPointer< vtkRenderWindowInteractor >  m_Iren;

  ImageSizeType m_NumberOfSamples;

  itk::IdentifierType m_Count;
  itk::IdentifierType m_Period;

  double m_Constant;
  double m_MinValue;
  double m_MaxValue;

  bool                m_ScreenCapture;
  bool                m_ColorValue;

};

#endif // VTKVISUALIZE2DLEVELSETASELEVATIONMAPBASE_H
