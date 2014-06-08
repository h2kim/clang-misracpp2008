//===-  Rule_18_4_1.cpp - Checker for MISRA C++ 2008 rule 18-4-1-----------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "misracpp2008.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/Diagnostic.h"

using namespace clang;

namespace misracpp2008 {

const static std::string ruleName = "18-4-1";

class Rule_18_4_1 : public RuleCheckerASTContext,
                    public RecursiveASTVisitor<Rule_18_4_1> {
public:
  Rule_18_4_1() : RuleCheckerASTContext() {}
  bool VisitCXXNewExpr(CXXNewExpr *decl) {
    if (doIgnore(decl->getStartLoc())) {
      return true;
    }

    bool doesNotThrow = decl->shouldNullCheckAllocation(*context);
    if (doesNotThrow) {
      // Iterate over children and try to figure out if this new expr
      // looks like a placement new which can be used legally.
      Stmt::child_range c = decl->children();
      for (StmtIterator it = c.first, iE = c.second; it != iE; ++it) {
        if (CastExpr *castExpr = dyn_cast<CastExpr>(*it)) {
          if (castExpr->getCastKind() == CK_BitCast) {
            // Looks legit. Bail out without generating an error.
            return true;
          }
        }
      }
    }

    // This new expr does not look like a placement new. Generate an error.
    reportError("Dynamic heap memory allocation shall not be used.",
                decl->getLocStart());
    return true;
  }

protected:
  virtual void doWork() {
    RuleCheckerASTContext::doWork();
    this->TraverseDecl(context->getTranslationUnitDecl());
  }
};

static RuleCheckerASTContextRegistry::Add<Rule_18_4_1>
X(ruleName.c_str(), "MISRA C++ 2008 rule checker");
}
