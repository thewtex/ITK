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
#ifndef __itkANTSNeighborhoodCorrelationImageToImageObjectMetric_hxx
#define __itkANTSNeighborhoodCorrelationImageToImageObjectMetric_hxx

#include "itkANTSNeighborhoodCorrelationImageToImageObjectMetric.h"
#include "itkNumericTraits.h"

namespace itk
{
template<class TFixedImage, class TMovingImage, class TVirtualImage>
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage,TVirtualImage>
::ANTSNeighborhoodCorrelationImageToImageObjectMetric()
{
  // We have our own GetValueAndDerivativeThreader's that we want
  // ImageToImageObjectMetric to use.
  this->m_DenseGetValueAndDerivativeThreader  = NeighborhoodScanningWindowDenseGetValueAndDerivativeThreader::New();
  // not implemented
  //this->m_SparseGetValueAndDerivativeThreader = NeighborhoodScanningWindowSparseGetValueAndDerivativeThreader::New();
}

template<class TFixedImage, class TMovingImage, class TVirtualImage>
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::~ANTSNeighborhoodCorrelationImageToImageObjectMetric()
{
}

template<class TFixedImage, class TMovingImage, class TVirtualImage>
typename ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>::MeasureType
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::GetValue() const
{
  itkExceptionMacro("GetValue not yet implemented.");
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
void
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::NeighborhoodScanningWindowDenseGetValueAndDerivativeThreader
::ThreadedExecution( EnclosingClassType * enclosingClass,
                     const DomainType & virtualImageSubRegion,
                     const ThreadIdType threadId )
{
  ANTSNeighborhoodCorrelationImageToImageObjectMetric * self = dynamic_cast< ANTSNeighborhoodCorrelationImageToImageObjectMetric * >( enclosingClass );

  VirtualPointType     virtualPoint;
  MeasureType          metricValueResult;
  MeasureType          metricValueSum = NumericTraits< MeasureType >::Zero;
  bool                 pointIsValid;
  ScanIteratorType     scanIt;
  ScanParametersType   scanParameters;
  ScanMemType          scanMem;

  DerivativeType & localDerivativeResult = this->m_LocalDerivativesPerThread[threadId];

  /* Create an iterator over the virtual sub region */
  self->InitializeScanning( virtualImageSubRegion, scanIt, scanMem,
      scanParameters );
  /* Iterate over the sub region */
  scanIt.GoToBegin();
  while (!scanIt.IsAtEnd())
    {
    /* Get the virtual point */
    self->m_VirtualDomainImage->TransformIndexToPhysicalPoint(
        scanIt.GetIndex(), virtualPoint);

    /* Call the user method in derived classes to do the specific
     * calculations for value and derivative. */
    try
      {
      this->UpdateQueues(scanIt, scanMem, scanParameters, threadId);
      pointIsValid = this->ComputeInformationFromQueues(scanIt,
          scanMem, scanParameters, threadId);
      this->ComputeMovingTransformDerivative(scanIt, scanMem,
          scanParameters, localDerivativeResult, metricValueResult,
          threadId );
      }
    catch (ExceptionObject & exc)
      {
      //NOTE: there must be a cleaner way to do this:
      std::string msg("Caught exception: \n");
      msg += exc.what();
      ExceptionObject err(__FILE__, __LINE__, msg);
      throw err;
      }

    /* Assign the results */
    if ( pointIsValid )
      {
      this->m_NumberOfValidPointsPerThread[threadId]++;
      metricValueSum += metricValueResult;
      /* Store the result. This depends on what type of
       * transform is being used. */
      this->StorePointDerivativeResult( scanIt.GetIndex(), threadId );
      }

    //next index
    ++scanIt;
    }

  /* Store metric value result for this thread. */
  this->m_MeasurePerThread[threadId] = metricValueSum;
}

template<class TFixedImage, class TMovingImage, class TVirtualImage>
void
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::InitializeScanning(
    const ImageRegionType &scanRegion, ScanIteratorType &scanIt,
    ScanMemType &, ScanParametersType &scanParameters ) const
{
  scanParameters.scanRegion   = scanRegion;
  scanParameters.fixedImage   = this->m_FixedImage;
  scanParameters.movingImage  = this->m_MovingImage;
  scanParameters.virtualImage = this->m_VirtualDomainImage;
  scanParameters.radius       = this->GetRadius();

  OffsetValueType numberOfFillZero = scanParameters.virtualImage->GetBufferedRegion().GetIndex(0)
      - (scanRegion.GetIndex(0) - scanParameters.radius[0]);
  if (numberOfFillZero < NumericTraits<OffsetValueType>::ZeroValue())
    numberOfFillZero = NumericTraits<OffsetValueType>::ZeroValue();
  scanParameters.numberOfFillZero = numberOfFillZero;

  scanIt = ScanIteratorType(scanParameters.radius, scanParameters.fixedImage, scanRegion);
  scanParameters.windowLength = scanIt.Size();
  scanParameters.scanRegionBeginIndexDim0 = scanIt.GetBeginIndex()[0];
}

template<class TFixedImage, class TMovingImage, class TVirtualImage>
void
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::NeighborhoodScanningWindowDenseGetValueAndDerivativeThreader
::UpdateQueuesAtBeginningOfLine(
    const ScanIteratorType &scanIt, ScanMemType &scanMem,
    const ScanParametersType &scanParameters, const ThreadIdType ) const
{
  ANTSNeighborhoodCorrelationImageToImageObjectMetric * self = dynamic_cast< ANTSNeighborhoodCorrelationImageToImageObjectMetric * >( this->m_EnclosingClass );

  const SizeValueType numberOfFillZero = scanParameters.numberOfFillZero;
  const SizeValueType hoodlen = scanParameters.windowLength;

  InternalComputationValueType zero
    = NumericTraits<InternalComputationValueType>::ZeroValue();
  scanMem.QsumFixed2 = SumQueueType(numberOfFillZero, zero);
  scanMem.QsumMoving2 = SumQueueType(numberOfFillZero, zero);
  scanMem.QsumFixed = SumQueueType(numberOfFillZero, zero);
  scanMem.QsumMoving = SumQueueType(numberOfFillZero, zero);
  scanMem.QsumFixedMoving = SumQueueType(numberOfFillZero, zero);
  scanMem.Qcount = SumQueueType(numberOfFillZero, zero);

  typedef InternalComputationValueType LocalRealType;

  // Now add the rest of the values from each hyperplane
  SizeValueType diameter = 2 * scanParameters.radius[0];

  const LocalRealType localZero = NumericTraits<LocalRealType>::ZeroValue();
  for (SizeValueType i = numberOfFillZero;
        i < ( diameter + NumericTraits<SizeValueType>::OneValue() ); i++)
    {
    LocalRealType sumFixed2      = localZero;
    LocalRealType sumMoving2     = localZero;
    LocalRealType sumFixed       = localZero;
    LocalRealType sumMoving      = localZero;
    LocalRealType sumFixedMoving = localZero;
    LocalRealType count = localZero;

    for ( SizeValueType indct = i; indct < hoodlen;
          indct += ( diameter + NumericTraits<SizeValueType>::OneValue() ) )
      {

      bool isInBounds = true;
      scanIt.GetPixel(indct, isInBounds);

      typename VirtualImageType::IndexType index = scanIt.GetIndex(indct);

      if (!isInBounds)
        {
        // std::cout << "DEBUG: error" << std::endl;
        continue;
        }

      VirtualPointType        virtualPoint;
      FixedOutputPointType    mappedFixedPoint;
      FixedImagePixelType     fixedImageValue;
      FixedImageGradientType  fixedImageGradient;
      MovingOutputPointType   mappedMovingPoint;
      MovingImagePixelType    movingImageValue;
      MovingImageGradientType movingImageGradient;
      bool pointIsValid;

      self->m_VirtualDomainImage->TransformIndexToPhysicalPoint(index,
          virtualPoint);

      try
        {
        pointIsValid = self->TransformAndEvaluateFixedPoint( index,
                          virtualPoint,
                          false/*compute gradient*/,
                          mappedFixedPoint,
                          fixedImageValue,
                          fixedImageGradient );
        if ( pointIsValid )
          {
          pointIsValid = self->TransformAndEvaluateMovingPoint(index,
                            virtualPoint,
                            false/*compute gradient*/,
                            mappedMovingPoint,
                            movingImageValue,
                            movingImageGradient );
          }
        }
      catch (ExceptionObject & exc)
        {
        //NOTE: there must be a cleaner way to do this:
        std::string msg("Caught exception: \n");
        msg += exc.what();
        ExceptionObject err(__FILE__, __LINE__, msg);
        throw err;
        }


      if ( pointIsValid )
        {
        sumFixed2 += fixedImageValue  * fixedImageValue;
        sumMoving2 += movingImageValue * movingImageValue;
        sumFixed += fixedImageValue;
        sumFixed += movingImageValue;
        sumFixedMoving += fixedImageValue * movingImageValue;
        count += NumericTraits<LocalRealType>::OneValue();
        }
      }//for indct

    scanMem.QsumFixed2.push_back(sumFixed2);
    scanMem.QsumMoving2.push_back(sumMoving2);
    scanMem.QsumFixed.push_back(sumFixed);
    scanMem.QsumMoving.push_back(sumMoving);
    scanMem.QsumFixedMoving.push_back(sumFixedMoving);
    scanMem.Qcount.push_back(count);
    }
}

template<class TFixedImage, class TMovingImage, class TVirtualImage>
void
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::NeighborhoodScanningWindowDenseGetValueAndDerivativeThreader
::UpdateQueuesToNextScanWindow(
    const ScanIteratorType &scanIt, ScanMemType &scanMem,
    const ScanParametersType &scanParameters, const ThreadIdType ) const
{
  ANTSNeighborhoodCorrelationImageToImageObjectMetric * self = dynamic_cast< ANTSNeighborhoodCorrelationImageToImageObjectMetric * >( this->m_EnclosingClass );

  const SizeValueType hoodlen = scanParameters.windowLength;

  typedef InternalComputationValueType LocalRealType;

  const LocalRealType localZero = NumericTraits<LocalRealType>::ZeroValue();

  LocalRealType sumFixed2      = localZero;
  LocalRealType sumMoving2     = localZero;
  LocalRealType sumFixed       = localZero;
  LocalRealType sumMoving      = localZero;
  LocalRealType sumFixedMoving = localZero;
  LocalRealType count          = localZero;

  SizeValueType diameter = 2 * scanParameters.radius[0];

  for ( SizeValueType indct = diameter; indct < hoodlen;
    indct += (diameter + NumericTraits<SizeValueType>::OneValue()))
    {
    bool isInBounds = true;

    scanIt.GetPixel(indct, isInBounds);
    typename VirtualImageType::IndexType index = scanIt.GetIndex(indct);

    if (!isInBounds)
    {
    continue;
    }

    VirtualPointType virtualPoint;
    FixedOutputPointType mappedFixedPoint;
    FixedImagePixelType fixedImageValue;
    FixedImageGradientType fixedImageGradient;
    MovingOutputPointType mappedMovingPoint;
    MovingImagePixelType movingImageValue;
    MovingImageGradientType movingImageGradient;
    bool pointIsValid;

    self->m_VirtualDomainImage->TransformIndexToPhysicalPoint(index, virtualPoint);
    try
      {
      pointIsValid = self->TransformAndEvaluateFixedPoint( index,
            virtualPoint,
            false/*compute gradient*/,
            mappedFixedPoint,
            fixedImageValue,
            fixedImageGradient );
      if (pointIsValid)
        {
        pointIsValid = self->TransformAndEvaluateMovingPoint( index,
              virtualPoint,
             false/*compute gradient*/,
             mappedMovingPoint,
             movingImageValue,
             movingImageGradient );
        }
      }
    catch (ExceptionObject & exc)
      {
      //NOTE: there must be a cleaner way to do this:
      std::string msg("Caught exception: \n");
      msg += exc.what();
      ExceptionObject err(__FILE__, __LINE__, msg);
      throw err;
      }
    if ( pointIsValid )
      {
      sumFixed2 += fixedImageValue  * fixedImageValue;
      sumMoving2 += movingImageValue * movingImageValue;
      sumFixed += fixedImageValue;
      sumFixed += movingImageValue;
      sumFixedMoving += fixedImageValue * movingImageValue;
      count += NumericTraits<LocalRealType>::OneValue();
      }
    }
    scanMem.QsumFixed2.push_back(sumFixed2);
    scanMem.QsumMoving2.push_back(sumMoving2);
    scanMem.QsumFixed.push_back(sumFixed);
    scanMem.QsumMoving.push_back(sumMoving);
    scanMem.QsumFixedMoving.push_back(sumFixedMoving);
    scanMem.Qcount.push_back(count);

    scanMem.QsumFixed2.pop_front();
    scanMem.QsumMoving2.pop_front();
    scanMem.QsumFixed.pop_front();
    scanMem.QsumMoving.pop_front();
    scanMem.QsumFixedMoving.pop_front();
    scanMem.Qcount.pop_front();
}

template<class TFixedImage, class TMovingImage, class TVirtualImage>
void
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::NeighborhoodScanningWindowDenseGetValueAndDerivativeThreader
::UpdateQueues( const ScanIteratorType &scanIt, ScanMemType &scanMem,
                const ScanParametersType &scanParameters, const ThreadIdType threadId) const
{
  if (scanIt.GetIndex()[0] == scanParameters.scanRegionBeginIndexDim0 )
    {
    this->UpdateQueuesAtBeginningOfLine(scanIt, scanMem, scanParameters, threadId);
    }
  else
    {
    this->UpdateQueuesToNextScanWindow(scanIt, scanMem, scanParameters, threadId);
    }
}

template<class TFixedImage, class TMovingImage, class TVirtualImage>
bool
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::NeighborhoodScanningWindowDenseGetValueAndDerivativeThreader
::ComputeInformationFromQueues( const ScanIteratorType &scanIt, ScanMemType &scanMem, const ScanParametersType &, const ThreadIdType ) const
{
  ANTSNeighborhoodCorrelationImageToImageObjectMetric * self = dynamic_cast< ANTSNeighborhoodCorrelationImageToImageObjectMetric * >( this->m_EnclosingClass );

  typedef InternalComputationValueType LocalRealType;

  const LocalRealType localZero = NumericTraits<LocalRealType>::ZeroValue();

  LocalRealType count = localZero;

  typename SumQueueType::iterator itcount = scanMem.Qcount.begin();
  while (itcount != scanMem.Qcount.end())
    {
    count += *itcount;
    ++itcount;
    }

  if (count < localZero)
    {
    // no points available in the queue, perhaps out of image region
    return false;
    }

  // If there are values, we need to calculate the different quantities
  LocalRealType sumFixed2      = localZero;
  LocalRealType sumMoving2     = localZero;
  LocalRealType sumFixed       = localZero;
  LocalRealType sumMoving      = localZero;
  LocalRealType sumFixedMoving = localZero;
  typename SumQueueType::iterator itFixed2      = scanMem.QsumFixed2.begin();
  typename SumQueueType::iterator itMoving2     = scanMem.QsumMoving2.begin();
  typename SumQueueType::iterator itFixed       = scanMem.QsumFixed.begin();
  typename SumQueueType::iterator itMoving      = scanMem.QsumMoving.begin();
  typename SumQueueType::iterator itFixedMoving = scanMem.QsumFixedMoving.begin();

  while (itFixed2 != scanMem.QsumFixed2.end())
    {
    sumFixed2 += *itFixed2;
    sumMoving2 += *itMoving2;
    sumFixed += *itFixed;
    sumMoving += *itMoving;
    sumFixedMoving += *itFixedMoving;

    ++itFixed2;
    ++itMoving2;
    ++itFixed;
    ++itMoving;
    ++itFixedMoving;
    }

  LocalRealType fixedMean  = sumFixed  / count;
  LocalRealType movingMean = sumMoving / count;

  LocalRealType sFixedFixed = sumFixed2 - fixedMean * sumFixed - fixedMean * sumFixed
    + count * fixedMean * fixedMean;
  LocalRealType sMovingMoving = sumMoving2 - movingMean * sumMoving - movingMean * sumMoving
    + count * movingMean * movingMean;
  LocalRealType sFixedMoving = sumFixedMoving - movingMean * sumFixed - fixedMean * sumMoving
    + count * movingMean * fixedMean;

  typename VirtualImageType::IndexType oindex = scanIt.GetIndex();

  VirtualPointType        virtualPoint;
  FixedOutputPointType    mappedFixedPoint;
  FixedImagePixelType     fixedImageValue;
  FixedImageGradientType  fixedImageGradient;
  MovingOutputPointType   mappedMovingPoint;
  MovingImagePixelType    movingImageValue;
  MovingImageGradientType movingImageGradient;
  bool pointIsValid;

  self->m_VirtualDomainImage->TransformIndexToPhysicalPoint(oindex, virtualPoint);

  try
    {
    pointIsValid = self->TransformAndEvaluateFixedPoint( oindex,
            virtualPoint,
            true/*compute gradient*/,
            mappedFixedPoint,
            fixedImageValue,
            fixedImageGradient );
    if ( pointIsValid )
      {
      pointIsValid = self->TransformAndEvaluateMovingPoint( oindex,
             virtualPoint,
             true/*compute gradient*/,
             mappedMovingPoint,
             movingImageValue,
             movingImageGradient );
      }
    }
  catch (ExceptionObject & exc)
    {
    //NOTE: there must be a cleaner way to do this:
    std::string msg("Caught exception: \n");
    msg += exc.what();
    ExceptionObject err(__FILE__, __LINE__, msg);
    throw err;
    }

  if ( pointIsValid )
    {
    scanMem.fixedA        = fixedImageValue  - fixedMean; // scanParameters.I->GetPixel(oindex) - fixedMean;
    scanMem.movingA       = movingImageValue - movingMean; // scanParameters.J->GetPixel(oindex) - movingMean;
    scanMem.sFixedMoving  = sFixedMoving;
    scanMem.sFixedFixed   = sFixedFixed;
    scanMem.sMovingMoving = sMovingMoving;

    scanMem.fixedImageGradient  = fixedImageGradient;
    scanMem.movingImageGradient = movingImageGradient;

    scanMem.mappedMovingPoint = mappedMovingPoint;
    }

  return pointIsValid;
}

template<class TFixedImage, class TMovingImage, class TVirtualImage>
void
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::NeighborhoodScanningWindowDenseGetValueAndDerivativeThreader
::ComputeMovingTransformDerivative(
  const ScanIteratorType &, ScanMemType &scanMem,
  const ScanParametersType &, DerivativeType &deriv,
  MeasureType &localCC, const ThreadIdType threadId) const
{
  MovingImageGradientType derivWRTImage;
  localCC = NumericTraits<MeasureType>::OneValue();

  typedef InternalComputationValueType LocalRealType;

  LocalRealType sFixedFixed   = scanMem.sFixedFixed;
  LocalRealType sMovingMoving = scanMem.sMovingMoving;
  LocalRealType sFixedMoving  = scanMem.sFixedMoving;
  LocalRealType fixedI        = scanMem.fixedA;
  LocalRealType movingI       = scanMem.movingA;

  const MovingImageGradientType movingImageGradient = scanMem.movingImageGradient;

  if (sFixedFixed == NumericTraits< LocalRealType >::Zero || sMovingMoving == NumericTraits< LocalRealType >::Zero )
    {
    deriv.Fill( NumericTraits< DerivativeValueType >::Zero );
    return;
    }

  for (ImageDimensionType qq = 0; qq < VirtualImageDimension; qq++)
    {
    derivWRTImage[qq] = 2.0 * sFixedMoving / (sFixedFixed * sMovingMoving) * (fixedI - sFixedMoving / sMovingMoving * movingI)
      * movingImageGradient[qq];
    }

  if (fabs(sFixedFixed * sMovingMoving) > 0)
    {
    localCC = sFixedMoving * sFixedMoving / (sFixedFixed * sMovingMoving);
    }

  /* Use a pre-allocated jacobian object for efficiency */
  JacobianType & jacobian =
    this->m_MovingTransformJacobianPerThread[threadId];

  /** For dense transforms, this returns identity */
  this->m_EnclosingClass->GetMovingTransform()->ComputeJacobianWithRespectToParameters(
    scanMem.mappedMovingPoint, jacobian);

  NumberOfParametersType numberOfLocalParameters = this->m_EnclosingClass->GetMovingTransform()->GetNumberOfLocalParameters();

  // this correction is necessary for consistent derivatives across N threads
  double floatingpointcorrectionresolution=10000.0;
  for (NumberOfParametersType par = 0; par < numberOfLocalParameters; par++)
    {
    double sum = NumericTraits< double >::Zero;
    for (ImageDimensionType dim = 0; dim < MovingImageDimension; dim++)
      {
      sum += derivWRTImage[dim] * jacobian(dim, par);
      }
    int floatingpointcorrection_int=static_cast<int>( sum * floatingpointcorrectionresolution );
    deriv[par] = (double)floatingpointcorrection_int/floatingpointcorrectionresolution;
    }
  return;
}

template<class TFixedImage, class TMovingImage, class TVirtualImage>
void
ANTSNeighborhoodCorrelationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Correlation window radius: " << m_Radius << std::endl;
}

} // end namespace itk

#endif
