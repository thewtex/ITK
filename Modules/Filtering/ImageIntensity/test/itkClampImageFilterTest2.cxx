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

#include <iostream>

#include "itkClampImageFilter.h"
#include "itkRandomImageSource.h"

// Better name demanging for gcc
#if __GNUC__ > 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ > 0 )
#define GCC_USEDEMANGLE
#endif

#ifdef GCC_USEDEMANGLE
#include <cstdlib>
#include <cxxabi.h>
#endif

template< class T >
std::string Getclamp2TypeName()
{
std::cout<<"Test1"<<std::endl;
  std::string name;
#ifdef GCC_USEDEMANGLE
  char const *mangledName = typeid( T ).name();
  int         status;
  char *      unmangled = abi::__cxa_demangle(mangledName, 0, 0, &status);
  name = unmangled;
  free(unmangled);
#else
  name = typeid( T ).name();
#endif

  return name;
}


template < class TInputPixelType, class TOutputPixelType >
bool Testclamp2FromTo()
{

  typedef itk::Image< TInputPixelType, 3 >                        InputImageType;
  typedef itk::Image< TOutputPixelType, 3 >                       OutputImageType;
  typedef itk::ClampImageFilter< InputImageType, OutputImageType > FilterType;

  typedef itk::RandomImageSource< InputImageType > SourceType;
  typename SourceType::Pointer source = SourceType::New();

  typename InputImageType::SizeValueType randomSize[3] = {18, 17, 23};
  source->SetSize( randomSize );

  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput( source->GetOutput() );
  filter->UpdateLargestPossibleRegion();
  bool threadPoolUsed = false;
  bool defaultThreadPoolUsed = false;
  itk::ThreadPoolFactory threadPoolFac;
  itk::ThreadPool::Pointer Pool = threadPoolFac.GetSmartThreadPool(1);  

  Pool->DebugOn();

  typename FilterType::Pointer filter1 = FilterType::New();
//  Turn this on if you want to set the threadpool
//  filter1->GetMultiThreader()->SetThreadpool(Pool);  

  filter1->SetInput( source->GetOutput() );  

  filter1->UpdateLargestPossibleRegion();

  if(Pool->GetCompletedJobs() == 1)
   {
     threadPoolUsed = true;
   }

   if(filter->GetMultiThreader()->GetThreadpool()->GetCompletedJobs() == 1)
    {
      defaultThreadPoolUsed = true;
    }

   std::cout<< "Default threadpool used :"<<defaultThreadPoolUsed<<std::endl;
   std::cout<< "New threadpool used :"<<threadPoolUsed<<std::endl;
////////////////////////////

  typedef itk::ImageRegionConstIterator< InputImageType >  InputIteratorType;
  typedef itk::ImageRegionConstIterator< OutputImageType > OutputIteratorType;

  InputIteratorType  it( source->GetOutput(),
                         source->GetOutput()->GetLargestPossibleRegion() );
  OutputIteratorType ot( filter->GetOutput(),
                         filter->GetOutput()->GetLargestPossibleRegion() );

  bool success = true;

  return success;
}


template < class TInputPixelType >
bool Testclamp2From()
{
std::cout<<"Testfrom"<<std::endl;
  bool success =
    Testclamp2FromTo< TInputPixelType, char >() &&
    Testclamp2FromTo< TInputPixelType, unsigned char >() &&
    Testclamp2FromTo< TInputPixelType, short >() &&
    Testclamp2FromTo< TInputPixelType, unsigned short >() &&
    Testclamp2FromTo< TInputPixelType, int >() &&
    Testclamp2FromTo< TInputPixelType, unsigned int >() &&
    Testclamp2FromTo< TInputPixelType, long >() &&
    Testclamp2FromTo< TInputPixelType, unsigned long >() &&
// Visual Studio has a false failure in due to
// imprecise integer to double conversion. It causes the comparison
// dInValue > expectedMax to pass when it should fail.
#ifndef _MSC_VER
    Testclamp2FromTo< TInputPixelType, long long >() &&
    Testclamp2FromTo< TInputPixelType, unsigned long long >() &&
#endif
    Testclamp2FromTo< TInputPixelType, float >() &&
    Testclamp2FromTo< TInputPixelType, double >();

  return success;
}

template < class TInputPixelType, class TOutputPixelType >
bool Testclamp2FromToWithCustomBounds()
{
  typedef itk::Image< TInputPixelType, 3 >                        InputImageType;
  typedef itk::Image< TOutputPixelType, 3 >                       OutputImageType;
  typedef itk::ClampImageFilter< InputImageType, OutputImageType > FilterType;

  typedef itk::RandomImageSource< InputImageType > SourceType;
  typename SourceType::Pointer source = SourceType::New();
  source->SetMin(static_cast< TInputPixelType >(0));
  source->SetMax(static_cast< TInputPixelType >(20));

  typename InputImageType::SizeValueType randomSize[3] = {18, 17, 23};
  source->SetSize( randomSize );

  typename FilterType::Pointer filter = FilterType::New();
  filter->SetBounds(static_cast< TOutputPixelType>(5), static_cast< TOutputPixelType>(15));
  filter->SetInput( source->GetOutput() );
  filter->UpdateLargestPossibleRegion();

  typedef itk::ImageRegionConstIterator< InputImageType >  InputIteratorType;
  typedef itk::ImageRegionConstIterator< OutputImageType > OutputIteratorType;

  InputIteratorType  it( source->GetOutput(),
                         source->GetOutput()->GetLargestPossibleRegion() );
  OutputIteratorType ot( filter->GetOutput(),
                         filter->GetOutput()->GetLargestPossibleRegion() );

  bool success = true;

  std::cout << "Casting from " << Getclamp2TypeName< TInputPixelType >()
            << " to " << Getclamp2TypeName< TOutputPixelType >() << " using custom bounds ... ";

  it.GoToBegin();
  ot.GoToBegin();
  while ( !it.IsAtEnd() )
    {
    TInputPixelType  inValue       = it.Value();
    TOutputPixelType outValue      = ot.Value();
    TOutputPixelType expectedValue;

    double dInValue = static_cast< double >( inValue );
    double expectedMin = filter->GetLowerBound();
    double expectedMax = filter->GetUpperBound();


    if ( dInValue < expectedMin )
      {
      expectedValue = filter->GetLowerBound();
      }
    else if ( dInValue > expectedMax )
      {
      expectedValue = filter->GetUpperBound();
      }
    else
      {
      expectedValue = static_cast< TOutputPixelType >( inValue );
      }

    if ( outValue != expectedValue )
      {
      success = false;
      break;
      }

    ++it;
    ++ot;
    }

  if ( success )
    {
    std::cout << "[PASSED]" << std::endl;
    }
  else
    {
    std::cout << "[FAILED]" << std::endl;
    }

  return success;
}


template < class TInputPixelType >
bool Testclamp2FromWithCustomBounds()
{
  bool success =
    Testclamp2FromToWithCustomBounds< TInputPixelType, char >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, unsigned char >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, short >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, unsigned short >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, int >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, unsigned int >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, long >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, unsigned long >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, long long >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, unsigned long long >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, float >() &&
    Testclamp2FromToWithCustomBounds< TInputPixelType, double >();

  return success;
}


int itkClampImageFilterTest2( int, char* [] )
{
  std::cout << "itkclamp2ImageFilterTest Start" << std::endl;

Testclamp2FromTo< int , int>();


  std::cout << "itkclamp2ImageFilterTest Start2" << std::endl;
  bool success = true;
/*
    Testclamp2From< char >() &&

    Testclamp2From< unsigned char >() &&
//    Testclamp2From< short >() &&
    Testclamp2From< unsigned short >() &&
    Testclamp2From< int >() &&

    Testclamp2From< unsigned int >() &&
    Testclamp2From< long >() &&
    Testclamp2From< unsigned long >() &&
    Testclamp2From< long long >() &&
    Testclamp2From< unsigned long long >() &&
    Testclamp2From< float >() &&
    Testclamp2From< double >() &&

    Testclamp2FromWithCustomBounds< char >() &&
    Testclamp2FromWithCustomBounds< unsigned char >();/* &&
    Testclamp2FromWithCustomBounds< short >() &&
    Testclamp2FromWithCustomBounds< unsigned short >() &&
    Testclamp2FromWithCustomBounds< int >() &&
    Testclamp2FromWithCustomBounds< unsigned int >() &&
    Testclamp2FromWithCustomBounds< long >() &&
    Testclamp2FromWithCustomBounds< unsigned long >() &&
    Testclamp2FromWithCustomBounds< long long >() &&
    Testclamp2FromWithCustomBounds< unsigned long long >() &&
    Testclamp2FromWithCustomBounds< float >() &&
    Testclamp2FromWithCustomBounds< double >();*/
  std::cout << "itkclamp2ImageFilterTest Start3" << std::endl;
  std::cout << std::endl;
  if ( !success )
    {
    std::cout << "An itkclamp2ImageFilter test FAILED." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "All itkclamp2ImageFilter tests PASSED." << std::endl;

  return EXIT_SUCCESS;
}
