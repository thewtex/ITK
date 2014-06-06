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

#if ITK_TEMPLATE_IMPORT_WORKS
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< bool > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< unsigned char > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< char > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< signed char > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< unsigned short > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< short > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< unsigned int > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< int > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< unsigned long > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< long > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< float > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< double > ) );
ITK_TEMPLATE_EXPORT( 1(class ITKCommon_EXPORT MetaDataObject< std::string > ) );
#else
ITK_NATIVE_TYPE_METADATAPRINT( bool );
ITK_NATIVE_TYPE_METADATAPRINT( unsigned char );
ITK_NATIVE_TYPE_METADATAPRINT( char );
ITK_NATIVE_TYPE_METADATAPRINT( signed char );
ITK_NATIVE_TYPE_METADATAPRINT( unsigned short );
ITK_NATIVE_TYPE_METADATAPRINT( short );
ITK_NATIVE_TYPE_METADATAPRINT( unsigned int );
ITK_NATIVE_TYPE_METADATAPRINT( int );
ITK_NATIVE_TYPE_METADATAPRINT( unsigned long );
ITK_NATIVE_TYPE_METADATAPRINT( long );
ITK_NATIVE_TYPE_METADATAPRINT( float );
ITK_NATIVE_TYPE_METADATAPRINT( double );
ITK_NATIVE_TYPE_METADATAPRINT( std::string );
#endif

} // end namespace itk
