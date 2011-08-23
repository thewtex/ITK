
#ifndef __itkIsoDataThresholdCalculator_h
#define __itkIsoDataThresholdCalculator_h

#include "itkThresholdCalculator.h"

namespace itk
{

/** \class IsoDataThresholdCalculator
 * \brief Computes the IsoData threshold for an image. Aka intermeans
 *
 * Iterative procedure based on the isodata algorithm [T.W. Ridler, S. Calvard, Picture
 * thresholding using an iterative selection method, IEEE Trans. System, Man and
 * Cybernetics, SMC-8 (1978) 630-632.]
 * The procedure divides the image into objects and background by taking an initial threshold,
 * then the averages of the pixels at or below the threshold and pixels above are computed.
 * The averages of those two values are computed, the threshold is incremented and the
 * process is repeated until the threshold is larger than the composite average. That is,
 * threshold = (average background + average objects)/2
 *
 * This class is templated over the input histogram type.
 * \warning This calculator assumes that the input histogram has only one dimension.
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
template <class THistogram, class TOutput=double>
class ITK_EXPORT IsoDataThresholdCalculator : public ThresholdCalculator<THistogram, TOutput>
{
public:
  /** Standard class typedefs. */
  typedef IsoDataThresholdCalculator      Self;
  typedef Object                          Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(IsoDataThresholdCalculator, Object);

  /** Type definition for the input image. */
  typedef THistogram  HistogramType;
  typedef TOutput     OutputType;

protected:
  IsoDataThresholdCalculator() {};
  virtual ~IsoDataThresholdCalculator() {};
  void GenerateData(void);

private:
  IsoDataThresholdCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkIsoDataThresholdCalculator.hxx"
#endif

#endif
