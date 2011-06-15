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
#ifndef __itkMetaContourConverter_txx
#define __itkMetaContourConverter_txx

#include "itkMetaContourConverter.h"

namespace itk
{

/** Constructor */
template< unsigned int NDimensions >
MetaContourConverter< NDimensions >
::MetaContourConverter()
{}

template< unsigned int NDimensions >
typename MetaContourConverter< NDimensions >::MetaObjectType *
MetaContourConverter< NDimensions>
::CreateMetaObject()
{
  return dynamic_cast<MetaObjectType *>(new ContourMetaObjectType);
}

/** Convert a metaContour into an Contour SpatialObject  */
template< unsigned int NDimensions >
typename MetaContourConverter< NDimensions >::SpatialObjectPointer
MetaContourConverter< NDimensions >
::MetaObjectToSpatialObject(const MetaObjectType *mo)
{
  const ContourMetaObjectType *Contour = dynamic_cast<const MetaContour *>(mo);

  ContourSpatialObjectPointer contour =
    ContourSpatialObjectType::New();

  double spacing[NDimensions];

  unsigned int ndims = Contour->NDims();
  for ( unsigned int i = 0; i < ndims; i++ )
    {
    spacing[i] = Contour->ElementSpacing()[i];
    }
  contour->GetIndexToObjectTransform()->SetScaleComponent(spacing);
  contour->GetProperty()->SetName( Contour->Name() );
  contour->SetId( Contour->ID() );
  contour->SetParentId( Contour->ParentID() );
  contour->GetProperty()->SetRed(Contour->Color()[0]);
  contour->GetProperty()->SetGreen(Contour->Color()[1]);
  contour->GetProperty()->SetBlue(Contour->Color()[2]);
  contour->GetProperty()->SetAlpha(Contour->Color()[3]);
  contour->SetClosed( const_cast<ContourMetaObjectType *>(Contour)->Closed() );
  contour->SetAttachedToSlice( const_cast<ContourMetaObjectType *>(Contour)->AttachedToSlice() );
  contour->SetDisplayOrientation( const_cast<ContourMetaObjectType *>(Contour)->DisplayOrientation() );

  // First the control points
  typedef typename ContourSpatialObjectType::ControlPointType ControlPointType;

  typename ContourMetaObjectType::ControlPointListType::const_iterator itCP =
    Contour->GetControlPoints().begin();

  for ( unsigned int identifier = 0; identifier < Contour->GetControlPoints().size(); identifier++ )
    {
    ControlPointType pnt;

    typedef typename ControlPointType::PointType PointType;
    PointType point;
    PointType pickedPoint;

    typedef typename ControlPointType::VectorType VectorType;
    VectorType normal;

    for ( unsigned int i = 0; i < ndims; i++ )
      {
      point[i] = ( *itCP )->m_X[i];
      }

    for ( unsigned int i = 0; i < ndims; i++ )
      {
      pickedPoint[i] = ( *itCP )->m_XPicked[i];
      }

    for ( unsigned int i = 0; i < ndims; i++ )
      {
      normal[i] = ( *itCP )->m_V[i];
      }

    pnt.SetID( ( *itCP )->m_Id );
    pnt.SetRed( ( *itCP )->m_Color[0] );
    pnt.SetGreen( ( *itCP )->m_Color[1] );
    pnt.SetBlue( ( *itCP )->m_Color[2] );
    pnt.SetAlpha( ( *itCP )->m_Color[3] );

    pnt.SetPosition(point);
    pnt.SetPickedPoint(pickedPoint);
    pnt.SetNormal(normal);

    contour->GetControlPoints().push_back(pnt);
    itCP++;
    }

  // Then the interpolated points
  typedef typename ContourSpatialObjectType::InterpolatedPointType  InterpolatedPointType;
  typename ContourMetaObjectType::InterpolatedPointListType::const_iterator
    itI = Contour->GetInterpolatedPoints().begin();

  for ( unsigned int identifier = 0; identifier < Contour->GetInterpolatedPoints().size(); identifier++ )
    {
    InterpolatedPointType pnt;

    typedef typename ControlPointType::PointType PointType;
    PointType point;

    typedef typename ControlPointType::VectorType VectorType;
    VectorType normal;

    for ( unsigned int i = 0; i < ndims; i++ )
      {
      point[i] = ( *itI )->m_X[i];
      }

    pnt.SetID( ( *itI )->m_Id );
    pnt.SetRed( ( *itI )->m_Color[0] );
    pnt.SetGreen( ( *itI )->m_Color[1] );
    pnt.SetBlue( ( *itI )->m_Color[2] );
    pnt.SetAlpha( ( *itI )->m_Color[3] );

    pnt.SetPosition(point);
    contour->GetInterpolatedPoints().push_back(pnt);
    itI++;
    }

  return contour.GetPointer();
}

/** Convert an Contour SpatialObject into a metaContour */
template< unsigned int NDimensions >
typename MetaContourConverter< NDimensions>::MetaObjectType *
MetaContourConverter< NDimensions >
::SpatialObjectToMetaObject(const SpatialObjectType *so)
{
  ContourSpatialObjectConstPointer contourSO =
    dynamic_cast<const ContourSpatialObjectType *>(so);
  MetaContour *Contour = new MetaContour(NDimensions);


  // fill in the control points information
  typename ContourSpatialObjectType::ControlPointListType::const_iterator itCP;

  for ( itCP = contourSO->GetControlPoints().begin();
        itCP != contourSO->GetControlPoints().end();
        itCP++ )
    {
    ContourControlPnt *pnt = new ContourControlPnt(NDimensions);

    pnt->m_Id = ( *itCP ).GetID();

    for ( unsigned int d = 0; d < NDimensions; d++ )
      {
      pnt->m_X[d] = ( *itCP ).GetPosition()[d];
      }

    for ( unsigned int d = 0; d < NDimensions; d++ )
      {
      pnt->m_XPicked[d] = ( *itCP ).GetPickedPoint()[d];
      }

    for ( unsigned int d = 0; d < NDimensions; d++ )
      {
      pnt->m_V[d] = ( *itCP ).GetNormal()[d];
      }

    pnt->m_Color[0] = ( *itCP ).GetRed();
    pnt->m_Color[1] = ( *itCP ).GetGreen();
    pnt->m_Color[2] = ( *itCP ).GetBlue();
    pnt->m_Color[3] = ( *itCP ).GetAlpha();

    Contour->GetControlPoints().push_back(pnt);
    }

  if ( NDimensions == 2 )
    {
    Contour->ControlPointDim("id x y xp yp v1 v2 r g b a");
    }
  else if ( NDimensions == 3 )
    {
    Contour->ControlPointDim("id x y z xp yp zp v1 v2 v3 r gn be a");
    }

  // fill in the interpolated points information
  typename ContourSpatialObjectType::InterpolatedPointListType::const_iterator itI;
  for ( itI = contourSO->GetInterpolatedPoints().begin();
        itI != contourSO->GetInterpolatedPoints().end();
        itI++ )
    {
    ContourInterpolatedPnt *pnt = new ContourInterpolatedPnt(NDimensions);

    pnt->m_Id = ( *itI ).GetID();
    for ( unsigned int d = 0; d < NDimensions; d++ )
      {
      pnt->m_X[d] = ( *itI ).GetPosition()[d];
      }

    pnt->m_Color[0] = ( *itI ).GetRed();
    pnt->m_Color[1] = ( *itI ).GetGreen();
    pnt->m_Color[2] = ( *itI ).GetBlue();
    pnt->m_Color[3] = ( *itI ).GetAlpha();

    Contour->GetInterpolatedPoints().push_back(pnt);
    }

  if ( NDimensions == 2 )
    {
    Contour->InterpolatedPointDim("id x y r g b a");
    }
  else if ( NDimensions == 3 )
    {
    Contour->InterpolatedPointDim("id x y z r g b a");
    }

  // Set the interpolation type
  switch ( contourSO->GetInterpolationType() )
    {
    case ContourSpatialObjectType::EXPLICIT_INTERPOLATION:
      Contour->Interpolation(MET_EXPLICIT_INTERPOLATION);
      break;
    case ContourSpatialObjectType::LINEAR_INTERPOLATION:
      Contour->Interpolation(MET_LINEAR_INTERPOLATION);
      break;
    case ContourSpatialObjectType::BEZIER_INTERPOLATION:
      Contour->Interpolation(MET_BEZIER_INTERPOLATION);
      break;
    default:
      Contour->Interpolation(MET_NO_INTERPOLATION);
    }

  float color[4];
  for ( unsigned int i = 0; i < 4; i++ )
    {
    color[i] = contourSO->GetProperty()->GetColor()[i];
    }
  Contour->Color(color);
  Contour->ID( contourSO->GetId() );
  Contour->Closed( contourSO->GetClosed() );
  Contour->AttachedToSlice( contourSO->GetAttachedToSlice() );
  Contour->DisplayOrientation( contourSO->GetDisplayOrientation() );

  if ( contourSO->GetParent() )
    {
    Contour->ParentID( contourSO->GetParent()->GetId() );
    }

  for ( unsigned int i = 0; i < NDimensions; i++ )
    {
    Contour->ElementSpacing(i, contourSO->GetIndexToObjectTransform()
                            ->GetScaleComponent()[i]);
    }
  return Contour;
}

} // end namespace itk

#endif
