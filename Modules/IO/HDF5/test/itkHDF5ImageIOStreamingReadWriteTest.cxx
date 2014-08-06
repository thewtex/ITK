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
#include "itkHDF5CommandRegionRecorder.h"
#include "itkHDF5ImageIOFactory.h"
#include "itkIOTestHelper.h"
#include "itkPipelineMonitorImageFilter.h"
#include "itkStreamingImageFilter.h"
#include "itkHDF5ImageIO.h"

typedef itk::HDF5CommandRegionRecorder::H5IndexType        H5IndexType;
typedef itk::HDF5CommandRegionRecorder::H5IndexHistoryType H5IndexHistoryType;

using namespace std;

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
  typename ImageType::RegionType imageRegion;
  typename ImageType::SizeType size;
  typename ImageType::IndexType index;
  typename ImageType::SpacingType spacing;
  typename ImageType::PointType origin;
  typename ImageType::DirectionType myDirection;
  for(unsigned i = 0; i < 3; i++)
    {
    size[i] = 5;
    index[i] = 0;
    spacing[i] = 1.0 / (static_cast<double>(i) + 1.0);
    origin[i] = static_cast<double>(i) + 7.0;
    }
  imageRegion.SetSize(size);
  imageRegion.SetIndex(index);
  typename ImageType::Pointer im =
    itk::IOTestHelper::AllocateImageFromRegionAndSpacing<ImageType>(imageRegion,spacing);
  //
  // fill image buffer
  vnl_random randgen(12345678);
  itk::ImageRegionIterator<ImageType> it(im,im->GetLargestPossibleRegion());
  for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    TPixel pix;
    itk::IOTestHelper::RandomPix(randgen,pix);
    it.Set(pix);
    }
  typedef typename itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(fileName);
  writer->SetInput(im);
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
  H5IndexHistoryType ExpectedStartHistory = CreateExpectedStartStreamingHistory( im->GetNumberOfComponentsPerPixel() );
  H5IndexHistoryType ExpectedSizeHistory = CreateExpectedSizeStreamingHistory( im->GetNumberOfComponentsPerPixel() );

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
  itk::ImageRegionIterator<ImageType> it2(im2,im2->GetLargestPossibleRegion());
  for(it.GoToBegin(),it2.GoToBegin(); !it.IsAtEnd() && !it2.IsAtEnd(); ++it,++it2)
    {
    if(it.Value() != it2.Value())
      {
      std::cout << "Original Pixel (" << it.Value()
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
