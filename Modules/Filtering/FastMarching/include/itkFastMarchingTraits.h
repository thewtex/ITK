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

#ifndef __itkFastMarchingTraits_h
#define __itkFastMarchingTraits_h

#include "itkIntTypes.h"

#include "itkConceptChecking.h"

#include "itkImage.h"
#include "itkImageToImageFilter.h"

#include "itkQuadEdgeMesh.h"
#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"

namespace itk
{
/**  \class FastMarchingTraits
  \brief Base class traits to be used by the FastMarchingBase

  \tparam TInputDomain Input Domain type (e.g. itk::Image or itk::QuadEdgeMesh)
  \tparam TNode Node type where the front propagates
  \tparam TOutputDomain Output Domain type (similar to TInputDomain)
  \tparam TSuperclass Superclass. When dealing with itk::Image, the Superclass
  will be itk::ImageToImageFilter; and itk::QuadEdgeMeshToQuadEdgeMeshFilter
  when dealing with itk::QuadEdgeMesh
  */
template< class TInputDomain,
          class TNode,
          class TOutputDomain,
          class TSuperclass >
class FastMarchingTraits
  {
public:
  /** \typedef Input Domain Type */
  typedef TInputDomain                                        InputDomainType;
  typedef typename InputDomainType::Pointer                   InputDomainPointer;
  typedef typename InputDomainType::PixelType                 InputPixelType;

  /** \typedef Node type */
  typedef TNode                                               NodeType;

  /** \typedef Output Domain Type */
  typedef TOutputDomain                                       OutputDomainType;
  typedef typename OutputDomainType::Pointer                  OutputDomainPointer;
  typedef typename OutputDomainType::PixelType                OutputPixelType;

  /**
  \class NodePair
  \brief Represents a Node and its associated value (front value)
  */
  class NodePair : public std::pair< NodeType, OutputPixelType >
    {
  public:
    typedef NodePair Self;
    typedef std::pair< NodeType, OutputPixelType > Superclass;

    NodePair() : Superclass() {}
    NodePair( const NodeType& iNode, const OutputPixelType& iValue ) :
      Superclass( iNode, iValue ) {}
    NodePair( const Self& iPair ) : Superclass( iPair ) {}

    void operator = ( const Self& iPair )
      {
      this->first = iPair.first;
      this->second = iPair.second;
      }

    void SetValue( const OutputPixelType& iValue )
      {
      this->second = iValue;
      }
    OutputPixelType GetValue() const
      {
      return this->second;
      }
    void SetNode( const NodeType& iNode )
      {
      this->first = iNode;
      }
    NodeType GetNode() const
      {
      return this->first;
      }

    bool operator < ( const Self& iRight ) const
      {
      return this->second < iRight.second;
      }

    bool operator > ( const Self& iRight ) const
      {
      return this->second > iRight.second;
      }

    bool operator <= ( const Self& iRight ) const
      {
      return this->second <= iRight.second;
      }

    bool operator >= ( const Self& iRight ) const
      {
      return this->second >= iRight.second;
      }
    };

  typedef NodePair                                         NodePairType;
  typedef VectorContainer< IdentifierType, NodePairType >  NodePairContainerType;
  typedef typename NodePairContainerType::Pointer          NodePairContainerPointer;
  typedef typename NodePairContainerType::Iterator         NodePairContainerIterator;
  typedef typename NodePairContainerType::ConstIterator    NodePairContainerConstIterator;

  typedef VectorContainer< IdentifierType, NodeType >      NodeContainerType;
  typedef typename NodeContainerType::Pointer              NodeContainerPointer;
  typedef typename NodeContainerType::Iterator             NodeContainerIterator;
  typedef typename NodeContainerType::ConstIterator        NodeContainerConstIterator;

  typedef TSuperclass                                      SuperclassType;

#ifdef ITK_USE_CONCEPT_CHECKING
  itkConceptMacro( DoubleConvertibleOutputCheck,
                  ( Concept::Convertible< double, OutputPixelType > ) );

  itkConceptMacro( OutputOStreamWritableCheck,
                  ( Concept::OStreamWritable< OutputPixelType > ) );
#endif
  };

/**  \class ImageFastMarchingTraits
  \brief one possible traits to be used when using itk::FastMarchingBase
inherited class with itk::Image.

  \tparam VDimension Image Dimension
  \tparam TInputPixel Input Pixel type
  \tparam TOutputPixel Output Pixel type

  \sa ImageFastMarchingTraits2
*/
template<unsigned int VDimension,
         typename TInputPixel,
         typename TOutputPixel > // = TInputPixel >
class ImageFastMarchingTraits :
    public FastMarchingTraits<
      Image< TInputPixel, VDimension >,
      Index< VDimension >,
      Image< TOutputPixel, VDimension >,
      ImageToImageFilter< Image< TInputPixel, VDimension >,
                          Image< TOutputPixel, VDimension > >
    >
  {
public:
  itkStaticConstMacro(ImageDimension, unsigned int, VDimension);
  };



/** \class ImageFastMarchingTraits2
  \brief possible traits to be used when using itk::FastMarchingBase
inherited class with itk::Image

  \tparam TInputImage Input Image type
  \tparam TOutputImage Output Image type

  \sa ImageFastMarchingTraits
  */
template< class TInputImage, class TOutputImage >
class ImageFastMarchingTraits2 :
    public FastMarchingTraits<
      TInputImage,
      typename TOutputImage::IndexType,
      TOutputImage,
      ImageToImageFilter< TInputImage, TOutputImage > >
{
public:
  typedef ImageFastMarchingTraits2 Self;

  itkStaticConstMacro(ImageDimension, unsigned int, TOutputImage::ImageDimension);

#ifdef ITK_USE_CONCEPT_CHECKING

  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  // Let's make sure TInputImage and TOutputImage have the same dimension
  itkConceptMacro( SameDimension,
                   ( Concept::SameDimension<
                      itkGetStaticConstMacro(InputImageDimension),
                      itkGetStaticConstMacro(ImageDimension) > ) );
#endif
};



/**  \class MeshFastMarchingTraits
  \brief possible traits to be used when using itk::FastMarchingBase
inherited class with itk::QuadEdgeMesh

  \tparam VDimension Point Dimension
  \tparam InputPixel Input Data type
  \tparam TInputMeshTraits Input Mesh traits
  \tparam TOutputPixel Output Data type
  \tparam TOutputMeshTraits Output Mesh traits

  \sa MeshFastMarchingTraits2
*/
template< unsigned int VDimension,
          typename TInputPixel,
          class TInputMeshTraits, //= QuadEdgeMeshTraits< TInputPixel, VDimension, bool, bool >,
          typename TOutputPixel, //= TInputPixel,
          class TOutputMeshTraits //= QuadEdgeMeshTraits< TOutputPixel, VDimension, bool, bool >
         >
class MeshFastMarchingTraits :
    public FastMarchingTraits<
      QuadEdgeMesh< TInputPixel, VDimension, TInputMeshTraits >,
      typename TInputMeshTraits::PointIdentifier,
      QuadEdgeMesh< TOutputPixel, VDimension, TOutputMeshTraits >,
      QuadEdgeMeshToQuadEdgeMeshFilter<
        QuadEdgeMesh< TInputPixel, VDimension, TInputMeshTraits >,
        QuadEdgeMesh< TOutputPixel, VDimension, TOutputMeshTraits > >
    >
  {
public:
  itkStaticConstMacro(PointDimension, unsigned int, VDimension);
  };

/**  \class MeshFastMarchingTraits2
  \brief possible traits to be used when using itk::FastMarchingBase
inherited class with itk::QuadEdgeMesh

  \tparam TInputMesh Input Mesh type
  \tparam TOutputMesh Output Mesh type

  \sa MeshFastMarchingTraits
*/
template< class TInputMesh, class TOutputMesh >
class MeshFastMarchingTraits2 :
    public FastMarchingTraits<
      TInputMesh,
      typename TOutputMesh::MeshTraits::PointIdentifier,
      TOutputMesh,
      QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh > >
  {
public:
  typedef MeshFastMarchingTraits2 Self;


  itkStaticConstMacro(PointDimension, unsigned int, TOutputMesh::PointDimension );


#ifdef ITK_USE_CONCEPT_CHECKING

  // make sure TInputMesh and TOutputMesh have the same dimension
  itkStaticConstMacro(InputPointDimension, unsigned int,
                      TInputMesh::PointDimension);

  // Let's make sure TInputImage and TOutputImage have the same dimension
  itkConceptMacro( SameDimension,
                   ( Concept::SameDimension<
                      itkGetStaticConstMacro(InputPointDimension),
                      itkGetStaticConstMacro(PointDimension) > ) );
#endif
  };
}
#endif // __itkFastMarchingTraits_h
