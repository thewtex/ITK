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
#ifndef __itkCompositeTransformReader_txx
#define __itkCompositeTransformReader_txx

#include "itkCompositeTransformReader.h"
#include "itkTransformFileReader.h"
#include "itkTransformFactoryBase.h"
#include "itkTransformFactory.h"
#include "itksys/SystemTools.hxx"
#include <stdio.h>
#include <sstream>

namespace itk
{
//---------------------------------------------------------
template<class TScalar, unsigned int NDimensions>
CompositeTransformReader<TScalar, NDimensions>
::CompositeTransformReader()
{
  m_CompositeTransform = NULL;
  m_FileName = "";
  m_ComponentFullPaths.clear();
  TransformFactoryBase::RegisterDefaultTransforms(); //needed?
  TransformFactory<CompositeTransformType>::RegisterTransform();
}

//---------------------------------------------------------
template<class TScalar, unsigned int NDimensions>
CompositeTransformReader<TScalar, NDimensions>
::~CompositeTransformReader()
{}

//---------------------------------------------------------
template<class TScalar, unsigned int NDimensions>
void
CompositeTransformReader<TScalar, NDimensions>
::CreateTransform(CompositeTransformPointer & ptr,
                                 const std::string & ClassName)
{
  // Instantiate the transform.
  // NOTE: This code taken from itkTransformIOBase. It's protected there
  // so can't call it directly.
  itkDebugMacro ("About to call ObjectFactory");
  LightObject::Pointer i;
  i = ObjectFactoryBase::CreateInstance ( ClassName.c_str() );
  itkDebugMacro ("After call ObjectFactory");
  ptr = dynamic_cast< CompositeTransformType * >( i.GetPointer() );
  if ( ptr.IsNull() )
    {
    std::ostringstream msg;
    msg << "Could not create an instance of " << ClassName << std::endl
        << "The usual cause of this error is not registering the "
        << "transform with TransformFactory" << std::endl;
    msg << "Currently registered Transforms: " << std::endl;
    std::list< std::string >
      names = TransformFactoryBase::GetFactory()->GetClassOverrideWithNames();
    std::list< std::string >::iterator it;
    for ( it = names.begin(); it != names.end(); it++ )
      {
      msg << "\t\"" << *it << "\"" << std::endl;
      }
    itkExceptionMacro ( << msg.str() );
    }
  // Correct extra reference count from CreateInstance()
  ptr->UnRegister();
}

//---------------------------------------------------------
template<class TScalar, unsigned int NDimensions>
void
CompositeTransformReader<TScalar, NDimensions>
::Update()
{
  /* Reader code based on itkTxtTransformIO::Read */

  if ( m_FileName == "" )
    {
    itkExceptionMacro ("No file name given");
    }

  CompositeTransformPointer compositeTransform;
  std::ifstream    in;

  in.open (this->GetFileName(), std::ios::in | std::ios::binary);
  if ( in.fail() )
    {
    in.close();
    itkExceptionMacro ("The file could not be opened for read access "
                        << std::endl << "Filename: \""
                        << this->GetFileName() << "\"");
    }

  //Path to filename, used for component files
  std::string filePath =
    itksys::SystemTools::GetFilenamePath( this->GetFileName() ) + "/";
  filePath = itksys::SystemTools::ConvertToOutputPath( filePath.c_str() );

  std::ostringstream InData;

  // in.get ( InData );
  std::filebuf *pbuf;
  pbuf = in.rdbuf();

  // get file size using buffer's members
  int size = pbuf->pubseekoff (0, std::ios::end, std::ios::in);
  pbuf->pubseekpos (0, std::ios::in);

  // allocate memory to contain file data
  char *buffer = new char[size + 1];

  // get file data
  pbuf->sgetn (buffer, size);
  buffer[size] = '\0';
  itkDebugMacro ("Read file transform Data");
  InData << buffer;

  delete[] buffer;
  std::string data = InData.str();
  in.close();

  // Read line by line
  std::string::size_type position = 0;

  m_ComponentFullPaths.clear();
  size_t transformCount = 0;
  size_t numberOfComponents = 0;

  //
  // check for line end convention
  std::string line_end("\n");

  if ( data.find('\n') == std::string::npos )
    {
    if ( data.find('\r') == std::string::npos )
      {
      itkExceptionMacro
        ("No line ending character found, not a valid ITK Transform TXT file");
      }
    line_end = "\r";
    }
  // Read and parse data
  while ( position != std::string::npos && position < data.size() )
    {
    // Find the next string
    std::string::size_type end = data.find (line_end, position);

    if( end == std::string::npos )
      {
      itkExceptionMacro("Couldn't find end of line in " << data );
      }

    std::string line = trim ( data.substr (position, end - position) );
    position = end + 1;
    itkDebugMacro ("Found line: \"" << line << "\"");

    if ( line.length() == 0 )
      {
      continue;
      }
    if ( line[0] == '#' || std::string::npos == line.find_first_not_of (" \t") )
      {
      // Skip lines beginning with #, or blank lines
      continue;
      }

    // Get the tag name
    end = line.find (":");
    if ( end == std::string::npos )
      {
      // Throw an error
      itkExceptionMacro ("Tags must be delimited by ':'");
      }
    std::string Name = trim ( line.substr (0, end) );
    std::string Value = trim ( line.substr ( end + 1, line.length() ) );
    // Push back
    itkDebugMacro ("Name: \"" << Name << "\"");
    itkDebugMacro ("Value: \"" << Value << "\"");
    itksys_ios::istringstream parse (Value);
    if ( Name == "Transform" )
      {
      CreateTransform(compositeTransform, Value); //Handles errors
      m_CompositeTransform = compositeTransform;
      }
    else if ( Name == "NumberOfComponentTransforms" )
      {
      numberOfComponents = atoi(Value.c_str());
      }
    else if ( Name == "File" )
      {
      if( m_CompositeTransform.IsNull() )
        {
        itkExceptionMacro(
          "File Format error."
          "Transform type must be defined before component files are listed.");
        }
      /* Read in a component transform */
      transformCount++;
      TransformFileReader::Pointer reader = TransformFileReader::New();
      std::string componentFullPath = filePath + Value;
      reader->SetFileName( componentFullPath );
      try
        {
        reader->Update();
        }
      catch (itk::ExceptionObject &ex)
        {
        itkExceptionMacro("Error reading component file: " << Value
                          << std::endl << ex );
        }
      ComponentTransformPointer component =
        dynamic_cast<ComponentTransformType *>
          ( reader->GetTransformList()->front().GetPointer() );
      if( component.IsNull() )
        {
        itkExceptionMacro(
          "Error converting component transform to proper type");
        }
      m_CompositeTransform->AddTransform( component );
      m_ComponentFullPaths.push_back( componentFullPath );
      }
    else
      {
      itkExceptionMacro("Unrecognized tag: " << Name);
      }
    }

  // Check that we read as many component files as were designated
  if( transformCount != numberOfComponents )
    {
    itkExceptionMacro("The number of component files found does not match "
      << "the number specified by the NumberOfComponentTransforms tag.");
    }

}

//---------------------------------------------------------
template<class TScalar, unsigned int NDimensions>
std::string
CompositeTransformReader<TScalar, NDimensions>
::trim(std::string const & source, char const *delims)
{
  std::string            result(source);
  std::string::size_type index = result.find_last_not_of(delims);

  if ( index != std::string::npos )
    {
    result.erase(++index);
    }

  index = result.find_first_not_of(delims);
  if ( index != std::string::npos )
    {
    result.erase(0, index);
    }
  else
    {
    result.erase();
    }
  return result;
}

//---------------------------------------------------------
template<class TScalar, unsigned int NDimensions>
void
CompositeTransformReader<TScalar, NDimensions>
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Composite Transform IO: ";
  if ( m_CompositeTransform.IsNull() )
    {
    os << indent << "(none)\n";
    }
  else
    {
    os << indent << m_CompositeTransform << "\n";
    }

  os << indent << "MasterFileName: " << m_FileName << std::endl;
  os << indent << "Component FileNames: " << std::endl;
  for( FileNamesContainer::const_iterator
        it = this->m_ComponentFullPaths.begin();
        it != m_ComponentFullPaths.end(); it++ )
    {
    os << indent << *it << std::endl;
    }
}
} // end namespace itk

#endif
