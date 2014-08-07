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
#include "itkHDF5ImageIOFactory.h"
#include "itkIOTestHelper.h"
#include "itkPipelineMonitorImageFilter.h"
#include "itkStreamingImageFilter.h"
#include "itkHDF5ImageIO.h"
#include "itkGenerateImageSource.h"
#include "itkImageRegionIteratorWithIndex.h"

typedef itk::HDF5CommandRegionRecorder::H5IndexType        H5IndexType;
typedef itk::HDF5CommandRegionRecorder::H5IndexHistoryType H5IndexHistoryType;

using namespace std;

namespace itk
{

/** \class DemoImageSource
 *
 * \brief Streamable process that will generate image regions from the write requests
 *
 * We do not allocate directly the Image, because a Image is a data and is not streamable
 * the writer would write the image in one pass, without streaming.
 *
 * Instead, we need to set a streamable source process as the writer input.
 * This source process, 'DemoImageSource', will allocate a region of the image
 * (and set pixels values) on the fly, based on the informations
 * received from the writer requests.
 */
template< class TOutputImage >
class DemoImageSource:public GenerateImageSource< TOutputImage >
{
  public:
    /** Standard class typedefs. */
    typedef DemoImageSource                 Self;
    typedef DemoImageSource< TOutputImage > Superclass;
    typedef SmartPointer< Self >            Pointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(DemoImageSource, GenerateImageSource);

    /** Set the value to fill the image. */
    itkSetMacro(Value, typename TOutputImage::PixelType);

  protected:
    DemoImageSource()
    {
      m_Value = NumericTraits< typename TOutputImage::PixelType >::Zero;
    }
    ~DemoImageSource(){}

    /** Does the real work. */
    virtual void GenerateData(){
      TOutputImage* out = this->GetOutput();
      out->SetBufferedRegion(out->GetRequestedRegion());
      out->Allocate();
      itk::ImageRegionIteratorWithIndex<TOutputImage> it(out,out->GetRequestedRegion());
      for(it.GoToBegin(); !it.IsAtEnd(); ++it)
        {
        typename TOutputImage::IndexType idx = it.GetIndex();
        it.Set(idx[2]*100 + idx[1]*10 + idx[0]);
        }
    };

  private:
    DemoImageSource(const Self &); //purposely not implemented
    void operator=(const Self &);  //purposely not implemented

    typename TOutputImage::PixelType m_Value;
};

}

/** Returns the nested list: [[0,0,0],[1,0,0],[2,0,0],[3,0,0],[4,0,0]]
 *  Or in case of multiple components:
 *    [[0,0,0,0],[1,0,0,0],[2,0,0,0],[3,0,0,0],[4,0,0,0]] */
H5IndexHistoryType CreateExpectedStartStreamingHistory(unsigned int numberOfComponents)
{
  H5IndexType ExpectedStart;
  ExpectedStart.push_back(0);
  ExpectedStart.push_back(0);
  ExpectedStart.push_back(0);
  if (numberOfComponents!=1) ExpectedStart.push_back(0);

  H5IndexHistoryType ExpectedStartHistory;
  for (unsigned it = 0; it < 5; it++)
    {
    ExpectedStart[0] = it;
    ExpectedStartHistory.push_back( ExpectedStart );
    }

  return ExpectedStartHistory;
}

/** Returns the nested list: [[1,5,5],[1,5,5],[1,5,5],[1,5,5],[1,5,5]]
 *  Or in case of multiple components:
 *    [[1,5,5,5],[1,5,5,5],[1,5,5,5],[1,5,5,5],[1,5,5,5]] */
H5IndexHistoryType CreateExpectedSizeStreamingHistory(unsigned int numberOfComponents)
{
  H5IndexType ExpectedSize;
  ExpectedSize.push_back(1);
  ExpectedSize.push_back(5);
  ExpectedSize.push_back(5);
  if (numberOfComponents!=1) ExpectedSize.push_back(3);

  H5IndexHistoryType ExpectedSizeHistory;
  for (unsigned it = 0; it < 5; it++)
    {
    ExpectedSizeHistory.push_back( ExpectedSize );
    }

  return ExpectedSizeHistory;
}

template <typename TPixel>
int HDF5ReadWriteTest2(const char *fileName)
{
  int success(EXIT_SUCCESS);
  typedef typename itk::Image<TPixel,3> ImageType;

  // Create a source object (in this case a constant image).
  typename ImageType::SizeValueType size[3];
  size[2] = 5;
  size[1] = 5;
  size[0] = 5;
  typename itk::DemoImageSource<ImageType>::Pointer imageSource =
      itk::DemoImageSource<ImageType>::New();
  imageSource->SetValue(static_cast<TPixel>(23)); // Not used.
  imageSource->SetSize(size);

  typedef typename itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(fileName);
  writer->SetInput(imageSource->GetOutput());
  writer->SetNumberOfStreamDivisions(5);

  // Add an observer which record streaming regions written in the file.
  itk::HDF5CommandRegionRecorder::Pointer write_observer = itk::HDF5CommandRegionRecorder::New();
  typedef itk::HDF5ImageIO ImageIOType;
  ImageIOType::Pointer write_hdf5IO = ImageIOType::New();
  write_hdf5IO->AddObserver( itk::IterationEvent(), write_observer);
  writer->SetImageIO(write_hdf5IO);

  try
    {
    writer->Write();
    }
  catch(itk::ExceptionObject &err)
    {
    std::cout << "itkHDF5ImageIOTest" << std::endl
              << "Exception Object caught: " << std::endl
              << err << std::endl;
    return EXIT_FAILURE;
    }
  // force writer close
  writer = typename WriterType::Pointer();

  // Create the expected streaming regions.
  H5IndexHistoryType ExpectedStartHistory = CreateExpectedStartStreamingHistory( imageSource->GetOutput()->GetNumberOfComponentsPerPixel() );
  H5IndexHistoryType ExpectedSizeHistory = CreateExpectedSizeStreamingHistory( imageSource->GetOutput()->GetNumberOfComponentsPerPixel() );

  // Check that streaming regions are as expected.
  if ( ! write_observer->CheckHistory( ExpectedStartHistory, ExpectedSizeHistory, cout ) )
    {
    success = EXIT_FAILURE;
    }

  typedef typename itk::ImageFileReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(fileName);
  reader->SetUseStreaming(true);

  // Add an observer which record streaming regions read in the file.
  itk::HDF5CommandRegionRecorder::Pointer read_observer = itk::HDF5CommandRegionRecorder::New();
  ImageIOType::Pointer read_hdf5IO = ImageIOType::New();
  read_hdf5IO->AddObserver( itk::IterationEvent(), read_observer);
  reader->SetImageIO(read_hdf5IO);

  typedef typename itk::PipelineMonitorImageFilter<ImageType> MonitorFilter;
  typename MonitorFilter::Pointer monitor = MonitorFilter::New();
  monitor->SetInput(reader->GetOutput());

  typedef typename itk::StreamingImageFilter<ImageType, ImageType> StreamingFilter;
  typename StreamingFilter::Pointer streamer = StreamingFilter::New();
  streamer->SetInput(monitor->GetOutput());
  streamer->SetNumberOfStreamDivisions(5);

  typename ImageType::Pointer im2;
  try
    {
    streamer->Update();
    }
  catch(itk::ExceptionObject &err)
    {
    std::cout << "itkHDF5ImageIOTest" << std::endl
              << "Exception Object caught: " << std::endl
              << err << std::endl;
    return EXIT_FAILURE;
    }
  im2 = streamer->GetOutput();
  itk::ImageRegionIteratorWithIndex<ImageType> it2(im2,im2->GetLargestPossibleRegion());
  typename ImageType::IndexType idx;
  TPixel origValue;
  for(it2.GoToBegin(); !it2.IsAtEnd(); ++it2)
    {
    idx = it2.GetIndex();
    origValue = idx[2]*100 + idx[1]*10 + idx[0];
    if(it2.Value() != origValue)
      {
      std::cout << "Original Pixel (" << origValue
                << ") doesn't match read-in Pixel ("
                << it2.Value() << std::endl;
      success = EXIT_FAILURE;
      break;
      }
    }
  itk::IOTestHelper::Remove(fileName);

  // Check that streaming regions are as expected.
  if ( ! read_observer->CheckHistory( ExpectedStartHistory, ExpectedSizeHistory, cout ) )
    {
    success = EXIT_FAILURE;
    }

  return success;
}

int
itkHDF5ImageIOStreamingReadWriteTest(int ac, char * av [])
{
  std::string prefix("");
  if(ac > 1)
    {
    prefix = *++av;
    --ac;
    itksys::SystemTools::ChangeDirectory(prefix.c_str());
    }
  itk::ObjectFactoryBase::RegisterFactory(itk::HDF5ImageIOFactory::New() );

  int result(0);
  result += HDF5ReadWriteTest2<unsigned char>("StreamingUCharImage.hdf5");
  result += HDF5ReadWriteTest2<float>("StreamingFloatImage.hdf5");
  result += HDF5ReadWriteTest2<itk::RGBPixel<unsigned char> >("StreamingRGBImage.hdf5");
  return result != 0;
}
