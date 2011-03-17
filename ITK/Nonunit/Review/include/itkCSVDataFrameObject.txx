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
#ifndef __itkCSVFileIO_txx
#define __itkCSVFileIO_txx

#include "itkCSVDataFrameObject.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

namespace itk
{

template <class TData>
void
CSVDataFrameObject <TData>
::GetReaderData(CSVFileReaderType* reader)
{
  this->m_Matrix            = reader->GetMatrix();
  this->m_ColumnHeaders     = reader->GetColumnHeaders();
  this->m_RowHeaders        = reader->GetRowHeaders();
  this->m_HaveColumnHeaders = reader->GetHaveColumnHeaders();
  this->m_HaveRowHeaders    = reader->GetHaveRowHeaders();
}

template <class TData>
const std::vector<TData>
CSVDataFrameObject <TData>
::GetRow(const unsigned int row_index)
{
  NumericVectorType row;
  unsigned int max_rows = this->m_Matrix.rows() - 1;
  if (row_index > max_rows)
    {
    std::cerr << "Index exceeds matrix dimension!" << std::endl;
    return row;
    }
  unsigned int vector_size = this->m_Matrix.cols();
  for (unsigned int i = 0; i < vector_size; i++)
    {
    row.push_back(this->m_Matrix[row_index][i]);
    }
  return row;
}

template <class TData>
const std::vector<TData>
CSVDataFrameObject <TData>
::GetRow (const std::string row_name)
{
  NumericVectorType row;

  if (! this->m_HaveRowHeaders)
    {
    std::cerr << "The dataset does not contain any row headers!" << std::endl;
    return row;
    }

  typename StringVectorType::iterator it;
  unsigned int index = 0;
  it = std::find(this->m_RowHeaders.begin(),this->m_RowHeaders.end(),row_name);
  index = distance(this->m_RowHeaders.begin(), it);

  if (it == this->m_RowHeaders.end())
    {
    std::cerr << "The row name " <<row_name <<" does not exist. " << std::endl;
    return row;
    }
  row = GetRow(index);
  return row;
}

template <class TData>
const std::vector<TData>
CSVDataFrameObject <TData>
::GetColumn(const unsigned int column_index)
{
  NumericVectorType column;
  unsigned int max_columns = this->m_Matrix.columns() - 1;
  if (column_index > max_columns)
    {
    std::cerr << "Index exceeds matrix dimension!" << std::endl;
    return column;
    }
  unsigned int vector_size = this->m_Matrix.rows();
  for (unsigned int i = 0; i < vector_size; i++)
    {
    column.push_back(this->m_Matrix[i][column_index]);
    }
  return column;
}

template <class TData>
const std::vector<TData>
CSVDataFrameObject <TData>
::GetColumn (const std::string column_name)
{
  NumericVectorType column;
  if (! this->m_HaveColumnHeaders)
    {
    std::cerr << "The dataset does not contain any column headers!" << std::endl;
    return column;
    }

  typename StringVectorType::iterator it;
  unsigned int index = 0;
  it = std::find(this->m_ColumnHeaders.begin(),this->m_ColumnHeaders.end(),column_name);
  index = distance(this->m_ColumnHeaders.begin(), it);

  if (it == this->m_ColumnHeaders.end())
    {
    std::cerr << "The column name " << column_name << " does not exist. " << std::endl;
    return column;
    }
  column = GetColumn(index);
  return column;
}


template <class TData>
const TData
CSVDataFrameObject <TData>
::GetData (const unsigned int row, unsigned int col)
{
  if (row > this->m_Matrix.rows()-1 || col > this->m_Matrix.cols()-1 )
    {
    std::cerr << "Index exceeds matrix dimensions" << std::endl;
    return vcl_numeric_limits<TData>::quiet_NaN();
    }
  return this->m_Matrix[row][col];
}

template <class TData>
const TData
CSVDataFrameObject <TData>
::GetData (const std::string row_name, const std::string column_name)
{
  typename StringVectorType::iterator rit;
  unsigned int row_index = 0;
  rit = std::find(this->m_RowHeaders.begin(),this->m_RowHeaders.end(),row_name);
  row_index = distance(this->m_RowHeaders.begin(), rit);

  if (rit == this->m_RowHeaders.end())
    {
    std::cerr << "The row name " <<row_name <<" does not exist. " << std::endl;
    return vcl_numeric_limits<TData>::quiet_NaN();
    }

  typename StringVectorType::iterator cit;
  unsigned int col_index = 0;
  cit = std::find(this->m_ColumnHeaders.begin(),this->m_ColumnHeaders.end(),column_name);
  col_index = distance(this->m_ColumnHeaders.begin(), cit);

  if (cit == this->m_ColumnHeaders.end())
    {
    std::cerr << "The column name " <<column_name <<" does not exist. " << std::endl;
    return vcl_numeric_limits<TData>::quiet_NaN();
    }

  return GetData(row_index, col_index);
}


template <class TData>
const TData
CSVDataFrameObject <TData>
::GetRowData (const std::string row_name, const unsigned int col_index)
{
  if (! this->m_HaveRowHeaders)
    {
    std::cerr << "The dataset does not contain any row headers!" << std::endl;
    return vcl_numeric_limits<TData>::quiet_NaN();
    }

  typename StringVectorType::iterator rit;
  unsigned int row_index = 0;
  rit = std::find(this->m_RowHeaders.begin(),this->m_RowHeaders.end(),row_name);
  row_index = distance(this->m_RowHeaders.begin(), rit);

  if (rit == this->m_RowHeaders.end())
    {
    std::cerr << "The row name " <<row_name <<" does not exist. " << std::endl;
    return vcl_numeric_limits<TData>::quiet_NaN();
    }

  return GetData (row_index, col_index);
}

template <class TData>
const TData
CSVDataFrameObject <TData>
::GetColumnData (const std::string column_name, const unsigned int row_index)
{
  if (! this->m_HaveColumnHeaders)
    {
    std::cerr << "The dataset does not contain any column headers!" << std::endl;
    return EXIT_FAILURE;
    }

  typename StringVectorType::iterator cit;
  unsigned int col_index = 0;
  cit = std::find(this->m_ColumnHeaders.begin(),this->m_ColumnHeaders.end(),column_name);
  col_index = distance(this->m_ColumnHeaders.begin(), cit);

  if (cit == this->m_ColumnHeaders.end())
    {
    std::cerr << "The column name " << column_name <<" does not exist. " << std::endl;
    return EXIT_FAILURE;
    }

  return GetData (row_index, col_index);
}

template<class TData>
const TData
CSVDataFrameObject <TData>
::operator() (const unsigned int row_index, unsigned int column_index)
{
  return GetData(row_index, column_index);
}

template<class TData>
const TData
CSVDataFrameObject <TData>
::operator() (const std::string row_name, const std::string column_name)
{
  return GetData(row_name, column_name);
}

template <class TData>
void
CSVDataFrameObject <TData>
::DisplayData()
{
  std::cout <<"\t\t";
  if (this->m_HaveColumnHeaders)
    {
    for (unsigned int i = 0; i < this->m_ColumnHeaders.size(); i++)
      {
      std::cout << this->m_ColumnHeaders[i] << "\t" ;
      }
    std::cout << std::endl;
    }

  for (unsigned int i = 0; i < this->m_Matrix.rows(); i++)
    {
    if (this->m_HaveRowHeaders)
      {
      std::cout << this->m_RowHeaders[i] <<"\t";
      }
    for (unsigned int j = 0; j < this->m_Matrix.cols(); j++)
      {
      std::cout << this->m_Matrix[i][j] <<"\t";
      }
    std::cout << std::endl;
    }
}

} //end namespace itk

#endif
