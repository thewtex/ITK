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

#include "itkFastMarchingStoppingCriterionBase.h"
#include "itkImage.h"
#include "itkMesh.h"
#include "itkFastMarchingTraits.h"

namespace itk
{
  template< class TTraits >
  class FastMarchingStoppingCriterionBaseHelperTest :
      public FastMarchingStoppingCriterionBase< TTraits >
    {
  public:
    typedef FastMarchingStoppingCriterionBaseHelperTest Self;
    typedef FastMarchingStoppingCriterionBase< TTraits > Superclass;
    typedef SmartPointer< Self >              Pointer;
    typedef SmartPointer< const Self >        ConstPointer;

    typedef typename Superclass::NodeType NodeType;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(FastMarchingStoppingCriterionBaseHelperTest,
                 FastMarchingStoppingCriterionBase );

    bool IsSatisfied() const { return true; }
    const std::string GetDescription() const { return "Description"; }

  protected:
    FastMarchingStoppingCriterionBaseHelperTest() : Superclass() {}
    ~FastMarchingStoppingCriterionBaseHelperTest() {}

    void SetCurrentNode( const NodeType& ) {}

  private:
    FastMarchingStoppingCriterionBaseHelperTest( const Self& );
    void operator = ( const Self& );
    };
}

int itkFastMarchingStoppingCriterionBaseTest( int argc, char* argv[] )
  {
  (void) argc;
  (void) argv;

  typedef itk::Image< float, 2> ImageType;

  typedef itk::ImageFastMarchingTraits2< ImageType, ImageType > ImageTraits;

  typedef itk::FastMarchingStoppingCriterionBaseHelperTest< ImageTraits >
    ImageStoppingCriterionType;

  ImageStoppingCriterionType::Pointer image_criterion =
      ImageStoppingCriterionType::New();

  typedef itk::Mesh< float, 3, itk::DefaultStaticMeshTraits< float, 3, 3 > >
      MeshType;

  typedef itk::MeshFastMarchingTraits2< MeshType, MeshType > MeshTraits;
  typedef itk::FastMarchingStoppingCriterionBaseHelperTest< MeshTraits >
      MeshStoppingCriterionType;

  MeshStoppingCriterionType::Pointer mesh_criterion =
      MeshStoppingCriterionType::New();

  return EXIT_SUCCESS;
  }
