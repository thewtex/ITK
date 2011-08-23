
#ifndef __itkKittlerIllingworthThresholdCalculator_h
#define __itkKittlerIllingworthThresholdCalculator_h

#include "itkThresholdCalculator.h"

namespace itk
{

/** \class KittlerIllingworthThresholdCalculator
 * \brief Computes the KittlerIllingworth's threshold for an image.
 *
 * Kittler and J. Illingworth, "Minimum error thresholding," Pattern Recognition, vol. 19, pp. 41-47, 1986.
 * C. A. Glasbey, "An analysis of histogram-based thresholding algorithms," CVGIP: Graphical Models and Image Processing, vol. 55, pp. 532-537, 1993.
 * Original Matlab code Copyright (C) 2004 Antti Niemisto
 * See http://www.cs.tut.fi/~ant/histthresh/ for an excellent slide presentation
 * and the original Matlab code.
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
class ITK_EXPORT KittlerIllingworthThresholdCalculator : public ThresholdCalculator<THistogram, TOutput>
{
public:
  /** Standard class typedefs. */
  typedef KittlerIllingworthThresholdCalculator      Self;
  typedef Object                                     Superclass;
  typedef SmartPointer<Self>                         Pointer;
  typedef SmartPointer<const Self>                   ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(KittlerIllingworthThresholdCalculator, Object);

  /** Type definition for the input image. */
  typedef THistogram  HistogramType;
  typedef TOutput     OutputType;

protected:
  KittlerIllingworthThresholdCalculator() {};
  virtual ~KittlerIllingworthThresholdCalculator() {};
  void GenerateData(void);

private:
  KittlerIllingworthThresholdCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  int Mean();
  double A( int j);
  double B( int j);
  double C( int j);

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkKittlerIllingworthThresholdCalculator.hxx"
#endif

#endif
