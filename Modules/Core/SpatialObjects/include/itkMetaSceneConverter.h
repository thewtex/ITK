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
#ifndef __itkMetaSceneConverter_h
#define __itkMetaSceneConverter_h

#include "metaScene.h"
#include "itkMetaEvent.h"
#include "itkSceneSpatialObject.h"
#include "itkDefaultStaticMeshTraits.h"

namespace itk
{
/** \class MetaSceneConverter
 * \brief This is the MetaSceneConverter class.
 *
 * \ingroup ITK-SpatialObjects
 */
template< unsigned int NDimensions,
          typename PixelType = unsigned char,
          typename TMeshTraits =
            DefaultStaticMeshTraits< PixelType, NDimensions, NDimensions >
          >
class ITK_EXPORT MetaSceneConverter
{
public:

  MetaSceneConverter();
  ~MetaSceneConverter();

  itkStaticConstMacro(MaximumDepth, unsigned int, 9999999);

  typedef itk::SceneSpatialObject< NDimensions > SceneType;
  typedef typename  SceneType::Pointer           ScenePointer;

  ScenePointer ReadMeta(const char *name);

  bool WriteMeta(SceneType *scene, const char *fileName,
                 unsigned int depth = MaximumDepth,
                 char *spatialObjectTypeName = NULL);

  const MetaEvent * GetEvent() const { return m_Event; }
  void  SetEvent(MetaEvent *event) { m_Event = event; }

  /** Set if the points should be saved in binary/ASCII */
  void SetBinaryPoints(bool binary) { m_BinaryPoints = binary; }

  void SetTransformPrecision(unsigned int precision)
  {
    m_TransformPrecision = precision;
  }

  unsigned int GetTransformPrecision(){ return m_TransformPrecision; }

  /** Set if the images should be written in different files */
  void SetWriteImagesInSeparateFile(bool separate)
  {
    m_WriteImagesInSeparateFile = separate;
  }

private:

  typedef itk::SpatialObject< NDimensions >         SpatialObjectType;
  typedef typename SpatialObjectType::Pointer       SpatialObjectPointer;
  typedef typename SpatialObjectType::TransformType TransformType;

  typedef std::list< MetaObject * > MetaObjectListType;

  MetaScene * CreateMetaScene(SceneType *scene,
                              unsigned int depth = MaximumDepth,
                              char *name = NULL);

  ScenePointer CreateSpatialObjectScene(MetaScene *scene);

  void SetTransform(MetaObject *obj, TransformType *transform);

  void SetTransform(SpatialObjectType *so, MetaObject *obj);

  double m_Orientation[100];
  double m_Position[10];
  double m_CenterOfRotation[10];

  MetaEvent *  m_Event;
  bool         m_BinaryPoints;
  bool         m_WriteImagesInSeparateFile;
  unsigned int m_TransformPrecision;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMetaSceneConverter.txx"
#endif

#endif
