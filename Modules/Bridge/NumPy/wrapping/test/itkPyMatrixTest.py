#==========================================================================
#
#   Copyright Insight Software Consortium
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0.txt
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#==========================================================================*/
from __future__ import print_function
import sys
import unittest
import datetime as dt

import itk
import numpy as np

class TestNumpyVnlMemoryviewInterface(unittest.TestCase):
    """ This tests numpy array <-> ITK Matrix conversion. """

    def setUp(self):
        pass

    def test_NumPyBridge_Matrix(self):
        "Try to convert a itk.Matrix into a Numpy array and back."
        vm1 = itk.vnl_matrix[itk.F]()
        vm1.set_size(2, 2)
        vm1.fill(0)
        vm1.put(1, 1, 1.3)
        vm1.put(1, 0, 2)
        m1 = itk.Matrix[itk.F](vm1)
        arr = itk.PyMatrix[itk.F, 2, 2].GetArrayViewFromMatrix(m1)
        m2 = itk.PyMatrix[itk.F, 2, 2].GetMatrixFromArray(arr)
        # Check that the matrices axes dimensions have not been flipped or changed
        self.assertEqual(itk.template(m1)[1][1], arr.shape[0])
        self.assertEqual(itk.template(m1)[1][2], arr.shape[1])
        self.assertEqual(itk.template(m1)[1][1], itk.tempalte(m2)[1][1])
        self.assertEqual(itk.template(m1)[2][2], itk.tempalte(m2)[2][2])
        # Compute any difference between the original matrix and the numpy array view
        diff = 0.0
        for ii in range(2):
          for jj in range(2):
            diff += abs(m1(ii, jj) - arr[ii, jj])
        self.assertEqual(0, diff)
        # Compute any difference between the two matrices
        diff = 0.0
        for ii in range(2):
          for jj in range(2):
            diff += abs(m1(ii, jj) - m2(ii, jj))
        self.assertEqual(0, diff)
        # Test deep copy
        arr_cp = itk.PyMatrix[itk.F,2,2].GetArrayFromMatrix(m1)
        self.assertEqual(m1(0, 0), arr_cp[0, 0])
        m2 = itk.PyMatrix[itk.F,2,2].GetMatrixFromArray(arr_cp)
        arr_cp[0, 0] = 2
        self.assertNotEqual(m2(0, 0), arr_cp[0, 0])

if __name__ == '__main__':
    unittest.main()
