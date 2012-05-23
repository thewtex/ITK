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

#ifndef __itkTestObjectDOMWriter_h
#define __itkTestObjectDOMWriter_h

#include "itkTestObject.h"
#include "itkDOMWriter.h"

namespace itk
{
    // DOM writer for TestObject
    class TestObjectDOMWriter : public DOMWriter<TestObject>
    {
    public:
        /** Standard class typedefs. */
        typedef TestObjectDOMWriter         Self;
        typedef DOMWriter<TestObject>       Superclass;
        typedef SmartPointer< Self >        Pointer;
        typedef SmartPointer< const Self >  ConstPointer;

        /** Method for creation through the object factory. */
        itkNewMacro(Self);

        /** Run-time type information (and related methods). */
        itkTypeMacro(TestObjectDOMWriter, DOMWriter);

        /** Function to generate the internal DOM object from the input user object. */
        virtual void GenerateDOM( DOMNode* domnode, const void* userdata = 0 ) const;

    protected:
        TestObjectDOMWriter() {}

    private:
        TestObjectDOMWriter(const Self &); //purposely not implemented
        void operator=(const Self &); //purposely not implemented
    };
} // namespace itk

#endif // __itkTestObjectDOMWriter_h
