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
/*
  This tests the classes ImageRandomNonRepeatingIteratorWithIndex and
  ImageRandomNonRepeatingConstIteratorWithIndex.  This was contributed
  by Rupert Brooks, McGill Centre for Intelligent
  Machines, Montreal, Canada.  It is heavily based on the
  ImageRandomIterator test program.
*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include <algorithm>
#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRandomNonRepeatingIteratorWithIndex.h"
#include "itkImageRandomNonRepeatingConstIteratorWithIndex.h"
int itkImageRandomNonRepeatingIteratorWithIndexTest(int, char* [] )
{
  const unsigned int ImageDimension = 3;
  typedef itk::Index< ImageDimension >                                    PixelType;
  typedef itk::Image< PixelType, ImageDimension >                         ImageType;
  typedef itk::ImageRegionIteratorWithIndex< ImageType >                  IteratorType;
  typedef itk::ImageRandomNonRepeatingIteratorWithIndex< ImageType >      RandomIteratorType;
  typedef itk::ImageRandomNonRepeatingConstIteratorWithIndex< ImageType > RandomConstIteratorType;
  typedef RandomConstIteratorType::PriorityImageType                      PriorityImageType;
  typedef PriorityImageType::PixelType                                    PriorityPixelType;
  typedef itk::ImageRegionIteratorWithIndex< PriorityImageType >          PriorityIteratorType;

  std::cout << "Creating images" << std::endl;

  ImageType::Pointer myImage = ImageType::New();
  ImageType::ConstPointer myConstImage = myImage.GetPointer();
  ImageType::SizeType size0;
  size0[0] = 50;
  size0[1] = 50;
  size0[2] = 50;
  unsigned long numberOfSamples = 10;
  ImageType::IndexType start0;
  start0.Fill(0);
  ImageType::RegionType region0;
  region0.SetIndex( start0 );
  region0.SetSize( size0 );
  myImage->SetLargestPossibleRegion( region0 );
  myImage->SetBufferedRegion( region0 );
  myImage->SetRequestedRegion( region0 );
  myImage->Allocate();
  // Make the priority image
  PriorityImageType::Pointer priorityImage = PriorityImageType::New();
  PriorityImageType::SizeType prioritySize;
  prioritySize[0] = 50;
  prioritySize[1] = 50;
  prioritySize[2] = 50;
  PriorityImageType::IndexType priorityStart;
  priorityStart.Fill(0);
  PriorityImageType::RegionType priorityRegion;
  priorityRegion.SetIndex( priorityStart );
  priorityRegion.SetSize( prioritySize );
  priorityImage->SetLargestPossibleRegion( priorityRegion );
  priorityImage->SetBufferedRegion( priorityRegion );
  priorityImage->SetRequestedRegion( priorityRegion );
  priorityImage->Allocate();
  // we will make most of this image ones, with a small region of
  // zeros.  Then pixels from the zero region should be selected
  // preferentially.
  std::cout << "Building Priority image" << std::endl;
  PriorityIteratorType pit( priorityImage, priorityRegion );
  pit.GoToBegin();
  while( !pit.IsAtEnd() )
  {
    pit.Set( 1 );
    ++pit;
  }
  PriorityImageType::IndexType substart;
  substart[0] = 15;
  substart[1] = 16;
  substart[2] = 17;
  PriorityImageType::SizeType subsize;
  subsize[0] = 3;
  subsize[1] = 4;
  subsize[2] = 5;
  PriorityImageType::RegionType subregion;
  subregion.SetIndex( substart );
  subregion.SetSize( subsize );
  PriorityIteratorType subit( priorityImage, subregion );
  subit.GoToBegin();
  while( !subit.IsAtEnd() )
    {
      subit.Set( 0 );
      ++subit;
    }

  //********
  std::cout << "Filling image with indices" << std::endl;

  RandomIteratorType it( myImage, region0 );
  it.SetNumberOfSamples(size0[0]*size0[1]*size0[2]);
  it.GoToBegin();
  ImageType::IndexType index0;
  // Because the random iterator does not repeat, this should
  // fill the image with indices
  while( !it.IsAtEnd() )
  {
    index0 = it.GetIndex();
    it.Set( index0 );
    ++it;
  }

  // Sample the image
  IteratorType ot( myImage, region0 );
  ot.GoToBegin();
  // if it repeated its going to have missed a few.
  std::cout << "Verifying iterators... ";
  while( !ot.IsAtEnd() )
    {
    index0 = ot.GetIndex();
    if( ot.Get() != index0 )
      {
        std::cerr << "Values don't correspond to what was stored "
          << std::endl;
        std::cerr << "Test failed at index ";
        std::cerr << index0 << std::endl;
        return EXIT_FAILURE;
      }
    //std::cout <<".";
    //std::cout << index0 << std::endl;
    ++ot;
    }
  std::cout << std::endl<<"   Done ! " << std::endl;

  // Verification
  RandomConstIteratorType cot( myConstImage, region0 );
  cot.SetNumberOfSamples( numberOfSamples );
  cot.GoToBegin();

  std::cout << "Verifying const iterator... ";
  std::cout << "Random walk of the Iterator over the image " << std::endl;
  while( !cot.IsAtEnd() )
  {
    index0 = cot.GetIndex();
    if( cot.Get() != index0 )
      {
      std::cerr << "Values don't correspond to what was stored "
        << std::endl;
      std::cerr << "Test failed at index ";
      std::cerr << index0 << " value is " << cot.Get() <<  std::endl;
      return EXIT_FAILURE;
      }
    std::cout << index0 << std::endl;
    ++cot;
  }
  std::cout << "   Done ! " << std::endl;

  // Verification
  std::cout << "Verifying iterator in reverse direction... " << std::endl;
  std::cout << "Should be a random walk too (a different one)" << std::endl;
  RandomIteratorType ior( myImage, region0 );
  ior.SetNumberOfSamples( numberOfSamples );
  ior.GoToEnd();
  --ior;

  while( !ior.IsAtBegin() )
  {
    index0 = ior.GetIndex();
    if( ior.Get() != index0 )
    {
      std::cerr << "Values don't correspond to what was stored "
        << std::endl;
      std::cerr << "Test failed at index ";
      std::cerr << index0 << " value is " << ior.Get() <<  std::endl;
      return EXIT_FAILURE;
    }
    std::cout << index0 << std::endl;
    --ior;
  }
  std::cout << index0 << std::endl; // print the value at the beginning index
  std::cout << "   Done ! " << std::endl;

  // Verification
  std::cout << "Verifying const iterator in reverse direction... ";
  RandomConstIteratorType cor( myImage, region0 );
  cor.SetNumberOfSamples( numberOfSamples ); // 0=x, 1=y, 2=z
  cor.GoToEnd();
  --cor; // start at the end position
  while( !cor.IsAtBegin() )
    {
    index0 = cor.GetIndex();
    if( cor.Get() != index0 )
      {
      std::cerr << "Values don't correspond to what was stored "
        << std::endl;
      std::cerr << "Test failed at index ";
      std::cerr << index0 << " value is " << cor.Get() <<  std::endl;
      return EXIT_FAILURE;
      }
    std::cout << index0 << std::endl;
    --cor;
    }
  std::cout << index0 << std::endl; // print the value at the beginning index
  std::cout << "   Done ! " << std::endl;
 // Verification
  std::cout << "Verifying const iterator in both directions... ";
  RandomConstIteratorType dor( myImage, region0 );
  dor.SetNumberOfSamples( numberOfSamples ); // 0=x, 1=y, 2=z
  dor.GoToEnd();
  --dor; // start at the last valid pixel position
  for (unsigned int counter = 0; ! dor.IsAtEnd(); ++counter)
    {
      index0 = dor.GetIndex();
      if( dor.Get() != index0 )
        {
          std::cerr << "Values don't correspond to what was stored "
                    << std::endl;
          std::cerr << "Test failed at index ";
          std::cerr << index0 << " value is " << dor.Get() <<  std::endl;
          return EXIT_FAILURE;
        }
      std::cout << index0 << std::endl;
      if (counter < 6)  { --dor; }
      else { ++dor; }
    }
  std::cout << index0 << std::endl; // print the value at the beginning index
  std::cout << "   Done ! " << std::endl;

  // Verification of the Iterator in a subregion of the image
  {
    std::cout << "Verifying Iterator in a Region smaller than the whole image... "
              << std::endl;
    ImageType::IndexType start;
    start[0] = 10;
    start[1] = 12;
    start[2] = 14;
    ImageType::SizeType size;
    size[0] = 11;
    size[1] = 12;
    size[2] = 13;
    ImageType::RegionType region;
    region.SetIndex( start );
    region.SetSize( size );
    RandomIteratorType cbot( myImage, region );
    cbot.SetNumberOfSamples( numberOfSamples ); // 0=x, 1=y, 2=z
    cbot.GoToBegin();
    while( !cbot.IsAtEnd() )
      {
      ImageType::IndexType index =  cbot.GetIndex();
      ImageType::PixelType pixel =  cbot.Get();
      if( index != pixel )
        {
        std::cerr << "Iterator in region test failed" << std::endl;
        std::cerr << pixel << " should be" << index << std::endl;
        return EXIT_FAILURE;
        }
      if( !region.IsInside( index ) )
        {
        std::cerr << "Iterator in region test failed" << std::endl;
        std::cerr << index << " is outside the region " << region << std::endl;
        return EXIT_FAILURE;
        }
      std::cout << index << std::endl;
      ++cbot;
      }
    std::cout << "   Done ! " << std::endl;
  }

  // Verification of the Const Iterator in a subregion of the image
  {
    std::cout << "Verifying Const Iterator in a Region smaller than the whole image... "
              << std::endl;
    ImageType::IndexType start;
    start[0] = 10;
    start[1] = 12;
    start[2] = 14;
    ImageType::SizeType size;
    size[0] = 11;
    size[1] = 12;
    size[2] = 13;
    ImageType::RegionType region;
    region.SetIndex( start );
    region.SetSize( size );
    RandomConstIteratorType cbot( myImage, region );
    cbot.SetNumberOfSamples( numberOfSamples );
    cbot.GoToBegin();
    while( !cbot.IsAtEnd() )
      {
      ImageType::IndexType index =  cbot.GetIndex();
      ImageType::PixelType pixel =  cbot.Get();
      if( index != pixel )
        {
        std::cerr << "Iterator in region test failed" << std::endl;
        std::cerr << pixel << " should be" << index << std::endl;
        return EXIT_FAILURE;
        }
      if( !region.IsInside( index ) )
        {
        std::cerr << "Iterator in region test failed" << std::endl;
        std::cerr << index << " is outside the region " << region << std::endl;
        return EXIT_FAILURE;
        }
      std::cout << index << std::endl;
      ++cbot;
      }
    std::cout << "   Done ! " << std::endl;
  }


  // Verifying iterator works with  the priority image

  {
    std::cout << "Verifying Iterator with respect to priority image... "
              << std::endl;

    RandomIteratorType cbot( myImage, region0 );
    cbot.SetPriorityImage(priorityImage);
    cbot.SetNumberOfSamples( numberOfSamples ); // 0=x, 1=y, 2=z
    cbot.GoToBegin();
    unsigned int count=0;
    while( !cbot.IsAtEnd() && count<(subsize[0]*subsize[1]*subsize[2]))
      {
      ImageType::IndexType index =  cbot.GetIndex();
      if( !subregion.IsInside( index ) )
        {
        std::cerr << "Iterator in priority region test failed" << std::endl;
        std::cerr << index << " is outside the region " << region0 << std::endl;
        return EXIT_FAILURE;
        }
      std::cout << index << std::endl;
      ++cbot;
      ++count;
      }
    // Now we have walked through all the pixels of low priority, next
    // one should be outside the region.
      if( subregion.IsInside( index0 ) )
        {
        std::cerr << "Iterator in priority region test failed" << std::endl;
        std::cerr << index0 << " is outside the region (should be in)" << region0 << std::endl;
        return EXIT_FAILURE;
        }
    std::cout << "   Done ! " << std::endl;
  }
  {
    std::cout << "Verifying const Iterator with respect to priority image... "
              << std::endl;

    RandomConstIteratorType cbot( myImage, region0 );
    cbot.SetPriorityImage(priorityImage);
    cbot.SetNumberOfSamples( numberOfSamples ); // 0=x, 1=y, 2=z
    cbot.GoToBegin();
    unsigned int count=0;
    while( !cbot.IsAtEnd() && count<(subsize[0]*subsize[1]*subsize[2]))
      {
      ImageType::IndexType index =  cbot.GetIndex();
      if( !subregion.IsInside( index ) )
        {
        std::cerr << "Iterator in priority region test failed" << std::endl;
        std::cerr << index << " is outside the region " << region0 << std::endl;
        return EXIT_FAILURE;
        }
      std::cout << index << std::endl;
      ++cbot;
      ++count;
      }
    // Now we have walked through all the pixels of low priority, next
    // one should be outside the region.
      if( subregion.IsInside( index0 ) )
        {
        std::cerr << "Iterator in priority region test failed" << std::endl;
        std::cerr << index0 << " is outside the region (should be in)" << region0 << std::endl;
        return EXIT_FAILURE;
        }
    std::cout << "   Done ! " << std::endl;
  }
  std::cout << "Test passed" << std::endl;
  return EXIT_SUCCESS;
}
int itkImageRandomNonRepeatingIteratorWithIndexTest2(int, char* [])
{
  const unsigned int ImageDimension = 2;
  typedef itk::Index< ImageDimension >             PixelType;
  typedef itk::Image< PixelType, ImageDimension >  ImageType;
  typedef itk::ImageRandomNonRepeatingConstIteratorWithIndex< ImageType >
    RandomConstIteratorType;
  const unsigned long N = 10;
  const int Seed = 42;
  ImageType::SizeType size;
  size.Fill(N);
  ImageType::IndexType start;
  start.Fill(0);
  ImageType::RegionType region;
  region.SetIndex( start );
  region.SetSize( size );
  ImageType::Pointer myImage = ImageType::New();
  myImage->SetLargestPossibleRegion( region );
  myImage->SetBufferedRegion( region );
  myImage->SetRequestedRegion( region );
  myImage->Allocate();
  typedef std::vector<ImageType::IndexType>            WalkType;
  typedef WalkType::iterator                           WalkIteratorType;
  typedef std::pair<WalkIteratorType,WalkIteratorType> WalkIteratorPairType;
  WalkType firstWalk(N);
  RandomConstIteratorType firstIt(myImage, region);
  firstIt.ReinitializeSeed( Seed );
  firstIt.SetNumberOfSamples( region.GetNumberOfPixels() );
  for (firstIt.GoToBegin(); !firstIt.IsAtEnd(); ++firstIt)
    {
    firstWalk.push_back( firstIt.GetIndex() );
    }
  WalkType secondWalk(N);
  RandomConstIteratorType secondIt(myImage, region);
  secondIt.ReinitializeSeed( Seed );
  secondIt.SetNumberOfSamples( region.GetNumberOfPixels() );
  for (secondIt.GoToBegin(); !secondIt.IsAtEnd(); ++secondIt)
    {
    secondWalk.push_back( secondIt.GetIndex() );
    }
  std::pair<WalkIteratorType,WalkIteratorType> mismatchTest;
  mismatchTest =
    std::mismatch( firstWalk.begin(),
                   firstWalk.end(),secondWalk.begin() );
  if ( mismatchTest.first != firstWalk.end() )
    {
    std::cerr << "Two iterations with the same seed do not"
              << " walk over the same pixels" << std::endl
              << "First mismatch found after "
              << mismatchTest.first - firstWalk.begin()
              << " iterations." << std::endl
              << "First walk index  : " << *(mismatchTest.first) << std::endl
              << "Second walk index : " << *(mismatchTest.second) << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test passed" << std::endl;
  return EXIT_SUCCESS;
}
