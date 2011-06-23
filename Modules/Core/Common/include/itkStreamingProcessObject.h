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
#ifndef __itkStreamingProcessObject_h
#define __itkStreamingProcessObject_h

#include "itkProcessObject.h"

namespace itk
{

/** \class StreamingProcessObject
* \brief StreamingProcessObject is a base class for processes which
* sequentially consume the input and may or may not have an output.
*
* StreammingProcessObject drives the Insight data processing pipeline
* multiple times to stream the data or perform out-of-core processing.
*
*
* \ingroup ITKSystemObjects
* \ingroup DataProcessing
* \ingroup Streamed
* \ingroup ITK-Common
*/
class ITKCommon_EXPORT StreamingProcessObject
  : public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef StreamingProcessObject                         Self;
  typedef ProcessObject                                  Superclass;
  typedef SmartPointer<Self>                             Pointer;
  typedef SmartPointer<const Self>                       ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(StreamingProcessObject,ProcessObject);

  /** \brief Override PropagateRequestedRegion from ProcessObject to
   *  aid in streaming.
   *
   *  Inside UpdateOutputData we iterate over streaming chunks, and
   *  each iteration call the upstream
   *  PropagateReqiestedRegion. Therefore, we don't need to proapage
   *  up the pipeline base on an output object.
   */
  virtual void PropagateRequestedRegion(DataObject *output);


  /** Override UpdateOutputData() from ProcessObject to divide upstream
   * updates into pieces. This filter does not have a GenerateData()
   * or ThreadedGenerateData() method.  Instead, all the work is done
   * in UpdateOutputData() since it must update a little, execute a little,
   * update some more, execute some more, etc. */
  virtual void UpdateOutputData(DataObject *output);

  /** the current request number of -1, it not currently streaming */
  virtual int GetCurrentRequestNumber( ) const { return m_CurrentRequestNumber; }

  virtual void ResetPipeline() { Superclass::ResetPipeline(); m_CurrentRequestNumber = -1; }

protected:
  StreamingProcessObject( void ) { m_CurrentRequestNumber = -1; }
  // ~StreamingProcessObject(); use default virtual implementation
  void PrintSelf(std::ostream& os, Indent indent) const { Superclass::PrintSelf(os,indent); }


  /** \warning this methods is not called and should not be overriden
    * in derived classes
    */
  virtual void GenerateData( void ) {}

  virtual unsigned int GetNumberOfInputRequestedRegions( void ) = 0;
  virtual void GenerateNthInputRequestedRegion( unsigned int inputRequestedRegionNumber ) = 0;

  virtual void StreamedGenerateData( unsigned int inputRequestedRegionNumber ) = 0;
  virtual void BeforeStreamedGenerateData( void ) {};
  virtual void AfterStreamedGenerateData( void ) {};

private:
  /** An internal helper method that is not intended to be
   * overloaded. It should be considered part of the GenerateData
   * methods but it has been separated to clarity.
   */
  void StreamingUpdateOutputData( );

private:
  StreamingProcessObject(const StreamingProcessObject&); //purposely not implemented
  void operator=(const StreamingProcessObject&); //purposely not implemented

  int m_CurrentRequestNumber;
};

} // end namespace itk

#endif //__itkStreamingProcessObject_h
