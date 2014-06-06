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

template class MetaDataObject< bool >;
template class MetaDataObject< unsigned char >;
template class MetaDataObject< char >;
template class MetaDataObject< signed char >;
template class MetaDataObject< unsigned short >;
template class MetaDataObject< short >;
template class MetaDataObject< unsigned int >;
template class MetaDataObject< int >;
template class MetaDataObject< unsigned long >;
template class MetaDataObject< long >;
template class MetaDataObject< float >;
template class MetaDataObject< double >;
template class MetaDataObject< std::string >;

} // end namespace itk
