//===-  BannedFunctionUsageChecker.h - Helper class -----------------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
// Copyright (C) 2014 Reto Schneider <code@reto-schneider.ch>
//
//===----------------------------------------------------------------------===//

#ifndef ILLEGAL_FUNCTION_USAGE_CHECKER_H
#define ILLEGAL_FUNCTION_USAGE_CHECKER_H

#include "misracpp2008.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Lex/Token.h"
#include <string>
#include <set>

using namespace clang;

namespace misracpp2008 {

/**
 * @brief Auxiliary for easier implementation of a checker which simply
 * checks for calls to illegal macros/functions.
 */
class BannedFunctionUsageChecker
    : public RuleCheckerASTContext,
      public RuleCheckerPPCallback,
      public RecursiveASTVisitor<BannedFunctionUsageChecker> {
public:
  /**
   * @brief Check if a referenced/used function is illegal and report an error
   * if a violation has been found.
   * @param expr Expression to be analyzed.
   * @return true
   */
  bool VisitDeclRefExpr(DeclRefExpr *expr) {
    if (doIgnore(expr->getLocation())) {
      return true;
    }

    std::string funName = expr->getNameInfo().getAsString();
    if (getIllegalFunctions().count(funName)) {
      reportRuleViolation(expr->getLocStart());
    }
    return true;
  }
  /**
   * @brief Check if a macro involves a call to a banned macro/function.
   * @param MacroNameTok Token to be analyzed.
   * @param Range Start and end location of the handled source code snipped.
   */
  virtual void MacroExpands(const Token &MacroNameTok, const MacroDirective *,
                            SourceRange Range, const MacroArgs *) {
    if (doIgnore(MacroNameTok.getLocation())) {
      return;
    }

    const std::string &macroName = MacroNameTok.getIdentifierInfo()->getName();
    if (getIllegalFunctions().count(macroName)) {
      if (doIgnore(Range.getBegin())) {
        return;
      }
      reportRuleViolation(Range.getBegin());
    }
  }

protected:
  BannedFunctionUsageChecker() {}
  virtual void doWork() override {
    RuleCheckerASTContext::doWork();
    this->TraverseDecl(context->getTranslationUnitDecl());
  }
  /**
   * @brief To be implemented by the subclass: Return a set of illegal
   * function/macro names.
   * @return A set of illegal function/macro names.
   */
  virtual const std::set<std::string> &getIllegalFunctions() const = 0;
  /**
   * @brief To be implemented by the subclass: Reporing an error at \c loc.
   * @param loc Location of the illegal function/macro call.
   */
  virtual void reportRuleViolation(clang::SourceLocation loc) = 0;
};
}

#endif
