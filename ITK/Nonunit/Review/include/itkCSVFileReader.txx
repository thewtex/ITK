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
#ifndef __itkCSVFileReader_txx
#define __itkCSVFileReader_txx

#include "itkCSVFileReader.h"
#include <vcl_limits.h>

#include <fstream>
#include <sstream>
#include <iostream>

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
void
CSVFileReader<TData>
::PrintSelf(std::ostream & os, Indent indent) const
{
  os << indent << "File Name: " << m_FileName << std::endl;
  os << indent << "FieldDelimiterCharacter: " << m_FieldDelimiterCharacter << std::endl;
  os << indent << "StringDelimier Character: " << m_StringDelimiterCharacter << std::endl;
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

  //Open the file for reading (first time)
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

  // check if column headers exist
  if (this->m_HaveColumnHeaders)
    {
    // first line of the file should be parsed into m_ColumnHeaders.
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

  // Count the number of entries in each line.
  while(std::getline(inputStream,line))
    {
    cols = 0;
    std::stringstream linestream(line);

    // If row headers exist, move past (but do not count) the row header in each line.
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

    // Count the numeric entries
    while(std::getline(linestream,cell, this->m_FieldDelimiterCharacter))
      {
      cols++;
      }
    rows++;

    // Determine the max #columns and #rows
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

  // Set the size of the Array2D accordingly
  this->m_Matrix.SetSize(rows,max_cols);

  // Close file
  inputStream.close();

  // Open the file again for reading
  inputStream.open(m_FileName.c_str(), std::ios::binary | std::ios::in);
  if ( inputStream.fail() )
    {
    std::string     message = "Can't open ";
    message += m_FileName;
    message += '\n';
    itkExceptionMacro(<< message );
    }


  // If column headers exist, the file should be read from the second line
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

    // If row headers exist, then they should be parsed into m_RowHeaders
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

    // Parse the numeric data
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

} //end namespace itk

#endif
