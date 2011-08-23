
#ifndef __itkThresholdCalculator_h
#define __itkThresholdCalculator_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkNumericTraits.h"
#include "itkSimpleDataObjectDecorator.h"
#include "itkProgressReporter.h"

namespace itk
{

/** \class ThresholdCalculator
 * \brief Base class to compute a threshold value based on the histogram of an image
 *
 * \author Richard Beare
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/10380/3279  or
 * http://www.insight-journal.org/browse/publication/811
 *
 * \ingroup Operators
 * \ingroup ITKThresholding
 */
template <class THistogram, class TOutput>
class ITK_EXPORT ThresholdCalculator : public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef ThresholdCalculator             Self;
  typedef ProcessObject                   Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThresholdCalculator, ProcessObject);

  /** Type definition for the input histogram. */
  typedef THistogram HistogramType;

  /** output object type */
  typedef TOutput                               OutputType;
  typedef SimpleDataObjectDecorator<OutputType> DecoratedOutputType;

  void SetInput( const HistogramType * input )
  {
    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput( 0, const_cast< HistogramType * >( input ) );
  }

  const HistogramType * GetInput() const
  {
    if ( this->GetNumberOfInputs() < 1 )
      {
      return 0;
      }
    return static_cast< const HistogramType * >( this->ProcessObject::GetInput(0) );
  }

  DecoratedOutputType * GetOutput()
  {
    if ( this->GetNumberOfOutputs() < 1 )
      {
      return 0;
      }
    return static_cast< DecoratedOutputType * >( this->ProcessObject::GetOutput(0) );
  }

  virtual typename DataObject::Pointer MakeOutput(unsigned int idx)
  {
    return DecoratedOutputType::New().GetPointer();
  }

  const OutputType & GetThreshold()
  {
    if ( this->GetNumberOfOutputs() < 1 )
      {
      itkExceptionMacro(<<"No output available.");
      }
    return static_cast< DecoratedOutputType * >( this->ProcessObject::GetOutput(0) )->Get();
  }

protected:
  ThresholdCalculator()
  {
    this->ProcessObject::SetNumberOfRequiredOutputs(1);
    this->ProcessObject::SetNthOutput( 0, this->MakeOutput(0) );
  }
  virtual ~ThresholdCalculator() {};

private:
  ThresholdCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#endif
