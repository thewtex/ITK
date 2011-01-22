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
#include "itkRealTimeInterval.h"
#include "itkNumericTraits.h"

namespace itk
{

/**
 * Constructor to initialize a time stamp
 */
RealTimeInterval::RealTimeInterval()
{
  this->m_Seconds = itk::NumericTraits< SecondsDifferenceType >::Zero;
  this->m_MicroSeconds = itk::NumericTraits< MicroSecondsDifferenceType >::Zero;
}

/**
 * Constructor to initialize a time stamp
 */
RealTimeInterval::RealTimeInterval( SecondsDifferenceType seconds, MicroSecondsDifferenceType micro_seconds )
{
  // Ensure that microseconds are carry over
  // to seconds if there are more than a million.
  seconds += micro_seconds / 1000000L;
  micro_seconds = micro_seconds % 1000000L;

  ALIGN_THE_ARROW_OF_TIME( seconds, micro_seconds );

  this->m_Seconds = seconds;
  this->m_MicroSeconds = micro_seconds;
}

/**
 * Destructor.
 */
RealTimeInterval::~RealTimeInterval()
{
}

/**
 * Set the interval to a given combination of seconds and micro seconds.
 */
void RealTimeInterval::Set( SecondsDifferenceType seconds, MicroSecondsDifferenceType micro_seconds )
{
  // Ensure that microseconds are carry over
  // to seconds if there are more than a million.
  seconds += micro_seconds / 1000000L;
  micro_seconds = micro_seconds % 1000000L;

  ALIGN_THE_ARROW_OF_TIME( seconds, micro_seconds );

  this->m_Seconds = seconds;
  this->m_MicroSeconds = micro_seconds;
}


/**
 * Return time in microseconds.
 */
double RealTimeInterval::GetTimeInMicroSeconds() const
{
  double result = static_cast< double >( this->m_Seconds );
  result *= 1e6;
  result += static_cast< double >( this->m_MicroSeconds );

  return result;
}

/**
 * Return time in second.
 */
double RealTimeInterval::GetTimeInSeconds() const
{
  double result = static_cast< double >( this->m_MicroSeconds );
  result /= 1e6;
  result += static_cast< double >( this->m_Seconds );

  return result;
}

/**
 * Return time in hours.
 */
double RealTimeInterval::GetTimeInHours() const
{
  const double result = this->GetTimeInSeconds() / 3600.00;
  return result;
}


/**
 * Return time in days.
 */
double RealTimeInterval::GetTimeInDays() const
{
  const double result = this->GetTimeInSeconds() / 86400.00;
  return result;
}


/*
 * Compute the time difference between two time intervals.
 */
RealTimeInterval
RealTimeInterval::operator-( const RealTimeInterval & other ) const
{
  SecondsDifferenceType       seconds       = this->m_Seconds      - other.m_Seconds;
  MicroSecondsDifferenceType  micro_seconds = this->m_MicroSeconds - other.m_MicroSeconds;

  ALIGN_THE_ARROW_OF_TIME( seconds, micro_seconds );

  RealTimeInterval result;

  result.m_Seconds = seconds;
  result.m_MicroSeconds = micro_seconds;

  return result;
}


/*
 * Compute the addition between two time intervals.
 */
RealTimeInterval
RealTimeInterval::operator+( const RealTimeInterval & other ) const
{
  SecondsDifferenceType       seconds       = this->m_Seconds      + other.m_Seconds;
  MicroSecondsDifferenceType  micro_seconds = this->m_MicroSeconds + other.m_MicroSeconds;

  ALIGN_THE_ARROW_OF_TIME( seconds, micro_seconds );

  RealTimeInterval result;

  result.m_Seconds = seconds;
  result.m_MicroSeconds = micro_seconds;

  return result;
}


/*
 * Add a time interval to this time stamp and update it.
 */
const RealTimeInterval &
RealTimeInterval::operator+=( const RealTimeInterval & other )
{
  SecondsDifferenceType       seconds       = this->m_Seconds      + other.m_Seconds;
  MicroSecondsDifferenceType  micro_seconds = this->m_MicroSeconds + other.m_MicroSeconds;

  ALIGN_THE_ARROW_OF_TIME( seconds, micro_seconds );

  this->m_Seconds = seconds;
  this->m_MicroSeconds = micro_seconds;

  return *this;
}

/*
 * Subtract a time interval from this time stamp and update it.
 */
const RealTimeInterval &
RealTimeInterval::operator-=( const RealTimeInterval & other )
{
  SecondsDifferenceType       seconds       = this->m_Seconds      - other.m_Seconds;
  MicroSecondsDifferenceType  micro_seconds = this->m_MicroSeconds - other.m_MicroSeconds;

  ALIGN_THE_ARROW_OF_TIME( seconds, micro_seconds );

  this->m_Seconds = seconds;
  this->m_MicroSeconds = micro_seconds;

  return *this;
}

/*
 * Compare two time Intervals.
 */
bool
RealTimeInterval::operator>( const Self & other ) const
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
 * Compare two time Intervals.
 */
bool
RealTimeInterval::operator<( const Self & other ) const
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
 * Compare two time Intervals.
 */
bool
RealTimeInterval::operator>=( const Self & other ) const
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
 * Compare two time Intervals.
 */
bool
RealTimeInterval::operator<=( const Self & other ) const
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
 * Compare two time Intervals.
 */
bool
RealTimeInterval::operator==( const Self & other ) const
{
  return ( ( this->m_MicroSeconds == other.m_MicroSeconds ) &&
           ( this->m_Seconds == other.m_Seconds ) );
}

/*
 * Compare two time Intervals.
 */
bool
RealTimeInterval::operator!=( const Self & other ) const
{
  return ( ( this->m_MicroSeconds != other.m_MicroSeconds ) ||
           ( this->m_Seconds != other.m_Seconds ) );
}

} // end of namespace itk
