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
#ifndef __itkConvertLabelMapFilter_txx
#define __itkConvertLabelMapFilter_txx

#include "itkConvertLabelMapFilter.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{
template< class TInputImage, class TOutputImage >
void
ConvertLabelMapFilter< TInputImage, TOutputImage >
::ThreadedProcessLabelObject(LabelObjectType *labelObject)
{
  typename OutputLabelObjectType::Pointer newLabelObject = OutputLabelObjectType::New();
  newLabelObject->CopyAllFrom(labelObject);
  this->GetOutput()->AddLabelObject(newLabelObject);
}
} // end namespace itk
#endif
