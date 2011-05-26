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
#ifndef __itkMaximumRatioDecisionRule_h
#define __itkMaximumRatioDecisionRule_h

#include <vector>
#include "vnl/vnl_matrix.h"

#include "itkNumericTraits.h"
#include "itkDecisionRule.h"

namespace itk
{
namespace Statistics
{
/** \class MaximumRatioDecisionRule
 *  \brief A decision rule that operates as a frequentist's
 *  approximation to Bayes rule.
 *
 * MaximumRatioDecisionRule returns the class label using a Bayesian
 * style decision rule. The discriminant scores are evaluated in the
 * context of class priors. If the discriminant scores are actual
 * conditional probabilites (likelihoods) and the class priors are
 * actual a priori class probabilities, then this decision rule operates
 * as Bayes rule, returning the class \f$i\f$ if
 * \f$p(x|i) p(i) > p(x|j) p(j)\f$ for all class \f$j\f$. The
 * discriminant scores and priors are not required to be true
 * probabilities.
 *
 * This class is named the MaximumRatioDecisionRule as it can be
 * implemented as returning the class \f$i\f$ if
 * \f$\frac{p(x|i)}{p(x|j)} > \frac{p(j)}{p(i)}\f$ for all class
 * \f$j\f$.
 *
 * A priori values need to be set before calling the Evaluate
 * method. If they are not set, a uniform prior is assumed.
 *
 * \sa MaximumDecisionRule, MinimumDecisionRule
 * \ingroup ITK-Statistics
 */

class ITK_EXPORT MaximumRatioDecisionRule : public DecisionRule
{
public:
  /** Standard class typedefs */
  typedef MaximumRatioDecisionRule Self;
  typedef DecisionRule              Superclass;
  typedef SmartPointer< Self >      Pointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(MaximumRatioDecisionRule, DecisionRule);

  /** Standard New() method support */
  itkNewMacro(Self);

  /** Types for discriminant values and vectors. */
  typedef Superclass::MembershipValueType MembershipValueType;
  typedef Superclass::MembershipVectorType MembershipVectorType;

  /** Types for class identifiers. */
  typedef Superclass::ClassIdentifierType ClassIdentifierType;

  /** Types for priors and values */
  typedef MembershipValueType                      PriorProbabilityValueType;
  typedef std::vector< PriorProbabilityValueType > PriorProbabilityVectorType;
  typedef PriorProbabilityVectorType::size_type    PriorProbabilityVectorSizeType;

  /** Typdef for backward compatibility */
  typedef PriorProbabilityVectorType APrioriVectorType;

  /**
   * Evaluate the decision rule, returning the class label associated
   * with the largest ratio of discriminant score to a priori probability.
   */
  virtual ClassIdentifierType Evaluate(const MembershipVectorType & discriminantScores) const;

  /** Set the prior probabilities. Needs to be called before
   * Evaluate(). If not set, assume a uniform prior.  */
  itkSetMacro(PriorProbabilities, PriorProbabilityVectorType);

  /** Set the prior probabilities. Needs to be called before
   * Evaluate(). If not set, assume a uniform prior.  This method is
   * provided for backward compatibility. Recommended method is
   * SetPriorProbabilities() */
  void SetAPriori(const PriorProbabilityVectorType& p)
  { this->SetPriorProbabilities(p); };

protected:
  MaximumRatioDecisionRule();
  virtual ~MaximumRatioDecisionRule() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  MaximumRatioDecisionRule(const Self &); //purposely not implemented
  void operator=(const Self &);            //purposely not implemented

  PriorProbabilityVectorType m_PriorProbabilities;

};  // end of class
} // end of Statistics namespace

// Backward compatibility mechanism. MaximumRatioDecisionRule used to be in
// itk namespace. Now it is in itk::Statistics namespace. Here, we
// make it available to both.
using Statistics::MaximumRatioDecisionRule;

} // end of ITK namespace
#endif
