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

#ifndef itkPyMatrix_h
#define itkPyMatrix_h

#include "itkMatrix.h"

// The python header defines _POSIX_C_SOURCE without a preceding #undef
#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#include <Python.h>

namespace itk
{

/** \class PyMatrix
 *
 *  \brief Helper class get views of itk.Matrix to NumPy arrays and back
 *
 *  This class will either receive a itk.Matrix data structure and create the equivalent
 *  Python array view or will receive a Python array and create a copy of it in an
 *  itk.Matrix data structure. This permits passing itk.Matrix data structures into python
 *  arrays from the NumPy python package.
 *
 *  \ingroup ITKBridgeNumPy
 */
template <typename TElement, unsigned int NRows = 3, unsigned int NColumns =3 >
class PyMatrix
{
public:
  /** Standard "Self" type alias. */
  using Self = PyMatrix;

  /** Type of the data from which the buffer will be converted */
  using DataType = TElement;
  using MatrixType = Matrix< TElement, NRows, NColumns >;

  /**
   * Get an Array with the content of the vnl matrix
   */
  static PyObject * _GetArrayViewFromMatrix( MatrixType * matrix);

  /**
   * Get a vnl matrix from a Python array
   */
  static const MatrixType _GetMatrixFromArray( PyObject *arr, PyObject *shape);
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPyMatrix.hxx"
#endif

#endif // _itkPyMatrix_h
