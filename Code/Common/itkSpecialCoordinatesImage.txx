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
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#ifndef __itkSpecialCoordinatesImage_txx
#define __itkSpecialCoordinatesImage_txx
#include "itkSpecialCoordinatesImage.h"
#include "itkProcessObject.h"

namespace itk
{
/**
 *
 */
template< class TPixel, unsigned int VImageDimension, class TTransform >
SpecialCoordinatesImage< TPixel, VImageDimension, TTransform >
::SpecialCoordinatesImage()
{
  m_Buffer = PixelContainer::New();

  this->SetIndexToPhysicalPointTransform( TransformType::New() );
}

//----------------------------------------------------------------------------
template< class TPixel, unsigned int VImageDimension, class TTransform >
void
SpecialCoordinatesImage< TPixel, VImageDimension, TTransform >
::Allocate()
{
  SizeValueType num;

  this->ComputeOffsetTable();
  num = static_cast<SizeValueType>(this->GetOffsetTable()[VImageDimension]);

  m_Buffer->Reserve(num);
}

template< class TPixel, unsigned int VImageDimension, class TTransform >
void
SpecialCoordinatesImage< TPixel, VImageDimension, TTransform >
::Initialize()
{
  //
  // We don't modify ourselves because the "ReleaseData" methods depend upon
  // no modification when initialized.
  //

  // Call the superclass which should initialize the BufferedRegion ivar.
  Superclass::Initialize();

  // Replace the handle to the buffer. This is the safest thing to do,
  // since the same container can be shared by multiple images (e.g.
  // Grafted outputs and in place filters).
  m_Buffer = PixelContainer::New();
}

template< class TPixel, unsigned int VImageDimension, class TTransform >
void
SpecialCoordinatesImage< TPixel, VImageDimension, TTransform >
::FillBuffer(const TPixel & value)
{
  const SizeValueType numberOfPixels =
    this->GetBufferedRegion().GetNumberOfPixels();

  for ( unsigned int i = 0; i < numberOfPixels; i++ )
    {
    ( *m_Buffer )[i] = value;
    }
}

template< class TPixel, unsigned int VImageDimension, class TTransform >
void
SpecialCoordinatesImage< TPixel, VImageDimension, TTransform >
::SetPixelContainer(PixelContainer *container)
{
  if ( m_Buffer != container )
    {
    m_Buffer = container;
    this->Modified();
    }
}


template< class TPixel, unsigned int VImageDimension, class TTransform >
void
SpecialCoordinatesImage< TPixel, VImageDimension, TTransform >
::CopyInformation(const DataObject *data)
{
  // call the superclass' method first
  Superclass::CopyInformation(data);

  if ( data )
    {
    // Attempt to cast data to a SpecialCoordinatesImage
    const SpecialCoordinatesImage< TPixel, VImageDimension, TTransform > *imgData;

    try
      {
      imgData = dynamic_cast< const SpecialCoordinatesImage< TPixel, VImageDimension, TTransform > * >( data );
      }
    catch ( ... )
      {
      return;
      }

    // Copy the transform to the DataObject if the DataObject is a
    // SpecialCoordinatesImage
    if ( imgData )
      {
      // Copy the meta data for this data type
      const TransformType *sourceTransform = imgData->GetIndexToPhysicalPointTransform();
      LightObject::Pointer anotherTransform = sourceTransform->CreateAnother();
      // This static_cast should always work since the pointer was created by
      // CreateAnother() called from the transform itself.
      TransformType *transformCopy = static_cast< TransformType * >( anotherTransform.GetPointer() );
      /** Set the fixed parameters first. Some transforms have parameters which depend on
        the values of the fixed parameters. For instance, the BSplineDeformableTransform
        checks the grid size (part of the fixed parameters) before setting the parameters. */
      transformCopy->SetFixedParameters( sourceTransform->GetFixedParameters() );
      transformCopy->SetParameters( sourceTransform->GetParameters() );

      this->SetIndexToPhysicalPointTransform( transformCopy );
      }
    }
}


/**
 *
 */
template< class TPixel, unsigned int VImageDimension, class TTransform >
void
SpecialCoordinatesImage< TPixel, VImageDimension, TTransform >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PixelContainer: " << std::endl;
  Indent nextIndent = indent.GetNextIndent();
  m_Buffer->Print( os, nextIndent );

  os << indent << "IndexToPhysicalPointTransform:" << std::endl;
  m_IndexToPhysicalPointTransform->Print( os, nextIndent );

  os << indent << "PhysicalPointToIndexTransform:" << std::endl;
  m_PhysicalPointToIndexTransform->Print( os, nextIndent );

  // m_Origin and m_Spacing are printed in the Superclass
}
} // end namespace itk

#endif
