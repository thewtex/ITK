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
#ifndef __itkCSVFileIO_cxx
#define __itkCSVFileIO_cxx

#include "itkCSVFileIO.h"
#include "itkArray2D.h"
#include "itkVector.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

namespace itk
{

//CSVFileParser Methods

/**Constructor */
CSVFileParser::CSVFileParser()
{
  this->m_FileName = "";
  this->m_FieldDelimiterCharacter = ',';
  this->m_StringDelimiterCharacter = '"';
  this->m_UseStringDelimiterCharacter = true;
  this->m_HaveColumnHeaders = true;
  this->m_HaveRowHeaders = true;
}

bool CSVFileParser::isLineWhiteSpace (std::string line)
{
  std::size_t i = 0;
  const char *s = line.c_str();
  for (; i < line.length();i++)
    {
    if (!isspace(s[i])) return false;
    }
  return true;
}

void CSVFileParser::ReadData()
{
  std::ifstream inputStream;
  inputStream.open(m_FileName.c_str(), std::ios::binary | std::ios::in);
  if ( inputStream.fail() )
    {
    ExceptionObject exception(__FILE__, __LINE__);
    std::string     message = "Can't open ";
    message += m_FileName;
    message += '\n';
    exception.SetDescription( message.c_str() );
    throw exception;
    }
  unsigned int i = 0;
  std::string line;
  std::string temp;
  std::string cell;

  while(std::getline(inputStream,line))
    {
    if (!isLineWhiteSpace (line))
      {
      this->m_Table.push_back(StringVector());
      std::stringstream linestream(line);

      while(std::getline(linestream,temp,this->m_FieldDelimiterCharacter))
        {
        if (m_UseStringDelimiterCharacter)
          {
          if (cell.empty())
            {
            if (temp[0] == this->m_StringDelimiterCharacter && temp[temp.length() - 1] == this->m_StringDelimiterCharacter)
              {
              std::string str = temp;
              str.assign(str,1,str.length()-2);
              this->m_Table[i].push_back(str);
              }
            if (temp[0] != this->m_StringDelimiterCharacter && temp[temp.length() - 1] != this->m_StringDelimiterCharacter)
              {
              this->m_Table[i].push_back(temp);
              }
            if (temp[0] == this->m_StringDelimiterCharacter && temp[temp.length() - 1] != this->m_StringDelimiterCharacter)
              {
              cell = temp;
              }
            }
          else
            {
            cell = cell + this->m_FieldDelimiterCharacter + temp;
            if (cell[0] == this->m_StringDelimiterCharacter && cell[cell.length() - 1] == this->m_StringDelimiterCharacter)
              {
              cell.assign(cell,1,cell.length()-2);
              this->m_Table[i].push_back(cell);
              cell.clear();
              }
            } //end else

          } //end if UseStringDelimiter

        else
          {
          m_Table[i].push_back(temp);
          }

        } //end inner while

      i++;
      } //end isLineWhitespace if

    } //end outer while
  inputStream.close();
}

// CSVDataFrameObject Methods

template <class TNumericData>
TNumericData
CSVDataFrameObject <TNumericData>
::StringToNumeric(const std::string str)
{
  TNumericData value;
  std::istringstream isstream(str);
  isstream >> value;

  if (isnan(value))
    {
    return std::numeric_limits<TNumericData>::quiet_NaN();
    }

  return value;
}

template <class TNumericData>
itk::Array2D <TNumericData>
CSVDataFrameObject <TNumericData>
::ParseData()
{
  this->ReadData();

  // Set the size of the matrix
  unsigned int rows = this->m_Table.size();
  unsigned int cols = this->m_Table[0].size();

  //Case 1: If both are on
  if(m_HaveColumnHeaders && m_HaveRowHeaders)
    {
    //push column headers
    for (unsigned int i = 1; i < cols; i++)
      {
      ColumnHeaders.push_back(m_Table[0][i]);
      }

    //push row headers
    for (unsigned int i = 1; i < rows; i++)
      {
      RowHeaders.push_back(m_Table[i][0]);
      }
    rows--; cols--;
    this->DataMatrix.SetSize(rows,cols);

    //put the numeric data in the matrix
    for (unsigned int i = 0; i < rows; i++)
      {
      for (unsigned int j = 0; j < cols; j++)
        {
        DataMatrix[i][j] = StringToNumeric (m_Table[i+1][j+1]);
        }
      }
    }

  // Case 2: If only Column Headers
  if (m_HaveColumnHeaders && !m_HaveRowHeaders)
    {
    //push column headers
    for (unsigned int i = 0; i < cols; i++)
      {
      ColumnHeaders.push_back(m_Table[0][i]);
      }
    rows--;
    this->DataMatrix.SetSize(rows,cols);
    //put the numeric data in the matrix
    for (unsigned int i = 0; i < rows; i++)
      {
      for (unsigned int j = 0; j < cols; j++)
        {
        DataMatrix[i][j] = StringToNumeric (m_Table[i+1][j]);
        }
      }
    }

  // Case 3: If only Row headers
  if (!m_HaveColumnHeaders && m_HaveRowHeaders)
    {
    //push the row headers
    for (unsigned int i = 0; i < rows; i++)
      {
      RowHeaders.push_back(m_Table[i][0]);
      }
    cols--;
    this->DataMatrix.SetSize(rows,cols);
    //put the numeric data in the matrix
    for (unsigned int i = 0; i < rows; i++)
      {
      for (unsigned int j = 0; j < cols; j++)
        {
        DataMatrix[i][j] = StringToNumeric (m_Table[i][j+1]);
        }
      }
    }

  //Case 4: If both are off
  if (!m_HaveColumnHeaders && !m_HaveRowHeaders)
    {
    this->DataMatrix.SetSize(rows,cols);
    //put the numeric data in the matrix
    for (unsigned int i = 0; i < rows; i++)
      {
      for (unsigned int j = 0; j < cols; j++)
        {
        DataMatrix[i][j] = StringToNumeric (m_Table[i][j]);
        }
      }
    }
  //delete the vector to free memory
  m_Table.clear();
  //std::cout << m_Table[0][0] << std::endl;
  return this->DataMatrix;
}

template <class TNumericData>
std::vector<TNumericData>
CSVDataFrameObject <TNumericData>
::GetRow(unsigned int row_index)
{
  NumericVectorType row;
  unsigned int max_rows = this->DataMatrix.rows() - 1;
  if (row_index > max_rows)
    {
    std::cerr << "Index exceeds matrix dimension!" << std::endl;
    return row;
    }
  unsigned int vector_size = this->DataMatrix.cols();
  for (unsigned int i = 0; i < vector_size; i++)
    {
    row.push_back(this->DataMatrix[row_index][i]);
    }
  return row;
}

template <class TNumericData>
std::vector<TNumericData>
CSVDataFrameObject <TNumericData>
::GetRow (std::string row_name)
{
  NumericVectorType row;

  if(! m_HaveRowHeaders)
    {
    std::cerr << "The dataset does not contain any row headers!" << std::endl;
    return row;
    }

  StringVector::iterator it;
  unsigned int index = 0;
  it = std::find(this->RowHeaders.begin(),this->RowHeaders.end(),row_name);
  index = distance(this->RowHeaders.begin(), it);

  if (it == this->RowHeaders.end())
    {
    std::cerr << "The row name " <<row_name <<" does not exist. " << std::endl;
    return row;
    }
  row = GetRow(index);
  return row;
}

template <class TNumericData>
std::vector<TNumericData>
CSVDataFrameObject <TNumericData>
::GetColumn(unsigned int column_index)
{
  NumericVectorType column;
  unsigned int max_columns = this->DataMatrix.columns() - 1;
  if (column_index > max_columns)
    {
    std::cerr << "Index exceeds matrix dimension!" << std::endl;
    return column;
    }
  unsigned int vector_size = this->DataMatrix.rows();
  for (unsigned int i = 0; i < vector_size; i++)
    {
    column.push_back(this->DataMatrix[i][column_index]);
    }
  return column;
}

template <class TNumericData>
std::vector<TNumericData>
CSVDataFrameObject <TNumericData>
::GetColumn (std::string column_name)
{
  NumericVectorType column;
  if(!m_HaveColumnHeaders)
    {
    std::cerr << "The dataset does not contain any column headers!" << std::endl;
    return column;
    }

  StringVector::iterator it;
  unsigned int index = 0;
  it = std::find(this->ColumnHeaders.begin(),this->ColumnHeaders.end(),column_name);
  index = distance(this->ColumnHeaders.begin(), it);

  if (it == this->ColumnHeaders.end())
    {
    std::cerr << "This column name " << column_name << " does not exist. " << std::endl;
    return column;
    }
  column = GetColumn(index);
  return column;
}

template <class TNumericData>
TNumericData
CSVDataFrameObject <TNumericData>
::GetData (unsigned int row, unsigned int col)
{
  if (row > this->DataMatrix.rows()-1 || col > this->DataMatrix.cols()-1 )
    {
    std::cerr << "Index exceeds matrix dimensions" << std::endl;
    return std::numeric_limits<TNumericData>::quiet_NaN();;
    }
  return this->DataMatrix[row][col];
}

template <class TNumericData>
TNumericData
CSVDataFrameObject <TNumericData>
::GetData (std::string row_name, std::string column_name)
{
  StringVector::iterator rit;
  unsigned int row_index = 0;
  rit = std::find(this->RowHeaders.begin(),this->RowHeaders.end(),row_name);
  row_index = distance(this->RowHeaders.begin(), rit);

  if (rit == this->RowHeaders.end())
    {
    std::cerr << "The row name " <<row_name <<" does not exist. " << std::endl;
    return std::numeric_limits<TNumericData>::quiet_NaN();;
    }

  StringVector::iterator cit;
  unsigned int col_index = 0;
  cit = std::find(this->ColumnHeaders.begin(),this->ColumnHeaders.end(),column_name);
  col_index = distance(this->ColumnHeaders.begin(), cit);

  if (cit == this->ColumnHeaders.end())
    {
    std::cerr << "The column name " <<column_name <<" does not exist. " << std::endl;
    return std::numeric_limits<TNumericData>::quiet_NaN();
    }

  return GetData(row_index, col_index);
}

template <class TNumericData>
TNumericData
CSVDataFrameObject <TNumericData>
::GetRowData (std::string row_name, unsigned int col_index)
{
  if(! m_HaveRowHeaders)
    {
    std::cerr << "The dataset does not contain any row headers!" << std::endl;
    return std::numeric_limits<TNumericData>::quiet_NaN();
    }

  StringVector::iterator rit;
  unsigned int row_index = 0;
  rit = std::find(this->RowHeaders.begin(),this->RowHeaders.end(),row_name);
  row_index = distance(this->RowHeaders.begin(), rit);

  if (rit == this->RowHeaders.end())
    {
    std::cerr << "The row name " <<row_name <<" does not exist. " << std::endl;
    return std::numeric_limits<TNumericData>::quiet_NaN();
    }

  return GetData (row_index, col_index);
}

template <class TNumericData>
TNumericData
CSVDataFrameObject <TNumericData>
::GetColumnData (std::string column_name, unsigned int row_index)
{
  if(!m_HaveColumnHeaders)
    {
    std::cerr << "The dataset does not contain any column headers!" << std::endl;
    return EXIT_FAILURE;
    }

  StringVector::iterator cit;
  unsigned int col_index = 0;
  cit = std::find(this->ColumnHeaders.begin(),this->ColumnHeaders.end(),column_name);
  col_index = distance(this->ColumnHeaders.begin(), cit);

  if (cit == this->ColumnHeaders.end())
    {
    std::cerr << "The column name " << column_name <<" does not exist. " << std::endl;
    return EXIT_FAILURE;
    }

  return GetData (row_index, col_index);
}

template <class TNumericData>
void
CSVDataFrameObject <TNumericData>
::DisplayData()
{
  std::cout <<"\t\t";
  if (m_HaveColumnHeaders)
    {
    for (unsigned int i = 0; i < ColumnHeaders.size(); i++)
      {
      std::cout << ColumnHeaders[i] << "\t" ;
      }
    std::cout << std::endl;
    }

  for (unsigned int i = 0; i < DataMatrix.rows(); i++)
    {
    if (m_HaveRowHeaders)
      {
      std::cout << RowHeaders[i] <<"\t";
      }
    for (unsigned int j = 0; j < DataMatrix.cols(); j++)
      {
      std::cout << DataMatrix[i][j] <<"\t";
      }
    std::cout << std::endl;
    }
}


} //end namespace itk

#endif
