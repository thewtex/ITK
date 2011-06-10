#ifndef __itkGPUImage_txx
#define __itkGPUImage_txx

#include "itkGPUImage.h"

namespace itk
{
//
// Constructor
//
template <class TPixel, unsigned int VImageDimension>
GPUImage< TPixel, VImageDimension >::GPUImage()
{
  m_GPUManager = GPUImageDataManager< GPUImage< TPixel, VImageDimension > >::New();
  m_GPUManager->SetTimeStamp( this->GetTimeStamp() );
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

  // allocate GPU memory
  this->ComputeOffsetTable();
  unsigned long numPixel = this->GetOffsetTable()[VImageDimension];
  m_GPUManager->SetBufferSize( sizeof(TPixel)*numPixel );
  m_GPUManager->SetImagePointer( this );
  m_GPUManager->SetCPUBufferPointer( Superclass::GetBufferPointer() );
  m_GPUManager->Allocate();

  /* prevent unnecessary copy from CPU to GPU at the beginning */
  m_GPUManager->SetTimeStamp( this->GetTimeStamp() );
}


template< class TPixel, unsigned int VImageDimension >
void GPUImage< TPixel, VImageDimension >::Initialize()
{
  // CPU image initialize
  Superclass::Initialize();

  // GPU image initialize
  m_GPUManager->Initialize();
  this->ComputeOffsetTable();
  unsigned long numPixel = this->GetOffsetTable()[VImageDimension];
  m_GPUManager->SetBufferSize( sizeof(TPixel)*numPixel );
  m_GPUManager->SetImagePointer( this );
  m_GPUManager->SetCPUBufferPointer( Superclass::GetBufferPointer() );
  m_GPUManager->Allocate();

  /* prevent unnecessary copy from CPU to GPU at the beginning */
  m_GPUManager->SetTimeStamp( this->GetTimeStamp() );
}

template <class TPixel, unsigned int VImageDimension>
void GPUImage< TPixel, VImageDimension >::FillBuffer(const TPixel & value)
{
  m_GPUManager->SetGPUBufferDirty();
  Superclass::FillBuffer( value );
}

template <class TPixel, unsigned int VImageDimension>
void GPUImage< TPixel, VImageDimension >::SetPixel(const IndexType & index, const TPixel & value)
{
  m_GPUManager->SetGPUBufferDirty();
  Superclass::SetPixel( index, value );
}

template <class TPixel, unsigned int VImageDimension>
TPixel & GPUImage< TPixel, VImageDimension >::GetPixel(const IndexType & index)
{
  /* Original version - very conservative
  m_GPUManager->SetGPUBufferDirty();
  return Superclass::GetPixel( index );
  */
  m_GPUManager->MakeCPUBufferUpToDate();
  return Superclass::GetPixel( index );
}

template <class TPixel, unsigned int VImageDimension>
TPixel & GPUImage< TPixel, VImageDimension >::operator[](const IndexType & index)
{
  /* Original version - very conservative
  m_GPUManager->SetGPUBufferDirty();
  return Superclass::operator[]( index );
  */
  m_GPUManager->MakeCPUBufferUpToDate();
  return Superclass::operator[]( index );
}

template <class TPixel, unsigned int VImageDimension>
void GPUImage< TPixel, VImageDimension >::SetPixelContainer(PixelContainer *container)
{
  Superclass::SetPixelContainer( container );
  m_GPUManager->SetCPUDirtyFlag( false );
  m_GPUManager->SetGPUDirtyFlag( true );
}

template <class TPixel, unsigned int VImageDimension>
TPixel* GPUImage< TPixel, VImageDimension >::GetBufferPointer()
{
  /* Original version - very conservative
   * Always set GPU dirty (even though pixel values are not modified)
  m_GPUManager->SetGPUBufferDirty();
  return Superclass::GetBufferPointer();
  */

  /* less conservative version - if you modify pixel value using
   * this pointer then you must set the image as modified manually!!! */
  m_GPUManager->MakeCPUBufferUpToDate();
  return Superclass::GetBufferPointer();
}

template <class TPixel, unsigned int VImageDimension>
const TPixel * GPUImage< TPixel, VImageDimension >::GetBufferPointer() const
{
  // const does not change buffer, but if CPU is dirty then make it up-to-date
  m_GPUManager->MakeCPUBufferUpToDate();
  return Superclass::GetBufferPointer();
}

template <class TPixel, unsigned int VImageDimension>
GPUDataManager::Pointer
GPUImage< TPixel, VImageDimension >::GetGPUDataManager()
{
  typedef typename GPUImageDataManager< GPUImage >::Superclass GPUImageDataSuperclass;
  typedef typename GPUImageDataSuperclass::Pointer             GPUImageDataSuperclassPointer;

  return static_cast< GPUImageDataSuperclassPointer >( m_GPUManager.GetPointer() );

  //return m_GPUManager.GetPointer();
}

template <class TPixel, unsigned int VImageDimension>
void
GPUImage< TPixel, VImageDimension >::Graft(const DataObject *data)
{
  typedef typename GPUImageDataManager< GPUImage >     GPUImageDataManagerType;
  typedef typename GPUImageDataManagerType::Superclass GPUImageDataSuperclass;
  typedef typename GPUImageDataSuperclass::Pointer     GPUImageDataSuperclassPointer;

  // call the superclass' implementation
  Superclass::Graft(data);

  // Pass regular pointer to Graft() instead of smart pointer due to type casting problem
  GPUImageDataManagerType* ptr = dynamic_cast<GPUImageDataManagerType*>((((GPUImage*)data)->GetGPUDataManager()).GetPointer());

  // Debug
  //std::cout << "GPU timestamp : " << m_GPUManager->GetMTime() << ", CPU timestamp : " << this->GetMTime() << std::endl;

  // call GPU data graft function
  m_GPUManager->SetImagePointer( this );
  m_GPUManager->Graft( ptr );

  // Synchronize timestamp of GPUImage and GPUDataManager
  m_GPUManager->SetTimeStamp( this->GetTimeStamp() );

  // Debug
  //std::cout << "GPU timestamp : " << m_GPUManager->GetMTime() << ", CPU timestamp : " << this->GetMTime() << std::endl;

}

} // namespace itk

#endif
