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

#ifndef __itkDOMHelpers_h
#define __itkDOMHelpers_h

#include "itkDOMNode.h"

#include <sstream>
#include <vector>
#include "itkArray.h"

namespace itk
{

/**
 * \class StringObject
 * \brief A special string type used for I/O operations in this DOM implementation.
 *
 * This class is created for the purpose of overloading the ">>" operator for the string type
 * to perform DOM-related operations. As ITK has overloaded this operator
 * for other operations, we need a new string type to use this operator in our implementation.
 */
class StringObject : public std::string
{
};


/////////////////////////////////////////////////////////////////////////////
// helper functions for converting a string to/from a std::vector
/////////////////////////////////////////////////////////////////////////////

/**
 * Function to convert a string to a vector of type std::vector<T>.
 * Number of elements to read is given by the parameter 'count':
 *   = 0, get the number of elements to read from data.size();
 *   > 0, read number of 'count' elements, and resize the data if necessary;
 *   < 0, default value (-1), automatically compute the number from the input stream.
 *
 * An exception will be thrown if errors were encountered during the conversion.
 */
template < class T >
inline std::vector<T>& FromString( const std::string& s, std::vector<T>& data, int count = -1 )
{
  std::istringstream iss( s, std::istringstream::in );
  iss.exceptions( iss.failbit | iss.badbit );
  //
  if ( count < 0 )
  {
    // compute the number of elements to be read from the input stream
    try
    {
      while ( true )
      {
        T value = T();
        iss >> value;
        data.push_back( value );
      }
    }
    catch ( std::istringstream::failure e )
    {
      if ( !iss.eof() ) throw e; // loop terminated abnomally if not because of EOF
    }
  }
  else
  {
    // the number of elements to be read is provided by count or, if count is 0, data.size()
    if ( count == 0 ) count = (int)data.size();
    if ( count > (int)data.size() ) data.resize( (size_t)count );
    for ( int i = 0; i < count; i++ )
    {
      T value = T();
      iss >> value;
      data[i] = value;
    }
  }
  //
  return data;
}
/**
 * Function to convert a string to a vector of type std::vector<T>,
 * assuming that the number of elements to read has already been set in the output data.
 *
 * An exception will be thrown if errors were encountered during the conversion.
 */
template < class T >
inline std::vector<T>& operator>>( const std::string& s, std::vector<T>& data )
{
  return FromString( s, data, 0 ); // assume number of elements to read has already been set in data
}

/**
 * Functions to convert a vector of type std::vector<T> to a string.
 * An exception will be thrown if errors were encountered during the conversion.
 */
template < class T >
inline std::string& ToString( std::string& s, const std::vector<T>& data )
{
  std::ostringstream oss( std::ostringstream::out );
  oss.exceptions( oss.badbit );
  for ( unsigned int i = 0; i < data.size(); i++ ) oss << " " << data[i];
  s = oss.str();
  return s;
}
template < class T >
inline StringObject& operator<<( StringObject& s, const std::vector<T>& data )
{
  return (StringObject&)ToString( s, data );
}


/////////////////////////////////////////////////////////////////////////////
// helper functions for converting a string to/from a itk::Array
/////////////////////////////////////////////////////////////////////////////

/**
 * Function to convert a string to an array of type itk::Array<T>.
 * Number of elements to read is given by the parameter 'count':
 *   = 0, get the number of elements to read from data.size();
 *   > 0, read number of 'count' elements, and resize the data if necessary;
 *   < 0, default value (-1), automatically compute the number from the input stream.
 *
 * An exception will be thrown if errors were encountered during the conversion.
 */
template < class T >
inline Array<T>& FromString( const std::string& s, Array<T>& data, int count = -1 )
{
  std::istringstream iss( s, std::istringstream::in );
  iss.exceptions( iss.failbit | iss.badbit );
  //
  if ( count < 0 )
  {
    // compute the number of elements to be read from the input stream
    std::vector<T> v;
    try
    {
      while ( true ) // loop until error occured
      {
        T value = T();
        iss >> value;
        v.push_back( value );
      }
    }
    catch ( std::istringstream::failure e )
    {
      if ( !iss.eof() ) throw e; // loop terminated abnomally if not because of EOF
    }
    //
    data.SetSize( v.size() );
    for ( unsigned int i = 0; i < v.size(); i++ ) data[i] = v[i];
  }
  else
  {
    // the number of elements to be read is provided by count or, if count is 0, data.size()
    if ( count == 0 ) count = (int)data.GetSize();
    if ( count > (int)data.GetSize() ) data.SetSize( (unsigned int)count );
    for ( int i = 0; i < count; i++ )
    {
      T value = T();
      iss >> value;
      data[i] = value;
    }
  }
  //
  return data;
}
/**
 * Function to convert a string to an array of type itk::Array<T>,
 * assuming that the number of elements to read has already been set in the output data.
 *
 * An exception will be thrown if errors were encountered during the conversion.
 */
template < class T >
inline Array<T>& operator>>( const std::string& s, Array<T>& data )
{
  return FromString( s, data, 0 ); // assume number of elements to read has already been set in data
}

/**
 * Functions to convert an array of type itk::Array<T> to a string.
 * An exception will be thrown if errors were encountered during the conversion.
 */
template < class T >
inline std::string& ToString( std::string& s, const Array<T>& data )
{
  std::ostringstream oss( std::ostringstream::out );
  oss.exceptions( oss.badbit );
  for ( unsigned int i = 0; i < data.GetSize(); i++ ) oss << " " << data[i];
  s = oss.str();
  return s;
}
template < class T >
inline StringObject& operator<<( StringObject& s, const Array<T>& data )
{
  return (StringObject&)ToString( s, data );
}


/////////////////////////////////////////////////////////////////////////////
// helper functions for converting a string to/from a basic data type
/////////////////////////////////////////////////////////////////////////////

/**
 * Functions to convert a string to a value of basic data type.
 * An exception will be thrown if errors were encountered during the conversion.
 */
template < class T >
inline T& FromString( const std::string& s, T& data )
{
  std::istringstream iss( s, std::istringstream::in );
  iss.exceptions( iss.failbit | iss.badbit );
  iss >> data;
  return data;
}
template < class T >
inline T& operator>>( const std::string& s, T& data )
{
  return FromString( s, data );
}

/**
 * Functions to convert a value of basic data type to a string.
 * An exception will be thrown if errors were encountered during the conversion.
 */
template < class T >
inline std::string& ToString( std::string& s, const T& data )
{
  std::ostringstream oss( std::ostringstream::out );
  oss.exceptions( oss.badbit );
  oss << data;
  s = oss.str();
  return s;
}
template < class T >
inline StringObject& operator<<( StringObject& s, const T& data )
{
  return (StringObject&)ToString( s, data );
}

} // namespace itk

#include "itkStringTools.h"

#endif // __itkDOMHelpers_h
