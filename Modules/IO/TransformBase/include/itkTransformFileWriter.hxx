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
#ifndef __itkTransformFileWriter_hxx
#define __itkTransformFileWriter_hxx

#include "itkTransformFileWriter.h"
#include "itkTransformFactoryBase.h"
#include "itkTransformIOFactory.h"

namespace itk
{
template<class ScalarType>
TransformFileWriterTemplate<ScalarType>
::TransformFileWriterTemplate() :
  m_FileName(""),
  m_Precision(7),
  m_AppendMode(false)
{
  TransformFactoryBase::RegisterDefaultTransforms();
}

template<class ScalarType>
TransformFileWriterTemplate<ScalarType>
::~TransformFileWriterTemplate()
{
}

/** Set the writer to append to the specified file */
template<class ScalarType>
void TransformFileWriterTemplate<ScalarType>
::SetAppendOn()
{
  this->SetAppendMode(true);
}

/** Set the writer to overwrite the specified file - This is the
* default mode. */
template<class ScalarType>
void TransformFileWriterTemplate<ScalarType>
::SetAppendOff()
{
  this->SetAppendMode(false);
}

/** Set the writer mode (append/overwrite). */
template<class ScalarType>
void TransformFileWriterTemplate<ScalarType>
::SetAppendMode(bool mode)
{
  this->m_AppendMode = mode;
}

/** Get the writer mode. */
template<class ScalarType>
bool TransformFileWriterTemplate<ScalarType>
::GetAppendMode()
{
  return ( this->m_AppendMode );
}

/* Converts between parameters with different precision types */
template<class ScalarType>
template<class TInputScalarType>
OptimizerParameters< ScalarType >
TransformFileWriterTemplate<ScalarType>
::ParametersTypeConvertor(const itk::OptimizerParameters< TInputScalarType >  &sourceParams)
{
  itk::OptimizerParameters< ScalarType > outputParams;
  outputParams.SetSize( sourceParams.GetSize() );
  for( itk::SizeValueType i = 0; i < sourceParams.GetSize(); ++i )
    {
    outputParams[i] = (ScalarType)( sourceParams[i] );
    }
  return outputParams;
}

/* The precision type of input transform should be converted to the asked precision type */
template<class ScalarType>
template<class TInputScalarType>
void TransformFileWriterTemplate<ScalarType>
::SetInputTransformWithOutputPrecisionType(const TransformBaseTemplate<TInputScalarType> *transform)
{
  std::string transformName = transform->GetTransformTypeAsString();
  // Transform name should be modified to have the output precision type.
  TransformName<ScalarType>::CorrectPrcisionType( transformName );

  TransformPointer convertedInputTransform;
  // Instantiate the transform
  LightObject::Pointer i = ObjectFactoryBase::CreateInstance ( transformName.c_str() );
  convertedInputTransform = dynamic_cast< TransformType * >( i.GetPointer() );
  if( convertedInputTransform.IsNull() )
    {
    itkExceptionMacro ( << "Could not create an instance of " << transformName );
    }
  convertedInputTransform->UnRegister();

  // The precision type of the input transform parameters should be converted to the requested output precision
  convertedInputTransform->SetParameters( this->ParametersTypeConvertor<TInputScalarType>( transform->GetParameters() ) );
  convertedInputTransform->SetFixedParameters( this->ParametersTypeConvertor<TInputScalarType>( transform->GetFixedParameters() ) );
  m_TransformList.push_back( ConstTransformPointer(convertedInputTransform.GetPointer()) );
}

/* Conversion between the precision types is not needed when input transform's precision type is
the same as requested precision type */
template<>
template<>
void TransformFileWriterTemplate<double>
::SetInputTransformWithOutputPrecisionType(const TransformBaseTemplate<double> *transform)
{
  m_TransformList.push_back( ConstTransformPointer(transform) );
}

template<>
template<>
void TransformFileWriterTemplate<float>
::SetInputTransformWithOutputPrecisionType(const TransformBaseTemplate<float> *transform)
{
  m_TransformList.push_back( ConstTransformPointer(transform) );
}

template<class ScalarType>
void TransformFileWriterTemplate<ScalarType>
::PushBackTransformList(const Object *transObj)
{
  Object::ConstPointer ptr;
  ptr = dynamic_cast<const TransformBaseTemplate<double> *>( transObj );
  if( ptr.IsNull() )
    {
    ptr = dynamic_cast<const TransformBaseTemplate<float> *>( transObj );
    if( ptr.IsNull() )
      {
      itkExceptionMacro("The input of writer should be whether a double precision"
                        "or a single precision transform type.");
      }
    else
      {
      this->SetInputTransformWithOutputPrecisionType<float>( dynamic_cast<const TransformBaseTemplate<float> *>( transObj ) );
      }
    }
  else
    {
    this->SetInputTransformWithOutputPrecisionType<double>( dynamic_cast<const TransformBaseTemplate<double> *>( transObj ) );
    }
}

/** Set the input transform and reinitialize the list of transforms */
template<class ScalarType>
void TransformFileWriterTemplate<ScalarType>
::SetInput(const Object *transform)
{
  m_TransformList.clear();
  this->PushBackTransformList(transform);
}

template<class ScalarType>
const typename TransformFileWriterTemplate<ScalarType>::TransformType *
TransformFileWriterTemplate<ScalarType>
::GetInput()
{
  ConstTransformPointer res = *(m_TransformList.begin());
  return res.GetPointer();
}

/** Add a transform to be written */
template<class ScalarType>
void TransformFileWriterTemplate<ScalarType>
::AddTransform(const Object *transform)
{
  /* Check for a CompositeTransform.
   * The convention is that there should be one, and it should
   * be the first transform in the file
   */
  std::string transformName = transform->GetNameOfClass();
  if( transformName.find("CompositeTransform") != std::string::npos )
    {
    if(this->m_TransformList.size() > 0)
      {
      itkExceptionMacro("Can only write a transform of type CompositeTransform "
                        "as the first transform in the file.");
      }
    }

  this->PushBackTransformList(transform);
}

template<class ScalarType>
void TransformFileWriterTemplate<ScalarType>
::Update()
{
  if ( m_FileName == "" )
    {
    itkExceptionMacro ("No file name given");
    }
  typename TransformIOBaseTemplate<ScalarType>::Pointer transformIO =
    TransformIOFactoryTemplate<ScalarType>::CreateTransformIO( m_FileName.c_str(), WriteMode );
  if ( transformIO.IsNull() )
    {
    itkExceptionMacro("Can't Create IO object for file "
                      << m_FileName);
    }
  transformIO->SetAppendMode(this->m_AppendMode);
  transformIO->SetFileName(m_FileName);
  transformIO->SetTransformList(this->m_TransformList);
  transformIO->Write();
}

template<class ScalarType>
void TransformFileWriterTemplate<ScalarType>
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "FileName: " << m_FileName << std::endl;
  os << indent << "Precision: " << m_Precision << std::endl;
}

} // namespace itk

#endif
