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
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkSimpleFastMutexLock.h"
#include "itkMutexLockHolder.h"


namespace itk
{
namespace Statistics
{

//  anonymous namespace for scoped globals
namespace
{
MersenneTwisterRandomVariateGenerator::Pointer  g_Instance = 0;
SimpleFastMutexLock g_InstanceLock;

}

MersenneTwisterRandomVariateGenerator::Pointer
MersenneTwisterRandomVariateGenerator
::CreateInstance()
{
  // Try the factory first
  MersenneTwisterRandomVariateGenerator::Pointer obj  = ObjectFactory< Self >::Create();
  // if the factory did not provide one, then create it here
  if ( !obj )
    {
      obj = new MersenneTwisterRandomVariateGenerator;
      // Remove extra reference from construction.
      obj->UnRegister();
    }
  return obj;
}


MersenneTwisterRandomVariateGenerator::Pointer
MersenneTwisterRandomVariateGenerator
::New()
{
  MersenneTwisterRandomVariateGenerator::Pointer obj  = MersenneTwisterRandomVariateGenerator::CreateInstance();
  obj->SetSeed ( GetInstance()->GetSeed() );
  return obj;
}

/**
 * Return the single instance of the MersenneTwisterRandomVariateGenerator
 */
MersenneTwisterRandomVariateGenerator::Pointer
MersenneTwisterRandomVariateGenerator
::GetInstance()
{
  MutexLockHolder<SimpleFastMutexLock> mutexHolder(g_InstanceLock);

  if ( !g_Instance )
    {
    g_Instance  = MersenneTwisterRandomVariateGenerator::CreateInstance();
    }
  /**
   * return the instance
   */
  return g_Instance;
}
}
}
