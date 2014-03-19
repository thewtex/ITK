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
#pragma once
#include <iostream>
#include "itkWinThreadpool.h"
#include <stdlib.h>

using namespace itk;
#define ITERATIONS 10

void * execute(void *ptr)
{
  // Here - get any args from ptr.
  std::cout << std::endl << "Thread fn starting.." << std::endl;
  int m = (int)ptr;
  std::cout << "Ptr received int :" << m << std::endl;
  int    n = 10;
  double factorial = 1.0;
  for( int j = 0; j < 90; j++ )
    {
    factorial = 1.0;
    for( int i = 1; i <= n; i++ )
      {
      factorial = factorial * i;
      }
    }
  std::cout << std::endl << "Thread fn DONE" << std::endl;

  return 0;
}

int main(int argc, char *argv[])
{

  try
    {
    int numThreadsInThreadPool = 25;
    if( argc > 1 )
      {
      numThreadsInThreadPool = atoi(argv[1]);
      }
    std::cout << "Testing itkPthreadPool. Num of threads :" << numThreadsInThreadPool << std::endl;
    WinThreadPool & myPool = WinThreadPool::GetPoolInstance(numThreadsInThreadPool);
    // assign work
    for( unsigned int i = 0; i < ITERATIONS; i++ )
      {
      WinJob winJob;
      winJob.ThreadArgs.otherArgs = (void *) i;
      winJob.ThreadFunction = (&execute);
      std::cout << "Thread id[" << winJob.Id << "] " << std::endl;
      int id = myPool.AssignWork(winJob);

      myPool.WaitForThread(id);

      }

    std::cout << "All threads finished" << std::endl;

    WinThreadPool::DeleteInstance();

    }
  catch( const std::exception& ex )
    {
    std::cout << ex.what() << std::endl;
    return -1;
    }
  catch( const std::string& ex )
    {
    std::cout << "Exception occured" << std::endl;
    return -1;
    }
  catch( ... )
    {
    std::cout << "Uncaught Exception occured" << std::endl;
    return -1;
    }
  return 0;
}

