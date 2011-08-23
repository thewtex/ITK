
#ifndef __itkOtsuThresholdCalculator_h
#define __itkOtsuThresholdCalculator_h

#include "itkThresholdCalculator.h"

namespace itk
{

/** \class OtsuThresholdCalculator
 * \brief Computes the Otsu's threshold for an image.
 *
 * This calculator computes the Otsu's threshold which separates an image
 * into foreground and background components. The method relies on a
 * histogram of image intensities. The basic idea is to maximize the
 * between-class variance.
 *
 * This class is templated over the input histogram type.
 *
 * \warning This calculator assumes that the input histogram has only one dimension.
 *
 * \ingroup Operators
 * \ingroup ITKThresholding
 */
template <class THistogram, class TOutput=double>
class ITK_EXPORT OtsuThresholdCalculator : public ThresholdCalculator<THistogram, TOutput>
{
public:
  /** Standard class typedefs. */
  typedef OtsuThresholdCalculator         Self;
  typedef Object                          Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(OtsuThresholdCalculator, Object);

  /** Type definition for the input image. */
  typedef THistogram  HistogramType;
  typedef TOutput     OutputType;

  /** for backward compatibility. Update() should be prefered. */
  void Compute()
  {
    this->Update();
  }

protected:
  OtsuThresholdCalculator() {};
  virtual ~OtsuThresholdCalculator() {};
  void GenerateData(void);

private:
  OtsuThresholdCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkOtsuThresholdCalculator.hxx"
#endif

#endif
