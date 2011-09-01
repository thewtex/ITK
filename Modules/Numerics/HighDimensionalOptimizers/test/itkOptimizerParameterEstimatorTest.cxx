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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkOptimizerParameterEstimator.h"
#include "itkObjectToObjectMetric.h"

#include "itkImage.h"
#include "itkTransform.h"
#include "itkAffineTransform.h"
#include "itkIdentityTransform.h"
#include "itkTranslationTransform.h"

using namespace itk;

namespace{

/* Create a simple metric to use for testing here. */
template< class TFixedImage,class TMovingImage,class TVirtualImage = TFixedImage >
class ITK_EXPORT ObjectToObjectMetricProxy:
  public itk::ObjectToObjectMetric
{
public:
  /** Standard class typedefs. */
  typedef ObjectToObjectMetricProxy                               Self;
  typedef itk::ObjectToObjectMetric                               Superclass;
  typedef itk::SmartPointer< Self >                               Pointer;
  typedef itk::SmartPointer< const Self >                         ConstPointer;

  typedef typename Superclass::MeasureType          MeasureType;
  typedef typename Superclass::DerivativeType       DerivativeType;
  typedef typename Superclass::ParametersType       ParametersType;
  typedef typename Superclass::ParametersValueType  ParametersValueType;

  itkTypeMacro(ObjectToObjectMetricProxy, ObjectToObjectMetric);

  itkNewMacro(Self);

  // Pure virtual functions that all Metrics must provide
  unsigned int GetNumberOfParameters() const { return 5; }

  MeasureType GetValue()
    {
    return 1.0;
    }

  void GetValueAndDerivative( MeasureType & value, DerivativeType & derivative )
    {
    value = 1.0;
    derivative.Fill(0.0);
    }

  unsigned int GetNumberOfLocalParameters() const
  { return 0; }

  bool HasLocalSupport() const
  { return false; }

  void UpdateTransformParameters( DerivativeType &, ParametersValueType ) {}

  const ParametersType & GetParameters() const
  { return m_Parameters; }

  void Initialize(void) throw ( itk::ExceptionObject ) {}

  void PrintSelf(std::ostream& os, itk::Indent indent) const
  { Superclass::PrintSelf( os, indent ); }

  ParametersType  m_Parameters;

  // Image related types
  typedef TFixedImage                             FixedImageType;
  typedef TMovingImage                            MovingImageType;
  typedef TVirtualImage                           VirtualImageType;

  typedef typename FixedImageType::ConstPointer   FixedImageConstPointer;
  typedef typename MovingImageType::ConstPointer  MovingImageConstPointer;
  typedef typename VirtualImageType::Pointer      VirtualImagePointer;

  /* Set/get images */
  /** Connect the Fixed Image.  */
  itkSetConstObjectMacro(FixedImage, FixedImageType);
  /** Get the Fixed Image. */
  itkGetConstObjectMacro(FixedImage, FixedImageType);
  /** Connect the Moving Image.  */
  itkSetConstObjectMacro(MovingImage, MovingImageType);
  /** Get the Moving Image. */
  itkGetConstObjectMacro(MovingImage, MovingImageType);
  /** Set all virtual domain image */
  itkSetObjectMacro(VirtualDomainImage, VirtualImageType);
  /** Get the virtual domain image */
  itkGetObjectMacro(VirtualDomainImage, VirtualImageType);

  /* Image dimension accessors */
  itkStaticConstMacro(FixedImageDimension, unsigned int,
      ::itk::GetImageDimension<FixedImageType>::ImageDimension);
  itkStaticConstMacro(MovingImageDimension, unsigned int,
      ::itk::GetImageDimension<MovingImageType>::ImageDimension);
  itkStaticConstMacro(VirtualImageDimension, unsigned int,
      ::itk::GetImageDimension<VirtualImageType>::ImageDimension);

  /**  Type of the Transform Base classes */
  typedef Transform<CoordinateRepresentationType,
    itkGetStaticConstMacro( MovingImageDimension ),
    itkGetStaticConstMacro( VirtualImageDimension )>  MovingTransformType;

  typedef Transform<CoordinateRepresentationType,
    itkGetStaticConstMacro( FixedImageDimension ),
    itkGetStaticConstMacro( VirtualImageDimension )>  FixedTransformType;

  typedef typename FixedTransformType::Pointer        FixedTransformPointer;
  typedef typename MovingTransformType::Pointer       MovingTransformPointer;

  /** Connect the fixed transform. */
  itkSetObjectMacro(FixedTransform, FixedTransformType);
  /** Get a pointer to the fixed transform.  */
  itkGetConstObjectMacro(FixedTransform, FixedTransformType);
  /** Connect the moving transform. */
  itkSetObjectMacro(MovingTransform, MovingTransformType);
  /** Get a pointer to the moving transform.  */
  itkGetConstObjectMacro(MovingTransform, MovingTransformType);

private:

  /** Provide these methods to satisfy pure virtuals within
   * SingleValuedCostFunction. This is a sign that we probalby shouldn't
   * be deriving this class from SingleValuedCostFunction. */
  MeasureType GetValue( const ParametersType& ) const { return 0; }

  void GetValueAndDerivative (const ParametersType &,
                              MeasureType &,
                              DerivativeType &) const {}

  FixedImageConstPointer  m_FixedImage;
  MovingImageConstPointer m_MovingImage;
  VirtualImagePointer     m_VirtualDomainImage;

  FixedTransformPointer   m_FixedTransform;
  MovingTransformPointer  m_MovingTransform;

  ObjectToObjectMetricProxy() {}
  ~ObjectToObjectMetricProxy() {}
};

/* global defines */
const int ImageDimension = 2;
typedef Image<double, ImageDimension>                    ImageType;
typedef ImageType::Pointer                               ImagePointerType;

}//namespace

/**
 */
int itkOptimizerParameterEstimatorTest(int , char* [])
{

  // Image begins
  const unsigned int  Dimension = 2;
  typedef double      PixelType;

  // Image Types
  typedef itk::Image<PixelType,Dimension>           FixedImageType;
  typedef itk::Image<PixelType,Dimension>           MovingImageType;
  typedef itk::Image<PixelType,Dimension>           VirtualImageType;

  FixedImageType::Pointer  fixedImage  = FixedImageType::New();
  MovingImageType::Pointer movingImage = MovingImageType::New();
  VirtualImageType::Pointer virtualImage = fixedImage;

  MovingImageType::SizeType    size;
  size.Fill(100);

  movingImage->SetRegions( size );
  fixedImage->SetRegions( size );
  // Image done

  // Transform begins
  typedef AffineTransform<double, Dimension>      MovingTransformType;
  MovingTransformType::Pointer movingTransform =  MovingTransformType::New();
  movingTransform->SetIdentity();

  typedef TranslationTransform<double, Dimension> FixedTransformType;
  FixedTransformType::Pointer fixedTransform =    FixedTransformType::New();
  fixedTransform->SetIdentity();
  // Transform done

  // Metric begins
  typedef ObjectToObjectMetricProxy<ImageType,ImageType>   MetricType;
  MetricType::Pointer metric = MetricType::New();

  metric->SetVirtualDomainImage( virtualImage );
  metric->SetFixedImage( fixedImage );
  metric->SetMovingImage( movingImage );

  metric->SetFixedTransform( fixedTransform );
  metric->SetMovingTransform( movingTransform );
  // Metric done

  // Testing OptimizerParameterEstimator

  typedef itk::OptimizerParameterEstimator< MetricType > OptimizerParameterEstimatorType;
  OptimizerParameterEstimatorType::Pointer parameterEstimator = OptimizerParameterEstimatorType::New();

  parameterEstimator->SetMetric(metric);
  //parameterEstimator->Print( std::cout );

  // Scales for the moving transform
  parameterEstimator->SetTransformForward(true); //by default
  parameterEstimator->SetScaleStrategy(OptimizerParameterEstimatorType::ScalesFromShift); //by default
  //parameterEstimator->SetScaleStrategy(OptimizerParameterEstimatorType::ScalesFromJacobian);
  OptimizerParameterEstimatorType::ScalesType movingScales(metric->GetMovingTransform()->GetNumberOfParameters());

  parameterEstimator->EstimateScales(movingScales);
  std::cout << "Scales for moving transform parameters = " << movingScales << std::endl;

  // Check the correctness
  OptimizerParameterEstimatorType::ScalesType theoreticalMovingScales(metric->GetMovingTransform()->GetNumberOfParameters());
  VirtualImageType::PointType upperPoint;
  virtualImage->TransformIndexToPhysicalPoint(virtualImage->GetLargestPossibleRegion().GetUpperIndex(), upperPoint);

  unsigned int param = 0;
  for (unsigned int row = 0; row < Dimension; row++)
    {
    for (unsigned int col = 0; col < Dimension; col++)
      {
      theoreticalMovingScales[param++] = upperPoint[col] * upperPoint[col];
      }
    }
  for (unsigned int row = 0; row < Dimension; row++)
    {
    theoreticalMovingScales[param++] = 1;
    }

  bool movingPass = true;
  for (unsigned int p = 0; p < theoreticalMovingScales.GetSize(); p++)
    {
    if (movingScales[p] != theoreticalMovingScales[p])
      {
      movingPass = false;
      break;
      }
    }
  bool nonUniformForAffine = false;
  for (unsigned int p = 1; p < movingScales.GetSize(); p++)
    {
    if (movingScales[p] != movingScales[0])
      {
      nonUniformForAffine = true;
      break;
      }
    }
  // Check done

  // Scales for the fixed transform
  parameterEstimator->SetTransformForward(false);
  OptimizerParameterEstimatorType::ScalesType fixedScales(metric->GetFixedTransform()->GetNumberOfParameters());

  parameterEstimator->EstimateScales(fixedScales);
  std::cout << "Scales for fixed transform parameters = " << fixedScales << std::endl;

  // Check the correctness
  OptimizerParameterEstimatorType::ScalesType theoreticalFixedScales(metric->GetFixedTransform()->GetNumberOfParameters());
  theoreticalFixedScales.Fill(1.0);

  bool fixedPass = true;
  for (unsigned int p = 0; p < theoreticalFixedScales.GetSize(); p++)
    {
    if (fixedScales[p] != theoreticalFixedScales[p])
      {
      fixedPass = false;
      break;
      }
    }
  bool uniformForTranslation = true;
  for (unsigned int p = 1; p < fixedScales.GetSize(); p++)
    {
    if (fixedScales[p] != fixedScales[0])
      {
      uniformForTranslation = false;
      break;
      }
    }
  // Check done

  // Testing OptimizerParameterEstimator done

  if (movingPass && fixedPass && nonUniformForAffine && uniformForTranslation)
    {
    std::cout << std::endl << "Test passed" << std::endl;
    return EXIT_SUCCESS;
    }
  else
    {
    std::cout << std::endl << "Test failed" << std::endl;
    return EXIT_FAILURE;
    }
}
