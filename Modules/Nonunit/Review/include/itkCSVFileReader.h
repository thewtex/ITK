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

#ifndef __itkCSVFileReader_h
#define __itkCSVFileReader_h

#include "itkLightProcessObject.h"
#include "itkMacro.h"
#include "itkArray2D.h"
#include "itkCSVDataFrameObject.h"
#include <vector>

namespace itk
{
/** \class itkCSVFileReader is used to read and parse CSV files into an
  * Array2D object.
  *
  * The data is converted to some user specified numeric type. If the data contains
  * Column and Row Headers, a vector of strings is maintained for each of these. Typically,
  * the user would instantiate a pointer to a reader. The reader parses
  * the data directly into a CSVDataFrameObject through use of the Parse() function. Much like
  * retrieving an image using an ImageFileReader, the data frame object
  * can be retrieved by calling GetDataFrameObject().
  *
  * One of the main uses of this class is to provide a way to read datasets
  * obtained from R. The files need not necessarily contain only homogenous data. There is a
  * check made on whether or not the data contains the same number of entries
  * in each row, and if not, a warning is printed to the user stating that
  * data is missing and that missing data will be set to NaN. The Array2D object
  * would contain the number of columns equal to the maximum number of entries found in a row.
  *
  * If the data contains a row which has more entries than the number of column headers, extra
  * column headers are created. For example, if there are 3 column headers and there exists a
  * row with 4 entries, then another header named "Col 4" is created.
    *
  * One thing to note is that the file that is read in is assumed to be correct according
  * to the user. So for example, if there is a line, which is just white space, between two
  * rows of data, there will be a row in the data matrix that will all be set to NaN. Similarly
  * it is for the user to specify if there are any row and/or column headers in the file by setting
  * the boolean variables, m_HasRowHeaders and m_HasColumnHeaders. Not doing so could result in
  * setting the header names to NaN and hence including them as part of the matrix in the data set.
  *
  * The method Parse() reads the file twice. Since,
  * an arbitrary number of rows and columns would exist in the file,
  * the number of lines and entries are counted first because the Array2D
  * object is not dynamically extendible. Once the number of rows and columns
  * has been determined, the file is read a second time and the data is parsed
  * into the Array2D object.
  * The use of a string delimiter is also supported. The field delimiter character
  * is used to delimit row and column headers while the string delimiter is used to account
  * for fields that may have the field delimiter character in them. For example, if the field delimiter
  * character, as by default, is set to ',', and suppose there are fields that have names such as
  * "this,field" then the string delimiter character should be set to '"'.
  * The boolean variable m_UseStringDelimiter can and should be used to specify
  * if any of the Column or Row Headers contain commas within the field
  * names.
  *
  */

template <class TData>
class CSVFileReader:public LightProcessObject
{
public:
 /** Standard class typedefs */
 typedef CSVFileReader             Self;
 typedef LightProcessObject        Superclass;
 typedef SmartPointer<Self>        Pointer;
 typedef SmartPointer<const Self>  ConstPointer;

 /** Standard New method. */
 itkNewMacro(Self);

 /** Run-time type information (and related methods) */
 itkTypeMacro(Self,Superclass);

 /* String vector types  */
 typedef std::vector <std::string>     StringVectorType;

 /** DataFrame Object type */
 typedef typename itk::CSVDataFrameObject<TData> DataFrameObjectType;
 typedef typename DataFrameObjectType::Pointer   DFOPointerType;

 typedef typename itk::Array2D<TData> MatrixType;

 /** Set Macros for members */
 itkSetStringMacro(FileName);
 itkSetMacro(FieldDelimiterCharacter,char);
 itkSetMacro(StringDelimiterCharacter,char);
 itkSetMacro(UseStringDelimiterCharacter,bool);
 itkSetMacro(HasRowHeaders,bool);
 itkSetMacro(HasColumnHeaders,bool);
 itkBooleanMacro(HasRowHeaders);
 itkBooleanMacro(HasColumnHeaders);
 itkBooleanMacro(UseStringDelimiterCharacter);

 itkGetMacro(DataFrameObject,DFOPointerType);

 /** Function to convert string to a numeric type */
 TData StringToNumeric (const std::string);

 /** Function for parsing the data. It returns a data frame object smart pointer */
 void Parse();

 /** Print the reader */
 void PrintSelf(std::ostream & os, Indent indent) const;

 CSVFileReader ();
 virtual ~CSVFileReader () {}

private:
 std::string                  m_FileName;
 char                         m_FieldDelimiterCharacter;
 char                         m_StringDelimiterCharacter;
 bool                         m_UseStringDelimiterCharacter;
 bool                         m_HasRowHeaders;
 bool                         m_HasColumnHeaders;
 DFOPointerType               m_DataFrameObject;
};

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCSVFileReader.txx"
#endif

#endif
