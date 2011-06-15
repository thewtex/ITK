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
#ifndef __itkMetaSceneConverter_txx
#define __itkMetaSceneConverter_txx

#include "itkMetaSceneConverter.h"
#include "itkMetaEllipseConverter.h"
#include "itkMetaTubeConverter.h"
#include "itkMetaDTITubeConverter.h"
#include "itkMetaVesselTubeConverter.h"
#include "itkMetaGroupConverter.h"
#include "itkMetaImageConverter.h"
#include "itkMetaImageMaskConverter.h"
#include "itkMetaBlobConverter.h"
#include "itkMetaGaussianConverter.h"
#include "itkMetaMeshConverter.h"
#include "itkMetaLandmarkConverter.h"
#include "itkMetaLineConverter.h"
#include "itkMetaSurfaceConverter.h"
#include "itkMetaLandmarkConverter.h"
#include "itkMetaArrowConverter.h"
#include "itkMetaContourConverter.h"

#include <algorithm>

namespace itk
{
/** Constructor */
template< unsigned int NDimensions, typename PixelType, typename TMeshTraits >
MetaSceneConverter< NDimensions, PixelType, TMeshTraits >
::MetaSceneConverter()
{
  // default behaviour of scene converter is not to save transform
  // with each spatial object.
  m_Event = NULL;
  m_BinaryPoints = false;
  m_TransformPrecision = 6;
  m_WriteImagesInSeparateFile = false;
}

/** Destructor */
template< unsigned int NDimensions, typename PixelType, typename TMeshTraits >
MetaSceneConverter< NDimensions, PixelType, TMeshTraits >
::~MetaSceneConverter()
{}

template< unsigned int NDimensions, typename PixelType, typename TMeshTraits >
void
MetaSceneConverter< NDimensions, PixelType, TMeshTraits >
::SetTransform(MetaObject *obj, TransformType *transform)
{
  typename SpatialObjectType::TransformType::InputPointType center =
    transform->GetCenter();
  typename SpatialObjectType::TransformType::MatrixType matrix =
    transform->GetMatrix();
  typename SpatialObjectType::TransformType::OffsetType offset =
    transform->GetOffset();

  unsigned int p = 0;
  for ( unsigned int row = 0; row < NDimensions; row++ )
    {
    for ( unsigned int col = 0; col < NDimensions; col++ )
      {
      m_Orientation[p++] = matrix[row][col];
      }
    }

  for ( unsigned int i = 0; i < NDimensions; i++ )
    {
    m_Position[i] = offset[i];
    m_CenterOfRotation[i] = center[i];
    }

  obj->CenterOfRotation(m_CenterOfRotation);
  obj->TransformMatrix(m_Orientation);
  obj->Offset(m_Position);
  obj->SetDoublePrecision(m_TransformPrecision);
}

template< unsigned int NDimensions, typename PixelType, typename TMeshTraits >
void
MetaSceneConverter< NDimensions, PixelType, TMeshTraits >
::SetTransform(SpatialObjectType *so, MetaObject *meta)
{
  typename SpatialObjectType::TransformType::Pointer transform =
    SpatialObjectType::TransformType::New();

  typename SpatialObjectType::TransformType::InputPointType center;
  typename SpatialObjectType::TransformType::MatrixType matrix;
  typename SpatialObjectType::TransformType::OffsetType offset;

  unsigned int p = 0;
  for ( unsigned int row = 0; row < NDimensions; row++ )
    {
    for ( unsigned int col = 0; col < NDimensions; col++ )
      {
      matrix[row][col] = ( meta->Orientation() )[p++];
      }
    }

  for ( unsigned int i = 0; i < NDimensions; i++ )
    {
    offset[i] = ( meta->Position() )[i];
    center[i] = ( meta->CenterOfRotation() )[i];
    }

  so->GetObjectToParentTransform()->SetCenter(center);
  so->GetObjectToParentTransform()->SetMatrix(matrix);
  so->GetObjectToParentTransform()->SetOffset(offset);
}

/** Convert a metaScene into a Composite Spatial Object
 *  Also Managed Composite Spatial Object to keep a hierarchy */
template< unsigned int NDimensions, typename PixelType, typename TMeshTraits >
typename MetaSceneConverter< NDimensions, PixelType, TMeshTraits >::ScenePointer
MetaSceneConverter< NDimensions, PixelType, TMeshTraits >
::CreateSpatialObjectScene(MetaScene *mScene)
{
  ScenePointer soScene = SceneType::New();

  MetaScene::ObjectListType *         list = mScene->GetObjectList();
  MetaScene::ObjectListType::iterator it = list->begin();
  MetaScene::ObjectListType::iterator itEnd = list->end();

  SpatialObjectPointer currentSO;

  while ( it != itEnd )
    {
    /** New object goes here */
    if ( !strcmp( ( *it )->ObjectTypeName(), "Tube" ) )
      {
      // If there is the subtype is a vessel
      if ( !strcmp( ( *it )->ObjectSubTypeName(), "Vessel" ) )
        {
        currentSO = this->MetaObjectToSpatialObject<MetaVesselTubeConverter< NDimensions > > ( *it );
        }
      else if ( !strcmp( ( *it )->ObjectSubTypeName(), "DTI" ) )
        {
        currentSO = this->MetaObjectToSpatialObject< MetaDTITubeConverter<NDimensions> > ( *it );
        }
      else
        {
        currentSO = this->MetaObjectToSpatialObject< MetaTubeConverter<NDimensions> > ( *it );
        }
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Group" )
         || !strcmp( ( *it )->ObjectTypeName(), "AffineTransform" ) )
      {
      currentSO = this->MetaObjectToSpatialObject< MetaGroupConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Ellipse" ) )
      {
      currentSO = this->MetaObjectToSpatialObject< MetaEllipseConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Arrow" ) )
      {
      currentSO = this->MetaObjectToSpatialObject< MetaArrowConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Image" ) )
      {
      // If there is the subtype is a mask
      if ( !strcmp( ( *it )->ObjectSubTypeName(), "Mask" ) )
        {
        currentSO = this->MetaObjectToSpatialObject< MetaImageMaskConverter< NDimensions > > ( *it );
        }
      else
        {
        currentSO = this->MetaObjectToSpatialObject< MetaImageConverter<NDimensions,PixelType> > ( *it );
        }
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Blob" ) )
      {
      currentSO = this->MetaObjectToSpatialObject< MetaBlobConverter<NDimensions> > ( *it );
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Gaussian" ) )
      {
      currentSO = this->MetaObjectToSpatialObject< MetaGaussianConverter<NDimensions> > ( *it );
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Landmark" ) )
      {
      currentSO = this->MetaObjectToSpatialObject< MetaLandmarkConverter<NDimensions> > ( *it );
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Surface" ) )
      {
      currentSO = this->MetaObjectToSpatialObject< MetaSurfaceConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Line" ) )
      {
      currentSO = this->MetaObjectToSpatialObject< MetaLineConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Mesh" ) )
      {
      typedef itk::Mesh< PixelType, NDimensions, TMeshTraits > MeshType;
      currentSO = this->MetaObjectToSpatialObject< MetaMeshConverter< NDimensions, PixelType, TMeshTraits > > ( *it );
      }
    else if ( !strcmp( ( *it )->ObjectTypeName(), "Contour" ) )
      {
      currentSO = this->MetaObjectToSpatialObject< MetaContourConverter< NDimensions > > ( *it );
      }
    else
      {
      std::string metaObjectType( ( *it )->ObjectTypeName() );
      typename ConverterMapType::iterator converterIt = this->m_ConverterMap.find(metaObjectType);

      if(converterIt == this->m_ConverterMap.end())
        {
        itkGenericExceptionMacro(<< "Unable to find MetaObject -> SpatialObject converter for "
                          << metaObjectType);
        }
      currentSO = converterIt->second->MetaObjectToSpatialObject( *it );
      }
    this->SetTransform(currentSO, *it);
    soScene->AddSpatialObject(currentSO);
    it++;
    }

  soScene->FixHierarchy();

  return soScene;
}

/** Read a meta file give the type */
template< unsigned int NDimensions, typename PixelType, typename TMeshTraits >
typename MetaSceneConverter< NDimensions, PixelType, TMeshTraits >::ScenePointer
MetaSceneConverter< NDimensions, PixelType, TMeshTraits >
::ReadMeta(const char *name)
{
  MetaScene *mScene = new MetaScene;

  if ( m_Event )
    {
    mScene->SetEvent(m_Event);
    }
  mScene->Read(name);
  ScenePointer soScene = CreateSpatialObjectScene(mScene);
  delete mScene;
  return soScene;
}

/** Write a meta file give the type */
template< unsigned int NDimensions, typename PixelType, typename TMeshTraits >
MetaScene *
MetaSceneConverter< NDimensions, PixelType, TMeshTraits >
::CreateMetaScene(SceneType *scene, unsigned int depth, char *name)
{
  MetaScene *metaScene = new MetaScene(NDimensions);

  metaScene->BinaryData(m_BinaryPoints);

  float *spacing = new float[NDimensions];
  for ( unsigned int i = 0; i < NDimensions; i++ )
    {
    spacing[i] = 1;
    }
  metaScene->ElementSpacing(spacing);
  delete[] spacing;

  typedef typename SceneType::ObjectListType ListType;

  ListType *childrenList = scene->GetObjects(depth, name);
  typename ListType::iterator it = childrenList->begin();
  typename ListType::iterator itEnd = childrenList->end();

  MetaObject *currentMeta;

  while ( it != itEnd )
    {
    if ( !strcmp( ( *it )->GetTypeName(), "GroupSpatialObject" )
         || !strcmp( ( *it )->GetTypeName(), "AffineTransformSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaGroupConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "TubeSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaTubeConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "VesselTubeSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaVesselTubeConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "DTITubeSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaDTITubeConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "EllipseSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaEllipseConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "ArrowSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaArrowConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "ImageSpatialObject" ) ||
              !strcmp( ( *it )->GetTypeName(), "ImageMaskSpatialObject" ) )
      {
      if ( !strcmp( ( *it )->GetTypeName(), "ImageMaskSpatialObject" ) )
        {
        currentMeta = this->SpatialObjectToMetaObject< MetaImageMaskConverter< NDimensions > > ( *it );
        currentMeta->ObjectSubTypeName("Mask");
        }
      else
        {
        currentMeta = this->SpatialObjectToMetaObject< MetaImageConverter< NDimensions, PixelType > > ( *it );
        currentMeta->ObjectSubTypeName("Image");
        }
      MetaImage *image = dynamic_cast<MetaImage *>(currentMeta);
      if ( m_WriteImagesInSeparateFile )
        {
        if ( ( *it )->GetProperty()->GetName().size() == 0 )
          {
          std::cout << "Error: you should set the image name when using"
                    << " WriteImagesInSeparateFile." << std::endl;
          std::cout << "The image will be written locally." << std::endl;
          image->ElementDataFileName("LOCAL");
          }
        else
          {
          std::string filename = ( *it )->GetProperty()->GetName();
          filename += ".raw";
          image->ElementDataFileName( filename.c_str() );
          }
        }
      else
        {
        image->ElementDataFileName("LOCAL");
        }
      image->BinaryData(true);
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "BlobSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaBlobConverter<NDimensions> > ( *it );
      currentMeta->BinaryData(true);
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "GaussianSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaGaussianConverter<NDimensions> > ( *it );
      currentMeta->BinaryData(true);
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "LandmarkSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaLandmarkConverter<NDimensions> > ( *it );
      currentMeta->BinaryData(true);
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "ContourSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaContourConverter<NDimensions> > ( *it );
      currentMeta->BinaryData(true);
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "SurfaceSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaSurfaceConverter< NDimensions > > ( *it );
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "LineSpatialObject" ) )
      {
      currentMeta = this->SpatialObjectToMetaObject< MetaLineConverter<NDimensions> > ( *it );
      currentMeta->BinaryData(true);
      }
    else if ( !strcmp( ( *it )->GetTypeName(), "MeshSpatialObject" ) )
      {
      typedef itk::Mesh< PixelType, NDimensions, TMeshTraits > MeshType;
      currentMeta = this->SpatialObjectToMetaObject< MetaMeshConverter< NDimensions, PixelType, TMeshTraits > > ( *it );
      }
    else
      {
      std::string spatialObjectType( ( *it )->GetTypeName() );

      typename ConverterMapType::iterator converterIt = this->m_ConverterMap.find(spatialObjectType);
      if(converterIt == this->m_ConverterMap.end())
        {
        itkGenericExceptionMacro(<< "Unable to find MetaObject -> SpatialObject converter for "
                          << spatialObjectType);
        }
      currentMeta = converterIt->second->SpatialObjectToMetaObject( *it );
      }
    if ( ( *it )->GetParent() )
      {
      currentMeta->ParentID( ( *it )->GetParent()->GetId() );
      }
    currentMeta->Name( ( *it )->GetProperty()->GetName().c_str() );
    this->SetTransform( currentMeta, ( *it )->GetObjectToParentTransform() );
    metaScene->AddObject(currentMeta);
    it++;
    }

  delete childrenList;

  return metaScene;
}

/** Write a meta file give the type */
template< unsigned int NDimensions, typename PixelType, typename TMeshTraits >
bool
MetaSceneConverter< NDimensions, PixelType, TMeshTraits >
::WriteMeta(SceneType *scene, const char *fileName,
            unsigned int depth, char *soName)
{
  MetaScene *metaScene = this->CreateMetaScene(scene, depth, soName);

  metaScene->Write(fileName);

  delete metaScene;

  return true;
}

template< unsigned int NDimensions, typename PixelType, typename TMeshTraits >
void
MetaSceneConverter< NDimensions, PixelType, TMeshTraits >
::RegisterMetaConverter(const char *metaTypeName,
                      const char *spatialObjectName,
                      MetaConverterBaseType *converter)
{
  std::string metaType(metaTypeName);
  std::string spatialObjectType(spatialObjectName);
  this->m_ConverterMap[metaType] = converter;
  this->m_ConverterMap[spatialObjectType] = converter;
}

} // end namespace itk

#endif
