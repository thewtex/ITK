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
#ifndef __itkLogicOpsFunctors_h
#define __itkLogicOpsFunctors_h

#include "itkNumericTraits.h"


namespace itk
{
namespace Functor
{
/** \class Equal2
 * \brief Functor for == operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
*/

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class Equal2
{
public:
  Equal2()
  {
    m_ForegroundValue=itk::NumericTraits<TOutput>::One;
    m_BackgroundValue=itk::NumericTraits<TOutput>::Zero;
  };
  ~Equal2() {};
  bool operator!=( const Equal2 & ) const
  {
    return false;
  }
  bool operator==( const Equal2 & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    if( A == static_cast<TInput1>(B) )
      {
      return m_ForegroundValue;
      }
    return m_BackgroundValue;
  }

  void SetForegroundValue(const TOutput &FG)
  {
    m_ForegroundValue=FG;
  }
  void SetBackgroundValue(const TOutput &BG)
  {
    m_BackgroundValue=BG;
  }
private:
  TOutput m_ForegroundValue;
  TOutput m_BackgroundValue;
};
/** \class Equal2
 * \brief Functor for != operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
*/

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class NotEqual2
{
public:
  NotEqual2()
  {
  m_ForegroundValue=itk::NumericTraits<TOutput>::One;
  m_BackgroundValue=itk::NumericTraits<TOutput>::Zero;
  };
  ~NotEqual2() {};
  bool operator!=( const NotEqual2 & ) const
  {
    return false;
  }
  bool operator==( const NotEqual2 & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    if( A != B )
      {
      return m_ForegroundValue;
      }
    return m_BackgroundValue;
  }
  void SetForegroundValue(const TOutput &FG)
  {
    m_ForegroundValue=FG;
  }
  void SetBackgroundValue(const TOutput &BG)
  {
    m_BackgroundValue=BG;
  }
private:

  TOutput m_ForegroundValue;
  TOutput m_BackgroundValue;
};



/** \class GreaterEqual2
 * \brief Functor for != operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
*/

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class GreaterEqual2
{
public:
  GreaterEqual2()
  {
  m_ForegroundValue=itk::NumericTraits<TOutput>::One;
  m_BackgroundValue=itk::NumericTraits<TOutput>::Zero;
  };
  ~GreaterEqual2() {};
  bool operator!=( const GreaterEqual2 & ) const
  {
    return false;
  }
  bool operator==( const GreaterEqual2 & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    if( A >= B )
      {
      return m_ForegroundValue;
      }
    return m_BackgroundValue;
  }
  void SetForegroundValue(const TOutput &FG)
  {
    m_ForegroundValue=FG;
  }
  void SetBackgroundValue(const TOutput &BG)
  {
    m_BackgroundValue=BG;
  }
private:

  TOutput m_ForegroundValue;
  TOutput m_BackgroundValue;
};
/** \class Greater2
 * \brief Functor for > operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
*/

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class Greater2
{
public:
  Greater2()
  {
  m_ForegroundValue=itk::NumericTraits<TOutput>::One;
  m_BackgroundValue=itk::NumericTraits<TOutput>::Zero;
  };
  ~Greater2() {};
  bool operator!=( const Greater2 & ) const
  {
    return false;
  }
  bool operator==( const Greater2 & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    if( A > B )
      {
      return m_ForegroundValue;
      }
    return m_BackgroundValue;
  }
  void SetForegroundValue(const TOutput &FG)
  {
    m_ForegroundValue=FG;
  }
  void SetBackgroundValue(const TOutput &BG)
  {
    m_BackgroundValue=BG;
  }
private:

  TOutput m_ForegroundValue;
  TOutput m_BackgroundValue;
};
/** \class LessEqual2
 * \brief Functor for <= operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
*/

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class LessEqual2
{
public:
  LessEqual2()
  {
  m_ForegroundValue=itk::NumericTraits<TOutput>::One;
  m_BackgroundValue=itk::NumericTraits<TOutput>::Zero;
  };
  ~LessEqual2() {};
  bool operator!=( const LessEqual2 & ) const
  {
    return false;
  }
  bool operator==( const LessEqual2 & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    if( A <= B )
      {
      return m_ForegroundValue;
      }
    return m_BackgroundValue;
  }
  void SetForegroundValue(const TOutput &FG)
  {
    m_ForegroundValue=FG;
  }
  void SetBackgroundValue(const TOutput &BG)
  {
    m_BackgroundValue=BG;
  }
private:

  TOutput m_ForegroundValue;
  TOutput m_BackgroundValue;
};
/** \class Less2
 * \brief Functor for < operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
*/

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class Less2
{
public:
  Less2()
  {
  m_ForegroundValue=itk::NumericTraits<TOutput>::One;
  m_BackgroundValue=itk::NumericTraits<TOutput>::Zero;
  };
  ~Less2() {};
  bool operator!=( const Less2 & ) const
  {
    return false;
  }
  bool operator==( const Less2 & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    if( A < B )
      {
      return m_ForegroundValue;
      }
    return m_BackgroundValue;
  }
  void SetForegroundValue(const TOutput &FG)
  {
    m_ForegroundValue=FG;
  }
  void SetBackgroundValue(const TOutput &BG)
  {
    m_BackgroundValue=BG;
  }
private:

  TOutput m_ForegroundValue;
  TOutput m_BackgroundValue;
};


}
}

#endif
