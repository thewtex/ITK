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

#ifndef __itkCSVFileIO_h
#define __itkCSVFileIO_h

#include "itkLightProcessObject.h"
#include "itkMacro.h"
#include "itkArray2D.h"
#include "itkVector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matrix_fixed.h"
#include "itkMatrix.h"
#include <vector>

namespace itk
{
/** \class itkCSVFileReader is used to read and parse CSV files into an
  * Array2D object. The method Parse() reads the file twice. Since,
  * an arbitrary number of rows and columns would exist in the file,
  * the number of lines and entries are counted first because the Array2D
  * object is not dynamically extendible. Once the number of rows and columns
  * has been determined, the file is read a second time and the data is parsed
  * into the Array2D object.The use of a string delimiter is also supported and
  * so if fields in the dataset contain commas,they would need to accounted for.
  * The boolean variable m_UseStringDelimiter can and should be specified
  * by the user if any of the Column or Row Headers contain commas within the field
  * names.
  *
  * The data is converted to some user specified numeric type if the data contains
  * Column and Row Headers, a vector is maintained for each of these. The Array2D
  * object can then be accessed by the CSVDataFrameObject class (below) and can be used to
  * access rows and columns as requested by the user.
  *
  * One of the main uses of this class is to provide a way to read datasets
  * obtained from R, which for the most part, should be homogeneous. There is a
  * check made on whether or not the data contains the same number of entries
  * in each row, and if not, a warning is printed to the user stating that
  * data is missing and that missing data will be set to NaN. The Array2D object
  * would contain the number of columns equal to the maximum number of entries found in a row.
  * If the data contains a row which has more entries than the number of column headers, extra
  * column headers are created. For example, if there are 3 column headers and there exists a
  * row with 4 entries, then another header named "Col 4" is created.
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

 /** Macros to set member variables */
 itkSetStringMacro(FileName);
 itkSetMacro(FieldDelimiterCharacter,char);
 itkSetMacro(StringDelimiterCharacter,char);
 itkSetMacro(HaveColumnHeaders,bool);
 itkSetMacro(HaveRowHeaders,bool);
 itkSetMacro(UseStringDelimiterCharacter,bool);

 itkBooleanMacro(HaveColumnHeaders);
 itkBooleanMacro(HaveRowHeaders);
 itkBooleanMacro(UseStringDelimiterCharacter)

 /* User specified numeric type */
 typedef TData                                  NumericType;

 /* String vector types  */
 typedef std::vector <std::string>              StringVectorType;

 /* Array2D is the itk object that will hold the numeric data */
 typedef typename itk::Array2D <NumericType>    MatrixType;

 /** Get member macros */
 itkGetStringMacro(FileName);
 itkGetMacro(FieldDelimiterCharacter,char);
 itkGetMacro(StringDelimiterCharacter,char);
 itkGetMacro(UseStringDelimiterCharacter,bool);
 itkGetMacro(Matrix,MatrixType);
 itkGetMacro(ColumnHeaders, StringVectorType);
 itkGetMacro(RowHeaders, StringVectorType);
 itkGetMacro(HaveRowHeaders,bool);
 itkGetMacro(HaveColumnHeaders,bool);



 /** Function to check if  a line is whitespace */
 bool isLineWhiteSpace (std::string line);

 /* Function to convert string to a numeric type */
 NumericType StringToNumeric (const std::string);

 /* Function for parsing the data and creating the vectors for the headers */
 void Parse();

 CSVFileReader ();
 virtual ~CSVFileReader () {}

private:
 std::string            m_FileName;
 char                   m_FieldDelimiterCharacter;
 char                   m_StringDelimiterCharacter;
 bool                   m_UseStringDelimiterCharacter;
 MatrixType             m_Matrix;
 StringVectorType       m_ColumnHeaders;
 StringVectorType       m_RowHeaders;
 bool                   m_HaveRowHeaders;
 bool                   m_HaveColumnHeaders;
};

/** \class DataFrameObject contains Get Functions for either accessing
  * data from the Array2D object or from the row and column headers.
  */

template <class TData>
class CSVDataFrameObject:public LightProcessObject
{
public:
 /* Standard class typedefs */
 typedef CSVDataFrameObject       Self;
 typedef LightProcessObject       SuperClass;
 typedef SmartPointer<Self>       Pointer;
 typedef SmartPointer<const Self> ConstPointer;

 /** Standard New method. */
  itkNewMacro(Self);

 /** Run-time type information (and related methods). */
 itkTypeMacro(Self,Superclass);

 /* Vector typedefs */
 typedef TData                                           NumericType;
 typedef typename std::vector <NumericType>              NumericVectorType;
 typedef typename CSVFileReader<TData>::StringVectorType StringVectorType;
 typedef typename CSVFileReader<TData>::MatrixType       MatrixType;

 /** Get macro for the matrix */
 itkGetMacro(Matrix,MatrixType);

 void GetReaderData(CSVFileReader<TData>* );

 /** Functions for retrieving data (indices for rows or columns begin at 0) */
 const std::vector<NumericType>    GetRow (const std::string);
 const std::vector<NumericType>    GetRow (const unsigned int);

 const std::vector<NumericType>    GetColumn (const std::string);
 const std::vector<NumericType>    GetColumn (const unsigned int);

 const NumericType                 GetData (const std::string, const std::string);
 const NumericType                 GetData (const unsigned int, const unsigned int);

 const NumericType                 GetColumnData (const std::string, const unsigned int);
 const NumericType                 GetRowData (const std::string, const unsigned int);

 const StringVectorType            GetRowNames()
 { return this->m_RowHeaders; }

 const StringVectorType            GetColumnNames()
 { return this->m_ColumnHeaders; }

 void DisplayData();

 const NumericType operator() (const unsigned int, const unsigned int);
 const NumericType operator() (const std::string, std::string);

 CSVDataFrameObject () {}
 virtual ~CSVDataFrameObject() {}

private:
 MatrixType             m_Matrix;
 StringVectorType       m_ColumnHeaders;
 StringVectorType       m_RowHeaders;
 bool                   m_HaveRowHeaders;
 bool                   m_HaveColumnHeaders;

 CSVDataFrameObject(const Self &); //purposely not implemented
 void operator=(const Self &);    	//purposely not implemented
};


/* \class CSVFileWriter writes the data from an object into
 * a csv file.
 */
template <class TValueType, unsigned int NRows = 0, unsigned int NColumns = 0>
class CSVFileWriter:public LightProcessObject
{
public:
 /** Standard class typedefs */
 typedef CSVFileWriter             Self;
 typedef LightProcessObject        Superclass;
 typedef SmartPointer <Self>       Pointer;
 typedef SmartPointer <const Self> ConstPointer;

 /** Method for creation through the object factory. */
 itkNewMacro(Self);

 /** Run-time type information (and related methods). */
 itkTypeMacro(Self,Superclass);

 /* Specify the name of the output file */
 itkSetStringMacro(FileName);

 // Matrix types
 typedef vnl_matrix<TValueType>                        vnlMatrixType;
 typedef vnl_matrix_fixed<TValueType, NRows, NColumns> vnlFixedMatrixType;
 typedef itk::Matrix<TValueType,NRows,NColumns>        itkMatrixType;

 typedef TValueType InputObjectType;

 /** Set the input object if the matrix is of vnl_matrix type or Array2D */
 void SetInput (vnlMatrixType* obj)
 {
   m_InputObject = obj->data_block();
   m_rows = obj->rows();
   m_cols = obj->cols();
 }

 /** Set the input object if the matrix is of vnl_matrix_fixed type */
 void SetInput (vnlFixedMatrixType* obj)
 {
   m_InputObject = obj->data_block();
   m_rows = obj->rows();
   m_cols = obj->cols();
 }

 /** Set the input object if the matrix is of itkMatrixType*/
 void SetInput (itkMatrixType* obj)
 {
   m_InputObject = obj->GetVnlMatrix().data_block() ;
   m_rows = obj->RowDimensions;
   m_cols = obj->ColumnDimensions;
 }

 virtual void Write ();

 CSVFileWriter() {}
 virtual ~CSVFileWriter () {}

private:
 std::string               m_FileName;
 TValueType               *m_InputObject;
 unsigned int              m_rows;
 unsigned int              m_cols;

 CSVFileWriter(const Self &);  //purposely not implemented
 void operator=(const Self &); //purposely not implemented
};

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCSVFileIO.txx"
#endif

#endif
