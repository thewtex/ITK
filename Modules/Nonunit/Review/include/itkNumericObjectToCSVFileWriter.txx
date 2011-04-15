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
#ifndef __itkNumericObjectToCSVFileWriter_txx
#define __itkNumericObjectToCSVFileWriter_txx

#include "itkNumericObjectToCSVFileWriter.h"
#include <fstream>
#include <iomanip>


namespace itk
{
template <class TValueType, unsigned int NRows, unsigned int NColumns>
NumericObjectToCSVFileWriter<TValueType,NRows, NColumns>
::NumericObjectToCSVFileWriter()
{
  this->m_FieldDelimiterCharacter = ',';
}

template <class TValueType, unsigned int NRows, unsigned int NColumns>
void
NumericObjectToCSVFileWriter<TValueType,NRows,NColumns>
::SetInput (vnlMatrixType* obj)
{
  this->m_InputObject = obj->data_block();
  this->m_rows = obj->rows();
  this->m_cols = obj->cols();
}

template <class TValueType, unsigned int NRows, unsigned int NColumns>
void
NumericObjectToCSVFileWriter<TValueType,NRows,NColumns>
::SetInput (vnlFixedMatrixType* obj)
{
  this->m_InputObject = obj->data_block();
  this->m_rows = obj->rows();
  this->m_cols = obj->cols();
}

template <class TValueType, unsigned int NRows, unsigned int NColumns>
void
NumericObjectToCSVFileWriter<TValueType,NRows,NColumns>
::SetInput (itkMatrixType* obj)
{
  this->m_InputObject = obj->GetVnlMatrix().data_block() ;
  this->m_rows = obj->RowDimensions;
  this->m_cols = obj->ColumnDimensions;
}

template <class TValueType, unsigned int NRows, unsigned int NColumns>
void
NumericObjectToCSVFileWriter<TValueType,NRows,NColumns>
::ColumnHeadersPushBack(const std::string & header)
{
  this->m_ColumnHeaders.push_back (header);
}

template <class TValueType, unsigned int NRows, unsigned int NColumns>
void
NumericObjectToCSVFileWriter<TValueType,NRows,NColumns>
::RowHeadersPushBack(const std::string & header)
{
  this->m_RowHeaders.push_back (header);
}

template <class TValueType, unsigned int NRows, unsigned int NColumns>
void
NumericObjectToCSVFileWriter<TValueType,NRows,NColumns>
::SetVectorAsColumnHeaders (const StringVectorType & v)
{
  this->m_ColumnHeaders = v;
}

template <class TValueType, unsigned int NRows, unsigned int NColumns>
void
NumericObjectToCSVFileWriter<TValueType,NRows,NColumns>
::SetVectorAsRowHeaders (const StringVectorType & v)
{
  this->m_RowHeaders = v;
}


template <class TValueType,unsigned int NRows, unsigned int NColumns>
void
NumericObjectToCSVFileWriter<TValueType,NRows,NColumns>
::PrepareForWriting()
{
  // throw an exception if no input object is provided
  if (this->m_InputObject == 0 )
    {
    itkExceptionMacro(<< "No input to writer!");
    }

  // throw an exception if no filename is provided
  if (m_FileName == "")
    {
    itkExceptionMacro(<< "No filename was specified!");
    }

  // output a warning if the number of row headers and number of rows in the object are not the same
  if (!this->m_RowHeaders.empty() && (this->m_RowHeaders.size() != this->m_rows))
    {
    std::cerr << "Warning: The number of row headers and the number of rows in the input object is not consistent." << std::endl;
    }
}

template <class TValueType, unsigned int NRows, unsigned int NColumns>
void
NumericObjectToCSVFileWriter<TValueType,NRows,NColumns>
::Write ()
{
  this->PrepareForWriting();

  std::ofstream outputStream(this->m_FileName.c_str(), std::ios::app);
  if ( outputStream.fail() )
    {
    std::string     message = "Can't open ";
    message += m_FileName;
    message += '\n';
    itkExceptionMacro(<< message );
    }

  try
    {
    if (!this->m_ColumnHeaders.empty())
      {
      for (unsigned int i = 0; i < this->m_ColumnHeaders.size();i++)
        {
        outputStream << this->m_ColumnHeaders[i];
        if (i < this->m_ColumnHeaders.size() - 1)
          {
          outputStream << this->m_FieldDelimiterCharacter ;
          }
        }
      outputStream << std::endl;
      }
    for (unsigned int i = 0; i < this->m_rows; i++)
      {
      if(!this->m_RowHeaders.empty())
        {
        if (i < this->m_RowHeaders.size() )
          {
          outputStream << this->m_RowHeaders[i] << this->m_FieldDelimiterCharacter;
          }
        }

      for (unsigned int j = 0; j < this->m_cols; j++)
        {
        outputStream << std::setprecision ( vcl_numeric_limits<TValueType>::digits10 )
                     << *(m_InputObject++);
        if (j < m_cols - 1)
          {
          outputStream << this->m_FieldDelimiterCharacter;
          }
        }
      outputStream << std::endl;
      }
    outputStream.close();
    }
  catch (itk::ExceptionObject& exp)
    {
    std::cerr << "Exception caught! " << std::endl;
    std::cerr << exp << std::endl;
    }
  outputStream.close();
}

template <class TValueType, unsigned int NRows, unsigned int NColumns>
void
NumericObjectToCSVFileWriter<TValueType,NRows,NColumns>
::PrintSelf (std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "File name: " << m_FileName << std::endl;
  os << indent << "Field Delimiter Character: " << m_FieldDelimiterCharacter << std::endl;
}


} //end namespace itk

#endif
