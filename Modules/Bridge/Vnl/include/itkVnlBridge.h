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
#ifndef __itkVnlBridge_h
#define __itkVnlBridge_h

namespace itk
{
class VnlBridge
{
public:
  template< class TGenericImage, class MatrixType >
  static typename TGenericImage::Pointer VnlMatrixToImage( MatrixType inputMatrix );

  template< class TGenericImage, class MatrixType >
  static MatrixType ImageToVnlMatrix( const TGenericImage * image );
};

} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVnlBridge.hxx"
#endif

#endif
