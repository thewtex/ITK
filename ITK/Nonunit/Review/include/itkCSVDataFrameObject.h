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

#ifndef __itkCSVDataFrameObject_h
#define __itkCSVDataFrameObject_h

#include "itkLightProcessObject.h"
#include "itkCSVFileReader.h"
#include "itkMacro.h"
#include <vector>

namespace itk
{

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

 typedef itk::CSVFileReader<TData> CSVFileReaderType;

 /* Vector typedefs */
 typedef TData                                           NumericType;
 typedef typename std::vector <NumericType>              NumericVectorType;
 typedef typename CSVFileReaderType::StringVectorType    StringVectorType;
 typedef typename CSVFileReaderType::MatrixType          MatrixType;

 /** Get macro for the matrix */
 itkGetMacro(Matrix,MatrixType);

 void GetReaderData(CSVFileReaderType* );

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

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCSVDataFrameObject.txx"
#endif

#endif
