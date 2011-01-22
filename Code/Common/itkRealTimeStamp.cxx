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
#include "itkRealTimeStamp.h"
#include "itkNumericTraits.h"

namespace itk
{

/*
 * Constructor to initialize a time stamp
 */
RealTimeStamp::RealTimeStamp()
{
  this->m_Seconds = itk::NumericTraits< SecondsCounterType >::Zero;
  this->m_MicroSeconds = itk::NumericTraits< MicroSecondsCounterType >::Zero;
}

/*
 * Constructor with values to initialize a time stamp.
 * This constructor should only be called from the RealTimeClock,
 * or from an file reading class.
 */
RealTimeStamp::RealTimeStamp( SecondsCounterType seconds, MicroSecondsCounterType micro_seconds )
{
  this->m_Seconds = seconds;
  this->m_MicroSeconds = micro_seconds;
}

/*
 * Destructor.
 */
RealTimeStamp::~RealTimeStamp()
{
}

/*
 * Return time in microseconds.
 */
double RealTimeStamp::GetTimeInMicroSeconds() const
{
  double result = static_cast< double >( this->m_Seconds );
  result *= 1e6;
  result += static_cast< double >( this->m_MicroSeconds );

  return result;
}

/*
 * Return time in second.
 */
double RealTimeStamp::GetTimeInSeconds() const
{
  double result = static_cast< double >( this->m_MicroSeconds );
  result /= 1e6;
  result += static_cast< double >( this->m_Seconds );

  return result;
}

/*
 * Return time in hours.
 */
double RealTimeStamp::GetTimeInHours() const
{
  const double result = this->GetTimeInSeconds() / 3600.00;
  return result;
}


/*
 * Return time in days.
 */
double RealTimeStamp::GetTimeInDays() const
{
  const double result = this->GetTimeInSeconds() / 86400.00;
  return result;
}


/*
 * Compute the time interval between two time stamps.
 */
RealTimeInterval
RealTimeStamp::operator-( const Self & other ) const
{
  SecondsDifferenceType      seconds       = this->m_Seconds      - other.m_Seconds;
  MicroSecondsDifferenceType micro_seconds = this->m_MicroSeconds - other.m_MicroSeconds;

  ALIGN_THE_ARROW_OF_TIME( seconds, micro_seconds );

  RealTimeInterval difference;

  difference.m_Seconds = seconds;
  difference.m_MicroSeconds = micro_seconds;

  return difference;
}


/*
 * Add a time interval to this time stamp to compute a new time stamp.
 */
RealTimeStamp
RealTimeStamp::operator+( const RealTimeInterval & difference ) const
{
  SecondsDifferenceType seconds = this->m_Seconds + difference.m_Seconds;

  if( seconds < 0 )
    {
    itkGenericExceptionMacro("RealTimeStamp can't go before the origin of time");
    }

  MicroSecondsDifferenceType micro_seconds = this->m_MicroSeconds + difference.m_MicroSeconds;

  CARRY_UNITS_OVER( seconds, micro_seconds );

  Self result;

  result.m_Seconds      = static_cast< SecondsCounterType >( seconds );
  result.m_MicroSeconds = static_cast< MicroSecondsCounterType >( micro_seconds );

  return result;
}


/*
 * Add a time interval to this time stamp to compute a new time stamp.
 */
RealTimeStamp
RealTimeStamp::operator-( const RealTimeInterval & difference ) const
{
  SecondsDifferenceType seconds = this->m_Seconds - difference.m_Seconds;

  if( seconds < 0 )
    {
    itkGenericExceptionMacro("RealTimeStamp can't go before the origin of time");
    }

  MicroSecondsDifferenceType micro_seconds = this->m_MicroSeconds - difference.m_MicroSeconds;

  CARRY_UNITS_OVER( seconds, micro_seconds );

  Self result;

  result.m_Seconds      = static_cast< SecondsCounterType >( seconds );
  result.m_MicroSeconds = static_cast< MicroSecondsCounterType >( micro_seconds );

  return result;
}

/*
 * Add a time interval to this time stamp and update it.
 */
const RealTimeStamp &
RealTimeStamp::operator+=( const RealTimeInterval & difference )
{
  SecondsDifferenceType seconds = this->m_Seconds + difference.m_Seconds;

  if( seconds < 0 )
    {
    itkGenericExceptionMacro("RealTimeStamp can't go before the origin of time");
    }

  MicroSecondsDifferenceType micro_seconds = this->m_MicroSeconds + difference.m_MicroSeconds;

  CARRY_UNITS_OVER( seconds, micro_seconds );

  this->m_Seconds      = static_cast< SecondsCounterType >( seconds );
  this->m_MicroSeconds = static_cast< MicroSecondsCounterType >( micro_seconds );

  return *this;
}

/*
 * Subtract a time interval from this time stamp and update it.
 */
const RealTimeStamp &
RealTimeStamp::operator-=( const RealTimeInterval & difference )
{
  SecondsDifferenceType seconds = this->m_Seconds - difference.m_Seconds;

  if( seconds < 0 )
    {
    itkGenericExceptionMacro("RealTimeStamp can't go before the origin of time");
    }

  MicroSecondsDifferenceType micro_seconds = this->m_MicroSeconds - difference.m_MicroSeconds;

  CARRY_UNITS_OVER( seconds, micro_seconds );

  this->m_Seconds      = static_cast< SecondsCounterType >( seconds );
  this->m_MicroSeconds = static_cast< MicroSecondsCounterType >( micro_seconds );

  return *this;
}

/*
 * Compare two time Stamps.
 */
bool
RealTimeStamp::operator>( const Self & other ) const
{
  if(  this->m_Seconds > other.m_Seconds )
    {
    return true;
    }

  if(  this->m_Seconds < other.m_Seconds )
    {
    return false;
    }

  return ( this->m_MicroSeconds > other.m_MicroSeconds );
}

/*
 * Compare two time Stamps.
 */
bool
RealTimeStamp::operator<( const Self & other ) const
{
  if(  this->m_Seconds < other.m_Seconds )
    {
    return true;
    }

  if(  this->m_Seconds > other.m_Seconds )
    {
    return false;
    }

  return ( this->m_MicroSeconds < other.m_MicroSeconds );
}

/*
 * Compare two time Stamps.
 */
bool
RealTimeStamp::operator>=( const Self & other ) const
{
  if(  this->m_Seconds > other.m_Seconds )
    {
    return true;
    }

  if(  this->m_Seconds < other.m_Seconds )
    {
    return false;
    }

  return ( this->m_MicroSeconds >= other.m_MicroSeconds );
}

/*
 * Compare two time Stamps.
 */
bool
RealTimeStamp::operator<=( const Self & other ) const
{
  if(  this->m_Seconds < other.m_Seconds )
    {
    return true;
    }

  if(  this->m_Seconds > other.m_Seconds )
    {
    return false;
    }

  return ( this->m_MicroSeconds <= other.m_MicroSeconds );
}


/*
 * Compare two time Stamps.
 */
bool
RealTimeStamp::operator==( const Self & other ) const
{
  return ( ( this->m_MicroSeconds == other.m_MicroSeconds ) &&
           ( this->m_Seconds == other.m_Seconds ) );
}

/*
 * Compare two time Stamps.
 */
bool
RealTimeStamp::operator!=( const Self & other ) const
{
  return ( ( this->m_MicroSeconds != other.m_MicroSeconds ) ||
           ( this->m_Seconds != other.m_Seconds ) );
}

} // end of namespace itk
