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

#include "itkCSVFileIO.h"
#include "itkArray2D.h"
#include "itkVector.h"
#include <vcl_limits.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

namespace itk
{

/**Constructor */
template <class TData>
CSVFileReader<TData>
::CSVFileReader()
{
  this->m_FileName = "";
  this->m_FieldDelimiterCharacter = ',';
  this->m_StringDelimiterCharacter = '"';
  this->m_HaveColumnHeaders = true;
  this->m_HaveRowHeaders = true;
  this->m_UseStringDelimiterCharacter = (this->m_HaveColumnHeaders or this->m_HaveRowHeaders);
}

template <class TData>
bool
CSVFileReader <TData>
::isLineWhiteSpace (std::string line)
{
  std::size_t i = 0;
  const char *s = line.c_str();
  for (; i < line.length();i++)
    {
    if (!isspace(s[i])) return false;
    }
  return true;
}

template <class TData>
TData
CSVFileReader <TData>
::StringToNumeric(const std::string str)
{
  TData value;
  std::istringstream isstream(str);
  isstream >> value;

  size_t isstreamtellg = static_cast<size_t> ( isstream.tellg() );

  return (!isstream.fail() || isstreamtellg == str.length() ) ?
    value : vcl_numeric_limits<TData>::quiet_NaN();
}

template <class TData>
void
CSVFileReader <TData>
::Parse()
{
  if (this->m_FileName == "")
    {
    itkExceptionMacro(<< "A filename was not specified!" );
    }

  //OPEN THE FILE FOR READING (FIRST TIME)
  std::ifstream inputStream;
  inputStream.open(m_FileName.c_str(), std::ios::binary | std::ios::in);
  if ( inputStream.fail() )
    {
    std::string     message = "Can't open ";
    message += m_FileName;
    message += '\n';
    itkExceptionMacro(<< message );
    }

  std::string line;
  std::string cell;
  unsigned int rows = 0;
  unsigned int cols = 0;

  unsigned int prev_cols = 0;
  unsigned int current_cols = 0;
  unsigned int max_cols = 0;
  bool isSame = true;
  int second_line_position = 0;

  // IF COLUMN HEADERS EXIST
  if (this->m_HaveColumnHeaders)
    {
    // FIRST LINE OF THE FILE SHOULD BE PARSED INTO m_ColumnHeaders.
    std::getline(inputStream,line);
    second_line_position = inputStream.tellg();
    std::stringstream linestream(line);

    // If using StringDelimiter, Need to account for commas in the header names.
    // Simply delimit using the string delimiter character and parse every entry
    // for which cellnum is odd, into Column Headers.
    if (this->m_UseStringDelimiterCharacter)
      {
      int cellnum = 0;
      while(std::getline(linestream,cell,this->m_StringDelimiterCharacter))
        {
        if ( cellnum%2 != 0)
          {
          this->m_ColumnHeaders.push_back ( cell );
          }
        cellnum++;
        }
      }

    // if not using StringDelimiter, just parse into ColumnHeaders
    else
      {
      while(std::getline(linestream,cell,this->m_FieldDelimiterCharacter))
        {
        this->m_ColumnHeaders.push_back(cell);
        }
      }

    // Get rid of the first entry in the column headers
    m_ColumnHeaders.erase(m_ColumnHeaders.begin() );

    prev_cols = m_ColumnHeaders.size();
    max_cols = prev_cols;
    }

  // COUNT THE NUMBER OF ENTRIES IN EACH LINE.
  while(std::getline(inputStream,line))
    {
    cols = 0;
    std::stringstream linestream(line);

    // IF ROW HEADERS EXIST, MOVE PAST (BUT DO NOT COUNT) THE ROW HEADER IN EACH LINE
    if (this->m_HaveRowHeaders)
      {
      if(this->m_UseStringDelimiterCharacter)
        {
        for (int k = 0; k < 2; k++)
          {
          std::getline(linestream,cell,this->m_StringDelimiterCharacter);
          }
        std::getline(linestream,cell, this->m_FieldDelimiterCharacter);
        }
      else
        {
        std::getline(linestream,cell, this->m_FieldDelimiterCharacter);
        }
      }

    // COUNT THE NUMERIC ENTRIES
    while(std::getline(linestream,cell, this->m_FieldDelimiterCharacter))
      {
      cols++;
      }
    rows++;

    // DETERMINE THE MAX #COLUMNS AND #ROWS
    current_cols = cols;
    if (!m_HaveColumnHeaders && rows == 1)
      {
      prev_cols = cols;
      max_cols = cols;
      }
    if (current_cols != prev_cols)
      {
      isSame = false;
      if (current_cols > max_cols) max_cols = current_cols;
      prev_cols = current_cols;
      }
    }

  // If not consistent, display a warning to the user.
  if (!isSame)
    {
    std::cout << "Warning: Data appears to contain missing data!";
    std::cout << "These will be set to NaN!";
    std::cout << std::endl;
    }
  // if same
  else
    {
    max_cols = cols;
    }

  // If the number of entries in a row exceeds the number of column headers,
  // then a column header should be set for that column (?).
  if (this->m_HaveColumnHeaders)
    {
    unsigned int num_col_headers = this->m_ColumnHeaders.size();
    if (max_cols > num_col_headers)
      {
      std::string col_name = "Col ";
      std::string num;
      for (unsigned int k = num_col_headers; k < max_cols; k++)
        {
        std::stringstream int2string;
        int2string << (k+1);
        num = int2string.str();
        col_name = col_name+num;
        this->m_ColumnHeaders.push_back ( col_name );
        col_name = "Col ";
        }
      }
    }

  std::cout << "#Rows = " << rows << std::endl;
  std::cout << "#Cols = " << max_cols << std::endl;

  // SET THE SIZE OF THE ARRAY2D ACCORDINGLY
  this->m_Matrix.SetSize(rows,max_cols);

  // CLOSE FILE
  inputStream.close();

  // OPEN THE FILE AGAIN FOR READING
  inputStream.open(m_FileName.c_str(), std::ios::binary | std::ios::in);
  if ( inputStream.fail() )
    {
    std::string     message = "Can't open ";
    message += m_FileName;
    message += '\n';
    itkExceptionMacro(<< message );
    }


  // IF COLUMN HEADERS EXIST, THE FILE SHOULD BE READ FROM THE SECOND LINE
  if (this->m_HaveColumnHeaders)
    {
    inputStream.seekg(second_line_position);
    }

  unsigned int i = 0;
  unsigned int j = 0;
  while(std::getline(inputStream,line))
    {
    std::stringstream linestream(line);
    j = 0;

    // IF ROW HEADERS EXIST, THEN THEY SHOULD BE PARSED INTO m_RowHeaders
    if (this->m_HaveRowHeaders)
      {
      // if the string delimiter character is used, it should be accounted for
      if (this->m_UseStringDelimiterCharacter)
        {
        for (int k = 0; k < 2; k++)
          {
          std::getline(linestream,cell,this->m_StringDelimiterCharacter);
          }
        this->m_RowHeaders.push_back(cell);
        }

      else
        {
        std::getline(linestream,cell,this->m_FieldDelimiterCharacter);
        this->m_RowHeaders.push_back( cell );
        }
       std::getline(linestream,cell,this->m_FieldDelimiterCharacter);
      }

    // PARSE THE NUMERIC DATA
    while(std::getline(linestream,cell,this->m_FieldDelimiterCharacter))
      {
      this->m_Matrix[i][j] = this->StringToNumeric(cell);
      j++;
      }

    // set missing data to NaN
    for (;j < this->m_Matrix.cols();j++)
      {
      m_Matrix[i][j] = vcl_numeric_limits<TData>::quiet_NaN();
      }
    i++;
    }
  inputStream.close();
}

template <class TData>
void
CSVDataFrameObject <TData>
::GetReaderData(CSVFileReader<TData>* reader)
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

/* As of now, the csv file writer can write 2D itk objects such as
itkMatrix, itkArray2D and vnl_matrix to a csv file */

template <class TObjectType>
void
CSVFileWriter <TObjectType>
::Write ()
{
  if (m_InputObject == 0 )
    {
    itkExceptionMacro(<< "No input to writer!");
    }

  if (m_FileName == "")
    {
    itkExceptionMacro(<< "No filename was specified!");
    }


  std::ofstream outputStream(m_FileName.c_str(), std::ios::app);
  if ( outputStream.fail() )
    {
    std::string     message = "Can't open ";
    message += m_FileName;
    message += '\n';
    itkExceptionMacro(<< message );
    }

  // cast to vnl_matrix type
  typedef vnl_matrix<double> MatrixType;
  MatrixType* matrixptr = new MatrixType;
  matrixptr = dynamic_cast<MatrixType*> (m_InputObject);

  for (unsigned int i = 0; i < matrixptr->rows(); i++)
   {
   for (unsigned int j = 0; j < matrixptr->cols(); j++)
     {
     outputStream << (*matrixptr)[i][j];
     if (j < matrixptr->cols() - 1)
       {
       outputStream << ",";
       }
     }
   outputStream << std::endl;
   }

  outputStream.close();
 }


} //end namespace itk

#endif
