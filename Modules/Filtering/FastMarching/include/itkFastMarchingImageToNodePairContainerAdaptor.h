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

#ifndef __itkFastMarchingImageToNodePairContainerAdaptor_h
#define __itkFastMarchingImageToNodePairContainerAdaptor_h

#include "itkObject.h"
#include "itkFastMarchingTraits.h"

namespace itk
{
/**
 * \class FastMarchingImageToNodePairContainerAdaptor
 * \brief
*/
template< class TInput, class TOutput, class TImage >
class FastMarchingImageToNodePairContainerAdaptor :
    public Object
  {
public:
  typedef FastMarchingImageToNodePairContainerAdaptor          Self;
  typedef LightObject                           Superclass;
  typedef SmartPointer< Self >                 Pointer;
  typedef SmartPointer< const Self >           ConstPointer;


  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(FastMarchingImageToNodePairContainerAdaptor, LightObject);

  typedef FastMarchingTraits< TInput, TOutput >     Traits;
  typedef typename Traits::NodePairType             NodePairType;
  typedef typename Traits::NodePairContainerType    NodePairContainerType;
  typedef typename Traits::NodePairContainerPointer NodePairContainerPointer;
  typedef typename Traits::LabelType                LabelType;
  typedef typename Traits::OutputPixelType          OutputPixelType;

  typedef TImage                            ImageType;
  typedef typename ImageType::Pointer       ImagePointer;
  typedef typename ImageType::ConstPointer  ImageConstPointer;
  typedef typename ImageType::PixelType     ImagePixelType;

  itkStaticConstMacro( ImageDimension, unsigned int,
                       Traits::ImageDimension );


  void SetAliveImage( const ImageType* iImage );

  void SetTrialImage( const ImageType* iImage );

  void SetForbiddenImage( const ImageType* iImage );

  itkSetMacro( IsForbiddenImageBinaryMask, bool );
  itkBooleanMacro( IsForbiddenImageBinaryMask );

  NodePairContainerType* GetAlivePoints();

  NodePairContainerType* GetTrialPoints();

  NodePairContainerType* GetForbiddenPoints();

  itkSetMacro( AliveValue, OutputPixelType );
  itkSetMacro( TrialValue, OutputPixelType );

  void Update();


protected:

  /** \brief Constructor */
  FastMarchingImageToNodePairContainerAdaptor();

  /** \brief Destructor */
  virtual ~FastMarchingImageToNodePairContainerAdaptor() {}

  ImageConstPointer m_AliveImage;
  ImageConstPointer m_TrialImage;
  ImageConstPointer m_ForbiddenImage;

  NodePairContainerPointer m_AlivePoints;
  NodePairContainerPointer m_TrialPoints;
  NodePairContainerPointer m_ForbiddenPoints;

  OutputPixelType m_AliveValue;
  OutputPixelType m_TrialValue;

  bool m_IsForbiddenImageBinaryMask;

  virtual void GenerateData();

  /** */
  void
  SetPointsFromImage( const ImageType* image, const LabelType& iLabel,
    const OutputPixelType& iValue );


private:

  FastMarchingImageToNodePairContainerAdaptor( const Self& );
  void operator = ( const Self& );

  };
}

#include "itkFastMarchingImageToNodePairContainerAdaptor.txx"
#endif // __itkFastMarchingImageToNodePairContainerAdaptor_h
