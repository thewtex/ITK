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

#ifndef __itkCSVArray2DDataFrameObject_h
#define __itkCSVArray2DDataFrameObject_h

#include "itkDataObject.h"
#include "itkArray2D.h"
#include "itkMacro.h"
#include <vector>

namespace itk
{
/** \class DataFrameObject contains Get methods for either accessing
  * data from the Array2D object, created from using a CSVFileReader object
  * or from the row and column headers.
  */

template <class TData>
class ITK_EXPORT CSVArray2DDataFrameObject:public DataObject
{
public:
 /* Standard class typedefs */
 typedef CSVArray2DDataFrameObject       Self;
 typedef DataObject                      SuperClass;
 typedef SmartPointer<Self>              Pointer;
 typedef SmartPointer<const Self>        ConstPointer;

 /** Standard New method. */
 itkNewMacro(Self);

 /** Run-time type information (and related methods). */
 itkTypeMacro(Self,Superclass);

 /* Vector typedefs */
 typedef typename std::vector<TData>              NumericVectorType;
 typedef typename std::vector<std::string>        StringVectorType;
 typedef typename itk::Array2D<TData>             MatrixType;

 /** Set macros */
 itkSetMacro(HasColumnHeaders,bool);
 itkSetMacro(HasRowHeaders,bool);
 itkBooleanMacro(HasColumnHeaders);
 itkBooleanMacro(HasRowHeaders);

 /** Get macro for the matrix */
 itkGetMacro(Matrix,MatrixType);
 itkGetMacro(ColumnHeaders,StringVectorType);
 itkGetMacro(RowHeaders,StringVectorType);

 /** Functions for retrieving data (indices for rows or columns begin at 0) */
 const NumericVectorType    GetRow (const std::string);
 const NumericVectorType    GetRow (const unsigned int);

 const NumericVectorType    GetColumn (const std::string);
 const NumericVectorType    GetColumn (const unsigned int);

 /** These two GetData functions take arguments in the order (row, column) */
 const TData                GetData (const std::string, const std::string);
 const TData                GetData (const unsigned int, const unsigned int);

 /** For the following two functions, you would pass the name of the row or column and the index
     of the data item that needs to be accessed */
 const TData                GetColumnData (const std::string, const unsigned int);
 const TData                GetRowData (const std::string, const unsigned int);

 void PrintSelf(std::ostream & os, Indent indent) const;

 /** The operator functions are used to access data from the matrix */
 const TData operator() (const unsigned int, const unsigned int);
 const TData operator() (const std::string, std::string);

 /** These functions are used to set the row and column headers from outside the class */
 void SetMatrixSize         ( unsigned int, unsigned int);
 void FillMatrix            ( TData value);
 void SetMatrixData         (unsigned int, unsigned int, TData);
 void RowHeadersPushBack    (std::string &);
 void ColumnHeadersPushBack (std::string &);
 void EraseFirstColumnHeader ();

 CSVArray2DDataFrameObject ();
 virtual ~CSVArray2DDataFrameObject() {}

private:
 MatrixType             m_Matrix;
 StringVectorType       m_ColumnHeaders;
 StringVectorType       m_RowHeaders;
 bool                   m_HasRowHeaders;
 bool                   m_HasColumnHeaders;

 CSVArray2DDataFrameObject(const Self &);      //purposely not implemented
 void operator=(const Self &);    	       //purposely not implemented
};

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCSVArray2DDataFrameObject.txx"
#endif

#endif
