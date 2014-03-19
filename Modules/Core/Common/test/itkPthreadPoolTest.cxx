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
#include "itkPThreadPool.h"
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

}

int itkPthreadPoolTest(int argc, char *argv[])
{

  try
    {
    int numThreadsInThreadPool = 8;
    if( argc > 1 )
      {
      numThreadsInThreadPool = atoi(argv[1]);
      }
    std::cout << "Testing itkPthreadPool. Num of threads :" << numThreadsInThreadPool << std::endl;
    PThreadPool& myPool = PThreadPool::GetPoolInstance(numThreadsInThreadPool);

    time_t t1 = time(NULL);
    // assign work
    for( unsigned int i = 0; i < ITERATIONS; i++ )
      {
      PThreadJob pthreadJob;
      pthreadJob.ThreadArgs.otherArgs = (void *) i;
      pthreadJob.ThreadFunction = (&execute);
      std::cout << "Thread id[" << pthreadJob.Id << "] " << std::endl;
      int id = myPool.AssignWork(pthreadJob);

      myPool.WaitForThread(id);

      }
/*
        //assigining small work again - looping only twice
        for(unsigned int i=0; i<2; i++) {
            PThreadJob* pthreadJob = new PThreadJob(i);



            std::cout << "Thread id[" << pthreadJob->id << "] " << std::endl;
            myPool->AssignWork(pthreadJob);
      std::cout<<"Waiting.."<<std::endl;

            while(pthreadJob->ActiveFlag){

                std::cout<<std::endl<<"Waiting for pthreadJob. id is "<<i;

            }

        }
  */

    std::cout << "All threads finished" << std::endl;
    // myPool.DestroyPool(2);

    time_t t2 = time(NULL);
    std::cout << t2 - t1 << " seconds elapsed\n" << std::endl;
    PThreadPool::DeleteInstance();
    }
  catch( const std::exception& ex )
    {
    std::cout << ex.what() << std::endl;
    return -1;
    }
  catch( const std::string& ex )
    {
    std::cout << ex << std::endl;
    return -1;
    }
  catch( ... )
    {
    std::cout << "Uncaught Exception occured" << std::endl;
    return -1;
    }
  return 0;
}

