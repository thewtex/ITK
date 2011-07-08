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
#ifndef __itkCSVArray2DFileReader_txx
#define __itkCSVArray2DFileReader_txx

#include "itkCSVArray2DFileReader.h"

#include "itksys/SystemTools.hxx"
#include <vcl_limits.h>

namespace itk
{

template <class TData>
CSVArray2DFileReader<TData>
::CSVArray2DFileReader()
{
  this->m_Array2DDataObject = Array2DDataObjectType::New();
}

template <class TData>
void
CSVArray2DFileReader<TData>
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << this->m_Array2DDataObject << std::endl;
}

template <class TData>
void
CSVArray2DFileReader <TData>
::Parse()
{
  size_t rows = 0;
  size_t columns = 0;
  this->m_InputStream.seekg(0);

  // Get the data dimension and set the matrix size
  this->GetDataDimension(rows,columns);
  this->m_Array2DDataObject->SetMatrixSize(rows,columns);

  /** initialize the matrix to NaN so that missing data will automatically be
   *  set to this value. */
  this->m_Array2DDataObject->FillMatrix(vcl_numeric_limits<TData>::quiet_NaN());

  std::string entry;

  // Get the Column Headers if there are any.
  if ( this->m_HasColumnHeaders )
    {
    this->m_Array2DDataObject->HasColumnHeadersOn();

    // push the entries into the column headers vector.
    for (unsigned int i = 0; i < columns+1; i++)
      {
      this->GetNextField(entry);
      this->m_Array2DDataObject->ColumnHeadersPushBack(entry);
      if ( this->m_Line.empty() )
        {
        break;
        }
      }

    /** if there are row headers, get rid of the first entry in the column
     *  headers as it will just be the name of the table. */
    if ( this->m_HasRowHeaders )
      {
      this->m_Array2DDataObject->EraseFirstColumnHeader();
      }
    }

  // Get the rest of the data
  for (unsigned int i = 0; i < rows; i++)
    {
    // if there are row headers, push them into the vector for row headers
    if ( this->m_HasRowHeaders )
      {
      this->m_Array2DDataObject->HasRowHeadersOn();
      this->GetNextField(entry);
      this->m_Array2DDataObject->RowHeadersPushBack(entry);
      }

    // parse the numeric data into the Array2D object
    for (unsigned int j = 0; j < columns; j++)
      {
      this->GetNextField(entry);
      this->m_Array2DDataObject->SetMatrixData(i,j,
                                             this->ConvertStringToValueType<TData>(entry));

      /** if the file contains missing data, m_Line will contain less data
       * fields. So we check if m_Line is empty and if it is, we break out of
       * this loop and move to the next line. */
      if ( this->m_Line.empty() )
        {
        break;
        }
      }
    }

  std::cout << "The file parsed successfully!" << std::endl;
  this->m_InputStream.close();
}

/** Update method */
template<class TData>
void
CSVArray2DFileReader<TData>
::Update()
{
  this->Parse();
}

/** Get the output */
template <class TData>
typename CSVArray2DFileReader<TData>::Array2DDataObjectPointer
CSVArray2DFileReader<TData>
::GetOutput()
{
  return this->GetArray2DDataObject();
}


} //end namespace itk

#endif
