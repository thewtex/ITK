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
#ifndef __itkANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader_hxx
#define __itkANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader_hxx

#include "itkANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader.h"

namespace itk
{

template < class TDomainPartitioner, class TImageToImageMetric, class TNeighborhoodCorrelationMetric >
void
ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric, TNeighborhoodCorrelationMetric >
::ThreadedExecution_impl(
    IdentityHelper<ThreadedImageRegionPartitioner<TImageToImageMetric::VirtualImageDimension> > itkNotUsed(self),
    const DomainType& virtualImageSubRegion,
    const ThreadIdType threadId )
{
  /* Store the casted pointer to avoid dynamic casting in tight loops. */
  this->m_ANTSAssociate = dynamic_cast< TNeighborhoodCorrelationMetric * >( this->m_Associate );
  if( this->m_ANTSAssociate == NULL )
    {
    itkExceptionMacro("Dynamic casting of associate pointer failed.");
    }

  DerivativeType & localDerivativeResult = this->m_LocalDerivativesPerThread[threadId];

  /* Create an iterator over the virtual sub region */
  ScanMemType          scanMem;
  ScanParametersType   scanParameters;
  ScanIteratorType     scanIt;
  this->InitializeScanning( virtualImageSubRegion, scanIt, scanMem, scanParameters );
  scanIt.GoToBegin();

  /* Iterate over the sub region */
  MeasureType          metricValueSum = NumericTraits< MeasureType >::Zero;
  while (!scanIt.IsAtEnd())
    {
    /* Call the user method in derived classes to do the specific
     * calculations for value and derivative. */
    try
      {
      this->UpdateQueues(scanIt, scanMem, scanParameters, threadId);
      const bool pointIsValid = this->ComputeInformationFromQueues(scanIt, scanMem, scanParameters, threadId);
      if( pointIsValid )
        {
        MeasureType metricValueResult;
        this->ComputeMovingTransformDerivative(scanIt, scanMem, scanParameters, localDerivativeResult, metricValueResult, threadId );
        /* Assign the results */
        this->m_NumberOfValidPointsPerThread[threadId]++;
        metricValueSum -= metricValueResult;
        /* Store the result. This depends on what type of
         * transform is being used. */
        if( this->GetComputeDerivative() )
          {
          this->StorePointDerivativeResult( scanIt.GetIndex(), threadId );
          }
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

    //next index
    ++scanIt;
    }

  /* Store metric value result for this thread. */
  this->m_MeasurePerThread[threadId] = metricValueSum;
}

template < class TDomainPartitioner, class TImageToImageMetric, class TNeighborhoodCorrelationMetric >
template < class T >
void
ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric, TNeighborhoodCorrelationMetric >
::ThreadedExecution_impl(
    IdentityHelper<T> itkNotUsed(self),
    const DomainType& domain,
    const ThreadIdType threadId )
{
    /* call base method */
    /* Store the casted pointer to avoid dynamic casting in tight loops. */
    this->m_ANTSAssociate = dynamic_cast< TNeighborhoodCorrelationMetric * >( this->m_Associate );
    if( this->m_ANTSAssociate == NULL )
      {
      itkExceptionMacro("Dynamic casting of associate pointer failed.");
      }

    Superclass::ThreadedExecution(domain, threadId);
}

template < class TDomainPartitioner, class TImageToImageMetric, class TNeighborhoodCorrelationMetric >
void
ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric, TNeighborhoodCorrelationMetric >
::UpdateQueuesAtBeginningOfLine( const ScanIteratorType &scanIt, ScanMemType &scanMem, const ScanParametersType &scanParameters, const ThreadIdType ) const
 {
   const SizeValueType numberOfFillZero = scanParameters.numberOfFillZero;
   const SizeValueType hoodlen = scanParameters.windowLength;

   InternalComputationValueType zero = NumericTraits<InternalComputationValueType>::ZeroValue();
   scanMem.QsumFixed2 = SumQueueType(numberOfFillZero, zero);
   scanMem.QsumMoving2 = SumQueueType(numberOfFillZero, zero);
   scanMem.QsumFixed = SumQueueType(numberOfFillZero, zero);
   scanMem.QsumMoving = SumQueueType(numberOfFillZero, zero);
   scanMem.QsumFixedMoving = SumQueueType(numberOfFillZero, zero);
   scanMem.Qcount = SumQueueType(numberOfFillZero, zero);

   typedef InternalComputationValueType LocalRealType;

   const LocalRealType localZero = NumericTraits<LocalRealType>::ZeroValue();
   const LocalRealType localOne = NumericTraits<LocalRealType>::OneValue();

   // Now add the rest of the values from each hyperplane
   const SizeValueType diameter = 2 * scanParameters.radius[0];
   const SizeValueType diameterPlusOne=diameter+localOne;

   for (SizeValueType i = numberOfFillZero; i < diameterPlusOne; i++)
     {
     LocalRealType sumFixed2      = localZero;
     LocalRealType sumMoving2     = localZero;
     LocalRealType sumFixed       = localZero;
     LocalRealType sumMoving      = localZero;
     LocalRealType sumFixedMoving = localZero;
     LocalRealType count = localZero;

     for ( SizeValueType indct = i; indct < hoodlen; indct += diameterPlusOne )
       {
         {
         typename ScanIteratorType::OffsetType internalIndex;
         typename ScanIteratorType::OffsetType offset;
         const bool isInBounds = scanIt.IndexInBounds( indct, internalIndex, offset );
         if (!isInBounds)
           {
           // std::cout << "DEBUG: error" << std::endl;
           continue;
           }
         }

       const typename VirtualImageType::IndexType index = scanIt.GetIndex(indct);

       FixedImagePointType     mappedFixedPoint;
       FixedImagePixelType     fixedImageValue;
       MovingImagePointType    mappedMovingPoint;
       MovingImagePixelType    movingImageValue;

       VirtualPointType        virtualPoint;
       this->m_ANTSAssociate->TransformVirtualIndexToPhysicalPoint(index, virtualPoint);
       try
         {
         const bool pointIsValid = (
           this->m_ANTSAssociate->TransformAndEvaluateFixedPoint( virtualPoint, mappedFixedPoint, fixedImageValue )
           && this->m_ANTSAssociate->TransformAndEvaluateMovingPoint( virtualPoint, mappedMovingPoint, movingImageValue ) );
         if ( pointIsValid )
           {
           sumFixed2 += fixedImageValue  * fixedImageValue;
           sumMoving2 += movingImageValue * movingImageValue;
           sumFixed += fixedImageValue;
           sumMoving += movingImageValue;
           sumFixedMoving += fixedImageValue * movingImageValue;
           count += NumericTraits<LocalRealType>::OneValue();
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

       }//for indct

     scanMem.QsumFixed2.push_back(sumFixed2);
     scanMem.QsumMoving2.push_back(sumMoving2);
     scanMem.QsumFixed.push_back(sumFixed);
     scanMem.QsumMoving.push_back(sumMoving);
     scanMem.QsumFixedMoving.push_back(sumFixedMoving);
     scanMem.Qcount.push_back(count);
     }
 }

template < class TDomainPartitioner, class TImageToImageMetric, class TNeighborhoodCorrelationMetric >
 void
 ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric, TNeighborhoodCorrelationMetric >
::UpdateQueuesToNextScanWindow( const ScanIteratorType &scanIt, ScanMemType &scanMem, const ScanParametersType &scanParameters, const ThreadIdType ) const
{
 typedef InternalComputationValueType LocalRealType;
 const LocalRealType localZero = NumericTraits<LocalRealType>::ZeroValue();
 const LocalRealType localOne = NumericTraits<LocalRealType>::OneValue();

 LocalRealType sumFixed2      = localZero;
 LocalRealType sumMoving2     = localZero;
 LocalRealType sumFixed       = localZero;
 LocalRealType sumMoving      = localZero;
 LocalRealType sumFixedMoving = localZero;
 LocalRealType count          = localZero;

 const SizeValueType diameter = 2 * scanParameters.radius[0];
 const SizeValueType diameterPlusOne=diameter + localOne;
 const SizeValueType hoodlen = scanParameters.windowLength;
 for ( SizeValueType indct = diameter; indct < hoodlen; indct += diameterPlusOne)
   {
   //typename ScanIteratorType::OffsetType internalIndex;
   //typename ScanIteratorType::OffsetType offset;
   //const bool isInBounds = scanIt.IndexInBounds( indct , internalIndex, offset );
   const bool isInBounds = scanIt.IndexInBounds( indct );
   if (!isInBounds)
     {
     continue;
     }

   const typename VirtualImageType::IndexType &index = scanIt.GetIndex(indct);

   VirtualPointType virtualPoint;
   this->m_ANTSAssociate->TransformVirtualIndexToPhysicalPoint(index, virtualPoint);
   try
     {
     FixedImagePointType mappedFixedPoint;
     FixedImagePixelType fixedImageValue;
     const bool fixedPointIsValid = this->m_ANTSAssociate->TransformAndEvaluateFixedPoint( virtualPoint, mappedFixedPoint, fixedImageValue );
     if(fixedPointIsValid)
       {
       MovingImagePointType mappedMovingPoint;
       MovingImagePixelType movingImageValue;
       const bool movingPointIsValid = this->m_ANTSAssociate->TransformAndEvaluateMovingPoint( virtualPoint, mappedMovingPoint, movingImageValue );
       if ( movingPointIsValid )
         {
         sumFixed2 += fixedImageValue  * fixedImageValue;
         sumMoving2 += movingImageValue * movingImageValue;
         sumFixed += fixedImageValue;
         sumMoving += movingImageValue;
         sumFixedMoving += fixedImageValue * movingImageValue;
         count += localOne;
         }
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

template < class TDomainPartitioner, class TImageToImageMetric, class TNeighborhoodCorrelationMetric >
void
ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric, TNeighborhoodCorrelationMetric >
::InitializeScanning( const ImageRegionType &scanRegion, ScanIteratorType &scanIt,
                      ScanMemType & scanMem, ScanParametersType &scanParameters ) const
{
  scanParameters.scanRegion   = scanRegion;
  scanParameters.fixedImage   = this->m_ANTSAssociate->m_FixedImage;
  scanParameters.movingImage  = this->m_ANTSAssociate->m_MovingImage;
  scanParameters.virtualImage = this->m_ANTSAssociate->GetVirtualImage();
  scanParameters.radius       = this->m_ANTSAssociate->GetRadius();

  OffsetValueType numberOfFillZero = this->m_ANTSAssociate->GetVirtualRegion().GetIndex(0)
      - (scanRegion.GetIndex(0) - scanParameters.radius[0]);

  if (numberOfFillZero < NumericTraits<OffsetValueType>::ZeroValue())
    {
    numberOfFillZero = NumericTraits<OffsetValueType>::ZeroValue();
    }

  scanParameters.numberOfFillZero = numberOfFillZero;

  scanIt = ScanIteratorType(scanParameters.radius, scanParameters.virtualImage, scanRegion);
  scanParameters.windowLength = scanIt.Size();
  scanParameters.scanRegionBeginIndexDim0 = scanIt.GetBeginIndex()[0];

  scanMem.fixedA = NumericTraits< QueueRealType >::Zero;
  scanMem.movingA = NumericTraits< QueueRealType >::Zero;
  scanMem.sFixedMoving = NumericTraits< QueueRealType >::Zero;
  scanMem.sFixedFixed = NumericTraits< QueueRealType >::Zero;
  scanMem.sMovingMoving = NumericTraits< QueueRealType >::Zero;

  scanMem.fixedImageGradient.Fill(0.0);
  scanMem.movingImageGradient.Fill(0.0);
  scanMem.mappedMovingPoint.Fill(0.0);
}

template < class TDomainPartitioner, class TImageToImageMetric, class TNeighborhoodCorrelationMetric >
void
ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric, TNeighborhoodCorrelationMetric >
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


template < class TDomainPartitioner, class TImageToImageMetric, class TNeighborhoodCorrelationMetric >
bool
ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric, TNeighborhoodCorrelationMetric >
::ComputeInformationFromQueues( const ScanIteratorType &scanIt, ScanMemType &scanMem, const ScanParametersType &, const ThreadIdType ) const
{
 typedef InternalComputationValueType LocalRealType;

 const LocalRealType localZero = NumericTraits<LocalRealType>::ZeroValue();

 LocalRealType count = localZero;

 typename SumQueueType::iterator itcount = scanMem.Qcount.begin();
 while (itcount != scanMem.Qcount.end())
   {
   count += *itcount;
   ++itcount;
   }

 if (count <= localZero)
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

 const LocalRealType fixedMean  = sumFixed  / count;
 const LocalRealType movingMean = sumMoving / count;

 const LocalRealType sFixedFixed   = sumFixed2 - fixedMean * sumFixed - fixedMean * sumFixed + count * fixedMean * fixedMean;
 const LocalRealType sMovingMoving = sumMoving2 - movingMean * sumMoving - movingMean * sumMoving + count * movingMean * movingMean;
 const LocalRealType sFixedMoving  = sumFixedMoving - movingMean * sumFixed - fixedMean * sumMoving + count * movingMean * fixedMean;

 typename VirtualImageType::IndexType oindex = scanIt.GetIndex();

 VirtualPointType        virtualPoint;
 FixedImagePointType    mappedFixedPoint;
 FixedImagePixelType     fixedImageValue;
 FixedImageGradientType  fixedImageGradient;
 MovingImagePointType   mappedMovingPoint;
 MovingImagePixelType    movingImageValue;
 MovingImageGradientType movingImageGradient;
 bool pointIsValid;

 this->m_ANTSAssociate->TransformVirtualIndexToPhysicalPoint(oindex, virtualPoint);

 try
   {
   pointIsValid = this->m_ANTSAssociate->TransformAndEvaluateFixedPoint( virtualPoint, mappedFixedPoint, fixedImageValue );
   if ( pointIsValid )
     {
     pointIsValid = this->m_ANTSAssociate->TransformAndEvaluateMovingPoint( virtualPoint, mappedMovingPoint, movingImageValue );
     if( pointIsValid && this->m_ANTSAssociate->GetComputeDerivative() )
       {
       if( this->m_ANTSAssociate->GetGradientSourceIncludesFixed() )
         {
         this->m_ANTSAssociate->ComputeFixedImageGradientAtPoint( mappedFixedPoint, fixedImageGradient );
         }
       if( this->m_ANTSAssociate->GetGradientSourceIncludesMoving() )
         {
         this->m_ANTSAssociate->ComputeMovingImageGradientAtPoint( mappedMovingPoint, movingImageGradient );
         }
       scanMem.fixedA        = fixedImageValue  - fixedMean;
       scanMem.movingA       = movingImageValue - movingMean;
       scanMem.sFixedMoving  = sFixedMoving;
       scanMem.sFixedFixed   = sFixedFixed;
       scanMem.sMovingMoving = sMovingMoving;

       scanMem.fixedImageGradient  = fixedImageGradient;
       scanMem.movingImageGradient = movingImageGradient;

       scanMem.mappedFixedPoint  = mappedFixedPoint;
       scanMem.mappedMovingPoint = mappedMovingPoint;
       scanMem.virtualPoint      = virtualPoint;
       }
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
 return pointIsValid;
}

template < class TDomainPartitioner, class TImageToImageMetric, class TNeighborhoodCorrelationMetric >
void
ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric, TNeighborhoodCorrelationMetric >
::ComputeMovingTransformDerivative( const ScanIteratorType &, ScanMemType &scanMem, const ScanParametersType &, DerivativeType &deriv, MeasureType &localCC, const ThreadIdType threadId) const
{
  typedef InternalComputationValueType LocalRealType;

  LocalRealType sFixedFixed   = scanMem.sFixedFixed;
  LocalRealType sMovingMoving = scanMem.sMovingMoving;
  LocalRealType sFixedMoving  = scanMem.sFixedMoving;
  LocalRealType fixedI        = scanMem.fixedA;
  LocalRealType movingI       = scanMem.movingA;

  const LocalRealType sFixedFixed_sMovingMoving = sFixedFixed * sMovingMoving;
  const LocalRealType inv_sFixedFixed_sMovingMoving = 1.0/sFixedFixed_sMovingMoving;

  localCC = NumericTraits<MeasureType>::OneValue();
  if ( fabs(sFixedFixed_sMovingMoving) > NumericTraits< LocalRealType >::epsilon() )
    {
    localCC = sFixedMoving * sFixedMoving * inv_sFixedFixed_sMovingMoving;
    }

  if( this->m_ANTSAssociate->GetComputeDerivative() )
    {
    deriv.Fill( NumericTraits<DerivativeValueType>::Zero );
    const MovingImageGradientType movingImageGradient = scanMem.movingImageGradient;

    if ( ! (sFixedFixed > NumericTraits<LocalRealType>::epsilon() && sMovingMoving > NumericTraits<LocalRealType>::epsilon() ) )
      {
      return;
      }

    MovingImageGradientType derivWRTImage;
    for (ImageDimensionType qq = 0; qq < TImageToImageMetric::VirtualImageDimension; qq++)
      {
      derivWRTImage[qq] = 2.0 * sFixedMoving * inv_sFixedFixed_sMovingMoving * (fixedI - sFixedMoving / sMovingMoving * movingI) * movingImageGradient[qq];
      }

    /* Use a pre-allocated jacobian object for efficiency */
    JacobianType & jacobian = this->m_MovingTransformJacobianPerThread[threadId];

    /** For dense transforms, this returns identity */
    this->m_Associate->GetMovingTransform()->ComputeJacobianWithRespectToParameters( scanMem.virtualPoint, jacobian );

    const NumberOfParametersType numberOfLocalParameters = this->m_Associate->GetMovingTransform()->GetNumberOfLocalParameters();
    for (NumberOfParametersType par = 0; par < numberOfLocalParameters; par++)
      {
      for (ImageDimensionType dim = 0; dim < TImageToImageMetric::MovingImageDimension; dim++)
        {
        deriv[par] += derivWRTImage[dim] * jacobian(dim, par);
        }
      }
    }
  return;
}

/*
 * Specific implementation for sparse threader. It reuse most of the routine from the dense threader by
 * reinitializing the scanning at every point.
 */
template < class TDomainPartitioner, class TImageToImageMetric, class TNeighborhoodCorrelationMetric >
bool
ANTSNeighborhoodCorrelationImageToImageMetricv4GetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric, TNeighborhoodCorrelationMetric >
::ProcessVirtualPoint_impl(IdentityHelper<ThreadedIndexedContainerPartitioner> itkNotUsed(self),
    const VirtualIndexType & virtualIndex, const VirtualPointType & itkNotUsed(virtualPoint),
    const ThreadIdType threadId )
{

  MeasureType          metricValueResult = NumericTraits< MeasureType >::Zero;
  bool                 pointIsValid;
  ScanIteratorType     scanIt;
  ScanParametersType   scanParameters;
  ScanMemType          scanMem;

  DerivativeType & localDerivativeResult = this->m_LocalDerivativesPerThread[threadId];


  // convert virtualPoint to a single point region
  ImageRegionType singlePointRegion;
  singlePointRegion.SetIndex(virtualIndex);
  typename ImageRegionType::SizeType singlePointSize;
  singlePointSize.Fill(1);
  singlePointRegion.SetSize(singlePointSize);

  // use scanning variables just for a single point region
  // iterate over the single point and initialize the scanning variables
  this->InitializeScanning( singlePointRegion, scanIt, scanMem, scanParameters );

  /* Iterate over the sub region of a single point*/
  scanIt.GoToBegin();

  try
    {
    pointIsValid = false;

    this->UpdateQueues(scanIt, scanMem, scanParameters, threadId);
    pointIsValid = this->ComputeInformationFromQueues(scanIt, scanMem, scanParameters, threadId);
    if( pointIsValid )
      {
      this->ComputeMovingTransformDerivative(scanIt, scanMem, scanParameters, localDerivativeResult, metricValueResult, threadId );
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

  /* Assign the results */
  if ( pointIsValid )
    {
    this->m_NumberOfValidPointsPerThread[threadId]++;
    this->m_MeasurePerThread[threadId] -= metricValueResult;
    /* Store the result. This depends on what type of
     * transform is being used. */
    if( this->GetComputeDerivative() )
      {
      this->StorePointDerivativeResult( scanIt.GetIndex(), threadId );
      }
    }

  return pointIsValid;
}


} // end namespace itk

#endif
