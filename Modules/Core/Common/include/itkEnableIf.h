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
#ifndef __itkEnableIf_h
#define __itkEnableIf_h

namespace itk
{
/* \brief This is an implementation of the "enable if" idiom.
 *
 * This template enables specialization of a templated function based
 * on some traits or concepts. It is implemented with SFINAE.
 *
 * If the parameter V is true then the Type trait is the second
 * provided template parameter, otherwise the implementation does not
 * exist and with SFINAE another implementation may be choosen.
 *
 * Example:
 * \code
 * template<class TImageType>
 * typename EnableIf<TImageType::VImageDimension == 2>::Type
 * InternalMethod ( unsigned int anything );
 *
 * template<class TImageType>
 * typename DisableIf<TImageType::VImageDimension == 2>::Type
 * InternalMehod ( unsigned int anything );
 * \endcode
 *
 */
template <bool V, class T = void> struct EnableIf {};
/** \cond HIDE_SPECIALIZATION_DOCUMENTATION */
template <class T> struct EnableIf<true, T> { typedef T Type; };
/**\endcond*/


/* \brief An implementation of the negation of the enable if idiom.
 *
 * \sa EnableIf
 */
template <bool V, class T = void> struct DisableIf {};
/** \cond HIDE_SPECIALIZATION_DOCUMENTATION */
template <class T> struct DisableIf<false, T> { typedef T Type; };
/**\endcond*/

}

#endif // __itkEnableIf_h
