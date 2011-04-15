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
#ifndef __itkCSVArray2DDataFrameObject_txx
#define __itkCSVArray2DDataFrameObject_txx

#include "itkCSVArray2DDataFrameObject.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

namespace itk
{
template <class TData>
CSVArray2DDataFrameObject<TData>
::CSVArray2DDataFrameObject()
{
  this->m_HasRowHeaders = false;
  this->m_HasColumnHeaders = false;
}

template <class TData>
const typename CSVArray2DDataFrameObject<TData>::NumericVectorType
CSVArray2DDataFrameObject <TData>
::GetRow(const unsigned int row_index)
{
  NumericVectorType row;
  unsigned int max_rows = m_Matrix.rows() - 1;
  if (row_index > max_rows)
    {
    itkExceptionMacro(<<" Row index exceeds matrix dimension!");
    }
  unsigned int vector_size = m_Matrix.cols();
  for (unsigned int i = 0; i < vector_size; i++)
    {
    row.push_back(m_Matrix[row_index][i]);
    }
  return row;
}

template <class TData>
const typename CSVArray2DDataFrameObject<TData>::NumericVectorType
CSVArray2DDataFrameObject<TData>
::GetRow (const std::string row_name)
{
  NumericVectorType row;

  if (! this->m_HasRowHeaders)
    {
    itkExceptionMacro( << "The dataset does not contain any row headers!");
    }

  typename StringVectorType::iterator it;
  unsigned int index = 0;
  it = std::find(this->m_RowHeaders.begin(),this->m_RowHeaders.end(),row_name);
  index = distance(this->m_RowHeaders.begin(), it);

  if (it == this->m_RowHeaders.end())
    {
    itkExceptionMacro( << "The row name " <<row_name <<" does not exist. ");
    }
  row = GetRow(index);
  return row;
}

template <class TData>
const typename CSVArray2DDataFrameObject<TData>::NumericVectorType
CSVArray2DDataFrameObject <TData>
::GetColumn(const unsigned int column_index)
{
  NumericVectorType column;
  unsigned int max_columns = m_Matrix.columns() - 1;
  if (column_index > max_columns)
    {
    itkExceptionMacro( << "Index exceeds matrix dimension!");
    }
  unsigned int vector_size = this->m_Matrix.rows();
  for (unsigned int i = 0; i < vector_size; i++)
    {
    column.push_back(m_Matrix[i][column_index]);
    }
  return column;
}

template <class TData>
const typename CSVArray2DDataFrameObject<TData>::NumericVectorType
CSVArray2DDataFrameObject <TData>
::GetColumn (const std::string column_name)
{
  NumericVectorType column;
  if (! this->m_HasColumnHeaders)
    {
    itkExceptionMacro( << "The dataset does not contain any column headers!");
    }

  typename StringVectorType::iterator it;
  unsigned int index = 0;
  it = std::find(this->m_ColumnHeaders.begin(),this->m_ColumnHeaders.end(),column_name);
  index = distance(this->m_ColumnHeaders.begin(), it);

  if (it == this->m_ColumnHeaders.end())
    {
    itkExceptionMacro( << "The column name " << column_name << " does not exist. " );
    }
  column = GetColumn(index);
  return column;
}


template <class TData>
const TData
CSVArray2DDataFrameObject <TData>
::GetData (const unsigned int row, unsigned int col)
{
  if (row > m_Matrix.rows()-1 || col > m_Matrix.cols()-1 )
    {
    itkExceptionMacro( << "Index exceeds matrix dimensions" );
    }
  return m_Matrix[row][col];
}

template <class TData>
const TData
CSVArray2DDataFrameObject <TData>
::GetData (const std::string row_name, const std::string column_name)
{
  typename StringVectorType::iterator rit;
  unsigned int row_index = 0;
  rit = std::find(this->m_RowHeaders.begin(),this->m_RowHeaders.end(),row_name);
  row_index = distance(this->m_RowHeaders.begin(), rit);

  if (rit == this->m_RowHeaders.end())
    {
    itkExceptionMacro( << "The row name " <<row_name <<" does not exist. " );
    }

  typename StringVectorType::iterator cit;
  unsigned int col_index = 0;
  cit = std::find(this->m_ColumnHeaders.begin(),this->m_ColumnHeaders.end(),column_name);
  col_index = distance(this->m_ColumnHeaders.begin(), cit);

  if (cit == this->m_ColumnHeaders.end())
    {
    itkExceptionMacro( << "The column name " <<column_name <<" does not exist. " );
    }

  return GetData(row_index, col_index);
}


template <class TData>
const TData
CSVArray2DDataFrameObject <TData>
::GetRowData (const std::string row_name, const unsigned int col_index)
{
  if (! this->m_HasRowHeaders)
    {
    itkExceptionMacro( << "The dataset does not contain any row headers!" );
    }

  typename StringVectorType::iterator rit;
  unsigned int row_index = 0;
  rit = std::find(this->m_RowHeaders.begin(),this->m_RowHeaders.end(),row_name);
  row_index = distance(this->m_RowHeaders.begin(), rit);

  if (rit == this->m_RowHeaders.end())
    {
    itkExceptionMacro( << "The row name " <<row_name <<" does not exist. " );
    }

  return GetData (row_index, col_index);
}

template <class TData>
const TData
CSVArray2DDataFrameObject <TData>
::GetColumnData (const std::string column_name, const unsigned int row_index)
{
  if (! this->m_HasColumnHeaders)
    {
    itkExceptionMacro( << "The dataset does not contain any column headers!" );
    }

  typename StringVectorType::iterator cit;
  unsigned int col_index = 0;
  cit = std::find(this->m_ColumnHeaders.begin(),this->m_ColumnHeaders.end(),column_name);
  col_index = distance(this->m_ColumnHeaders.begin(), cit);

  if (cit == this->m_ColumnHeaders.end())
    {
    itkExceptionMacro( << "The column name " << column_name <<" does not exist. " );
    }

  return GetData (row_index, col_index);
}

template<class TData>
const TData
CSVArray2DDataFrameObject <TData>
::operator() (const unsigned int row_index, unsigned int column_index)
{
  return GetData(row_index, column_index);
}

template<class TData>
const TData
CSVArray2DDataFrameObject <TData>
::operator() (const std::string row_name, const std::string column_name)
{
  return GetData(row_name, column_name);
}

template <class TData>
void
CSVArray2DDataFrameObject <TData>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "Number of rows in matrix: " << this->m_Matrix.rows() << std::endl;
  os << "Number of columns in matrix: " << this->m_Matrix.cols() << std::endl << std::endl;
  os << "Column Headers existence: " << this->m_HasColumnHeaders << std::endl;
  os << "Row Headers existence: " << this->m_HasRowHeaders << std::endl;

  os << "Below is the data: " << std::endl << std::endl;

  if (this->m_HasColumnHeaders)
    {
    os << indent << indent ;
    for (unsigned int i = 0; i < m_ColumnHeaders.size(); i++)
      {
      os << this->m_ColumnHeaders[i] << indent ;
      }
    os << std::endl;
    }

  for (unsigned int i = 0; i < m_Matrix.rows(); i++)
    {
    if (this->m_HasRowHeaders)
      {
      os << this->m_RowHeaders[i] << indent;
      }
    for (unsigned int j = 0; j < m_Matrix.cols(); j++)
      {
      os << this->m_Matrix[i][j] << indent;
      }
    os << std::endl;
    }
}

template <class TData>
void
CSVArray2DDataFrameObject<TData>
::SetMatrixSize(unsigned int rows, unsigned int columns)
{
  m_Matrix.SetSize(rows,columns);
}

template <class TData>
void
CSVArray2DDataFrameObject<TData>
::SetMatrixData(unsigned int row_index, unsigned int column_index, TData item)
{
  m_Matrix[row_index][column_index] = item;
}

template <class TData>
void
CSVArray2DDataFrameObject<TData>
::RowHeadersPushBack(std::string & header)
{
  m_RowHeaders.push_back (header);
}

template <class TData>
void
CSVArray2DDataFrameObject<TData>
::ColumnHeadersPushBack(std::string & header)
{
  m_ColumnHeaders.push_back (header);
}

template <class TData>
void
CSVArray2DDataFrameObject<TData>
::EraseFirstColumnHeader()
{
  m_ColumnHeaders.erase (m_ColumnHeaders.begin() );
}

template <class TData>
void
CSVArray2DDataFrameObject<TData>
::FillMatrix (TData value)
{
  this->m_Matrix.Fill(value);
}


} //end namespace itk

#endif
