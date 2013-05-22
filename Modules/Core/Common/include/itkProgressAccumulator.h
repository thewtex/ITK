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
#ifndef __itkProgressAccumulator_h
#define __itkProgressAccumulator_h

#include "itkCommand.h"
#include "itkProcessObject.h"
#include <vector>

namespace itk
{
/**
 * \class ProgressAccumulator
 * \brief Facilitates progress reporting for filters that wrap around
 *        multiple other filters.
 *
 * This object allows a mini-pipeline filters to easily keep track of the
 * progress performed by the internal filters.
 * See DiscreteGaussianImageFilter.hxx for an implementation example.
 *
 * \sa DiscreteGaussianImageFilter
 *
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT ProgressAccumulator:public Object
{
public:
  /** Standard class typedefs. */
  typedef ProgressAccumulator        Self;
  typedef Object                     Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Typedef for inputting filters */
  typedef ProcessObject              GenericFilterType;
  typedef GenericFilterType::Pointer GenericFilterPointer;

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(ProgressAccumulator, Object);

  /** Get the total progress accumulated by this object */
  itkGetConstMacro(AccumulatedProgress, float);

  /** Set the mini-pipeline filter */
  itkSetObjectMacro(MiniPipelineFilter, ProcessObject);

  /** Set the mini-pipeline filter */
  itkGetModifiableObjectMacro(MiniPipelineFilter, ProcessObject);

  /**
   * Register a filter with the progress accumulator and specify the
   * fraction of the overall progress associated with this filter.
   * The sum of the weights for all filters that are registered should
   * be 1.
   * However, if streaming is used, the weight should be divided by the
   * number of streams because each stream will reset the filter
   * after capturing its progress.
   * For example, if the desired weight of a filter is 0.4, but it is
   * streamed into 4 streams, the weight should be 0.1.
   * The ProgressAccumulator will then ensure that each of the 4 runs
   * of this filter will add 0.1 to the filter's progress.
   */
  void RegisterInternalFilter(GenericFilterType *filter, float weight);

  /**
   * Unregister all filters that have been registered with this object
   */
  void UnregisterAllFilters();

  /**
   * Reset the progress accumulator.  This method should be called in
   * the beginning of the GenerateData() method in the mini-pipeline
   * filter.
   */
  void ResetProgress();

  /**
   * Reset the filter progress but keep the accumulated progress.
   * This method is no longer necessary because the ProgressAccumulator
   * now internally checks if a filter has been restarted and updates
   * the accumulated progress automatically.
   */
  void ResetFilterProgressAndKeepAccumulatedProgress();

protected:
  ProgressAccumulator();
  virtual ~ProgressAccumulator();
  void PrintSelf(std::ostream & s, Indent indent) const;

private:
  /**  Command for observing progress of pipeline filters */
  typedef MemberCommand< Self > CommandType;
  typedef CommandType::Pointer  CommandPointer;

  /** Structure associated with each filter in the pipeline */
  struct FilterRecord {
    // Pointer to the filter
    GenericFilterPointer Filter;

    // The weight of the filter in total progress of the mini-pipeline
    float Weight;

    // The tags for adding/removing observers to mini-pipeline filter
    unsigned long ProgressObserverTag;
    unsigned long IterationObserverTag;
    unsigned long StartObserverTag;
    unsigned long EndObserverTag;
  };

  /** A callback function that is called by the progressing filters */
  void ReportProgress(Object *object, const EventObject & event);

  /** The client mini-pipeline filter */
  GenericFilterPointer m_MiniPipelineFilter;

  /** An array of record structures */
  typedef std::vector< struct FilterRecord > FilterRecordVector;

  /** The total accumulated progress */
  float m_AccumulatedProgress;

  /** The total accumulated progress for multiple runs of the mini-pipeline */
  float m_BaseAccumulatedProgress;

  /**
   * A list of progress proportions of the different filters in the
   * pipeline
   */
  FilterRecordVector m_FilterRecord;

  /** The callback command */
  CommandPointer m_CallbackCommand;
};
} // End namespace itk

#endif // __itkProgressAccumulator_h_
