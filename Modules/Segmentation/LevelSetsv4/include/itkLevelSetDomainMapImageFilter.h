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

#ifndef __itkLevelSetDomainMapImageFilter_h
#define __itkLevelSetDomainMapImageFilter_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include <list>
#include <vector>

namespace itk
{
/**
  \class LevelSetDomainMapImageFilter
  \tparam TInputImage Image where the pixel type is a container of ids
  \tparam TOutputImage Image where the pixel type is an integer to split the region
  \ingroup ITKLevelSetsv4
*/
template < class TInputImage, class TOutputImage >
class ITK_EXPORT LevelSetDomainMapImageFilter : public ImageToImageFilter<
  TInputImage, TOutputImage >
{
  public:
    typedef LevelSetDomainMapImageFilter                      Self;
    typedef ImageToImageFilter< TInputImage,TOutputImage >    Superclass;
    typedef SmartPointer< Self >                              Pointer;
    typedef SmartPointer< const Self >                        ConstPointer;

    itkStaticConstMacro ( ImageDimension, unsigned int,
                          TInputImage::ImageDimension );

    /** Method for creation through object factory */
    itkNewMacro ( Self );

    /** Run-time type information */
    itkTypeMacro ( LevelSetDomainMapImageFilter, ImageToImageFilter );

    /** Display */
    void PrintSelf ( std::ostream& os, Indent indent ) const;

    typedef TInputImage                           InputImageType;
    typedef typename InputImageType::ConstPointer InputImageConstPointer;
    typedef typename InputImageType::PixelType    InputImagePixelType;
    typedef typename InputImageType::RegionType   InputImageRegionType;
    typedef typename InputImageType::SizeType     InputImageSizeType;
    typedef typename InputImageSizeType::SizeValueType
                                                  InputImageSizeValueType;
    typedef typename InputImageType::IndexType    InputImageIndexType;

    typedef TOutputImage                           OutputImageType;
    typedef typename OutputImageType::Pointer      OutputImagePointer;
    typedef typename OutputImageType::IndexType    OutputImageIndexType;
    typedef typename OutputImageType::PixelType    OutputImagePixelType;

    typedef ImageRegionConstIteratorWithIndex< InputImageType >
                                                           InputConstIteratorType;
    typedef ImageRegionIteratorWithIndex< InputImageType > InputIndexIteratorType;
    typedef ImageRegionIterator< InputImageType >          InputIteratorType;

    typedef ImageRegionConstIteratorWithIndex< OutputImageType >
                                                            OutputConstIteratorType;
    typedef ImageRegionIteratorWithIndex< OutputImageType > OutputIndexIteratorType;
    typedef ImageRegionIterator< OutputImageType >          OutputIteratorType;

    struct NounToBeDefined // ~ kind of cache to speed up computations
      {
      NounToBeDefined() {}

      NounToBeDefined( const InputImageRegionType& reg,
                      const InputImagePixelType& iList ) :
        m_Region( reg ), m_List( iList ) {}

      InputImageRegionType m_Region;
      InputImagePixelType m_List;
      };

    // IdentifierType = 0 means it is background, there is no level sets
    // nothing to be done at this pixel...
    // Note that the identifier for a given index is given by the output image
    // of this filter
//    std::map< IdentifierType, InputImageRegionType > m_SetOfRegions;
//    std::map< IdentifierType, InputImagePixelType >  m_LevelSetList;

    typedef std::map< IdentifierType, NounToBeDefined > DomainIteratorType;

    DomainIteratorType m_LevelSetMap;

  protected:
    LevelSetDomainMapImageFilter();
    ~LevelSetDomainMapImageFilter() {}

    /** Check if given region is consistent for the same set of overlapping
     *  level set support */
    void ConsistencyCheck( bool& subRegionConsistent, InputImageRegionType& subRegion );

    /** Identify image partitions where each partition has the same overlapping
     *  level set support */
    void GenerateData();

  private:
    LevelSetDomainMapImageFilter ( Self& );   // intentionally not implemented
    void operator= ( const Self& );   // intentionally not implemented
  };

} /* namespace itk */

#include "itkLevelSetDomainMapImageFilter.hxx"
#endif
