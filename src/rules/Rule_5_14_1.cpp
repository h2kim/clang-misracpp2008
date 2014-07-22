//===-  Rule_5_14_1.cpp - Checker for MISRA C++ 2008 rule 5-14-1-----------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
// Copyright (C) 2014 Reto Schneider <code@reto-schneider.ch>
//
//===----------------------------------------------------------------------===//

#include "misracpp2008.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/Diagnostic.h"

using namespace clang;

namespace misracpp2008 {

const static std::string ruleName = "5-14-1";

class Rule_5_14_1 : public RuleCheckerASTContext,
                    public RecursiveASTVisitor<Rule_5_14_1> {
public:
  Rule_5_14_1() : RuleCheckerASTContext() {}

  bool VisitBinLAnd(BinaryOperator *S) {
    return rightHandOperatorHasSideEffect(S);
  }
  bool VisitBinLOr(BinaryOperator *S) {
    return rightHandOperatorHasSideEffect(S);
  }

protected:
  bool rightHandOperatorHasSideEffect(BinaryOperator *S) {
    Expr *rightHandExpr = S->getRHS();
    if (rightHandExpr->HasSideEffects(*context)) {
      reportError(RULE_TEXT_5_14_1, rightHandExpr->getLocStart());
    }
    return true;
  }

  virtual void doWork() {
    RuleCheckerASTContext::doWork();
    this->TraverseDecl(context->getTranslationUnitDecl());
  }
};

static RuleCheckerASTContextRegistry::Add<Rule_5_14_1>
X(ruleName.c_str(), "MISRA C++ 2008 rule checker");
}
