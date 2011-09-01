#define ROUND(X)(int)(X+0.5)
const double PI = 2*acos(0.0);

namespace itk
{
template <class TInputImage, class TProbabilityPrecisionType>
TPointCorrelationFunctionSegmentationInitialization<TInputImage, TProbabilityPrecisionType>
::TPointCorrelationFunctionSegmentationInitialization()
{
  /**Initialize the sample coordinate vector*/
  x = new std::vector<int>;
  y = new std::vector<int>;
  /**calculate the sample coordinates*/
  this->GetSampleCoordinates();
}

template <class TInputImage, class TProbabilityPrecisionType>
void
TPointCorrelationFunctionSegmentationInitialization<TInputImage, TProbabilityPrecisionType>
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
  typename MembershipImageType::RegionType outputRegion;
  typename MembershipImageType::RegionType::IndexType outputstart;
  typename MembershipImageType::RegionType::SizeType size;

  outputstart[0] = 0;
  outputstart[1] = 0;
  size[0] = this->GetInput()->GetLargestPossibleRegion().GetSize()[0] - m_fwin + 1;
  size[1] = this->GetInput()->GetLargestPossibleRegion().GetSize()[1] - m_fwin + 1;

  outputRegion.SetSize(size);
  outputRegion.SetIndex(outputstart);

  outputPtr->SetLargestPossibleRegion(outputRegion);

  //outputPtr->SetLargestPossibleRegion( this->GetInput()->GetLargestPossibleRegion() );

  if( m_NumberOfClasses == 0 )
  {
    itkExceptionMacro(
      << "Number of classes unspecified");
  }
  outputPtr->SetVectorLength( m_NumberOfClasses*m_fwin/2 );

}

/** count the number of label types in the inputImage
 * */
template <class TInputImage, class TProbabilityPrecisionType>
void
TPointCorrelationFunctionSegmentationInitialization<TInputImage, TProbabilityPrecisionType>
::GetNumOfClass()
{
  const InputImageType *                inputImage = this->GetInput();
  typename InputImageType::RegionType   imageRegion  = inputImage->GetLargestPossibleRegion();
  InputImageIteratorType                itrInputImage( inputImage, imageRegion );
  bool flag = false;

  this->inputImage_x = inputImage->GetLargestPossibleRegion().GetSize()[0];
  this->inputImage_y = inputImage->GetLargestPossibleRegion().GetSize()[1];

  itrInputImage.GoToBegin();
  while( !itrInputImage.IsAtEnd() )
  {
    for(unsigned int k = 0; k < lable_type.size(); k++ )
    {
      if (itrInputImage.Get() == lable_type[k])
      {
        flag = true;
        break;
      }
    }
    if (flag == false)
    {
      lable_type.push_back(itrInputImage.Get());
    }
    flag = false;

    ++itrInputImage;
  }

  this->TempDemension = lable_type.size();
}
/**generate a 3D binary image for collecting the 2PCF feature vector
 * */
template <class TInputImage, class TProbabilityPrecisionType>
void
TPointCorrelationFunctionSegmentationInitialization<TInputImage,TProbabilityPrecisionType>
::GenerateMask()
{
  const InputImageType *                inputImage = this->GetInput();
  MaskImage = MaskImageType::New();
  MaskImageType::SizeType Masksize;
  Masksize[0] = inputImage_x;
  Masksize[1] = inputImage_y;
  Masksize[2] = this->TempDemension;

  MaskImageType::IndexType Maskstart;
  Maskstart[0] = 0;
  Maskstart[1] = 0;
  Maskstart[2] = 0;
  MaskImageType::RegionType region;
  region.SetSize(Masksize);
  region.SetIndex(Maskstart);

  MaskImage->SetRegions(region);
  MaskImage->Allocate();

  MaskImageType::IndexType p2;
  typename InputImageType::IndexType p1;
        for(unsigned int k = 0; k <this->TempDemension; k++)
        {
                 for(unsigned int i = 0; i < inputImage_x ; i++)
                 {
                        for(unsigned int j = 0 ; j < inputImage_y; j++ )
                        {
                                 p1[0] = (long int)i;
                                 p1[1] = (long int)j;
                                 InputPixelType m1 = inputImage->GetPixel(p1);

                                 p2[0] = (long int)i;
                                 p2[1] = (long int)j;
                                 p2[2] = (long int)k;
                                 if(m1 == lable_type[k] )
                                 {
                                         MaskImage->SetPixel(p2, 0);
                                         //if the point belong to the desired point set 0
                                 }
                                else
                                {
                                         MaskImage->SetPixel(p2,255);
                                }

                        }
                 }
        }
}


//Function GetSampleCoordinates
//According to the fwin and rays calculate the sample coordinates
template <class TInputImage, class TProbabilityPrecisionType>
void
TPointCorrelationFunctionSegmentationInitialization<TInputImage, TProbabilityPrecisionType>
::GetSampleCoordinates()
{
  unsigned int length = m_fwin/2;
  for (unsigned int r = 0; r< m_rays; r++)
  {
    x->push_back(length);
    x->push_back(length);
    y->push_back(length);
    y->push_back(length);
  }
  for (unsigned int l = 1; l <= length ; l++)
  {
    for(unsigned int r = 0; r < m_rays; r++)
    {
      double theta = -r*PI/m_rays;
      x->push_back(ROUND(cos(theta)*l + length));
      x->push_back(ROUND(-cos(theta)*l + length));
      y->push_back(ROUND(sin(theta)*l + length));
      y->push_back(ROUND(-sin(theta)*l +length));
    }
  }
}

/**Return the feature vector
 * */
template <class TInputImage, class TProbabilityPrecisionType>
double
TPointCorrelationFunctionSegmentationInitialization<TInputImage, TProbabilityPrecisionType>
::Get_feature(int _x, int _y, int label_num, int l)
{
  MaskImageType::IndexType p1,p2;
  MaskImageType::PixelType m1,m2;
  int eval_feature = 0;
  double rv;

  for(unsigned int i = 0; i < m_rays; i++ )
   {
    p1[0] = (*x)[l*m_rays*2+i] + _x;
    p1[1] = (*y)[l*m_rays*2+i] + _y;
    p1[2] = label_num;

    p2[0] = (*x)[l*m_rays*2 + i + 1] + _x;
    p2[1] = (*y)[l*m_rays*2 + i + 1] + _y;
    p2[2] = label_num;

    m1 = MaskImage->GetPixel(p1);
    m2 = MaskImage->GetPixel(p2);

    if (m1 == m2 && m1 == 0){
      eval_feature += 1;
    }
  }
  rv =(double)eval_feature/(double)m_rays ;
  return rv;
}


template <class TInputImage, class TProbabilityPrecisionType>
void
TPointCorrelationFunctionSegmentationInitialization<TInputImage, TProbabilityPrecisionType>
::GenerateData()
{

  const InputImageType *                inputImage = this->GetInput();
  typename InputImageType::RegionType   imageRegion  = inputImage->GetLargestPossibleRegion();
  InputImageIteratorType                itrInputImage( inputImage, imageRegion );

  this->GetNumOfClass();
  if(this->TempDemension != m_NumberOfClasses)
  {
    std::cerr<< "The number of classes unmatched with input image"<<std::endl;
  }
  this->GenerateMask();
  this->AllocateOutputs();

  OutputImageType *membershipImage = this->GetOutput();
  MembershipImageIteratorType itrMembershipImage( membershipImage, this->GetOutput()->GetLargestPossibleRegion());
  unsigned int vectorlength = m_NumberOfClasses*m_fwin/2;
  MembershipPixelType membershipPixel( vectorlength);
  MeasurementVectorType mv;

  itrMembershipImage.GoToBegin();
  itrInputImage.GoToBegin();

  this->GetSampleCoordinates();

  while ( !itrMembershipImage.IsAtEnd() )
  {
    for (unsigned int j = 0; j < m_NumberOfClasses; j++)
    {
      for ( unsigned int i = 0; i < m_fwin/2 ; i++ )
      {
             membershipPixel[i] = this->Get_feature(itrMembershipImage.GetIndex()[0], itrMembershipImage.GetIndex()[1],j,i );
      }
    }
       itrMembershipImage.Set( membershipPixel );
       ++itrMembershipImage;
      }
}
}
