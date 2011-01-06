#ifndef __ITKGPUIMAGE_TXX__
#define __ITKGPUIMAGE_TXX__

#include "itkGPUImage.h"

namespace itk
{
//
// Constructor
//
template <class TPixel, unsigned int VImageDimension>
GPUImage< TPixel, VImageDimension >::GPUImage()
{
m_gpuManager = GPUDataManager::New();
}

template <class TPixel, unsigned int VImageDimension>
GPUImage< TPixel, VImageDimension >::~GPUImage()
{

}

template <class TPixel, unsigned int VImageDimension>
void GPUImage< TPixel, VImageDimension >::Allocate()
{
// allocate CPU memory - calling Allocate() in superclass
Superclass::Allocate();
m_gpuManager->SetCPUBufferPointer( Superclass::GetBufferPointer() );

// allocate GPU memory
this->ComputeOffsetTable();
unsigned long numPixel = this->GetOffsetTable()[VImageDimension];
m_gpuManager->SetBufferSize( sizeof(TPixel)*numPixel );
m_gpuManager->Allocate();
}


template< class TPixel, unsigned int VImageDimension >
void GPUImage< TPixel, VImageDimension >::Initialize()
{
Superclass::Initialize();
m_gpuManager->SetCPUDirtyFlag( false );
m_gpuManager->SetGPUDirtyFlag( true );
}

template <class TPixel, unsigned int VImageDimension>
void GPUImage< TPixel, VImageDimension >::FillBuffer(const TPixel & value)
{
m_gpuManager->SetGPUBufferDirty();
Superclass::FillBuffer( value );
}

template <class TPixel, unsigned int VImageDimension>
void GPUImage< TPixel, VImageDimension >::SetPixel(const IndexType & index, const TPixel & value)
{
m_gpuManager->SetGPUBufferDirty();
Superclass::SetPixel( index, value );
}

template <class TPixel, unsigned int VImageDimension>
TPixel & GPUImage< TPixel, VImageDimension >::GetPixel(const IndexType & index)
{
m_gpuManager->SetGPUBufferDirty();
return Superclass::GetPixel( index );
}

template <class TPixel, unsigned int VImageDimension>
TPixel & GPUImage< TPixel, VImageDimension >::operator[](const IndexType & index)
{
m_gpuManager->MakeCPUBufferUpToDate();
m_gpuManager->SetGPUDirtyFlage( true );
return Superclass::[]( index );
}

template <class TPixel, unsigned int VImageDimension>
typename GPUImage< TPixel, VImageDimension >::PixelContainer * GPUImage< TPixel, VImageDimension >::GetPixelContainer()
{
m_gpuManager->SetGPUBufferDirty();
return Superclass::GetPixelContainer();
}

template <class TPixel, unsigned int VImageDimension>
void GPUImage< TPixel, VImageDimension >::SetPixelContainer(PixelContainer *container)
{
Superclass::SetPixelContainer( container );
m_gpuManager->SetCPUDirtyFlage( false );
m_gpuManager->SetGPUDirtyFlage( true );
}

template <class TPixel, unsigned int VImageDimension>
TPixel* GPUImage< TPixel, VImageDimension >::GetBufferPointer()
{
m_gpuManager->SetGPUBufferDirty();
return Superclass::GetBufferPointer();
}

template <class TPixel, unsigned int VImageDimension>
GPUDataManager::Pointer GPUImage< TPixel, VImageDimension >::GetGPUDataManager()
{
return m_gpuManager;
}

} // namespace itk

#endif
