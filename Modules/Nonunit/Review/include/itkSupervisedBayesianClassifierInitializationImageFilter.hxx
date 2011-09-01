#ifndef __itkBayesianClassifierInitializationImageFilter_txx
#define __itkBayesianClassifierInitializationImageFilter_txx

#include "itkScalarImageKmeansImageFilter.h"
#include "itkGaussianDensityFunction.h"

namespace itk
{

template <class TInputImage, class TProbabilityPrecisionType>
SupervisedBayesianClassifierInitializationImageFilter<TInputImage, TProbabilityPrecisionType>
::SupervisedBayesianClassifierInitializationImageFilter()
  :  m_NumberOfClasses( 0 )
{
  m_MembershipFunctionContainer = NULL;
}

template <class TInputImage, class TProbabilityPrecisionType>
void
SupervisedBayesianClassifierInitializationImageFilter<TInputImage,
              TProbabilityPrecisionType>
::AddMean(double m)
{
  this->mean.push_back(m);
}
template <class TInputImage, class TProbabilityPrecisionType>
void
SupervisedBayesianClassifierInitializationImageFilter<TInputImage,
              TProbabilityPrecisionType>
::AddCov(double c)
{
  this->cov.push_back(c);
}
// GenerateOutputInformation method. Here we force update on the entire input
// image. It does not make sense having K-Means etc otherwise
template <class TInputImage, class TProbabilityPrecisionType>
void
SupervisedBayesianClassifierInitializationImageFilter<TInputImage,
                                            TProbabilityPrecisionType>
::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // get pointers to the input and output
  typename OutputImageType::Pointer outputPtr = this->GetOutput();
  if ( !outputPtr )
    {
    return;
    }

  // Set the size of the output region
  outputPtr->SetBufferedRegion( this->GetInput()->GetLargestPossibleRegion() );
  outputPtr->SetLargestPossibleRegion( this->GetInput()->GetLargestPossibleRegion() );

  if( m_NumberOfClasses == 0 )
    {
    itkExceptionMacro(
       << "Number of classes unspecified");
    }
  outputPtr->SetVectorLength( m_NumberOfClasses );
}


template <class TInputImage, class TProbabilityPrecisionType>
void
SupervisedBayesianClassifierInitializationImageFilter<TInputImage,
                                            TProbabilityPrecisionType>
::InitializeMembershipFunctions()
{
  // Typedefs for the KMeans filter, Covariance calculator...
  typedef ScalarImageKmeansImageFilter< InputImageType > KMeansFilterType;
  typedef typename KMeansFilterType::OutputImageType     KMeansOutputImageType;
  typedef ImageRegionConstIterator<
                  KMeansOutputImageType >                ConstKMeansIteratorType;
  typedef Array< double >                                CovarianceArrayType;
  typedef Array< double >         MeansArrayType;
  typedef Array< double >                                ClassCountArrayType;

  typedef Statistics::GaussianDensityFunction<
          MeasurementVectorType >                        GaussianMembershipFunctionType;
  typedef VectorContainer< unsigned short, ITK_TYPENAME
    GaussianMembershipFunctionType::MeanType* >          MeanEstimatorsContainerType;
  typedef VectorContainer< unsigned short, ITK_TYPENAME
    GaussianMembershipFunctionType::CovarianceType* >    CovarianceEstimatorsContainerType;


  // Run k means to get the means from the input image
  typename KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();
  kmeansFilter->SetInput( this->GetInput() );
  kmeansFilter->SetUseNonContiguousLabels( false );

  for( unsigned k=0; k < m_NumberOfClasses; k++ )
    {
    const double userProvidedInitialMean = k;
    //TODO: Choose more reasonable defaults for specifying the initial means
    //to the KMeans filter. We could also add this as an option of the filter.
    kmeansFilter->AddClassWithInitialMean( userProvidedInitialMean );
    }


  //typename KMeansFilterType::ParametersType
   MeansArrayType     estimatedMeans(m_NumberOfClasses); //= kmeansFilter->GetFinalMeans(); // mean of each class
  //
  CovarianceArrayType estimatedCovariances( m_NumberOfClasses ); // covariance of each class


  const InputImageType *                inputImage = this->GetInput();
  typename InputImageType::RegionType   imageRegion  = inputImage->GetLargestPossibleRegion();
  InputImageIteratorType                itrInputImage( inputImage, imageRegion );

  for(unsigned int i = 0; i < m_NumberOfClasses; i++)
  {
    estimatedCovariances[i] = this->cov[i];
    estimatedMeans[i] = this->mean[i];
  }
  // Create gaussian membership functions.
  typename MeanEstimatorsContainerType::Pointer meanEstimatorsContainer =
                                       MeanEstimatorsContainerType::New();
  typename CovarianceEstimatorsContainerType::Pointer covarianceEstimatorsContainer =
                                       CovarianceEstimatorsContainerType::New();
  meanEstimatorsContainer->Reserve( m_NumberOfClasses );
  covarianceEstimatorsContainer->Reserve( m_NumberOfClasses );

  m_MembershipFunctionContainer = MembershipFunctionContainerType::New();
  m_MembershipFunctionContainer->Initialize(); // Clear elements

  for ( unsigned int i = 0; i < m_NumberOfClasses; ++i )
    {
    meanEstimatorsContainer->InsertElement( i,
         new typename GaussianMembershipFunctionType::MeanType(1) );
    covarianceEstimatorsContainer->
      InsertElement( i, new typename GaussianMembershipFunctionType::CovarianceType() );
    typename GaussianMembershipFunctionType::MeanType*       meanEstimators =
             const_cast< ITK_TYPENAME GaussianMembershipFunctionType::MeanType * >
                           (meanEstimatorsContainer->GetElement(i));
    typename GaussianMembershipFunctionType::CovarianceType* covarianceEstimators =
              const_cast< ITK_TYPENAME GaussianMembershipFunctionType::CovarianceType * >
              (covarianceEstimatorsContainer->GetElement(i));
    meanEstimators->SetSize(1);
    covarianceEstimators->SetSize( 1, 1 );

    meanEstimators->Fill( estimatedMeans[i] );
    covarianceEstimators->Fill( estimatedCovariances[i] );
    typename GaussianMembershipFunctionType::Pointer gaussianDensityFunction
                                       = GaussianMembershipFunctionType::New();
    gaussianDensityFunction->SetMean( meanEstimatorsContainer->GetElement( i ) );
    gaussianDensityFunction->SetCovariance( covarianceEstimatorsContainer->GetElement( i ) );


    m_MembershipFunctionContainer->InsertElement(i,
            dynamic_cast< MembershipFunctionType * >( gaussianDensityFunction.GetPointer() ) );
    }

}


template <class TInputImage, class TProbabilityPrecisionType>
void
SupervisedBayesianClassifierInitializationImageFilter<TInputImage, TProbabilityPrecisionType>
::GenerateData()
{
  // TODO Check if we need a progress accumulator
  const InputImageType *                inputImage = this->GetInput();
  typename InputImageType::RegionType   imageRegion  = inputImage->GetLargestPossibleRegion();
  InputImageIteratorType                itrInputImage( inputImage, imageRegion );

  this->InitializeMembershipFunctions();

  if( m_MembershipFunctionContainer->Size() != m_NumberOfClasses )
    {
    itkExceptionMacro(
       << "Number of membership functions should be the same as the number of classes");
    }

  this->AllocateOutputs();

  // create vector image of membership probabilities
  OutputImageType *membershipImage = this->GetOutput();

  MembershipImageIteratorType itrMembershipImage( membershipImage, imageRegion );
  MembershipPixelType membershipPixel( m_NumberOfClasses );
  MeasurementVectorType mv;

  itrMembershipImage.GoToBegin();
  itrInputImage.GoToBegin();
  while ( !itrMembershipImage.IsAtEnd() )
    {
    mv[0] = itrInputImage.Get();
    for ( unsigned int i = 0; i < m_NumberOfClasses; i++ )
      {
      membershipPixel[i] = (m_MembershipFunctionContainer->GetElement(i))->Evaluate( mv );
      }
    itrMembershipImage.Set( membershipPixel );
    ++itrInputImage;
    ++itrMembershipImage;
    }

}


template <class TInputImage, class TProbabilityPrecisionType>
void
SupervisedBayesianClassifierInitializationImageFilter<TInputImage, TProbabilityPrecisionType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

} // end namespace itk

#endif
