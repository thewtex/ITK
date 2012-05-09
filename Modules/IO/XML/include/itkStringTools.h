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

#ifndef __itkStringTools_h
#define __itkStringTools_h

#include <string>
#include <vector>
#include <map>

#include <algorithm> // std::transform()
#include <cctype> // std::toupper(), std::tolower()

namespace itk
{

class StringTools
{
public:
  /** Method to trim the spaces or user-specified characters on both ends of a string. */
  static std::string& Trim( std::string& str, const std::string& dislike = " \t\n\r" )
  {
    if ( str.size() )
    {
      std::string::size_type pos = str.find_first_not_of( dislike );
      if ( pos != std::string::npos )
      {
        if ( pos ) str.erase( 0, pos );
        pos = str.find_last_not_of( dislike );
        if ( (++pos) < str.size() ) str.erase( pos );
      }
      else str.clear();
    }
    return str;
  }

  /** Method to trim the spaces or user-specified characters on left end of a string. */
  static std::string& TrimLeft( std::string& str, const std::string& dislike = " \t\n\r" )
  {
    if ( str.size() )
    {
      std::string::size_type pos = str.find_first_not_of( dislike );
      if ( pos != std::string::npos )
      {
        if ( pos ) str.erase( 0, pos );
      }
      else str.clear();
    }
    return str;
  }

  /** Method to trim the spaces or user-specified characters on right end of a string. */
  static std::string& TrimRight( std::string& str, const std::string& dislike = " \t\n\r" )
  {
    if ( str.size() )
    {
      std::string::size_type pos = str.find_last_not_of( dislike );
      if ( pos != std::string::npos )
      {
        if ( (++pos) < str.size() ) str.erase( pos );
      }
      else str.clear();
    }
    return str;
  }

  /** Method to covert lower-case characters to upper cases in a string. */
  static std::string& ToUpperCase( std::string& str )
  {
    // explicit cast needed to resolve ambiguity
    std::transform( str.begin(), str.end(), str.begin(), (int(*)(int))std::toupper );
    return str;
  }

  /** Method to covert upper-case characters to lower cases in a string. */
  static std::string& ToLowerCase( std::string& str )
  {
    // explicit cast needed to resolve ambiguity
    std::transform( str.begin(), str.end(), str.begin(), (int(*)(int))std::tolower );
    return str;
  }

  /** Method to split a string into two parts with user-defined delimiters. */
  static std::string& Split( std::string& s, std::string& rpart, const std::string& delims = "=:" )
  {
    std::string::size_type pos = s.find_first_of( delims );
    if ( pos != std::string::npos )
    {
      rpart = s.substr( pos+1 );
      s.erase( pos );
    }
    else rpart = "";
    return s;
  }

  /** Method to split a string into a sequence of strings with user-defined delimiters. */
  static void Split( const std::string& s, std::vector<std::string>& result, const std::string& delims = ";|" )
  {
    std::string str = s;
    while ( str.size() )
    {
      std::string::size_type pos = str.find_first_of( delims );
      if ( pos != std::string::npos )
      {
        std::string front = str.substr( 0, pos );
        result.push_back( front );
        //
        str = str.substr( pos+1 );
      }
      else
      {
        result.push_back( str );
        str = "";
      }
    }
  }

  /**
   * Method to split a string into a sequence of sub-strings with user-defined delimiters,
   * then each sub-string is further splitted into a <key,value> pair with separators "=:".
   */
  static void Split( const std::string& s, std::map<std::string,std::string>& result, const std::string& delims = ";|" )
  {
    std::vector<std::string> items;
    Split( s, items );
    //
    for ( int i = 0; i < items.size(); i++ )
    {
      std::string value = "";
      std::string key = Split( items[i], value );
      result[ Trim(key) ] = Trim(value);
    }
  }

  /** Method to test whether one string matches with another. */
  static bool MatchWith( const std::string& s1, const std::string& s2, bool IgnoreCase = true )
  {
    if ( IgnoreCase )
    {
      std::string ls1 = s1; ToLowerCase( ls1 );
      std::string ls2 = s2; ToLowerCase( ls2 );
      return ( ls1 == ls2 ? true : false );
    }
    return ( s1 == s2 ? true : false );
  }

  /** Method to test whether a string starts with a user-given sub-string. */
  static bool StartWith( const std::string& s1, const std::string& s2, bool IgnoreCase = true )
  {
    if ( IgnoreCase )
    {
      std::string ls1 = s1; ToLowerCase( ls1 );
      std::string ls2 = s2; ToLowerCase( ls2 );
      return ( ls1.find(ls2) == 0 ? true : false );
    }
    return ( s1.find(s2) == 0 ? true : false );
  }

  /** Method to test whether a string ends with a user-given sub-string. */
  static bool EndWith( const std::string& s1, const std::string& s2, bool IgnoreCase = true )
  {
    std::string::size_type pos = s1.size() - s2.size(); // to be confirmed, may need +1 or -1
    if ( IgnoreCase )
    {
      std::string ls1 = s1; ToLowerCase( ls1 );
      std::string ls2 = s2; ToLowerCase( ls2 );
      return ( ls1.rfind(ls2) == pos ? true : false );
    }
    return ( s1.rfind(s2) == pos ? true : false );
  }

  /** Method to test whether a string contains a user-given sub-string. */
  static bool ContainSub( const std::string& s1, const std::string& s2, bool IgnoreCase = true )
  {
    if ( IgnoreCase )
    {
      std::string ls1 = s1; ToLowerCase( ls1 );
      std::string ls2 = s2; ToLowerCase( ls2 );
      return ( ls1.find(ls2) != std::string::npos ? true : false );
    }
    return ( s1.find(s2) != std::string::npos ? true : false );
  }
}; // class StringTools

} // namespace itk

#endif // __itkStringTools_h
