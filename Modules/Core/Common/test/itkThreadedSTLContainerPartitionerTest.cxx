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
#include "itkDomainThreader.h"
#include "itkThreadedSTLContainerPartitioner.h"

class DomainThreaderAssociate
{
public:
  typedef DomainThreaderAssociate Self;

  typedef std::vector< int > DomainContainerType;
  typedef itk::ThreadedSTLContainerPartitioner< DomainContainerType > ThreadedPartitionerType;

  class TestDomainThreader: public itk::DomainThreader< ThreadedPartitionerType, Self >
  {
  public:
    typedef TestDomainThreader                                   Self;
    typedef itk::DomainThreader< ThreadedPartitionerType, Self > Superclass;
    typedef itk::SmartPointer< Self >                            Pointer;
    typedef itk::SmartPointer< const Self >                      ConstPointer;

    typedef Superclass::DomainPartitionerType     DomainPartitionerType;
    typedef Superclass::DomainType                DomainType;

    itkNewMacro( Self );

    typedef std::vector< int >              BorderValuesType;
    typedef std::vector< BorderValuesType > DomainBorderValuesInThreadedExecutionType;

    const DomainBorderValuesInThreadedExecutionType & GetDomainInThreadedExecution() const
      {
      return m_DomainInThreadedExecution;
      }

  protected:
    TestDomainThreader() {};

  private:
    virtual void BeforeThreadedExecution()
      {
      this->m_DomainInThreadedExecution.resize( this->GetNumberOfThreadsUsed() );
      BorderValuesType unsetBorderValues( 2, -1 );
      for( itk::ThreadIdType i = 0; i < m_DomainInThreadedExecution.size(); ++i )
        {
        m_DomainInThreadedExecution[i] = unsetBorderValues;
        }
      }

    virtual void ThreadedExecution( const DomainType& subdomain,
                                    const itk::ThreadIdType threadId )
      {
      if( threadId == 0 )
        {
        std::cout << "This is the : " << this->m_Associate->m_ClassDescriptor << std::endl;
        }
      this->m_DomainInThreadedExecution[threadId].resize( 2 );
      this->m_DomainInThreadedExecution[threadId][0] = *(subdomain.m_StartIterator);
      DomainType::IteratorType it = subdomain.m_EndIterator;
      --it;
      this->m_DomainInThreadedExecution[threadId][1] = *it;
      }

    virtual void AfterThreadedExecution()
      {
      std::cout << "\nDomain partition per thread:" << std::endl;
      for( itk::ThreadIdType i = 0; i < m_DomainInThreadedExecution.size(); ++i )
        {
        std::cout << "ThreadId: " << i << "\t" << m_DomainInThreadedExecution[i][0] << " " << m_DomainInThreadedExecution[i][1] << std::endl;
        }
      std::cout << std::endl;
      }

    DomainBorderValuesInThreadedExecutionType m_DomainInThreadedExecution;
    TestDomainThreader( const Self & ); // purposely not implemented
    void operator=( const Self & );     // purposely not implemented
  }; // end TestDomainThreader class

  DomainThreaderAssociate()
    {
    m_TestDomainThreader = TestDomainThreader::New();
    m_ClassDescriptor    = "enclosing class";
    }

  TestDomainThreader * GetDomainThreader()
    {
    return m_TestDomainThreader.GetPointer();
    }

  void Execute( const TestDomainThreader::DomainType & completeDomain )
    {
    m_TestDomainThreader->Execute(this, completeDomain);
    }

  private:
    TestDomainThreader::Pointer m_TestDomainThreader;

    std::string m_ClassDescriptor;
};


int ThreadedSTLContainerPartitionerRunTest(
  DomainThreaderAssociate & enclosingClass,
  itk::ThreadIdType numberOfThreads,
  const DomainThreaderAssociate::TestDomainThreader::DomainType & fullDomain )
{
  std::cout << "Testing with " << numberOfThreads << std::endl;

  DomainThreaderAssociate::TestDomainThreader::Pointer domainThreader = enclosingClass.GetDomainThreader();

  // Exercise GetMultiThreader().
  domainThreader->GetMultiThreader();
  domainThreader->SetMaximumNumberOfThreads( numberOfThreads );
  // Possible if numberOfThreads < GlobalMaximumNumberOfThreads
  if( domainThreader->GetMaximumNumberOfThreads() != numberOfThreads )
    {
    std::cerr << "Failed setting requested number of threads: "
              << numberOfThreads << std::endl
              << "domainThreader->GetMaximumNumberOfThreads(): "
              << domainThreader->GetMaximumNumberOfThreads() << std::endl;
    return EXIT_FAILURE;
    }

  enclosingClass.Execute( fullDomain );

  /* Did we use as many threads as requested? */
  std::cout << "Requested numberOfThreads: " << numberOfThreads << std::endl
            << "actual: threader->GetNumberOfThreadsUsed(): "
            << domainThreader->GetNumberOfThreadsUsed() << "\n\n" << std::endl;

  /* Check the results. */
  typedef DomainThreaderAssociate::TestDomainThreader::BorderValuesType BorderValuesType;
  int previousEndIndex = -1;
  const DomainThreaderAssociate::TestDomainThreader::DomainBorderValuesInThreadedExecutionType domainInThreadedExecution = domainThreader->GetDomainInThreadedExecution();
  for( itk::ThreadIdType i = 0; i < domainThreader->GetNumberOfThreadsUsed(); ++i )
    {
    BorderValuesType subRange = domainInThreadedExecution[i];
    /* Check that the sub range was assigned something at all */
    if( subRange[0] == -1 ||
        subRange[1] == -1 )
        {
        std::cerr << "Error: subRange " << i << " is was not set: "
                  << subRange[i];
        return EXIT_FAILURE;
        }
    /* Check that we got the begin of the range */
    if( i == 0 && subRange[0] != *(fullDomain.m_StartIterator) )
        {
        std::cerr << "Error: subRange[0][0] should be " << *(fullDomain.m_StartIterator)
                  << ", but it's " << subRange[0] << ".";
        return EXIT_FAILURE;
        }
    /* Check that we got the end of the range */
    BorderValuesType::const_iterator fullIt = fullDomain.m_EndIterator;
    --fullIt;
    if( i == numberOfThreads-1 && subRange[1] != *fullIt )
        {
        std::cerr << "Error: subRange[N-1][1] should be " << *fullIt
                  << ", but it's " << subRange[1] << ".";
        return EXIT_FAILURE;
        }
    /* Check that the sub-range endings and beginnings are continuous */
    if( i > 0 )
      {
      if( previousEndIndex + 1 != subRange[0] )
        {
        std::cerr << "Error: subRange " << i << " is not continuous with "
                  << "previous subRange." << std::endl
                  << "previousEndIndex: " << previousEndIndex << std::endl
                  << "subRange[0]: " << subRange[0] << std::endl;
        return EXIT_FAILURE;
        }
      }
    previousEndIndex = subRange[1];
    }

  return EXIT_SUCCESS;
}

// Helper function.
void getIteratorFromIndex( const unsigned int index, const DomainThreaderAssociate::DomainContainerType & container, DomainThreaderAssociate::DomainContainerType::const_iterator & it )
{
  it = container.begin();
  for( unsigned int ii = 0; ii <= index; ++ii )
    {
    ++it;
    }
}

int itkThreadedSTLContainerPartitionerTest(int, char* [])
{
  DomainThreaderAssociate enclosingClass;
  DomainThreaderAssociate::TestDomainThreader::ConstPointer domainThreader = enclosingClass.GetDomainThreader();

  /* Check # of threads */
  std::cout << "GetGlobalMaximumNumberOfThreads: "
            << domainThreader->GetMultiThreader()->GetGlobalMaximumNumberOfThreads()
            << std::endl;
  std::cout << "GetGlobalDefaultNumberOfThreads: "
            << domainThreader->GetMultiThreader()->GetGlobalDefaultNumberOfThreads()
            << std::endl;
  std::cout << "domainThreader->GetMultiThreader()->NumberOfThreads(): " << domainThreader->GetMultiThreader()->GetNumberOfThreads()
            << std::endl;

  typedef DomainThreaderAssociate::TestDomainThreader::DomainType DomainType;
  DomainType::ContainerType container( 256 );
  for( unsigned int i = 0; i < 256; ++i )
    {
    container[i] = i;
    }
  DomainType fullDomain;
  fullDomain.m_StartIterator = container.begin();
  fullDomain.m_EndIterator   = container.end();

  /* Test with single thread */
  getIteratorFromIndex( 0, container, fullDomain.m_StartIterator );
  getIteratorFromIndex( 102, container, fullDomain.m_EndIterator ); //set total range to prime to test uneven division
  itk::ThreadIdType numberOfThreads = 1;
  if( ThreadedSTLContainerPartitionerRunTest( enclosingClass, numberOfThreads, fullDomain )
        != EXIT_SUCCESS )
    {
    return EXIT_FAILURE;
    }

  /* Test with range that doesn't start at 0 */
  getIteratorFromIndex( 2, container, fullDomain.m_StartIterator );
  getIteratorFromIndex( 104, container, fullDomain.m_EndIterator ); //set total range to prime to test uneven division
  numberOfThreads = 1;
  if( ThreadedSTLContainerPartitionerRunTest( enclosingClass, numberOfThreads, fullDomain )
        != EXIT_SUCCESS )
    {
    return EXIT_FAILURE;
    }

  /* Test with multiple threads */
  if( domainThreader->GetMultiThreader()->GetGlobalMaximumNumberOfThreads() > 1 )
    {
    /* Test with default number of threads. */
    getIteratorFromIndex( 6, container, fullDomain.m_StartIterator );
    getIteratorFromIndex( 108, container, fullDomain.m_EndIterator ); //set total range to prime to test uneven division
    numberOfThreads =
      domainThreader->GetMultiThreader()->GetGlobalDefaultNumberOfThreads();
    if( ThreadedSTLContainerPartitionerRunTest( enclosingClass, numberOfThreads, fullDomain )
          != EXIT_SUCCESS )
      {
      return EXIT_FAILURE;
      }

    /* Test with max number of threads and check that we only used as
     * many as is reasonable. */
    itk::ThreadIdType maxNumberOfThreads =
      domainThreader->GetMultiThreader()->GetGlobalMaximumNumberOfThreads();
    getIteratorFromIndex( 6, container, fullDomain.m_StartIterator );
    getIteratorFromIndex( 6+maxNumberOfThreads-2, container, fullDomain.m_EndIterator );
    if( ThreadedSTLContainerPartitionerRunTest( enclosingClass, maxNumberOfThreads, fullDomain )
          != EXIT_SUCCESS )
      {
      return EXIT_FAILURE;
      }
    if( domainThreader->GetNumberOfThreadsUsed() != maxNumberOfThreads-1 )
      {
      std::cerr << "Error: Expected to use only " << maxNumberOfThreads-1
                << "threads, but used " << domainThreader->GetNumberOfThreadsUsed()
                << "." << std::endl;
      }
    }
  else
    {
    std::cout << "No multi-threading available. " << std::endl;
    }

  return EXIT_SUCCESS;
}
