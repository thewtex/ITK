/*=========================================================================
 *
 *  Copyright NumFOCUS
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
#ifndef itkAtomicShim_h
#define itkAtomicShim_h

namespace itk
{

/** \class AtomicShim
 * \brief Shim for std::atomic when not available.
 *
 * For use in a single-threaded environment.
 */
template <typename TValue>
class AtomicShim
{
public:
  using ValueType = TValue;
  using Self = AtomicShim<TValue>;

  // Conversion
  operator TValue() const { return this->m_Value; }
  // Assignment
  Self &
  operator=(const TValue value)
  {
    this->m_Value = value;
    return *this;
  }
  // Construction
  AtomicShim(const TValue value)
    : m_Value(value)
  {}
  AtomicShim() = default;

  TValue
  fetch_add(TValue increment)
  {
    m_Value += increment;
    return m_Value;
  }

  Self &
  operator++()
  {
    ++(this->m_Value);
    return *this;
  }
  Self &
  operator--()
  {
    --(this->m_Value);
    return *this;
  }

private:
  ValueType m_Value{ 0 };
};
} // end namespace itk

#endif
