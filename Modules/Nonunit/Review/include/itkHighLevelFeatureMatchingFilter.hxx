#ifndef __itkHighLevelFeatureMatchingFilter_hxx
#define __itkHighLevelFeatureMatchingFilter_hxx

namespace itk
{
template<class TImage,class TProbabilityPrecisionType>
HighLevelFeatureMatchingFilter<TImage, TProbabilityPrecisionType>
::HighLevelFeatureMatchingFilter()
{

}

template<class TImage, class TProbabilityPrecisionType>
void
HighLevelFeatureMatchingFilter<TImage,TProbabilityPrecisionType>
::SetInputImage1(const TImage* image)
{
SetNthInput(0, const_cast<TImage*>(image));
}

template<class TImage,class TProbabilityPrecisionType>
void
HighLevelFeatureMatchingFilter<TImage,TProbabilityPrecisionType>
::SetInputImage2(const TImage* image)
{
SetNthInput(1, const_cast<TImage*>(image));
}

template<class TImage, class TProbabilityPrecisionType>
void
HighLevelFeatureMatchingFilter<TImage, TProbabilityPrecisionType>
::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();
  typename OutputImageType::Pointer outputPtr = this->GetOutput();
  if ( !outputPtr )
  {
    return;
  }

  // Set the size of the output region
  typename OutputImageType::IndexType start;
  typename OutputImageType::SizeType size;
  size[0]  = 2000;
  start[0] = 0;
  typename OutputImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);

  outputPtr->SetBufferedRegion(region);
  outputPtr->SetLargestPossibleRegion(region);
  outputPtr->SetVectorLength(7);
}
/*

*/
template<class TImage,class TProbabilityPrecisionType>
void
HighLevelFeatureMatchingFilter<TImage, TProbabilityPrecisionType>
::GenerateData()
{
  BTLFilterPointer BTLfilter = BinaryImageToLabelMapFilterType::New();
  BTLfilter->SetInput(this->GetInput(0));
  BTLfilter->Update();
  std::cerr << " THERE ARE" << BTLfilter->GetOutput()->GetNumberOfLabelObjects() << " objects in base image" << std::endl;

  L2IPointer l2i = L2IType::New();
  l2i->SetInput(BTLfilter->GetOutput());
  l2i->Update();

  BTLFilterPointer BTLfilter_F = BinaryImageToLabelMapFilterType::New();
  BTLfilter_F->SetInput(this->GetInput(1));
  BTLfilter_F->Update();
  std::cerr << "THERE ARE" << BTLfilter_F->GetOutput()->GetNumberOfLabelObjects() << " objects in Float image " << std::endl;
  L2IPointer l2i_F = L2IType::New();
  l2i_F->SetInput(BTLfilter_F->GetOutput());
  l2i_F->Update();


  int NumberOfLabelBase=0;
  GeoFilter=LabelGeometryImageFilterType::New();
  GeoFilter->SetInput(l2i->GetOutput());
  GeoFilter->CalculatePixelIndicesOn();
  GeoFilter->CalculateOrientedBoundingBoxOn();
  GeoFilter->CalculateOrientedLabelRegionsOn();
  GeoFilter->CalculateOrientedIntensityRegionsOn();
  GeoFilter->Update();

  allLabels = GeoFilter->GetLabels();
  NumberOfLabelBase = GeoFilter->GetNumberOfLabels();

  signed long i=0;
  std::cout<<"Number of labels: in base image "<< GeoFilter->GetNumberOfLabels() << std::endl;


  //iterator
  //typename OutputImageType::RegionType region;
  //region =  outputimage->GetLargestPossibleRegion();
  //MembershipImageIteratorType itrMembershipImage(outputimage,region);
  //OutputPixelType membershipPixel(6);

  //Temp Vector image
  const unsigned int ObjectFeatureDimension=6;
  typedef itk::Vector<float,ObjectFeatureDimension>
      PixelVectorImageType;
  typedef itk::Image<PixelVectorImageType,1>VectorImageType;
  VectorImageType::Pointer LabelFeatureBase=VectorImageType::New();
  VectorImageType::PixelType pixelValue;
  VectorImageType::IndexType pixelIndexBase;
  VectorImageType::IndexType start;
  VectorImageType::SizeType  size;
  size[0]  = 2000;  // size along X
  start[0] = 0;
  VectorImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex( start );
  LabelFeatureBase->SetRegions( region );
  LabelFeatureBase->Allocate();
  VectorImageType::PixelType  initialValue;
  initialValue.Fill( 0.0 );
  LabelFeatureBase->FillBuffer( initialValue);


  typename LabelGeometryImageFilterType::LabelsType allLabels = GeoFilter->GetLabels();
  typename LabelGeometryImageFilterType::LabelsType::iterator allLabelsIt;
  NumberOfLabelBase = GeoFilter->GetNumberOfLabels();
  std::cout << "Number of labels: in base image " << GeoFilter->GetNumberOfLabels() << std::endl;
  for( allLabelsIt = allLabels.begin()+1; allLabelsIt != allLabels.end(); allLabelsIt++ )
  {
    typename LabelGeometryImageFilterType::LabelPixelType labelValue = *allLabelsIt;
    pixelValue[0]=double(labelValue);
    pixelValue[1]=GeoFilter->GetVolume(labelValue);
    pixelValue[2]=GeoFilter->GetEccentricity(labelValue);
    labelPoint=GeoFilter->GetCentroid(labelValue);
    pixelValue[3]=labelPoint[0];
    pixelValue[4]=labelPoint[1];
    pixelValue[5]=GeoFilter->GetOrientation(labelValue);
    pixelIndexBase[0]=i;
    i++;
    LabelFeatureBase->SetPixel(pixelIndexBase,pixelValue);
    pixelValue=LabelFeatureBase->GetPixel(pixelIndexBase);
    LabelFeatureBase->Update();
  }

  VectorImageType::Pointer LabelFeatureFloat=VectorImageType::New();
  VectorImageType::IndexType pixelIndexFloat;
  LabelFeatureFloat->SetRegions( region );
  LabelFeatureFloat->Allocate();
  initialValue.Fill( 0.0 );
  LabelFeatureFloat->FillBuffer( initialValue );
  int NumberOfLabelFloat=0;
  GeoFilterFloat=LabelGeometryImageFilterType::New();
  GeoFilterFloat->SetInput(l2i_F->GetOutput());

  GeoFilterFloat->CalculatePixelIndicesOn();
  GeoFilterFloat->CalculateOrientedBoundingBoxOn();
  GeoFilterFloat->CalculateOrientedLabelRegionsOn();
  GeoFilterFloat->CalculateOrientedIntensityRegionsOn();

  GeoFilterFloat->Update();
  typename LabelGeometryImageFilterType::LabelsType allLabelsFloat = GeoFilterFloat->GetLabels();
  typename LabelGeometryImageFilterType::LabelsType::iterator allLabelsItFloat;

  std::cout << "Number of labels in Float image: " << GeoFilterFloat->GetNumberOfLabels() << std::endl;
  i=0;
  for( allLabelsItFloat = allLabelsFloat.begin()+1; allLabelsItFloat != allLabelsFloat.end(); allLabelsItFloat++ )
  {
    typename LabelGeometryImageFilterType::LabelPixelType labelValueFloat = *allLabelsItFloat;
    pixelValue[0]=double(labelValueFloat);
    pixelValue[1]=GeoFilter->GetVolume(labelValueFloat);
    pixelValue[2]=GeoFilter->GetEccentricity(labelValueFloat);
    labelPoint=GeoFilter->GetCentroid(labelValueFloat);
    pixelValue[3]=labelPoint[0];
    pixelValue[4]=labelPoint[1];
    pixelValue[5]=GeoFilter->GetOrientation(labelValueFloat);
    pixelIndexFloat[0]=i;
    i++;
    LabelFeatureFloat->SetPixel(pixelIndexFloat,pixelValue);
    pixelValue=LabelFeatureFloat->GetPixel(pixelIndexFloat);
    LabelFeatureFloat->Update();
  }
  double SubSize=0;
  double SubEccen=0;
  double SubDist=0;
  double MinDistance=99999;
  double CentroidDistanceik=999999;
  double CentroidDistancejl=999999;
  VectorImageType::PixelType pixelValueBase;
  VectorImageType::PixelType pixelValueFloat;
  VectorImageType::PixelType base;

  const unsigned int PairFeatureDimension=7;
  typedef itk::Vector<float,PairFeatureDimension> PairVectorType;
  typedef itk::Image<PairVectorType,1> PairVectorImageType;
  PairVectorImageType::Pointer MatchingPairs=PairVectorImageType::New();
  PairVectorImageType::PixelType PairValue;
  PairVectorImageType::IndexType PairIndex;
  PairVectorImageType::IndexType startPair;
  PairVectorImageType::SizeType  sizePair;
  sizePair[0]  = 2000;  // size along X
  startPair[0] = 0;
  PairVectorImageType::RegionType regionPair;
  regionPair.SetSize(sizePair);
  regionPair.SetIndex( startPair );
  MatchingPairs->SetRegions( regionPair );
  MatchingPairs->Allocate();
  PairVectorImageType::PixelType  initialValuePair;
  // A vector can initialize all its components to the
  // same value by using the Fill() method.
  initialValuePair.Fill(0 );
  // Now the image buffer can be initialized with this
  // vector value.
  MatchingPairs->FillBuffer( initialValuePair );
  PairIndex[0]=0;
  //unsigned int NumberOfPairs= NumberOfLabelBase*NumberOfLabelFloat;
  unsigned int NumberOfMatchingPairs=0;
  PairVectorImageType::PixelType PairValue1;
for(signed long i=0;i<NumberOfLabelBase-1;i++)
{
pixelIndexBase[0]=i;
pixelValueBase=LabelFeatureBase->GetPixel(pixelIndexBase);
for(signed long j=0;j<NumberOfLabelFloat-1;j++)
{
pixelIndexFloat[0]=j;
pixelValueFloat=LabelFeatureFloat->GetPixel(pixelIndexFloat);
base[1]=std::min(pixelValueBase[1],pixelValueFloat[1]);
base[2]=std::min(pixelValueBase[2],pixelValueFloat[2]);
SubSize=std::abs(pixelValueBase[1]-pixelValueFloat[1]);
SubEccen=std::abs(pixelValueBase[2]-pixelValueFloat[2]);
if (SubSize<=(m_SizeTol*base[1])&&SubEccen<=(m_EccenTol*base[2]))
{
NumberOfMatchingPairs++;
PairValue[0]=int(i);
PairValue[1]=int(j);
MatchingPairs->SetPixel(PairIndex,PairValue);
PairIndex[0]++;
}
  MatchingPairs->Update();
  float tmp1=0.0;
  float tmp2=0.0;
  double Theta=0.0;

  this->AllocateOutputs();
  OutputImageType *outputimage = this->GetOutput();
  typename OutputImageType::RegionType region;
  region =  outputimage->GetLargestPossibleRegion();
  MembershipImageIteratorType itrMembershipImage(outputimage,region);
  OutputPixelType FinalPixel(7);

  VectorImageType::PixelType pixelValueBasek;

  VectorImageType::PixelType pixelValueFloatl;
  typename OutputImageType::IndexType FinalIndex;

  double r11, r12, r21, r22;
  for (unsigned long index1=0;index1<NumberOfMatchingPairs;index1++)
{
    //For i and j
    PairIndex[0]=index1;
    PairValue=MatchingPairs->GetPixel(PairIndex);
    pixelIndexBase[0]=PairValue[0];
    pixelIndexFloat[0]=PairValue[1];
    pixelValueBase=LabelFeatureBase->GetPixel(pixelIndexBase);
    pixelValueFloat=LabelFeatureFloat->GetPixel(pixelIndexFloat);
    for(unsigned long index2=0;index2<NumberOfMatchingPairs;index2++)
    {
      PairIndex[0]=index2;
      PairValue1=MatchingPairs->GetPixel(PairIndex);
      pixelIndexBase[0]=PairValue1[0];
      pixelIndexFloat[0]=PairValue1[1];
      pixelValueBasek=LabelFeatureBase->GetPixel(pixelIndexBase);
      pixelValueFloatl=LabelFeatureFloat->GetPixel(pixelIndexFloat);
      if ((PairValue1[0]!=PairValue[0])&&(PairValue1[1]!=PairValue[1]))
      {
  //      cout<<PairValue<<"---"<<PairValue1<<endl;
        tmp1=(pixelValueBase[3]-pixelValueBasek[3])*(pixelValueBase[3]-pixelValueBasek[4]);
                          tmp2=(pixelValueBase[4]-pixelValueBasek[4])*(pixelValueBase[4]-pixelValueBasek[4]);
                          CentroidDistanceik=tmp1-tmp2;
                          CentroidDistanceik=std::sqrt(std::abs(CentroidDistanceik));
                           tmp1=(pixelValueFloat[3]-pixelValueFloatl[3])*(pixelValueFloat[3]-pixelValueFloatl[3]);
                           tmp2=(pixelValueFloat[4]-pixelValueFloatl[4])*(pixelValueFloat[4]-pixelValueFloatl[4]);
                          CentroidDistancejl=tmp1-tmp2;
                           CentroidDistancejl=std::sqrt(std::abs(CentroidDistancejl));
                           SubDist=std::abs(CentroidDistanceik-CentroidDistancejl);
                          MinDistance=std::min(CentroidDistanceik,CentroidDistancejl);
                           if(SubDist<=MinDistance*m_DistTol)
                           {
                                  //Compute model Transformation
                                  Theta=std::atan((pixelValueFloat[4]-pixelValueFloatl[4])/(pixelValueFloat[3]-pixelValueFloatl[3]))-std::atan((pixelValueBase[4]-pixelValueBasek[4])/(pixelValueBase[3]-pixelValueBasek[3]));
                                  //cout<<"Thetaaaaaaaaaaaaaaaaa"<<Theta<<endl;
                                    if ((std::abs(pixelValueBase[5]-pixelValueFloat[5]-Theta)<=m_OrientTol)&&(std::abs(pixelValueBasek[5]-pixelValueFloatl[5]-Theta)<=m_OrientTol))
                                    {
            //Include (i,j) and (k,l)
            FinalIndex[0]=i;
            FinalPixel[0]=PairValue[0];
            FinalPixel[1]=PairValue[1];
            FinalPixel[2]=Theta;
            r11 = cos(Theta); r12 = -sin(Theta); r21 = -r12; r22 = r11;
            double Tx1 = pixelValueFloat[3] - r11*pixelValueBase[3] - r12*pixelValueBase[4];
            double Ty1 = pixelValueFloat[4] - r21*pixelValueBase[3] - r22*pixelValueBase[4];
            double Tx2 = pixelValueFloatl[3] - r11*pixelValueBasek[3] - r12*pixelValueBasek[4];
            double Ty2 = pixelValueFloatl[4] - r21*pixelValueBasek[3] - r22*pixelValueBasek[4];
            FinalPixel[3]=PairValue1[0];
            FinalPixel[4]=PairValue1[1];
            FinalPixel[5]=(Tx1+Tx2)/2;
            FinalPixel[6]=(Ty1+Ty2)/2;
            outputimage->SetPixel(FinalIndex,FinalPixel);
                                                i++;
            outputimage->Update();
                                  }
                            }

      }
    }
  }
}
}
}
}
#endif
