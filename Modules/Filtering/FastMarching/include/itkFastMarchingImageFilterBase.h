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

#ifndef __itkFastMarchingImageFilterBase_h
#define __itkFastMarchingImageFilterBase_h

#include "itkFastMarchingBase.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkNeighborhoodIterator.h"
#include "itkArray.h"

namespace itk
{
/**
 * \class FastMarchingImageFilterBase
 * \brief Fast Marching Method on Image
 *
 * The speed function can be specified as a speed image or a
 * speed constant. The speed image is set using the method
 * SetInput(). If the speed image is NULL, a constant speed function
 * is used and is specified using method the SetSpeedConstant().
 *
 * If the speed function is constant and of value one, fast marching results
 * is an approximate distance function from the initial alive points.
 *
 * There are two ways to specify the output image information
 * ( LargestPossibleRegion, Spacing, Origin):
 * \li it is copied directly from the input speed image
 * \li it is specified by the user.
 * Default values are used if the user does not specify all the information.
 *
 * The output information is computed as follows.
 *
 * If the speed image is NULL or if the OverrideOutputInformation is set to
 * true, the output information is set from user specified parameters. These
 * parameters can be specified using methods
 * \li FastMarchingImageFilterBase::SetOutputRegion(),
 * \li FastMarchingImageFilterBase::SetOutputSpacing(),
 * \li FastMarchingImageFilterBase::SetOutputDirection(),
 * \li FastMarchingImageFilterBase::SetOutputOrigin().
 *
 * Else the output information is copied from the input speed image.
 *
 * Implementation of this class is based on Chapter 8 of
 * "Level Set Methods and Fast Marching Methods", J.A. Sethian,
 * Cambridge Press, Second edition, 1999.
 *
 * \tparam TTraits traits
 *
 * \sa ImageFastMarchingTraits
 * \sa ImageFastMarchingTraits2
*/
template< class TTraits >
class FastMarchingImageFilterBase :
    public FastMarchingBase< TTraits >
  {
public:
  typedef TTraits Traits;

  typedef FastMarchingImageFilterBase Self;
  typedef FastMarchingBase< Traits >  Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(FastMarchingImageFilterBase, FastMarchingBase);


  typedef typename Superclass::InputDomainType     InputImageType;
  typedef typename Superclass::InputDomainPointer  InputImagePointer;
  typedef typename Superclass::InputPixelType      InputPixelType;

  typedef typename Superclass::OutputDomainType     OutputImageType;
  typedef typename Superclass::OutputDomainPointer  OutputImagePointer;
  typedef typename Superclass::OutputPixelType      OutputPixelType;
  typedef typename OutputImageType::SpacingType     OutputSpacingType;
  typedef typename OutputImageType::SizeType        OutputSizeType;
  typedef typename OutputImageType::RegionType      OutputRegionType;
  typedef typename OutputImageType::PointType       OutputPointType;
  typedef typename OutputImageType::DirectionType   OutputDirectionType;

  typedef typename Traits::NodeType                 NodeType;
  typedef typename Traits::NodePairType             NodePairType;
  typedef typename Traits::NodePairContainerType    NodePairContainerType;
  typedef typename Traits::NodePairContainerPointer NodePairContainerPointer;
  typedef typename Traits::NodePairContainerConstIterator
    NodePairContainerConstIterator;

  typedef typename Traits::NodeContainerType        NodeContainerType;
  typedef typename Traits::NodeContainerPointer     NodeContainerPointer;
  typedef typename Traits::NodeContainerConstIterator
    NodeContainerConstIterator;

  /*
  typedef typename Superclass::ElementIdentifier ElementIdentifier;

  typedef typename Superclass::PriorityQueueElementType PriorityQueueElementType;

  typedef typename Superclass::PriorityQueueType PriorityQueueType;
  typedef typename Superclass::PriorityQueuePointer PriorityQueuePointer;
  */

  typedef typename Superclass::LabelType LabelType;

  itkStaticConstMacro( ImageDimension, unsigned int, Traits::ImageDimension );

  typedef Image< unsigned char, ImageDimension >  LabelImageType;
  typedef typename LabelImageType::Pointer        LabelImagePointer;

  typedef Image< unsigned int, ImageDimension >
    ConnectedComponentImageType;
  typedef typename ConnectedComponentImageType::Pointer ConnectedComponentImagePointer;

  typedef NeighborhoodIterator<LabelImageType> NeighborhoodIteratorType;
  typedef typename NeighborhoodIteratorType::RadiusType NeighborhoodRadiusType;

  itkGetObjectMacro( LabelImage, LabelImageType );


  /** Set the alive points from the given image.
     * All indices above zero are assigned the given trial value.
     */
  template< typename TPixel >
  void SetAlivePointsFromImage(
    typename Image< TPixel, ImageDimension >::Pointer image,
    const OutputPixelType& value = NumericTraits<OutputPixelType>::Zero )
    {
    SetPointsFromImage< TPixel >( image, Superclass::Alive, value );
    }

  /** Set the initial trial points from the given image.
     * All indices above zero are assigned the given initial trial value.
     */
  template< typename TPixel >
  void SetTrialPointsFromImage(
    typename Image< TPixel, ImageDimension >::Pointer image,
    const OutputPixelType& value = NumericTraits<OutputPixelType>::Zero
    )
    {
    SetPointsFromImage< TPixel >( image, Superclass::InitialTrial, value );
    }

  /** Set the forbidden points from the given image.
     * All indices different than zero are made forbidden.
     */
  template< typename TPixel >
  void SetForbiddenPointsFromImage(
    typename Image< TPixel, ImageDimension >::Pointer image )
    {
    typedef ImageRegionConstIteratorWithIndex< LabelImageType > IteratorType;
    typename NodeContainerType::Pointer nodes = NodeContainerType::New();
    nodes->Initialize();

    IteratorType it( image, image->GeBufferedRegion() );

    // Walk image
    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
      {
      // Test if index value is greater than zero, if so add the node
      if (it.Get() != NumericTraits< TPixel >::Zero)
        {
        nodes->push_back( it.GetIndex() );
        } //end if image iterator != zero
      } // end for each pixel

    this->SetForbiddenPoints( nodes );
    this->Modified();
    }


  /** \brief Set a binary mask to limit the propagation of front.
    *
    * Only location with null pixel values are used. Corresponding labels are set
    * to Forbidden to limit the propagation of the front and output values are set
    * to 0.
    *
    * \param[in] iImage input image
    * \tparam TPixel PixelType for the input image
   */
  template< typename TPixel >
  void SetBinaryMask( Image< TPixel, ImageDimension >* iImage )
    {
    OutputSizeType nullsize;
    nullsize.Fill( 0 );

    if( this->m_ForbiddenPoints.IsNull() )
      {
      this->m_ForbiddenPoints = NodeContainerType::New();
      }


    typedef Image< TPixel, ImageDimension > InternalImageType;
    typedef ImageRegionConstIteratorWithIndex< InternalImageType >
        InternalRegionIterator;
    InternalRegionIterator b_it( iImage, iImage->GetLargestPossibleRegion() );
    b_it.GoToBegin();

    TPixel zero_value = NumericTraits< TPixel >::Zero;
    NodeType idx;

    while( !b_it.IsAtEnd() )
      {
      if( b_it.Get() == zero_value )
        {
        idx = b_it.GetIndex();

        this->m_ForbiddenPoints->push_back( idx );
        }
      ++b_it;
      }
    this->Modified();
    }


  /** The output largeset possible, spacing and origin is computed as follows.
   * If the speed image is NULL or if the OverrideOutputInformation is true,
   * the output information is set from user specified parameters. These
   * parameters can be specified using methods SetOutputRegion(),
   * SetOutputSpacing(), SetOutputDirection(), and SetOutputOrigin().
   * Else if the speed image is not NULL, the output information
   * is copied from the input speed image. */
  virtual void SetOutputSize(const OutputSizeType & size)
  { m_OutputRegion = size; }
  virtual OutputSizeType GetOutputSize() const
  { return m_OutputRegion.GetSize(); }
  itkSetMacro(OutputRegion, OutputRegionType);
  itkGetConstReferenceMacro(OutputRegion, OutputRegionType);
  itkSetMacro(OutputSpacing, OutputSpacingType);
  itkGetConstReferenceMacro(OutputSpacing, OutputSpacingType);
  itkSetMacro(OutputDirection, OutputDirectionType);
  itkGetConstReferenceMacro(OutputDirection, OutputDirectionType);
  itkSetMacro(OutputOrigin, OutputPointType);
  itkGetConstReferenceMacro(OutputOrigin, OutputPointType);
  itkSetMacro(OverrideOutputInformation, bool);
  itkGetConstReferenceMacro(OverrideOutputInformation, bool);
  itkBooleanMacro(OverrideOutputInformation);

protected:

  /** Constructor */
  FastMarchingImageFilterBase();

  /** Destructor */
  virtual ~FastMarchingImageFilterBase();

  struct InternalNodeStructure;

  OutputRegionType  m_BufferedRegion;
  NodeType          m_StartIndex;
  NodeType          m_LastIndex;

  OutputRegionType    m_OutputRegion;
  OutputPointType     m_OutputOrigin;
  OutputSpacingType   m_OutputSpacing;
  OutputDirectionType m_OutputDirection;
  bool                m_OverrideOutputInformation;

  /** Generate the output image meta information. */
  virtual void GenerateOutputInformation();

  virtual void EnlargeOutputRequestedRegion(DataObject *output);

  LabelImagePointer               m_LabelImage;
  ConnectedComponentImagePointer  m_ConnectedComponentImage;

  IdentifierType GetTotalNumberOfNodes() const;

  void SetOutputValue( OutputImageType* oDomain,
                       const NodeType& iNode,
                       const OutputPixelType& iValue );

  /** Returns the output value for a given node */
  const OutputPixelType GetOutputValue( OutputImageType* oImage,
                                  const NodeType& iNode ) const;

  /** Returns the label value for a given node */
  const unsigned char GetLabelValueForGivenNode( const NodeType& iNode ) const;

  /** Set the label value for a given node */
  void SetLabelValueForGivenNode( const NodeType& iNode,
                                 const LabelType& iLabel );

  /** Update values for the neighbors of a given node */
  virtual void UpdateNeighbors( OutputImageType* oImage,
                                const NodeType& iNode );

  /** Update value for a given node */
  virtual void UpdateValue( OutputImageType* oImage,
                            const NodeType& iValue );

  /** Make sure the given node does not violate any topological constraint*/
  bool CheckTopology( OutputImageType* oImage,
                      const NodeType& iNode );
  void InitializeOutput( OutputImageType* oImage );

  /** Find the nodes were the front will propagate given a node */
  void GetInternalNodesUsed( OutputImageType* oImage,
                            const NodeType& iNode,
                            std::vector< InternalNodeStructure >& ioNodesUsed );

  /** Solve the quadratic equation */
  double Solve( OutputImageType* oImage,
               const NodeType& iNode,
               std::vector< InternalNodeStructure >& ioNeighbors ) const;

  // --------------------------------------------------------------------------
  // --------------------------------------------------------------------------

  /**
   * Functions and variables to check for topology changes (2D/3D only).
   */

  // Functions/data for the 2-D case
  void InitializeIndices2D();
  bool IsChangeWellComposed2D( const NodeType& ) const;
  bool IsCriticalC1Configuration2D( const std::vector<bool>& ) const;
  bool IsCriticalC2Configuration2D( const std::vector<bool>& ) const;
  bool IsCriticalC3Configuration2D( const std::vector<bool>& ) const;
  bool IsCriticalC4Configuration2D( const std::vector<bool>& ) const;

  Array<unsigned char>  m_RotationIndices[4];
  Array<unsigned char>  m_ReflectionIndices[2];

  // Functions/data for the 3-D case
  void InitializeIndices3D();
  bool IsCriticalC1Configuration3D( const std::vector<bool>& ) const;
  unsigned int IsCriticalC2Configuration3D( const std::vector<bool>& ) const;
  bool IsChangeWellComposed3D( const NodeType& ) const;

  Array<unsigned char>                        m_C1Indices[12];
  Array<unsigned char>                        m_C2Indices[8];

  // Functions for both 2D/3D cases
  bool DoesVoxelChangeViolateWellComposedness( const NodeType& ) const;
  bool DoesVoxelChangeViolateStrictTopology( const NodeType& ) const;

private:

  FastMarchingImageFilterBase( const Self& );
  void operator = ( const Self& );

  /** */
  template< typename TPixel >
  void SetPointsFromImage(
    typename Image< TPixel, ImageDimension >::Pointer image,
    const LabelType& iLabel,
    const OutputPixelType& iValue )
    {
    if ( iLabel == Superclass::Alive || iLabel == Superclass::InitialTrial )
      {
      // Setup
      NodeContainerPointer nodes = NodeContainerType::New();
      nodes->Initialize();

      typedef ImageRegionConstIteratorWithIndex< LabelImageType > IteratorType;
      IteratorType it( image, image->GeBufferedRegion() );

      // Walk image
      for (it.GoToBegin(); !it.IsAtEnd(); ++it)
        {
        // Test if index value is greater than zero, if so add the node
        if (it.Get() != NumericTraits< TPixel >::Zero)
          {
          NodePairType node_pair( it.GetIndex(), iValue );
          nodes->push_back( node_pair );
          } //end if image iterator > zero
        } // end for each pixel

      // Set node container
      if( iLabel == Superclass::Alive )
        {
        this->SetAlivePoints( nodes );
        }
      if( iLabel == Superclass::InitialTrial )
        {
        this->SetTrialPoints( nodes );
        }
      this->Modified();
      }
    }
  };
}

#include "itkFastMarchingImageFilterBase.txx"
#endif // __itkFastMarchingImageFilterBase_h
