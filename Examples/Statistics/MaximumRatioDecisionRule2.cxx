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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

// Software Guide : BeginLatex
// \index{itk::Statistics::Maximum\-Ratio\-Decision\-Rule}
//
// The \code{Evaluate()} method of the \doxygen{MaximumRatioDecisionRule}
// returns the index, $i$ if
// \begin{equation}
//   \frac{f_{i}(\overrightarrow{x})}{f_{j}(\overrightarrow{x})} >
//   \frac{K_{j}}{K_{i}} \textrm{ for all } j \not= i
// \end{equation}
// where the $i$ is the index of a class which has membership function
// $f_{i}$ and its prior value (usually, the \emph{a priori}
// probability of the class) is $K_{i}$
//
// We include the header files for the class as well as the header file for
// the \code{std::vector} class that will be the container for the
// discriminant scores.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "itkMaximumRatioDecisionRule.h"
#include <vector>
// Software Guide : EndCodeSnippet

int main(int, char*[])
{
  // Software Guide : BeginLatex
  //
  // The instantiation of the function is done through the usual
  // \code{New()} method and a smart pointer.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef itk::Statistics::MaximumRatioDecisionRule DecisionRuleType;
  DecisionRuleType::Pointer decisionRule = DecisionRuleType::New();
  // Software Guide : EndCodeSnippet


  // Software Guide : BeginLatex
  //
  // We create the discriminant score vector and fill it with three
  // values. We also create a vector (\code{aPrioris}) for the \emph{a
  // priori} values. The \code{Evaluate( discriminantScores )} will
  // return 1.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  std::vector< double > discriminantScores;
  discriminantScores.push_back( 0.1 );
  discriminantScores.push_back( 0.3 );
  discriminantScores.push_back( 0.6 );

  DecisionRuleType::APrioriVectorType aPrioris;
  aPrioris.push_back( 0.1 );
  aPrioris.push_back( 0.8 );
  aPrioris.push_back( 0.1 );

  decisionRule->SetAPriori( aPrioris );
  std::cout << "MaximumRatioDecisionRule: The index of the chosen = "
            << decisionRule->Evaluate( discriminantScores )
            << std::endl;
  // Software Guide : EndCodeSnippet

  return 0;
}
