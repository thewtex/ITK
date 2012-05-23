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

#ifndef __itkTestObjectDOMReader_h
#define __itkTestObjectDOMReader_h

#include "itkTestObject.h"
#include "itkDOMReader.h"

namespace itk
{
    // DOM reader for TestObject
    class TestObjectDOMReader : public DOMReader<TestObject>
    {
    public:
        /** Standard class typedefs. */
        typedef TestObjectDOMReader         Self;
        typedef DOMReader<TestObject>       Superclass;
        typedef SmartPointer< Self >        Pointer;
        typedef SmartPointer< const Self >  ConstPointer;

        /** Method for creation through the object factory. */
        itkNewMacro(Self);

        /** Run-time type information (and related methods). */
        itkTypeMacro(TestObjectDOMReader, DOMReader);

        /** Function to generate the output object from the implicitly created DOM object. */
        virtual void GenerateOutput( const DOMNode* domnode, const void* userdata = 0 );

    protected:
        TestObjectDOMReader() {}

    private:
        TestObjectDOMReader(const Self &); //purposely not implemented
        void operator=(const Self &); //purposely not implemented
    };
} // namespace itk

#endif // __itkTestObjectDOMReader_h
