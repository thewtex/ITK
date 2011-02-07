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
#ifndef __itkSpecialCoordinatesImageSource_h
#define __itkSpecialCoordinatesImageSource_h

namespace itk
{
/** \class SpecialCoordinatesImageSource
 *  \brief Mixin class that enables an ImageSource to specify the
 *  index-to-physical Transform of its output when the ImageSource is
 *  templated over a SpecialCoordinatesImage.
 *
 * This class is templated over an ImageSource and is meant to facilitate
 * the ImageSource in setting all the information in an output
 * SpecialCoordinatesImage. It augments the interface of the template
 * parameter ImageSource with the methods
 * SetIndexToPhysicalPointTransform()  and
 * GetIndexToPhysicalPointTransform(). These methods enable
 * specification of the Transform that describes the mapping between
 * index space and image samples in physical space (see
 * SpecialImageCoordinates).
 *
 * Usage example:
 *
 * \code
 * typedef itk::AffineTransform< double, 3 >                        TransformType;
 * typedef itk::SpecialCoordinatesImage< double, 3, TransformType > ImageType;
 * typedef itk::GaussianImageSource< ImageType >                    SourceBaseType;
 * typedef itk::SpecialCoordinatesImageSource< SourceBaseType >     SourceType;
 *
 * SourceBaseType::SizeType size = {{256, 256}};
 * TransformType::Pointer transform = TransformType::New();
 *
 * SourceType::Pointer filter = SourceType::New();
 * filter->SetSize(size);
 * filter->SetIndexToPhysicalPointTransform(transform);
 * filter->Update();
 * \endcode
 *
 * \ingroup DataSources
 */
template< class TSource >
class ITK_EXPORT SpecialCoordinatesImageSource : public TSource
{
public:

  /** Standard class typedefs. */
  typedef SpecialCoordinatesImageSource Self;
  typedef TSource                       Superclass;
  typedef SmartPointer< Self >          Pointer;
  typedef SmartPointer< const Self >    ConstPointer;

  typedef typename Superclass::OutputImageType OutputImageType;

  typedef typename OutputImageType::TransformType    TransformType;
  typedef typename OutputImageType::TransformPointer TransformPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(SpecialCoordinatesImageSource, Superclass);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  itkSetObjectMacro(IndexToPhysicalPointTransform, TransformType);
  itkGetConstObjectMacro(IndexToPhysicalPointTransform, TransformType);

  /**
   * Calls GenerateOutputInformation() in the superclass and sets the
   * index-to-physical poing transform in the output.
   */
  void GenerateOutputInformation()
  {
    this->Superclass::GenerateOutputInformation();

    OutputImageType *output = this->GetOutput(0);
    output->SetIndexToPhysicalPointTransform(m_IndexToPhysicalPointTransform);
  }

protected:
  SpecialCoordinatesImageSource()
    {
    m_IndexToPhysicalPointTransform = TransformType::New();
    }
  ~SpecialCoordinatesImageSource() {};
  void PrintSelf(std::ostream & os, Indent indent) const
  {
    this->Superclass::PrintSelf(os, indent);

    os << indent << "IndexToPhysicalPointTransform" << std::endl;
    m_IndexToPhysicalPointTransform->Print( os, indent.GetNextIndent() );
  }

private:
  SpecialCoordinatesImageSource(const SpecialCoordinatesImageSource &);
  void operator=(const SpecialCoordinatesImageSource &);

  TransformPointer m_IndexToPhysicalPointTransform;
};
} // end namespace itk

#endif // __itkSpecialCoordinatesImageSource_h
