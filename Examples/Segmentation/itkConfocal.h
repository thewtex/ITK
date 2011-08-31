
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "iostream"
#include "itkMedianImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkIntensityWindowingImageFilter.h"
#include <itkRescaleIntensityImageFilter.h>
#include "itkLabelStatisticsImageFilter.h"
#include "itkNumericTraits.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

typedef   unsigned short InputPixelType;
typedef   unsigned char OutputPixelType;
typedef   float           InternalPixelType;

typedef itk::Image< InputPixelType,  3 >   InputImageType3D;
typedef itk::Image< OutputPixelType, 3 >   OutputImageType3D;
typedef itk::Image< InternalPixelType, 3 >   InternalImageType3D;
// Software Guide : EndCodeSnippet


typedef itk::ImageFileReader< InputImageType3D  >  ReaderType;
typedef itk::ImageFileWriter< OutputImageType3D >  WriterType;
typedef itk::MedianImageFilter<InputImageType3D,
        InternalImageType3D>  MedianFilterType;
typedef itk::IntensityWindowingImageFilter< InternalImageType3D,
        InternalImageType3D >  IntensityFilterType;
typedef itk::RecursiveGaussianImageFilter< InternalImageType3D,
        InternalImageType3D > GaussianFilterType;

typedef itk::ResampleImageFilter< InternalImageType3D,
        InternalImageType3D>  ResampleFilterType;
typedef itk::IdentityTransform< double, 3 >  TransformType;
typedef itk::LinearInterpolateImageFunction<  InternalImageType3D,
        double >  InterpolatorType;
typedef InputImageType3D::SizeType::SizeValueType SizeValueType;
typedef itk::OtsuThresholdImageFilter< InternalImageType3D,
        InternalImageType3D>  OtsuFilterType;
typedef itk::RescaleIntensityImageFilter<InternalImageType3D,
        InternalImageType3D> RescaleType;

typedef unsigned char LabelPixelType;
typedef itk::Image< LabelPixelType,
      3>   LabelImageType;
typedef itk::BinaryThresholdImageFilter< InternalImageType3D,
      InternalImageType3D > ThresholdFilterType;
typedef itk::ConnectedComponentImageFilter< InternalImageType3D,
      LabelImageType > ConnectedComponentType;
typedef itk::RelabelComponentImageFilter< LabelImageType,
      LabelImageType > RelabelComponentType;
typedef itk::LabelStatisticsImageFilter< InternalImageType3D,
      LabelImageType> StatisticsFilterType;

typedef itk::NumericTraits<InternalPixelType>::RealType RealType;
