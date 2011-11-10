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

#ifndef __itkLevelSetTovtkImageData_h
#define __itkLevelSetTovtkImageData_h

#include "itkLevelSetTovtkImageDataBase.h"

#include "itkLevelSetDenseImageBase.h"
#include "itkWhitakerSparseLevelSetImage.h"
#include "itkShiSparseLevelSetImage.h"
#include "itkMalcolmSparseLevelSetImage.h"

#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkLabelMapToLabelImageFilter.h"

namespace itk
{
template< class TLevelSet >
class LevelSetTovtkImageData
  {};

/** \class LevelSetTovtkImageData
 *  \ingroup ITKLevelSetsv4Visualization
 */
template< class TImage >
class LevelSetTovtkImageData< LevelSetDenseImageBase< TImage > > :
    public LevelSetTovtkImageDataBase< LevelSetDenseImageBase< TImage > >
{
public:
  typedef TImage                                  ImageType;
  typedef LevelSetDenseImageBase< ImageType >     LevelSetType;

  typedef LevelSetTovtkImageData                      Self;
  typedef LevelSetTovtkImageDataBase< LevelSetType >  Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( LevelSetTovtkImageData, LevelSetTovtkImageDataBase );

  typedef typename LevelSetType::Pointer  LevelSetPointer;

  vtkImageData* GetOutput() const
    {
    return m_Converter->GetOutput();
    }

protected:
  LevelSetTovtkImageData()
    {
    m_Converter = ConverterType::New();
    }
  virtual ~LevelSetTovtkImageData() {}

  void GenerateData()
    {
    if( !this->m_LevelSet->GetImage() )
      {
      itkGenericExceptionMacro( <<"this->m_LevelSet->GetImage() is NULL" );
      }
    this->m_Converter->SetInput( this->m_LevelSet->GetImage() );
    this->m_Converter->Update();
    }

private:
  LevelSetTovtkImageData( const Self& );
  void operator = ( const Self& );

  typedef ImageToVTKImageFilter< ImageType >  ConverterType;
  typedef typename ConverterType::Pointer     ConverterPointer;

  ConverterPointer m_Converter;
};

// -----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
class LevelSetTovtkImageData< WhitakerSparseLevelSetImage< TOutput, VDimension > > :
    public LevelSetTovtkImageDataBase< WhitakerSparseLevelSetImage< TOutput, VDimension > >
{
public:
  typedef WhitakerSparseLevelSetImage< TOutput, VDimension > LevelSetType;

  typedef LevelSetTovtkImageData                      Self;
  typedef LevelSetTovtkImageDataBase< LevelSetType >  Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( LevelSetTovtkImageData, LevelSetTovtkImageDataBase );

  typedef typename LevelSetType::Pointer  LevelSetPointer;

  vtkImageData* GetOutput() const
    {
    return this->m_Converter->GetOutput();
    }

protected:
  LevelSetTovtkImageData()
    {
    m_InternalImage = ImageType::New();
    m_Converter = ConverterType::New();
    }
  virtual ~LevelSetTovtkImageData() {}

  void GenerateData()
    {
    if( this->m_LevelSet->GetLabelMap() == NULL )
      {
      itkGenericExceptionMacro( <<"this->m_LevelSet->GetLabelMap() is NULL" );
      }

    typename LevelSetType::LabelMapPointer labelmap = this->m_LevelSet->GetLabelMap();

    this->m_InternalImage->SetRegions( labelmap->GetLargestPossibleRegion() );
    this->m_InternalImage->SetSpacing( labelmap->GetSpacing() );
    this->m_InternalImage->SetOrigin( labelmap->GetOrigin() );
    this->m_InternalImage->SetDirection( labelmap->GetDirection() );
    this->m_InternalImage->SetNumberOfComponentsPerPixel( 1 );
    this->m_InternalImage->Allocate();

    typedef ImageRegionIteratorWithIndex< ImageType > IteratorType;

    IteratorType it( this->m_InternalImage, this->m_InternalImage->GetLargestPossibleRegion() );
    it.GoToBegin();

    typename ImageType::IndexType idx;

    while( !it.IsAtEnd() )
      {
      idx = it.GetIndex();
      it.Set( this->m_LevelSet->Evaluate( idx ) );
      ++it;
      }

    this->m_Converter->SetInput( this->m_InternalImage );
    this->m_Converter->Update();
    }

private:
  LevelSetTovtkImageData( const Self& );
  void operator = ( const Self& );

  typedef Image< TOutput, VDimension >  ImageType;
  typedef typename ImageType::Pointer   ImagePointer;

  typedef ImageToVTKImageFilter< ImageType >  ConverterType;
  typedef typename ConverterType::Pointer     ConverterPointer;

  ImagePointer     m_InternalImage;
  ConverterPointer m_Converter;
};


// -----------------------------------------------------------------------------
template< unsigned int VDimension >
class LevelSetTovtkImageData< ShiSparseLevelSetImage< VDimension > > :
    public LevelSetTovtkImageDataBase< ShiSparseLevelSetImage< VDimension > >
{
public:
  typedef ShiSparseLevelSetImage< VDimension > LevelSetType;

  typedef LevelSetTovtkImageData                      Self;
  typedef LevelSetTovtkImageDataBase< LevelSetType >  Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( LevelSetTovtkImageData, LevelSetTovtkImageDataBase );

  typedef typename LevelSetType::Pointer  LevelSetPointer;

  vtkImageData* GetOutput() const
    {
    return this->m_Converter->GetOutput();
    }

protected:
  LevelSetTovtkImageData()
    {
    m_LabelMapToLabelImageFilter = LabelMapToLabelImageFilterType::New();
    m_Converter = ConverterType::New();
    }
  virtual ~LevelSetTovtkImageData() {}

  void GenerateData()
    {
    if( this->m_LevelSet->GetLabelMap() == NULL )
      {
      itkGenericExceptionMacro( <<"this->m_LevelSet->GetLabelMap() is NULL" );
      }

    LabelMapPointer labelmap = this->m_LevelSet->GetLabelMap();

    this->m_LabelMapToLabelImageFilter->SetInput( labelmap );
    this->m_LabelMapToLabelImageFilter->Update();

    this->m_Converter->SetInput( m_LabelMapToLabelImageFilter->GetOutput() );
    this->m_Converter->Update();
    }

private:
  LevelSetTovtkImageData( const Self& );
  void operator = ( const Self& );

  typedef typename LevelSetType::LabelMapType     LabelMapType;
  typedef typename LevelSetType::LabelMapPointer  LabelMapPointer;

  typedef Image< int8_t, VDimension >   ImageType;
  typedef typename ImageType::Pointer   ImagePointer;

  typedef LabelMapToLabelImageFilter< LabelMapType, ImageType >   LabelMapToLabelImageFilterType;
  typedef typename LabelMapToLabelImageFilterType::Pointer        LabelMapToLabelImageFilterPointer;

  LabelMapToLabelImageFilterPointer   m_LabelMapToLabelImageFilter;

  typedef ImageToVTKImageFilter< ImageType >  ConverterType;
  typedef typename ConverterType::Pointer     ConverterPointer;

  ConverterPointer                    m_Converter;
};

// -----------------------------------------------------------------------------
template< unsigned int VDimension >
class LevelSetTovtkImageData< MalcolmSparseLevelSetImage< VDimension > > :
    public LevelSetTovtkImageDataBase< MalcolmSparseLevelSetImage< VDimension > >
{
public:
  typedef MalcolmSparseLevelSetImage< VDimension > LevelSetType;

  typedef LevelSetTovtkImageData                      Self;
  typedef LevelSetTovtkImageDataBase< LevelSetType >  Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( LevelSetTovtkImageData, LevelSetTovtkImageDataBase );

  typedef typename LevelSetType::Pointer  LevelSetPointer;

  vtkImageData* GetOutput() const
    {
    return this->m_Converter->GetOutput();
    }

protected:
  LevelSetTovtkImageData()
    {
    m_LabelMapToLabelImageFilter = LabelMapToLabelImageFilterType::New();
    m_Converter = ConverterType::New();
    }
  virtual ~LevelSetTovtkImageData() {}

  void GenerateData()
    {
    if( this->m_LevelSet->GetLabelMap() == NULL )
      {
      itkGenericExceptionMacro( <<"this->m_LevelSet->GetLabelMap() is NULL" );
      }

    LabelMapPointer labelmap = this->m_LevelSet->GetLabelMap();

    this->m_LabelMapToLabelImageFilter->SetInput( labelmap );
    this->m_LabelMapToLabelImageFilter->Update();

    this->m_Converter->SetInput( m_LabelMapToLabelImageFilter->GetOutput() );
    this->m_Converter->Update();
    }

private:
  LevelSetTovtkImageData( const Self& );
  void operator = ( const Self& );

  typedef typename LevelSetType::LabelMapType     LabelMapType;
  typedef typename LevelSetType::LabelMapPointer  LabelMapPointer;

  typedef Image< int8_t, VDimension >   ImageType;
  typedef typename ImageType::Pointer   ImagePointer;

  typedef LabelMapToLabelImageFilter< LabelMapType, ImageType >   LabelMapToLabelImageFilterType;
  typedef typename LabelMapToLabelImageFilterType::Pointer        LabelMapToLabelImageFilterPointer;

  LabelMapToLabelImageFilterPointer   m_LabelMapToLabelImageFilter;

  typedef ImageToVTKImageFilter< ImageType >  ConverterType;
  typedef typename ConverterType::Pointer     ConverterPointer;

  ConverterPointer                    m_Converter;
};
}

#endif // __itkLevelSetTovtkImageData_h
