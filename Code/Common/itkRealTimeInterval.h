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
#ifndef __itkRealTimeInterval_h
#define __itkRealTimeInterval_h

#include "itkIntTypes.h"

namespace itk
{
/** \class RealTimeInterval
 * \brief The RealTimeInterval is a data structure for representing the time
 * span between two RealTimeStamps, with similar high precision and a large
 * dynamic range to what the RealTimeStamps offer.
 *
 * This class represents the difference between two time points, typically for
 * applications that need to mark the time of acquisition of data with high
 * precision (microseconds) and a large dynamic range (years). This class will
 * be the natural representation for the duration of a video sequence, or for
 * the time that has passed between the acquisition of one images and a
 * subsequent one.
 *
 * \sa RealTimeStamp
 * \sa RealTimeClock
 *
 */

class RealTimeInterval
{
public:

  typedef RealTimeInterval    Self;

  /** Internal types used to represent seconds and microseconds. */
  typedef   int64_t   SecondsDifferenceType;
  typedef   int64_t   MicroSecondsDifferenceType;

  /** Constructor */
  RealTimeInterval();

  /** Constructor with values. Intentionally made public */
  RealTimeInterval( SecondsDifferenceType, MicroSecondsDifferenceType );

  /** Destructor */
  ~RealTimeInterval();

  /** Return time in multiple units. */
  double GetTimeInMicroSeconds() const;
  double GetTimeInSeconds() const;
  double GetTimeInHours() const;
  double GetTimeInDays() const;

  /** Arithmetic operations between RealTimeInterval and RealTimeInterval. */
  Self operator-( const Self & ) const;
  Self operator+( const Self & ) const;
  const Self & operator-=( const Self & );
  const Self & operator+=( const Self & );

  /** Comparison operations. */
  bool operator>( const Self & ) const;
  bool operator<( const Self & ) const;
  bool operator==( const Self & ) const;
  bool operator!=( const Self & ) const;
  bool operator<=( const Self & ) const;
  bool operator>=( const Self & ) const;

  /** Set with values. The units and signs of the seconds and microseconds will
   * be harmonized internally. */
  void Set( SecondsDifferenceType, MicroSecondsDifferenceType );


private:

  friend class RealTimeStamp;

  /** Number of Seconds and Microseconds since... */
  SecondsDifferenceType        m_Seconds;

  /** Number of Microseconds since the second.
   *  should be in the range -999,999 to 999,999
   *  and it must always have the same sign as
   *  the m_Seconds member variable. */
  MicroSecondsDifferenceType   m_MicroSeconds;


};


#define ALIGN_THE_ARROW_OF_TIME( seconds, micro_seconds ) \
if( seconds > 0 && micro_seconds < 0 ) \
  { \
  seconds -= 1; \
  micro_seconds = 1000000L - micro_seconds; \
  } \
if( seconds < 0 && micro_seconds > 0 ) \
  { \
  seconds += 1; \
  micro_seconds = 1000000L + micro_seconds; \
  }


} // end of namespace itk

#endif  // __itkRealTimeInterval_h
