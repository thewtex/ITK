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

#ifndef __itkGPUImage_h__
#define __itkGPUImage_h__

#include "itkImage.h"
#include "itkGPUDataManager.h"

namespace itk
{
  template <class TPixel, unsigned int VImageDimension = 2>
  class ITK_EXPORT GPUImage : public Image<TPixel,VImageDimension>
  {
  public:
    typedef GPUImage                        Self;
    typedef Image<TPixel,VImageDimension>   Superclass;
    typedef SmartPointer<Self>              Pointer;
    typedef SmartPointer<const Self>        ConstPointer;
    typedef WeakPointer<const Self>         ConstWeakPointer;

    itkNewMacro(Self);

    itkTypeMacro(GPUImage, Image);

    itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

    typedef typename Superclass::PixelType           PixelType;
    typedef typename Superclass::ValueType           ValueType;
    typedef typename Superclass::InternalPixelType   InternalPixelType;
    typedef typename Superclass::IOPixelType         IOPixelType;
    typedef typename Superclass::DirectionType       DirectionType;
    typedef typename Superclass::SpacingType         SpacingType;
    typedef typename Superclass::IOPixelType         IOPixelType;
    typedef typename Superclass::PixelContainer      PixelContainer;
    typedef typename Superclass::SizeType            SizeType;
    typedef typename Superclass::IndexType           IndexType;
    typedef typename Superclass::OffsetType          OffsetType;
    typedef typename Superclass::RegionType          RegionType;
    typedef typename PixelContainer::Pointer         PixelContainerPointer;
    typedef typename PixelContainer::ConstPointer    PixelContainerConstPointer;

    typedef DefaultPixelAccessor< PixelType >   AccessorType;
    typedef DefaultPixelAccessorFunctor< Self > AccessorFunctorType;
    typedef NeighborhoodAccessorFunctor< Self > NeighborhoodAccessorFunctorType;

    //
    // Allocate CPU and GPU memory space
    //
    void Allocate();

    virtual void Initialize();

    void FillBuffer(const TPixel & value);

    void SetPixel(const IndexType & index, const TPixel & value);

    TPixel & GetPixel(const IndexType & index);

    const TPixel & operator[](const IndexType & index) const
    { return Superclass::GetPixel(index); }

    TPixel & operator[](const IndexType & index);

    //
    // Get CPU buffer pointer
    //
    TPixel* GetBufferPointer();

    const TPixel * GetBufferPointer() const;

    /** Return the Pixel Accessor object */
    AccessorType GetPixelAccessor(void)
    { m_GPUManager->SetGPUBufferDirty(); return Superclass::GetPixelAccessor; }

    /** Return the Pixel Accesor object */
    const AccessorType GetPixelAccessor(void) const
    {
      m_GPUManager->MakeCPUBufferUpToDate();
      return Superclass::GetPixelAccessor();
    }

    /** Return the NeighborhoodAccessor functor */
    NeighborhoodAccessorFunctorType GetNeighborhoodAccessor()
    { m_GPUManager->SetGPUBufferDirty(); return NeighborhoodAccessorFunctorType(); }

    /** Return the NeighborhoodAccessor functor */
    const NeighborhoodAccessorFunctorType GetNeighborhoodAccessor() const
    {
      m_GPUManager->MakeCPUBufferUpToDate();
      return NeighborhoodAccessorFunctorType();
    }

    void SetPixelContainer(PixelContainer *container);

     /** Return a pointer to the container. */
    PixelContainer * GetPixelContainer()
    { m_GPUManager->SetGPUBufferDirty(); return Superclass::GetPixelContainer(); }

    const PixelContainer * GetPixelContainer() const
    {
      m_GPUManager->MakeCPUBufferUpToDate();
      return Superclass::GetPixelContainer();
    }

    void SetCurrentCommandQueue( int queueid ) { m_GPUManager->SetCurrentCommandQueue( queueid ); } ;

    int  GetCurrentCommandQueueID() { m_GPUManager->GetCurrentCommandQueueID(); };

    GPUDataManager::Pointer GetGPUDataManager();

  protected:
    GPUImage();
    virtual ~GPUImage();

  private:

    // functions that are purposely not implemented
    GPUImage(const Self&);
    void operator=(const Self&);

    GPUDataManager::Pointer m_GPUManager;
  };

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_GPUImage(_, EXPORT, TypeX, TypeY)     \
  namespace itk                                         \
  {                                                     \
  _( 2 ( class EXPORT GPUImage< ITK_TEMPLATE_2 TypeX > ) ) \
  namespace Templates                                   \
  {                                                     \
  typedef GPUImage< ITK_TEMPLATE_2 TypeX > GPUImage##TypeY; \
  }                                                     \
  }

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkGPUImage+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkGPUImage.txx"
#endif

#endif
