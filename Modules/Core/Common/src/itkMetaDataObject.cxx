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
#include "itkMetaDataObject.h"

namespace itk
{

NATIVE_TYPE_METADATAPRINT( unsigned char );
NATIVE_TYPE_METADATAPRINT( char );
NATIVE_TYPE_METADATAPRINT( unsigned short );
NATIVE_TYPE_METADATAPRINT( short );
NATIVE_TYPE_METADATAPRINT( unsigned int );
NATIVE_TYPE_METADATAPRINT( int );
NATIVE_TYPE_METADATAPRINT( unsigned long );
NATIVE_TYPE_METADATAPRINT( long );
NATIVE_TYPE_METADATAPRINT( float );
NATIVE_TYPE_METADATAPRINT( double );
NATIVE_TYPE_METADATAPRINT( std::string );

} // end namespace itk
