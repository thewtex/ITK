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
/*
OME Bio-Formats ITK plugin for calling Bio-Formats from the Insight Toolkit.
Copyright (c) 2008, UW-Madison LOCI.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the UW-Madison LOCI nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY UW-MADISON LOCI ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL UW-MADISON LOCI BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
IMPORTANT NOTE: Although this specific file is distributed according to a
"BSD-style" license and the Apache 2 license, it requires to be linked to the
OME Bio-Formats Java library at run-time to do anything useful. The OME
Bio-Formats Java library is licensed under the GPL v2 or later.  Therefore, if
you wish to distribute this software in binary form with Bio-Formats itself,
your combined binary work must be distributed under the terms of the GPL v2
or later license.
*/

/*
Adapted from the Slicer3 project: http://www.slicer.org/
http://viewvc.slicer.org/viewcvs.cgi/trunk/Libs/MGHImageIO/

See slicer-license.txt for Slicer3's licensing information.
*/

// Special thanks to Alex Gouaillard, Sebastien Barre,
// Luis Ibanez and Jim Miller for fixes and suggestions.

#include <fstream>

#include "itkBioFormatsImageIO.h"
#include "itkIOCommon.h"
#include "itkExceptionObject.h"

#include <cmath>

#include <stdio.h>
#include <stdlib.h>

#if defined (_WIN32)
#define PATHSTEP ';'
#define SLASH '\\'
#else
#define PATHSTEP ':'
#define SLASH '/'
#endif

//--------------------------------------
//
// BioFormatsImageIO
//

namespace itk {

BioFormatsImageIO::BioFormatsImageIO()
{
  itkDebugMacro("BioFormatsImageIO constructor");

  this->m_FileType = Binary;

  try
    {
    jace::StaticVmLoader* tmpLoader = (jace::StaticVmLoader*)jace::helper::getVmLoader();

    if( tmpLoader == NULL )
      {

      // initialize the Java virtual machine
      itkDebugMacro("Creating JVM...");
      jace::OptionList list;
      jace::StaticVmLoader loader(JNI_VERSION_1_4);

      const char name[] = "ITK_AUTOLOAD_PATH";
      const char* namePtr;
      namePtr = name;
      char * path;
      path = getenv(name);
      std::string dir("");

      if( path == NULL)
        {
        itkExceptionMacro("ITK_AUTOLOAD_PATH is not set, you must set this environment variable and point it to the directory containing the bio-formats.jar file");
        }

      dir.assign(path);

      if( dir.at(dir.length() - 1) != SLASH )
        {
        dir.append(1,SLASH);
        }

      std::string classpath = dir+"jace-runtime.jar";

      classpath += PATHSTEP+dir+"bio-formats.jar";
      classpath += PATHSTEP+dir+"loci_tools.jar";

      list.push_back(jace::ClassPath( classpath ) );

      list.push_back(jace::CustomOption("-Xcheck:jni"));
      list.push_back(jace::CustomOption("-Xmx256m"));
      list.push_back(jace::CustomOption("-Djava.awt.headless=true"));
      list.push_back(jace::CustomOption("-Djava.library.path=" + dir));
      jace::helper::createVm(loader, list, false);
      itkDebugMacro("JVM created.");
      }
    }
  catch ( jace::JNIException & jniException )
    {
    itkExceptionMacro("Exception creating JVM: " << jniException.what());
    }

  try
    {
    itkDebugMacro("Creating Bio-Formats objects...");
    m_Reader = m_ImageReader = new jace::proxy::loci::formats::ImageReader;
    m_Reader = m_ChannelFiller = new jace::proxy::loci::formats::ChannelFiller(*m_Reader);

    m_ChannelSeparator = NULL;
    m_Reader = m_ChannelMerger = new jace::proxy::loci::formats::ChannelMerger(*m_Reader);
    m_Writer = new jace::proxy::loci::formats::ImageWriter;
    itkDebugMacro("Created m_Reader and m_Writer.");
    }
  catch ( jace::proxy::java::lang::Exception & e )
    {
    itkExceptionMacro("A Java error occurred: " << jace::proxy::loci::common::DebugTools::getStackTrace(e));
    }
  catch ( jace::JNIException & jniException )
    {
    itkExceptionMacro("A JNI error occurred: " << jniException.what());
    }
  catch (std::exception& e)
    {
    itkExceptionMacro("A C++ error occurred: " << e.what());
    }
}

BioFormatsImageIO::~BioFormatsImageIO()
  {
  if (m_ImageReader != NULL)
    {
    delete m_ImageReader;
    m_ImageReader = NULL;
    }
  if (m_ChannelFiller != NULL)
    {
    delete m_ChannelFiller;
    m_ChannelFiller = NULL;
    }
  if (m_ChannelSeparator != NULL)
    {
    delete m_ChannelSeparator;
    m_ChannelSeparator = NULL;
    }
  if (m_ChannelMerger != NULL)
    {
    delete m_ChannelMerger;
    m_ChannelMerger = NULL;
    }
  if (m_Writer != NULL)
    {
    delete m_Writer;
    m_Writer = NULL;
    }
}

bool BioFormatsImageIO::CanReadFile(const char* FileNameToRead)
  {
  itkDebugMacro( "BioFormatsImageIO::CanReadFile: FileNameToRead = " << FileNameToRead);

  std::string filename(FileNameToRead);

  if (filename == "")
    {
    itkDebugMacro("A file name must be specified.");
    return false;
    }

  bool isType = 0;
  try
    {
    // call Bio-Formats to check file type
    isType = ((jace::proxy::loci::formats::IFormatHandler*) m_Reader)->isThisType(filename);
    itkDebugMacro("isType = " << isType);
    }
  catch ( jace::proxy::java::lang::Exception & e)
    {
    itkExceptionMacro("A Java error occurred: " << jace::proxy::loci::common::DebugTools::getStackTrace(e));
    }
  catch ( jace::JNIException & jniException )
    {
    itkExceptionMacro("A JNI error occurred: " << jniException.what());
    }
  catch (std::exception& e)
    {
    itkExceptionMacro("A C++ error occurred: " << e.what());
    }
  return isType;
}

void BioFormatsImageIO::ReadImageInformation()
{
  itkDebugMacro( "BioFormatsImageIO::ReadImageInformation: m_FileName = " << m_FileName);

  try
    {
    // attach OME metadata object
    jace::proxy::loci::formats::meta::IMetadata omeMeta =
      jace::proxy::loci::formats::MetadataTools::createOMEXMLMetadata();

    m_Reader->setMetadataStore(omeMeta);

    // initialize dataset
    itkDebugMacro("Initializing...");
    m_Reader->setId(m_FileName);
    itkDebugMacro("Initialized.");

    int seriesCount = m_Reader->getSeriesCount();
    itkDebugMacro("Series count = " << seriesCount);

    // set ITK byte order
    bool little = m_Reader->isLittleEndian();
    if (little)
      {
      SetByteOrderToLittleEndian(); // m_ByteOrder
      }
    else
      {
      SetByteOrderToBigEndian(); // m_ByteOrder
      }

    // set ITK component type
    int pixelType = m_Reader->getPixelType();
    itkDebugMacro("Bytes per pixel = " << jace::proxy::loci::formats::FormatTools::getBytesPerPixel(pixelType) );
    IOComponentType itkComponentType;
    if (pixelType == jace::proxy::loci::formats::FormatTools::UINT8())
      {
      itkComponentType = UCHAR;
      }
    else if (pixelType == jace::proxy::loci::formats::FormatTools::INT8())
      {
      itkComponentType = CHAR;
      }
    else if (pixelType == jace::proxy::loci::formats::FormatTools::UINT16())
      {
      itkComponentType = USHORT;
      }
    else if (pixelType == jace::proxy::loci::formats::FormatTools::INT16())
      {
      itkComponentType = SHORT;
      }
    else if (pixelType == jace::proxy::loci::formats::FormatTools::UINT32())
      {
      itkComponentType = UINT;
      }
    else if (pixelType == jace::proxy::loci::formats::FormatTools::INT32())
      {
      itkComponentType = INT;
      }
    else if (pixelType == jace::proxy::loci::formats::FormatTools::FLOAT())
      {
      itkComponentType = FLOAT;
      }
    else if (pixelType == jace::proxy::loci::formats::FormatTools::DOUBLE())
      {
      itkComponentType = DOUBLE;
      }
    else
      {
      itkComponentType = UNKNOWNCOMPONENTTYPE;
      }

    SetComponentType(itkComponentType); // m_ComponentType

    if (itkComponentType == UNKNOWNCOMPONENTTYPE)
      {
      itkExceptionMacro("Unknown pixel type: " << pixelType);
      }

    // get pixel resolution and dimensional extents
    int sizeX = m_Reader->getSizeX();
    int sizeY = m_Reader->getSizeY();
    int sizeZ = m_Reader->getSizeZ();
    int sizeC = m_Reader->getSizeC();
    int sizeT = m_Reader->getSizeT();
    int effSizeC = m_Reader->getEffectiveSizeC();
    int rgbChannelCount = m_Reader->getRGBChannelCount();
    int imageCount = m_Reader->getImageCount();

    itkDebugMacro("Dimensional extents:" << std::endl
      << "\tSizeX = " << sizeX << std::endl
      << "\tSizeY = " << sizeY << std::endl
      << "\tSizeZ = " << sizeZ << std::endl
      << "\tSizeC = " << sizeC << std::endl
      << "\tSizeT = " << sizeT << std::endl
      << "\tRGB Channel Count = " << rgbChannelCount << std::endl
      << "\tEffective SizeC = " << effSizeC << std::endl
      << "\tImage Count = " << imageCount);

    // NB: Always return 5D, to be unambiguous.
    int numDims = 5;

    SetNumberOfDimensions(numDims);
    m_Dimensions[0] = sizeX;
    m_Dimensions[1] = sizeY;

    m_Dimensions[2] = sizeZ;
    m_Dimensions[3] = sizeT;
    m_Dimensions[4] = effSizeC;

    // set ITK pixel type
    IOPixelType itkPixelType;

    if (rgbChannelCount == 1)
      {
      itkPixelType = SCALAR;
      }
    else if (rgbChannelCount == 3)
      {
      itkPixelType = RGB;
      }
    else
      {
      itkPixelType = VECTOR;
      }

    SetPixelType(itkPixelType); // m_PixelType
    SetNumberOfComponents(rgbChannelCount); // m_NumberOfComponents

    // Get physical resolution

    // CTR - Review invalid memory access error on some systems (OS X 10.5)
    jace::proxy::loci::formats::meta::MetadataRetrieve retrieve =
      jace::proxy::loci::formats::MetadataTools::asRetrieve(omeMeta);

   double physX = retrieve.getPixelsPhysicalSizeX(0).doubleValue();
   if (physX == 0.0)
     {
     physX = 1.0;
     }
   itkDebugMacro("After physX = " << physX);

   double physY = retrieve.getPixelsPhysicalSizeY(0).doubleValue();
   if ( physY == 0.0 )
      {
      physY = 1.0;
      }
   itkDebugMacro("After physY = " << physY);

   double physZ = retrieve.getPixelsPhysicalSizeZ(0).doubleValue();
   if ( physZ == 0.0 )
     {
     physZ = 1.0;
     }

   m_Spacing[0] = physX;
   m_Spacing[1] = physY;

   // TODO: verify m_Spacing.length > 2

   if ( imageCount > 1 )
     {
     m_Spacing[2] = physZ;
     }
   itkDebugMacro("After physZ = " << physZ);

   itkDebugMacro("Physical resolution = " << physX << " x " << physY << " x " << physZ);

   double timeIncrement = 1.0;
   jace::proxy::java::lang::Double timeIncrementDouble = retrieve.getPixelsTimeIncrement(0);
   if ( !timeIncrementDouble.isNull() )
     {
     timeIncrement = retrieve.getPixelsTimeIncrement(0).doubleValue();
     }

   m_Spacing[3] = timeIncrement;

   itkDebugMacro("Physical resolution = " << physX << " x " << physY
                 << " x " << physZ << " x " << timeIncrement);
    }
  catch ( jace::proxy::java::lang::Exception & e )
    {
    itkExceptionMacro("A Java error occurred: " << jace::proxy::loci::common::DebugTools::getStackTrace(e));
    }
  catch ( jace::JNIException & jniException )
    {
    itkExceptionMacro("A JNI error occurred: " << jniException.what());
    }
  catch (std::exception& e)
    {
    itkExceptionMacro("A C++ error occurred: " << e.what());
    }
}

void BioFormatsImageIO::Read(void* pData)
{
  itkDebugMacro("BioFormatsImageIO::Read");

  try
    {
    const int pixelType = m_Reader->getPixelType();
    const int bpp = jace::proxy::loci::formats::FormatTools::getBytesPerPixel(pixelType);
    const int rgbChannelCount = m_Reader->getRGBChannelCount();

    itkDebugMacro("Pixel type:" << std::endl
      << "Pixel type code = " << pixelType << std::endl
      << "Bytes per pixel = " << bpp << std::endl
      << "RGB channel count = " << rgbChannelCount);

    // check IO region to identify the planar extents desired
    ImageIORegion region = GetIORegion();
    int regionDim = region.GetImageDimension();
    int xStart = 0, xCount = 1;
    int yStart = 0, yCount = 1;
    int zStart = 0, zCount = 1;
    int tStart = 0, tCount = 1;
    int cStart = 0, cCount = 1;

    int xIndex = 0;
    int yIndex = 1;
    int zIndex = 2;
    int tIndex = 3;
    int cIndex = 4;

    for (int dim = 0; dim < regionDim; dim++)
      {
      int index = region.GetIndex(dim);
      int size = region.GetSize(dim);
      if (dim == xIndex)
        {
        xStart = index;
        xCount = size;
        }
      else if (dim == yIndex)
        {
        yStart = index;
        yCount = size;
        }
      else if (dim == zIndex)
        {
        zStart = index;
        zCount = size;
        }
      else if (dim == tIndex)
        {
        tStart = index;
        tCount = size;
        }
      else if (dim == cIndex)
        {
        cStart = index;
        cCount = size;
        }
      }

    const int bytesPerPlane = xCount * yCount * bpp * rgbChannelCount;
    const bool isInterleaved = m_Reader->isInterleaved();

    itkDebugMacro("Region extents:" << std::endl
      << "\tRegion dimension = " << regionDim << std::endl
      << "\tX: start = " << xStart << ", count = " << xCount << std::endl
      << "\tY: start = " << yStart << ", count = " << yCount << std::endl
      << "\tZ: start = " << zStart << ", count = " << zCount << std::endl
      << "\tT: start = " << tStart << ", count = " << tCount << std::endl
      << "\tC: start = " << cStart << ", count = " << cCount << std::endl
      << "\tBytes per plane = " << bytesPerPlane << std::endl
      << "\tIsInterleaved = " << isInterleaved);


    itkDebugMacro(<< "Image count = " << m_Reader->getImageCount() );

    // allocate temporary array
    const bool canDoDirect = (rgbChannelCount == 1 || isInterleaved);

    jbyte * tmpData = NULL;

    if ( !canDoDirect )
      {
      tmpData = new jbyte[bytesPerPlane];
      }

    jbyte* jData = (jbyte*) pData;
    ByteArray buf(bytesPerPlane); // pre-allocate buffer

    for (int c=cStart; c<cStart+cCount; c++)
      {
      for (int t=tStart; t<tStart+tCount; t++)
        {
        for (int z=zStart; z<zStart+zCount; z++)
          {
          int no = m_Reader->getIndex(z, c, t);
          itkDebugMacro("Reading image plane " << no + 1
            << " (Z=" << z << ", T=" << t << ", C=" << c << ")"
            << " of " << m_Reader->getImageCount() << " available planes)");

          m_Reader->openBytes(no, buf, xStart, yStart, xCount, yCount);

          JNIEnv* env = jace::helper::attach();
          jbyteArray jArray = static_cast<jbyteArray>(buf.getJavaJniArray());

          if (canDoDirect)
            {
            env->GetByteArrayRegion(jArray, 0, bytesPerPlane, jData);
            }
          else
            {
            // need to reorganize byte array after copy
            env->GetByteArrayRegion(jArray, 0, bytesPerPlane, tmpData);

            // reorganize elements
            int pos = 0;
            for (int x=0; x<xCount; x++)
              {
              for (int y=0; y<yCount; y++)
                {
                for (int i=0; i<rgbChannelCount; i++)
                  {
                  for (int b=0; b<bpp; b++)
                    {
                    int index = yCount * (xCount * (rgbChannelCount * b + i) + x) + y;
                    jData[pos++] = tmpData[index];
                    }
                  }
                }
              }
            }
          jData += bytesPerPlane;
          }
        }
      }

    // delete temporary array
    if (tmpData != NULL)
      {
      delete tmpData;
      tmpData = NULL;
      }

    ((jace::proxy::loci::formats::IFormatHandler*)m_Reader)->close();
    }
  catch ( jace::proxy::java::lang::Exception & e )
    {
    itkExceptionMacro("A Java error occurred: " << jace::proxy::loci::common::DebugTools::getStackTrace(e));
    }
  catch ( jace::JNIException & jniException )
    {
    itkExceptionMacro("A JNI error occurred: " << jniException.what());
    }
  catch (std::exception& e)
    {
    itkExceptionMacro("A C++ error occurred: " << e.what());
    }
  itkDebugMacro("Done.");
}

bool BioFormatsImageIO::CanWriteFile(const char* name)
{
  itkDebugMacro("BioFormatsImageIO::CanWriteFile: name = " << name);

  std::string filename(name);

  if (filename == "")
    {
    itkDebugMacro("A FileName must be specified.");
    return false;
    }

  bool isType = 0;

  try
    {
    // call Bio-Formats to check file type
    jace::proxy::loci::formats::ImageWriter writer;
    isType = writer.isThisType(filename);
    itkDebugMacro("isType = " << isType);
    }
  catch ( jace::proxy::java::lang::Exception & e )
    {
    itkExceptionMacro("A Java error occurred: " << jace::proxy::loci::common::DebugTools::getStackTrace(e));
    }
  catch ( jace::JNIException & jniException )
    {
    itkExceptionMacro("A JNI error occurred: " << jniException.what());
    }
  catch (std::exception& e)
    {
    itkExceptionMacro("A C++ error occurred: " << e.what());
    }

  // The Bioformats library provides implementations for file writing,
  // however, they have not been exposed here yet. Therefore, we return
  // false here until such functionalities are implemented in the Write()
  // and WriteImageInformation().
  //
  // Once the implementations are fixed, we should return "isType" instead.
  //
  return false;
}

void BioFormatsImageIO::WriteImageInformation()
{
  itkDebugMacro("BioFormatsImageIO::WriteImageInformation");
  // NB: Nothing to do.
}

void BioFormatsImageIO::Write(const void * itkNotUsed(buffer) )
{
  itkDebugMacro("BioFormatsImageIO::Write");
  // CTR TODO - implement Write function
}

} // end namespace itk
