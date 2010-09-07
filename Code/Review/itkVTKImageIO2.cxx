/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkVTKImageIO2.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkVTKImageIO2.h"
#include "itkByteSwapper.h"

#include <itksys/ios/sstream>
#include <itksys/SystemTools.hxx>

#include <stdio.h>
#include <string.h>

namespace itk
{
VTKImageIO2::VTKImageIO2()
{
  this->SetNumberOfDimensions(2);
  m_ByteOrder = LittleEndian;
  m_FileType = Binary;
  m_HeaderSize = 0;

  this->AddSupportedReadExtension(".vtk");

  this->AddSupportedWriteExtension(".vtk");
}

VTKImageIO2::~VTKImageIO2()
{}

bool VTKImageIO2::CanReadFile(const char *filename)
{
  std::ifstream file;
  char          buffer[256];
  std::string   fname(filename);

  if ( fname.find(".vtk") >= fname.length() )
    {
    return false;
    }

  try
    {
    this->OpenFileForReading(file, filename);
    }
  catch ( ... )
    {
    return false;
    }

  // Check to see if its a vtk structured points file
  file.getline(buffer, 255);
  file.getline(buffer, 255);
  file.getline(buffer, 255);
  file.getline(buffer, 255);

  fname = buffer;

  if ( fname.find("STRUCTURED_POINTS") < fname.length()
       || fname.find("structured_points") < fname.length() )
    {
    return true;
    }
  else
    {
    return false;
    }
}

void VTKImageIO2::SetPixelTypeFromString(const std::string & pixelType)
{
  if ( pixelType.find("float") < pixelType.length() )
    {
    SetComponentType(FLOAT);
    }
  else if ( pixelType.find("double") < pixelType.length() )
    {
    SetComponentType(DOUBLE);
    }
  else if ( pixelType.find("unsigned_char") < pixelType.length() )
    {
    SetComponentType(UCHAR);
    }
  else if ( pixelType.find("char") < pixelType.length() )
    {
    SetComponentType(CHAR);
    }
  else if ( pixelType.find("unsigned_short") < pixelType.length() )
    {
    SetComponentType(USHORT);
    }
  else if ( pixelType.find("short") < pixelType.length() )
    {
    SetComponentType(SHORT);
    }
  else if ( pixelType.find("unsigned_int") < pixelType.length() )
    {
    SetComponentType(UINT);
    }
  else if ( pixelType.find("int") < pixelType.length() )
    {
    SetComponentType(INT);
    }
  else if ( pixelType.find("unsigned_long") < pixelType.length() )
    {
    SetComponentType(ULONG);
    }
  else if ( pixelType.find("long") < pixelType.length() )
    {
    SetComponentType(LONG);
    }
  else
    {
    itkExceptionMacro(<< "Unrecognized type");
    }
}

void VTKImageIO2::InternalReadImageInformation(std::ifstream & file)
{
  char        line[255];
  std::string text;

  this->OpenFileForReading( file, m_FileName.c_str() );

  file.getline(line, 255);
  file.getline(line, 255);
  file.getline(line, 255);
  text = line;
  if ( text.find("ASCII") < text.length()
       || text.find("ascii") < text.length() )
    {
    this->SetFileTypeToASCII();
    }
  else if ( text.find("BINARY") < text.length()
            || text.find("binary") < text.length() )
    {
    this->SetFileTypeToBinary();
    }
  else
    {
    itkExceptionMacro(<< "Unrecognized type");
    }
  file.getline(line, 255);
  text = line;
  if ( text.find("STRUCTURED_POINTS") >= text.length()
       && text.find("structured_points") >= text.length() )
    {
    itkExceptionMacro(<< "Not structured points, can't read");
    }

  file.getline(line, 255);
  text = line;

  // set values in case we don't find them
  this->SetNumberOfDimensions(3);
  this->SetSpacing(0, 1.0);
  this->SetSpacing(1, 1.0);
  this->SetSpacing(2, 1.0);
  this->SetOrigin(0, 0.0);
  this->SetOrigin(1, 0.0);
  this->SetOrigin(2, 0.0);

  if ( text.find("DIMENSIONS") < text.length()
       || text.find("dimensions") < text.length() )
    {
    unsigned int dims[3];
    sscanf(line, "%*s %d %d %d", dims, dims + 1, dims + 2);
    if ( dims[1] <= 1 && dims[2] <= 1 )
      {
      this->SetNumberOfDimensions(2);
      }
    if ( dims[2] <= 1 )
      {
      this->SetNumberOfDimensions(2);
      }
    else
      {
      this->SetNumberOfDimensions(3);
      }
    for ( unsigned int i = 0; i < this->GetNumberOfDimensions(); i++ )
      {
      this->SetDimensions(i, dims[i]);
      }
    }
  else
    {
    itkExceptionMacro(<< "No dimensions defined");
    }

  for ( bool readAttribute = false; !readAttribute; )
    {
    file.getline(line, 255);
    text = line;

    if ( text.find("SPACING") < text.length()
         || text.find("spacing") < text.length()
         || text.find("ASPECT_RATIO") < text.length()
         || text.find("aspect_ratio") < text.length() )
      {
      double spacing[3];
      sscanf(line, "%*s %lf %lf %lf", spacing, spacing + 1, spacing + 2);
      for ( unsigned int i = 0; i < m_NumberOfDimensions; i++ )
        {
        this->SetSpacing(i, spacing[i]);
        }
      }

    else if ( text.find("ORIGIN") < text.length()
              || text.find("origin") < text.length() )
      {
      double origin[3];
      sscanf(line, "%*s %lf %lf %lf", origin, origin + 1, origin + 2);
      for ( unsigned int i = 0; i < m_NumberOfDimensions; i++ )
        {
        this->SetOrigin(i, origin[i]);
        }
      }

    else if ( text.find("VECTOR") < text.length()
              || text.find("vector") < text.length() )
      {
      readAttribute = true;

      this->SetNumberOfComponents(3);
      this->SetPixelType(VECTOR);
      char pixelType[256];
      sscanf(line, "%*s %*s %s", pixelType);
      text = pixelType;

      this->SetPixelTypeFromString(text);
      }

    else if ( text.find("COLOR_SCALARS") < text.length()
              || text.find("color_scalars") < text.length() )
      {
      readAttribute = true;

      int numComp = 1;
      sscanf(line, "%*s %*s %d", &numComp);
      if ( numComp == 1 )
        {
        this->SetPixelType(SCALAR);
        }
      else if ( numComp == 3 )
        {
        this->SetPixelType(RGB);
        }
      else if ( numComp == 4 )
        {
        this->SetPixelType(RGBA);
        }
      else
        {
        SetPixelType(VECTOR);
        }
      if ( this->GetFileType() == ASCII )
        {
        this->SetNumberOfComponents(numComp);
        this->SetComponentType(FLOAT);
        }
      else
        {
        this->SetNumberOfComponents(numComp);
        this->SetComponentType(UCHAR);
        }
      }

    else if ( text.find("SCALARS") < text.length()
              || text.find("scalars") < text.length() )
      {
      readAttribute = true;

      char pixelType[256];
      int  numComp = 1;
      // numComp is optional
      sscanf(line, "%*s %*s %s %d", pixelType, &numComp);
      text = pixelType;
      if ( numComp == 1 )
        {
        this->SetPixelType(SCALAR);
        }
      else
        {
        this->SetPixelType(VECTOR);
        }
      this->SetPixelTypeFromString(text);
      this->SetNumberOfComponents(numComp);

      // maybe "LOOKUP_TABLE default"
      std::streampos pos = file.tellg();
      file.getline(line, 255);
      text = line;
      if ( !( text.find("LOOKUP_TABLE") < text.length()
              || text.find("lookup_table") < text.length() ) )
        {
        // it was data and not table
        file.seekg(pos);
        }
      } //found scalars

    else if ( text.find("TENSORS") < text.length() ||
              text.find("tensors") < text.length() )
      {
      readAttribute = true;

      char pixelType[256];
      sscanf(line, "%*s %*s %s", pixelType);
      text = pixelType;
      this->SetPixelType(SYMMETRICSECONDRANKTENSOR);
      this->SetNumberOfComponents(6);
      this->SetPixelTypeFromString(text);
      }

    if ( !file.good() )
      {
      itkExceptionMacro(<< "Error reading header");
      }
    }

  // set the header size based on how much we just read
  this->m_HeaderSize = static_cast< SizeType >( file.tellg() );
}

void VTKImageIO2::ReadHeaderSize(std::ifstream & file)
{
  char        line[255];
  std::string text;

  this->OpenFileForReading( file, m_FileName.c_str() );

  file.getline(line, 255); // HEADER
  file.getline(line, 255); // TITLE
  file.getline(line, 255); // DATA TYPE
  file.getline(line, 255); // GEOMEETRY/TOPOLOGY TYPE

  file.getline(line, 255); // DIMENSIONS

  for ( bool readAttribute = false; !readAttribute; )
    {
    file.getline(line, 255); // SPACING|ORIGIN|COLOR_SCALARS|SCALARS|VECTOR|TENSORS
    text = line;

    if ( text.find("SCALARS") < text.length()
         || text.find("scalars") < text.length()
         || text.find("VECTOR") < text.length()
         || text.find("vector") < text.length()
         || text.find("COLOR_SCALARS") < text.length()
         || text.find("color_scalars") < text.length()
         || text.find("TENSORS") < text.length()
         || text.find("tensors") < text.length() )
      {
      readAttribute = true;

      // maybe "LOOKUP_TABLE default"
      std::streampos pos = file.tellg();
      file.getline(line, 255);
      text = line;
      if ( !( text.find("LOOKUP_TABLE") < text.length()
              || text.find("lookup_table") < text.length() ) )
        {
        // it was data and not table
        file.seekg(pos);
        }
      else
            {}
      } //found scalars
    }

  if ( file.fail() )
    {
    itkExceptionMacro(<< "Failed reading header information");
    }

  // set the header size based on how much we just read
  this->m_HeaderSize = static_cast< SizeType >( file.tellg() );
}

template <class TComponent>
void ReadBuffer(std::istream& is, TComponent *buffer,
  const ImageIOBase::SizeType num, const ImageIOBase::IOPixelType pixelType,
  const ImageIOBase::SizeType components )
{
  typedef typename itk::NumericTraits<TComponent>::PrintType PrintType;
  PrintType temp;
  TComponent *ptr = buffer;
  if ( pixelType == ImageIOBase::SYMMETRICSECONDRANKTENSOR )
    {
    ImageIOBase::SizeType i=0;
    // More than the resulting components because of symmetry.
    const ImageIOBase::SizeType fileComponents = num / 6 * 9;
    if( components == 6 )
      {
      while( i < fileComponents )
        {
        // First row: hit hit hit
        is >> temp;
        *ptr = static_cast<TComponent>( temp );
        ++i;
        ++ptr;
        is >> temp;
        *ptr = static_cast<TComponent>( temp );
        ++i;
        ++ptr;
        is >> temp;
        *ptr = static_cast<TComponent>( temp );
        ++i;
        ++ptr;
        // Second row: skip hit hit
        is >> temp;
        ++i;
        is >> temp;
        *ptr = static_cast<TComponent>( temp );
        ++i;
        ++ptr;
        is >> temp;
        *ptr = static_cast<TComponent>( temp );
        ++i;
        ++ptr;
        // Third row: skip skip hit
        is >> temp;
        ++i;
        is >> temp;
        ++i;
        is >> temp;
        *ptr = static_cast<TComponent>( temp );
        ++i;
        ++ptr;
        }
      }
    else
      {
      ::itk::ExceptionObject e_(__FILE__, __LINE__,
        "itk::ERROR: VTKImageIO2: Unsupported number of components in tensor.",
        ITK_LOCATION);
      throw e_;
      }
    }
  else
    {
    for( ImageIOBase::SizeType i=0; i < num; i++, ptr++ )
      {
      is >> temp;
      *ptr = static_cast<TComponent>( temp );
      }
    }
}

void VTKImageIO2::ReadBufferAsASCII( std::istream& is, void *buffer,
                                    IOComponentType ctype,
                                    const ImageIOBase::SizeType numComp,
                                    const ImageIOBase::IOPixelType pixelType,
                                    const ImageIOBase::SizeType components )
{
  switch (ctype)
    {
    case UCHAR:
      {
      unsigned char *buf = reinterpret_cast<unsigned char*>(buffer);
      ReadBuffer( is, buf, numComp, pixelType, components );
      }
      break;
    case CHAR:
      {
      char *buf = reinterpret_cast<char*>(buffer);
      ReadBuffer( is, buf, numComp, pixelType, components );
      }
      break;

    case USHORT:
      {
      unsigned short *buf = reinterpret_cast<unsigned short*>(buffer);
      ReadBuffer( is, buf, numComp, pixelType, components );
      }
      break;

    case SHORT:
      {
      short *buf = reinterpret_cast<short*>(buffer);
      ReadBuffer( is, buf, numComp, pixelType, components );
      }
      break;

    case UINT:
      {
      unsigned int *buf = reinterpret_cast<unsigned int*>(buffer);
      ReadBuffer( is, buf, numComp, pixelType, components );
      }
      break;

    case INT:
      {
      int *buf = reinterpret_cast<int*>(buffer);
      ReadBuffer( is, buf, numComp, pixelType, components );
      }
      break;

    case ULONG:
      {
      unsigned long *buf = reinterpret_cast<unsigned long*>(buffer);
      ReadBuffer( is, buf, numComp, pixelType, components );
      }
      break;

    case LONG:
      {
      long *buf = reinterpret_cast<long*>(buffer);
      ReadBuffer( is, buf, numComp, pixelType, components );
      }
      break;

    case FLOAT:
      {
      float *buf = reinterpret_cast<float*>(buffer);
      ReadBuffer( is, buf, numComp, pixelType, components );
      }
      break;

    case DOUBLE:
      {
      double *buf = reinterpret_cast<double*>(buffer);
      ReadBuffer( is, buf, numComp, pixelType, components );
      }
      break;

    default:
      break;
    }
}

void VTKImageIO2::ReadSymmetricTensorBufferAsBinary( std::istream& is,
  void *buffer,
  StreamingImageIOBase::SizeType num )
{
  std::streamsize bytesRemaining = num;
  const SizeType componentSize = this->GetComponentSize();
  SizeType pixelSize = componentSize * 6;
  char zero[1024];
  memset( zero, 0, 1024 );

  if( this->GetNumberOfComponents() != 6 )
    {
    itkExceptionMacro(<< "Unsupported tensor dimension.");
    }
  while( bytesRemaining )
    {
    // row 1
    is.read( static_cast< char * >( buffer ), 3 * componentSize );
    buffer = static_cast< char * >( buffer ) + 3 * componentSize;
    // row 2
    is.seekg( componentSize, std::ios::cur );
    is.read( static_cast< char * >( buffer ), 2 * componentSize );
    buffer = static_cast< char * >( buffer ) + 2 * componentSize;
    // row 3
    is.seekg( 2 * componentSize, std::ios::cur );
    is.read( static_cast< char * >( buffer ), componentSize );
    buffer = static_cast< char * >( buffer ) + componentSize;
    bytesRemaining -= pixelSize;
    }

  if( is.fail() )
    {
    itkExceptionMacro(<< "Failure during writing of file.");
    }
}

void VTKImageIO2::Read(void *buffer)
{
  std::ifstream file;

  if ( this->RequestedToStream() )
    {
    itkAssertOrThrowMacro(m_FileType != ASCII, "Can not stream with ASCII type files");

    if( this->GetPixelType() == ImageIOBase::SYMMETRICSECONDRANKTENSOR )
      {
      itkExceptionMacro(<< "Cannot stream read binary second rank tensors.");
      }

    // open and stream read
    this->OpenFileForReading( file, this->m_FileName.c_str() );

    itkAssertOrThrowMacro(this->GetHeaderSize() != 0, "Header size is unknown when it shouldn't be!");
    this->StreamReadBufferAsBinary(file, buffer);
    }
  else
    {
    // open the file
    this->OpenFileForReading( file, this->m_FileName.c_str() );

    itkAssertOrThrowMacro(this->GetHeaderSize() != 0, "Header size is unknown when it shouldn't be!");

    if ( file.fail() )
      {
      itkExceptionMacro(<< "Failed seeking to data position");
      }

    // seek pass the header
    std::streampos dataPos = static_cast< std::streampos >( this->GetHeaderSize() );
    file.seekg(dataPos, std::ios::beg);

    //We are positioned at the data. The data is read depending on whether
    //it is ASCII or binary.
    if ( m_FileType == ASCII )
      {
      this->ReadBufferAsASCII( file, buffer, this->GetComponentType(),
                               this->GetImageSizeInComponents(),
                               this->GetPixelType(),
                               this->GetNumberOfComponents() );
      }
    else
      {
      // read the image
      if( this->GetPixelType() == ImageIOBase::SYMMETRICSECONDRANKTENSOR )
        {
        this->ReadSymmetricTensorBufferAsBinary( file, buffer, this->GetImageSizeInBytes() );
        }
      else
        {
        this->ReadBufferAsBinary( file, buffer, this->GetImageSizeInBytes() );
        }

      int size = this->GetComponentSize();
      switch ( size )
        {
        case 1:
          break;
        case 2:
          ByteSwapper< uint16_t >::SwapRangeFromSystemToBigEndian( (uint16_t *)buffer, this->GetImageSizeInComponents() );
          break;
        case 4:
          ByteSwapper< uint32_t >::SwapRangeFromSystemToBigEndian( (uint32_t *)buffer, this->GetImageSizeInComponents() );
          break;
        case 8:
          ByteSwapper< uint64_t >::SwapRangeFromSystemToBigEndian( (uint64_t *)buffer, this->GetImageSizeInComponents() );
          break;
        default:
          itkExceptionMacro(<< "Unknown component size" << size);
        }
      }
    }
}

void VTKImageIO2::ReadImageInformation()
{
  std::ifstream file;

  this->InternalReadImageInformation(file);
}

bool VTKImageIO2::CanWriteFile(const char *name)
{
  std::string filename = name;

  if ( filename != ""
       && filename.find(".vtk") < filename.length() )
    {
    return true;
    }
  return false;
}

void VTKImageIO2::WriteImageInformation( const void *itkNotUsed(buffer) )
{
  std::ofstream file;

  // this will truncate the file
  this->OpenFileForWriting(file, m_FileName.c_str(), true);

  // Check the image region for proper dimensions, etc.
  unsigned int numDims = this->GetNumberOfDimensions();
  if ( numDims < 1 || numDims > 3 )
    {
    itkExceptionMacro(<< "VTK Writer can only write 1, 2 or 3-dimensional images");
    return;
    }

  // Write the VTK header information
  file << "# vtk DataFile Version 3.0\n";
  file << "VTK File Generated by Insight Segmentation and Registration Toolkit (ITK)\n";

  if ( this->GetFileType() == ASCII )
    {
    file << "ASCII\n";
    }
  else
    {
    file << "BINARY\n";
    }

  file.setf(itksys_ios::ios::scientific,
            itksys_ios::ios::floatfield);

  file.precision(16);

  file.flush();

  // Write characteristics of the data
  file << "DATASET STRUCTURED_POINTS\n";
  file << "DIMENSIONS "
       << this->GetDimensions(0) << " "
       << ( ( this->GetNumberOfDimensions() > 1 ) ? this->GetDimensions(1) : 1 ) << " "
       << ( ( this->GetNumberOfDimensions() > 2 ) ? this->GetDimensions(2) : 1 ) << " "
       << "\n";
  file << "SPACING "
       << this->GetSpacing(0) << " "
       << ( ( this->GetNumberOfDimensions() > 1 ) ? this->GetSpacing(1) : 1.0 ) << " "
       << ( ( this->GetNumberOfDimensions() > 2 ) ? this->GetSpacing(2) : 1.0 ) << " "
       << "\n";
  file << "ORIGIN "
       << this->GetOrigin(0) << " "
       << ( ( this->GetNumberOfDimensions() > 1 ) ? this->GetOrigin(1) : 0.0 ) << " "
       << ( ( this->GetNumberOfDimensions() > 2 ) ? this->GetOrigin(2) : 0.0 ) << " "
       << "\n";

  file << "POINT_DATA " << this->GetImageSizeInPixels() << "\n";

  // NOTE: we don't write out RGB pixel types with ascii due to complication of
  // the required datatypes and the different ranges
  if ( ( ( this->GetPixelType() == RGB && this->GetNumberOfComponents() == 3 )
         || ( this->GetPixelType() == RGBA && this->GetNumberOfComponents() == 4 ) )
       &&
       ( this->GetComponentType() == UCHAR )
       && ( this->GetFileType() == Binary ) )
    {
    file << "COLOR_SCALARS color_scalars" << " "
         << this->GetNumberOfComponents() << "\n";
    }
  // Prefer the VECTORS representation when possible:
  else if ( this->GetPixelType() == VECTOR && this->GetNumberOfComponents() == 3 )
    {
    file << "VECTORS vectors "
         << this->GetComponentTypeAsString(m_ComponentType) << "\n";
    }
  else if( this->GetPixelType() == SYMMETRICSECONDRANKTENSOR )
    {
    file << "TENSORS tensors "
      << this->GetComponentTypeAsString(m_ComponentType) << "\n";
    }
  else
    {
    // According to VTK documentation number of components should in
    // range (1,4):
    // todo this should be asserted or checked ealier!
    file << "SCALARS scalars "
         << this->GetComponentTypeAsString(m_ComponentType) << " "
         << this->GetNumberOfComponents() << "\n"
         << "LOOKUP_TABLE default\n";
    }

  // set the header size based on how much we just wrote
  this->m_HeaderSize = static_cast< SizeType >( file.tellp() );
}

namespace {
template <class TComponent>
void WriteBuffer(std::ostream& os, const TComponent *buffer, const ImageIOBase::SizeType num,
  const ImageIOBase::IOPixelType pixelType, const ImageIOBase::SizeType components )
{
  const TComponent *ptr = buffer;
  typedef typename itk::NumericTraits<TComponent>::PrintType PrintType;

  if ( pixelType == ImageIOBase::SYMMETRICSECONDRANKTENSOR )
    {
    ImageIOBase::SizeType i=0;
    if( components == 3 )
      {
      PrintType zero( itk::NumericTraits<TComponent>::Zero );
      PrintType e12;
      while( i < num )
        {
        // row 1
        os << PrintType(*ptr++) << ' ';
        e12 = *ptr++;
        os << e12 << ' ';
        os << zero << '\n';
        // row 2
        os << e12 << ' ';
        os << PrintType(*ptr++) << ' ';
        os << zero << '\n';
        // row 3
        os << zero << ' ' << zero << ' ' << zero << "\n\n";
        i += 3;
        }
      }
    else if( components == 6 )
      {
      PrintType e12;
      PrintType e13;
      PrintType e23;
      while( i < num )
        {
        // row 1
        os << PrintType(*ptr++) << ' ';
        e12 = *ptr++;
        os << e12 << ' ';
        e13 = *ptr++;
        os << e13 << '\n';
        // row 2
        os << e12 << ' ';
        os << PrintType(*ptr++) << ' ';
        e23 = *ptr++;
        os << e23 << '\n';
        // row 3
        os << e13 << ' ';
        os << e23 << ' ';
        os << PrintType(*ptr++) << "\n\n";
        i += 6;
        }
      }
    else
      {
      ::itk::ExceptionObject e_(__FILE__, __LINE__,
        "itk::ERROR: VTKImageIO2: Unsupported number of components in tensor.",
        ITK_LOCATION);
      throw e_;
      }
    }
  else
    {
    // The behavior of ImageIOBase.
    for ( ImageIOBase::SizeType i = 0; i < num; i++ )
      {
      if ( !( i % 6 ) && i )
        {
        os << '\n';
        }
      os << PrintType(*ptr++) << ' ';
      }
    }
}
}
void VTKImageIO2::WriteBufferAsASCII(std::ostream& os, const void *buffer,
                                     IOComponentType ctype,
                                     const ImageIOBase::SizeType numComp,
                                     const ImageIOBase::IOPixelType pixelType,
                                     const ImageIOBase::SizeType components )
{
  switch (ctype)
    {
    case UCHAR:
      {
      typedef const unsigned char * Type;
      Type buf = reinterpret_cast<Type>(buffer);
      WriteBuffer(os, buf, numComp, pixelType, components);
      }
      break;
    case CHAR:
      {
      typedef const char * Type;
      Type buf = reinterpret_cast<Type>(buffer);
      WriteBuffer(os, buf, numComp, pixelType, components);
      }
      break;

    case USHORT:
      {
      typedef const unsigned short * Type;
      Type buf = reinterpret_cast<Type>(buffer);
      WriteBuffer(os, buf, numComp, pixelType, components);
      }
      break;

    case SHORT:
      {
      typedef const short * Type;
      Type buf = reinterpret_cast<Type>(buffer);
      WriteBuffer(os, buf, numComp, pixelType, components);
      }
      break;

    case UINT:
      {
      typedef const unsigned int * Type;
      Type buf = reinterpret_cast<Type>(buffer);
      WriteBuffer(os, buf, numComp, pixelType, components);
      }
      break;

    case INT:
      {
      typedef const int * Type;
      Type buf = reinterpret_cast<Type>(buffer);
      WriteBuffer(os, buf, numComp, pixelType, components);
      }
      break;

    case ULONG:
      {
      typedef const unsigned long * Type;
      Type buf = reinterpret_cast<Type>(buffer);
      WriteBuffer(os, buf, numComp, pixelType, components);
      }
      break;

    case LONG:
      {
      typedef const long * Type;
      Type buf = reinterpret_cast<Type>(buffer);
      WriteBuffer(os, buf, numComp, pixelType, components);
      }
      break;

    case FLOAT:
      {
      typedef const float * Type;
      Type buf = reinterpret_cast<Type>(buffer);
      WriteBuffer(os, buf, numComp, pixelType, components);
      }
      break;

    case DOUBLE:
      {
      typedef const double * Type;
      Type buf = reinterpret_cast<Type>(buffer);
      WriteBuffer(os, buf, numComp, pixelType, components);
      }
      break;

    default:
      break;
    }

}
void VTKImageIO2::WriteSymmetricTensorBufferAsBinary(std::ostream& os,
  const void *buffer,
  StreamingImageIOBase::SizeType num)
{
  std::streamsize bytesRemaining = num;
  const SizeType componentSize = this->GetComponentSize();
  SizeType pixelSize;
  char zero[1024];
  memset( zero, 0, 1024 );

  switch( this->GetNumberOfComponents() )
    {
  case 3:
      {
      pixelSize = componentSize * 3;
      while( bytesRemaining )
        {
        // row 1
        os.write( static_cast< const char * >( buffer ), 2 * componentSize );
        os.write( zero , componentSize );
        buffer = static_cast< const char * >( buffer ) + componentSize;
        // row 2
        os.write( static_cast< const char * >( buffer ), 2 * componentSize );
        buffer = static_cast< const char * >( buffer ) + 2 * componentSize;
        os.write( zero , componentSize );
        // row 3
        os.write( zero , 3 * componentSize );
        bytesRemaining -= pixelSize;
        }
      break;
      }
  case 6:
      {
      pixelSize = componentSize * 6;
      while( bytesRemaining )
        {
        // row 1
        os.write( static_cast< const char * >( buffer ), 3 * componentSize );
        buffer = static_cast< const char * >( buffer ) + componentSize;
        // row 2
        os.write( static_cast< const char * >( buffer ), componentSize );
        buffer = static_cast< const char * >( buffer ) + 2 * componentSize;
        os.write( static_cast< const char * >( buffer ), 2 * componentSize );
        buffer = static_cast< const char * >( buffer ) - componentSize;
        // row 3
        os.write( static_cast< const char * >( buffer ), componentSize );
        buffer = static_cast< const char * >( buffer ) + 2 * componentSize;
        os.write( static_cast< const char * >( buffer ), 2 * componentSize );
        buffer = static_cast< const char * >( buffer ) + 2 * componentSize;
        bytesRemaining -= pixelSize;
        }
      break;
      }
  default:
    itkExceptionMacro(<< "Unsupported tensor dimension.");
    break;
    }

  if( os.fail() )
    {
    itkExceptionMacro(<< "Failure during writing of file.");
    }
}

void VTKImageIO2::Write(const void *buffer)
{
  if ( this->RequestedToStream() )
    {
    itkAssertOrThrowMacro(m_FileType != ASCII, "Can not stream with ASCII type files");

    if( this->GetPixelType() == ImageIOBase::SYMMETRICSECONDRANKTENSOR )
      {
      itkExceptionMacro(<< "Cannot stream write binary second rank tensors.");
      }

    std::ofstream file;

    // we assume that GetActualNumberOfSplitsForWriting is called before
    // this methods and it will remove the file if a new header needs to
    // be written
    if ( !itksys::SystemTools::FileExists( m_FileName.c_str() ) )
      {
      this->WriteImageInformation(buffer);

      // open and stream write
      this->OpenFileForWriting(file, this->m_FileName.c_str(), false);

      // write one byte at the end of the file to allocate (this is a
      // nifty trick which should not write the entire size of the file
      // just allocate it, if the system supports sparse files)
      std::streampos seekPos = this->GetImageSizeInBytes() + this->GetHeaderSize() - 1;
      file.seekp(seekPos, std::ios::cur);
      file.write("\0", 1);
      file.seekp(0);
      }
    else
      {
      // must always recheck the header size incase something has changed
      std::ifstream ifile;
      this->ReadHeaderSize(ifile);

      itkAssertOrThrowMacro(this->GetHeaderSize() != 0, "Header size is unknown when it shouldn't be!");

      // open and stream write
      this->OpenFileForWriting(file, this->m_FileName.c_str(), false);
      }

    this->StreamWriteBufferAsBinary(file, buffer);
    }

  else
    {
    // this will truncate file and write header
    this->WriteImageInformation(buffer);

    std::ofstream file;
    // open the file
    this->OpenFileForWriting(file, this->m_FileName.c_str(), false);

    itkAssertOrThrowMacro(this->GetHeaderSize() != 0, "Header size is unknown when it shouldn't be!");

    // seek pass the header
    std::streampos dataPos = static_cast< std::streampos >( this->GetHeaderSize() );
    file.seekp(dataPos, std::ios::beg);

    if ( file.fail() )
      {
      itkExceptionMacro(<< "Failed seeking to data position");
      }

    // Write the actual pixel data
    if ( m_FileType == ASCII )
      {
      this->WriteBufferAsASCII( file, buffer, this->GetComponentType(),
                                this->GetImageSizeInComponents(),
                                this->GetPixelType(),
                                this->GetNumberOfComponents() );
      }
    else //binary
      {
      // the binary data must be written in big endian format
      if ( !ByteSwapper< uint16_t >::SystemIsBigEndian() )
        {
        // only swap  when needed
        const ImageIOBase::BufferSizeType numbytes =
          static_cast< ImageIOBase::BufferSizeType >( this->GetImageSizeInBytes() );

        char *tempmemory = new char[numbytes];
        memcpy(tempmemory, buffer, numbytes);
        switch ( this->GetComponentSize() )
          {
          case 1:
            break;
          case 2:
            ByteSwapper< uint16_t >::SwapRangeFromSystemToBigEndian( (uint16_t *)( tempmemory ),
                                                                     static_cast< BufferSizeType >( this->
                                                                                                    GetImageSizeInComponents() ) );
            break;
          case 4:
            ByteSwapper< uint32_t >::SwapRangeFromSystemToBigEndian( (uint32_t *)( tempmemory ),
                                                                     static_cast< BufferSizeType >( this->
                                                                                                    GetImageSizeInComponents() ) );
            break;
          case 8:
            ByteSwapper< uint64_t >::SwapRangeFromSystemToBigEndian( (uint64_t *)( tempmemory ),
                                                                     static_cast< BufferSizeType >( this->
                                                                                                    GetImageSizeInComponents() ) );
            break;
          }

        // write the image
        if( this->GetPixelType() == ImageIOBase::SYMMETRICSECONDRANKTENSOR )
          {
          this->WriteSymmetricTensorBufferAsBinary( file, tempmemory, this->GetImageSizeInBytes() );
          }
        else
          {
          if ( !this->WriteBufferAsBinary( file, tempmemory, this->GetImageSizeInBytes() ) )
            {
            itkExceptionMacro(<< "Could not write file: " << m_FileName);
            }
          }
        delete[] tempmemory;
        }
      else
        {
        // write the image
        if( this->GetPixelType() == ImageIOBase::SYMMETRICSECONDRANKTENSOR )
          {
          this->WriteSymmetricTensorBufferAsBinary( file, buffer, this->GetImageSizeInBytes() );
          }
        else
          {
          if ( !this->WriteBufferAsBinary( file, buffer, this->GetImageSizeInBytes() ) )
            {
            itkExceptionMacro(<< "Could not write file: " << m_FileName);
            }
          }
        }
      }
    }
}

void VTKImageIO2::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // end namespace itk
