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

#include "itkCSVFileIO.h"
#include "itkArray2D.h"
#include <iostream>
#include <vector>

const double epsilon = 1e-10;

// function for displaying vectors
template <class T>
void PrintVector (const std::vector<T>& v1)
{
 if (v1.empty())
 {
  std::cout << "Empty vector." << std::endl;
 }
 else
 {
 typename std::vector <T >::const_iterator it;
 for (it = v1.begin(); it != v1.end(); it++)
   {
   std::cout << *it << " ";
   }
 std::cout << std::endl;
 }
}

// function for comparing matrices
template <class T>
bool testMatrix (const itk::Array2D<T>& m1, const itk::Array2D<T>& m2)
{
 bool pass = true;

 for (unsigned int i = 0; i < m1.rows(); i++)
   {
   for (unsigned int j = 0; j < m1.cols(); j++)
     {
     if (vcl_fabs(m1[i][j] - m2[i][j]) > epsilon) pass = false;
     }
   }
 return pass;
}

// function for comparing numeric vectors
template <class T>
bool testVector (const std::vector<T>& v1, const std::vector<T>& v2)
{
 bool pass = true;

 for (unsigned int i = 0; i < v1.size(); i++)
   {
   if (vcl_fabs(v1[i] - v2[i]) > epsilon)
     {
     pass = false;
     }
   }
 return pass;
}

// function for comparing string vectors
bool testStringVector (const std::vector<std::string>& v1, const std::vector<std::string>& v2)
{
 bool pass = true;

 for (unsigned int i = 0; i < v1.size(); i++)
   {
   if (v1[i].compare( v2[i] ) != 0 )
     {
     pass = false;
     }
   }
 return pass;
}

// function for comparing numeric values
template <class T>
bool testValue (const T& test, const T& real)
{
  return (vcl_fabs(test - real) > epsilon)? false:true;
}


int itkCSVFileReaderTest (int argc, char *argv[])
{
 if (argc < 2)
 {
 std::cerr << "Usage: " << argv[0] << " Filename" << std::endl;
 return EXIT_FAILURE;
 }

 double nan = vcl_numeric_limits<double>::quiet_NaN();

 // Read and Parse the data
 typedef itk::CSVFileReader<double> ReaderType;
 ReaderType::Pointer reader = ReaderType::New();

 std::string filename = "nonexistentfilename.csv";
 reader->SetFileName ( filename );
 reader->SetFieldDelimiterCharacter(',');
 reader->SetStringDelimiterCharacter('"');
 reader->HaveColumnHeadersOn();
 reader->HaveRowHeadersOn();
 reader->UseStringDelimiterCharacterOn();

 // Try Non-existent filename
 try
   {
   reader->Parse();
   }
 catch(itk::ExceptionObject& exp)
   {
   std::cerr << "Exception caught!" << std::endl;
   std::cerr << exp << std::endl;
   }

 filename = argv[1];
 reader->SetFileName( filename );

 try
   {
   // Data is loaded into an itkArray2D object
   reader->Parse();
   }
 catch(itk::ExceptionObject& exp)
   {
   std::cerr << "Exception caught!" << std::endl;
   std::cerr << exp << std::endl;
   }

 typedef itk::CSVDataFrameObject<double> DataFrameObjectType;
 DataFrameObjectType::Pointer dfo = DataFrameObjectType::New();

 // Get the matrix, row and column headers from the reader object
 dfo->GetReaderData(reader);

 // Test the matrix
 typedef itk::Array2D<double> MatrixType;
 MatrixType test_matrix;
 test_matrix = dfo->GetMatrix();

 MatrixType matrix(3,6);
 matrix[0][0] = nan; matrix[0][1] = 1e+09;   matrix[0][2] = 5;    matrix[0][3] = 9;
 matrix[0][4] = 6.1; matrix[0][5] = nan;
 matrix[1][0] = 99;  matrix[1][1] = 0;       matrix[1][2] = 3.75; matrix[1][3] = 0.008;
 matrix[1][4] = nan; matrix[1][5] = nan;
 matrix[2][0] = 1;   matrix[2][1] = 2.2;     matrix[2][2] = 9;    matrix[2][3] = 5.6;
 matrix[2][4] = nan; matrix[2][5] = 3e+10;

 if (!testMatrix(matrix,test_matrix) )
   {
   std::cerr << "Matrices are not the same! Test Failed!" << std::endl;
   return 1;
   }
 dfo->DisplayData();
 std::cout << std::endl;

 // Test Row Names
 std::vector <std::string> test_row_names;
 test_row_names = dfo->GetRowNames();

 std::vector <std::string> row_names;
 row_names.push_back( "Jan" );
 row_names.push_back( "Feb" );
 row_names.push_back( "Mar,April" );

 if (!testStringVector(row_names, test_row_names))
   {
   std::cerr << "Row names do not match! Test failed!" << std::endl;
   return 1;
   }
 std::cout << "Row names: ";
 PrintVector( test_row_names );

 // Test Column Names
 std::vector <std::string> test_col_names;
 test_col_names = dfo->GetColumnNames();

 std::vector <std::string> col_names;
 col_names.push_back( "Africa" );
 col_names.push_back( "Asia" );
 col_names.push_back( "Aus" );
 col_names.push_back( "US,Can" );
 col_names.push_back( "Col 5" );
 col_names.push_back( "Col 6" );

 if (!testStringVector(col_names, test_col_names))
   {
   std::cerr << "Column names do not match! Test failed!" << std::endl;
   return 1;
   }
 std::cout << "Col names: ";
 PrintVector( test_col_names );

 // Test a row (using index access)
 std::vector <double> test_row_1;
 test_row_1 = dfo->GetRow(1);

 std::vector <double> row_1;
 row_1.push_back(99);
 row_1.push_back(0);
 row_1.push_back(3.75);
 row_1.push_back(0.008);

 if (!testVector(row_1,test_row_1))
   {
   std::cerr << "The vectors do not match! Test failed!" << std::endl;
   return 1;
   }
 std::cout << "Row 1 : ";
 PrintVector(test_row_1);

 // Test a row (using string access)
 std::vector <double> test_row_Jan;
 test_row_Jan = dfo->GetRow("Jan");

 std::vector <double> row_Jan;
 row_Jan.push_back( vcl_numeric_limits<double>::quiet_NaN() );
 row_Jan.push_back(1e+9);
 row_Jan.push_back(5);
 row_Jan.push_back(9);
 row_Jan.push_back(6.1);

 if (!testVector(row_Jan,test_row_Jan))
   {
   std::cerr << "The vectors do not match! Test failed!" << std::endl;
   return 1;
   }
 std::cout << "Row Jan : ";
 PrintVector(test_row_Jan);

 // Test a column (using index)
 std::vector <double> test_col_2;
 test_col_2 = dfo->GetColumn(2);

 std::vector <double> col_2;
 col_2.push_back (5);
 col_2.push_back (3.75);
 col_2.push_back (9);

 if (!testVector(col_2,test_col_2))
   {
   std::cerr << "The vectors do not match! Test failed!" << std::endl;
   return 1;
   }

 std::cout << "Column 2 : ";
 PrintVector(col_2);

 // Test a column (using string access)
 std::vector <double> test_col_Africa;
 test_col_Africa = dfo->GetColumn("Africa");

 std::vector <double> col_Africa;
 col_Africa.push_back (vcl_numeric_limits<double>::quiet_NaN() );
 col_Africa.push_back (99);
 col_Africa.push_back (1);

 if (!testVector(col_Africa,test_col_Africa))
   {
   std::cerr << "The vectors do not match! Test failed!" << std::endl;
   return 1;
   }
 std::cout << "Column Africa : ";
 PrintVector(col_Africa);

 // Test a row that does not exist
 std::vector <double> test_row_Oct;
 test_row_Oct = dfo->GetRow("Oct");
 if(!test_row_Oct.empty())
   {
   std::cerr << "Row should be empty! Test Failed!";
   return 1;
   }

 // Test column that does not exist
 std::vector<double> test_col_Eur;
 test_col_Eur = dfo->GetColumn("Eur");
 if(!test_col_Eur.empty())
   {
   std::cerr << "Column should be empty! Test Failed!";
   return 1;
   }

 // Test data items
 double test_item1,test_item2,test_item3,test_item4;
 double item1 = 9,item2 = 5.6,item3 = 0,item4 = 1e+09;

 test_item1 = dfo->GetData(2,2);
 if (!testValue(test_item1,item1))
   {
   std::cerr << "Wrong value! Test Failed!";
   return 1;
   }
 std::cout << "Data(2,2) : " << test_item1 << std::endl;

 test_item2 = dfo->GetData("Mar,April","US,Can");
 if (!testValue(test_item2,item2))
   {
   std::cerr << "Wrong value! Test failed!";
   return 1;
   }
 std::cout << "Data('Mar,April','US,Can') : " << test_item2 << std::endl;

 test_item3 = dfo->GetRowData("Feb",1.0);
 if (!testValue(test_item3,item3))
   {
   std::cerr << "Wrong value! Test failed!";
   return 1;
   }
 std::cout << "Data(Feb,1) : " << test_item3 << std::endl;

 test_item4 = dfo->GetColumnData("Asia",0.0);
 if (!testValue(test_item4,item4))
   {
   std::cerr << "Wrong value! Test failed!";
   return 1;
   }
 std::cout << "Data(Asia,0) : " <<test_item4 << std::endl;

 // Test using non existing data items
 double test_item5 = dfo->GetData(5,3);
 if (!testValue(test_item5,vcl_numeric_limits<double>::quiet_NaN() ))
   {
   std::cerr << "Wrong value! Test failed!";
   return 1;
   }

 double test_item6 = dfo->GetData("Feb","Europe");
 if (!testValue(test_item6,vcl_numeric_limits<double>::quiet_NaN() ))
   {
   std::cerr << "Wrong value! Test failed!";
   return 1;
   }

 // Test the () operator
 double op_test_item1 = (*dfo)(0,4);
 if (!testValue(op_test_item1, 6.1 ))
   {
   std::cerr << "Wrong value! Test failed!";
   return 1;
   }
 std::cout << "Data(0,4) : " << op_test_item1 << std::endl;

 double op_test_item2 = (*dfo)("Feb","Aus");
 if (!testValue(op_test_item2, 3.75))
   {
   std::cerr << "Wrong value! Test failed!";
   return 1;
   }
 std::cout << "Data(Feb,Aus) : " << op_test_item2 << std::endl;

 return EXIT_SUCCESS;
}
