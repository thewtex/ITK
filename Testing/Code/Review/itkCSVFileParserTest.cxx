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
#include <iostream>
#include <vector>

template <class T>
void DisplayVector (std::vector<T> v1)
{
 if (v1.empty())
 {
  std::cout << "Empty vector." << std::endl;
 }
 else
 {
 typename std::vector <T >::iterator it;
 for (it = v1.begin(); it != v1.end(); it++)
 {
 std::cout << *it << " ";
 }
 std::cout << std::endl;
 }
}

int itkCSVFileParserTest (int argc, char *argv[])
{
 if (argc < 2)
 {
 std::cerr << "Usage: " << argv[0] << " Filename" << std::endl;
 return EXIT_FAILURE;
 }

 // Parse the data
 typedef itk::CSVDataFrameObject<float> ParserType;
 ParserType parser;

 parser.SetFileName(argv[1]);
 parser.SetFieldDelimiterCharacter(',');
 parser.SetStringDelimiterCharacter('"');
 parser.HaveColumnHeadersOn();
 parser.HaveRowHeadersOn();
 parser.UseStringDelimiterCharacterOn();

 // Data is loaded into an itkArray2D object
 typedef itk::Array2D <float> MatrixType;
 MatrixType matrix = parser.ParseData();
 std::cout << " The data contains: " << matrix.rows() <<" rows and " << matrix.cols() <<" columns."	<< std::endl;

 // Display the data
 parser.DisplayData();

 //Display Row names and Column names
 std::vector <std::string> row_names;
 std::vector <std::string> col_names;
 row_names = parser.GetRowNames();
 col_names = parser.GetColumnNames();
 std::cout << "Row names: ";
 DisplayVector(row_names);
 std::cout << "Column names: ";
 DisplayVector(col_names);

 // Display a row from the dataset (int)
 std::vector <float> row;
 row = parser.GetRow(1);
 std::cout << "Row 1 : ";
 DisplayVector(row);

 // Display a row from the dataset (string)
 std::vector <float> row2;
 row2 = parser.GetRow("Jan");
 std::cout << "Row Jan : ";
 DisplayVector(row2);

 // Display a column from the dataset (int)
 std::vector <float> col2;
 col2 = parser.GetColumn(2);
 std::cout << "Column 2 : ";
 DisplayVector(col2);

 //Display a column from the dataset (string)
 std::vector <float> col;
 col = parser.GetColumn("Europe");
 std::cout << "Column Europe : ";
 DisplayVector(col);

 // Row that does not exist
 std::vector <float> row3;
 row3 = parser.GetRow("Oct");
 std::cout << "Row Oct : ";
 DisplayVector(row3);

 // Column that does not exist
 std::vector<float> column;
 column = parser.GetColumn("Australia");
 std::cout <<"Column Australia : ";
 DisplayVector(column);

 // Display data items
 float item1,item2,item3,item4;
 item1 = parser.GetData(2,2);
 item2 = parser.GetData("Mar","Africa,America");
 item3 = parser.GetRowData("Feb",1);
 item4 = parser.GetColumnData("Asia",0);
 std::cout << "Data(2,2) : " << item1 << std::endl;
 std::cout << "Data(Mar,'Africa,America') : " << item2 << std::endl;
 std::cout << "Data(Feb,1) : " << item3 << std::endl;
 std::cout << "Data(Asia,0) : " <<item4 << std::endl;

 // Non existing data item
 float item5;
 item5 = parser.GetData(3,3);
 std::cout << "Data(3,3) : " << item5 << std::endl;
 item5 = parser.GetData("Mar","America");
 std::cout << "Data(Mar,America) : " << item5 << std::endl;

 // displaying using operator ()
 std::cout << "Data(2,2) : " << parser(2,2) << std::endl;
 std::cout << "Data(Jan,Asia): " <<parser("Jan","Asia") << std::endl;

 std::cout << "Data(Feb,Australia): " << parser("Feb","Australia") << std::endl;

 return EXIT_SUCCESS;
}
