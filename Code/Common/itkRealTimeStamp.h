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
#ifndef __itkRealTimeStamp_h
#define __itkRealTimeStamp_h
#include "itkWin32Header.h"
#include "itkRealTimeInterval.h"

namespace itk
{
/** \class RealTimeStamp
 * \brief The RealTimeStamp is a data structure for representing time with high
 * precision and a large dynamic range.
 *
 * This class represents time typically for applications that need to mark the
 * time of acquisition of data with high precision (microseconds) and a large
 * dynamic range (years).
 *
 * \sa RealTimeInterval
 * \sa RealTimeClock
 *
 */

class ITKCommon_EXPORT RealTimeStamp
{
public:

  typedef  RealTimeStamp     Self;

  friend class RealTimeClock;

  /** Constructor */
  RealTimeStamp();

  /** Destructor */
  ~RealTimeStamp();

  /** Return time in multiple units. */
  double GetTimeInMicroSeconds() const;
  double GetTimeInSeconds() const;
  double GetTimeInHours() const;
  double GetTimeInDays() const;

  /** Arithmetic operations between RealTimeInterval and RealTimeStamp. */
  RealTimeInterval operator-( const Self & ) const;
  Self operator+( const RealTimeInterval & ) const;
  Self operator-( const RealTimeInterval & ) const;
  const Self & operator+=( const RealTimeInterval & );
  const Self & operator-=( const RealTimeInterval & );

  /** Comparison operations. */
  bool operator>( const Self & ) const;
  bool operator<( const Self & ) const;
  bool operator==( const Self & ) const;
  bool operator!=( const Self & ) const;
  bool operator<=( const Self & ) const;
  bool operator>=( const Self & ) const;

private:

  typedef   uint64_t   SecondsCounterType;
  typedef   uint64_t   MicroSecondsCounterType;

  /** Constructor with values. Intentionally made private */
  RealTimeStamp( SecondsCounterType, MicroSecondsCounterType );

  typedef   RealTimeInterval::SecondsDifferenceType        SecondsDifferenceType;
  typedef   RealTimeInterval::MicroSecondsDifferenceType   MicroSecondsDifferenceType;

  /** Number of Seconds and Microseconds since... */
  SecondsCounterType        m_Seconds;
  MicroSecondsCounterType   m_MicroSeconds;

};

#define CARRY_UNITS_OVER( seconds, micro_seconds ) \
if ( micro_seconds > 1000000L ) \
  { \
  seconds += 1; \
  micro_seconds -= 1000000L; \
  } \
if ( micro_seconds < 0 ) \
  { \
  seconds -= 1; \
  micro_seconds += 1000000L; \
  }


} // end of namespace itk

#endif  // __itkRealTimeStamp_h
