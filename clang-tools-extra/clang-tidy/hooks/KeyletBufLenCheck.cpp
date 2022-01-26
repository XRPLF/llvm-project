//===--- KeyletBufLenCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "KeyletBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void KeyletBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("util_keylet"))),
	     hasArgument(1, expr().bind("keyletSize")),
	     hasArgument(2, expr().bind("keyletType")),
	     hasArgument(3, expr().bind("para")),
	     hasArgument(4, expr().bind("parb")),
	     hasArgument(5, expr().bind("parc")),
	     hasArgument(6, expr().bind("pard")),
	     hasArgument(7, expr().bind("pare")),
	     hasArgument(8, expr().bind("parf")));

  Finder->addMatcher(CallExpr, this);
}

void KeyletBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const char *KeyletTypeNames[] =
    {
     0, "KEYLET_HOOK", "KEYLET_HOOK_STATE", "KEYLET_ACCOUNT", "KEYLET_AMENDMENTS",
     "KEYLET_CHILD", "KEYLET_SKIP", "KEYLET_FEES", "KEYLET_NEGATIVE_UNL", "KEYLET_LINE",
     "KEYLET_OFFER", "KEYLET_QUALITY", "KEYLET_EMITTED_DIR", "KEYLET_TICKET", "KEYLET_SIGNERS",
     "KEYLET_CHECK", "KEYLET_DEPOSIT_PREAUTH", "KEYLET_UNCHECKED", "KEYLET_OWNER_DIR", "KEYLET_PAGE",
     "KEYLET_ESCROW", "KEYLET_PAYCHAN", "KEYLET_EMITTED"
    };
  const char *KeyletLineName[] = { "high account ID", "low account ID", "currency code" };

  const Expr *KeyletSize = Result.Nodes.getNodeAs<Expr>("keyletSize");
  const Expr *KeyletType = Result.Nodes.getNodeAs<Expr>("keyletType");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> KeyletSizeValue = KeyletSize->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> KeyletTypeValue = KeyletType->getIntegerConstantExpr(Context);

  if (KeyletSizeValue && (*KeyletSizeValue < KEYLET_SIZE)) {
    diag(KeyletSize->getBeginLoc(), "output buffer of util_keylet needs %0 bytes for the keylet") << KEYLET_SIZE;
  }

  if (KeyletTypeValue) {
    // less strict than rippled source, accomodating KEYLET_EMITTED
    if ((*KeyletTypeValue < 1) || (*KeyletTypeValue > 22)) {
      SmallVector<char, 2> B;
      KeyletTypeValue->toString(B);
      std::string S(B.begin(), B.end());
      diag(KeyletType->getBeginLoc(), "invalid keylet type %0") << S;
    } else {
      const Expr *GenPar[6];
      SmallVector<Optional<llvm::APSInt>, 6> GenVal;
      char NameBuffer[5] = "par\0"; // ends w/ 2 0s
      for (size_t i = 0; i < 6; ++i) {
	NameBuffer[3] = static_cast<char>('a' + i);
	GenPar[i] = Result.Nodes.getNodeAs<Expr>(NameBuffer);
	assert(GenPar[i]);
	GenVal.emplace_back(GenPar[i]->getIntegerConstantExpr(Context));
      }

      size_t Type = static_cast<size_t>(KeyletTypeValue->getExtValue());
      switch (Type) {
	case 1: // KEYLET_HOOK
	case 3: // KEYLET_ACCOUNT
	case 14: // KEYLET_SIGNERS
	case 18: // KEYLET_OWNER_DIR
	  if (GenVal[1] && (GenVal[1]->getExtValue() != ACCOUNT_ID_SIZE)) {
	    diag(GenPar[1]->getBeginLoc(), "account ID of keylet type '%0' must have %1 bytes") <<
	      KeyletTypeNames[Type] << ACCOUNT_ID_SIZE;
	  }

	  for (size_t i = 2; i < 6; ++i) {
	    if (GenVal[i] && (GenVal[i]->getExtValue() != 0)) {
	      diag(GenPar[i]->getBeginLoc(), "last 4 generic parameters of keylet type '%0' must be 0") << KeyletTypeNames[Type];
	    }
	  }

	  break;

	case 2: // KEYLET_HOOK_STATE
	  if (GenVal[1] && (GenVal[1]->getExtValue() != ACCOUNT_ID_SIZE)) {
	    diag(GenPar[1]->getBeginLoc(), "account ID of keylet type 'KEYLET_HOOK_STATE' must have %0 bytes") << ACCOUNT_ID_SIZE;
	  }

	  if (GenVal[3] && (GenVal[3]->getExtValue() != KEY_SIZE)) {
	    diag(GenPar[3]->getBeginLoc(), "state key of keylet type 'KEYLET_HOOK_STATE' must have %0 bytes") << KEY_SIZE;
	  }

	  for (size_t i = 4; i < 6; ++i) {
	    if (GenVal[i] && (GenVal[i]->getExtValue() != 0)) {
	      diag(GenPar[i]->getBeginLoc(), "last 2 generic parameters of keylet type 'KEYLET_HOOK_STATE' must be 0");
	    }
	  }

	  break;

	case 4: // KEYLET_AMENDMENTS
	case 7: // KEYLET_FEES
	case 8: // KEYLET_NEGATIVE_UNL
	case 12: // KEYLET_EMITTED_DIR
	  for (size_t i = 0; i < 6; ++i) {
	    if (GenVal[i] && (GenVal[i]->getExtValue() != 0)) {
	      diag(GenPar[i]->getBeginLoc(), "generic parameters of keylet type '%0' must be 0") << KeyletTypeNames[Type];
	    }
	  }

	  break;

	case 5: // KEYLET_CHILD
	case 17: // KEYLET_UNCHECKED
	case 22: // KEYLET_EMITTED
	  if (GenVal[1] && (GenVal[1]->getExtValue() != KEY_SIZE)) {
	    diag(GenPar[1]->getBeginLoc(), "key of keylet type '%0' must have %1 bytes") <<
	      KeyletTypeNames[Type] << KEY_SIZE;
	  }

	  for (size_t i = 2; i < 6; ++i) {
	    if (GenVal[i] && (GenVal[i]->getExtValue() != 0)) {
	      diag(GenPar[i]->getBeginLoc(), "last 4 generic parameters of keylet type '%0' must be 0") << KeyletTypeNames[Type];
	    }
	  }

	  break;

	case 6: // KEYLET_SKIP
	  for (size_t i = 2; i < 6; ++i) {
	    if (GenVal[i] && (GenVal[i]->getExtValue() != 0)) {
	      diag(GenPar[i]->getBeginLoc(), "last 4 generic parameters of keylet type 'KEYLET_SKIP' must be 0");
	    }
	  }

	  break;

	case 9: // KEYLET_LINE
	  for (size_t j = 0; j < 3; ++j) {
	    size_t i = 2 * j + 1;
	    if (GenVal[i] && (GenVal[i]->getExtValue() != 20)) { // ACCOUNT_ID_SIZE, or Currency Code size
	      diag(GenPar[i]->getBeginLoc(), "%0 of keylet type 'KEYLET_LINE' must have 20 bytes") << KeyletLineName[j];
	    }
	  }

	  break;

	case 10: // KEYLET_OFFER
	case 15: // KEYLET_CHECK
	case 20: // KEYLET_ESCROW
	  if (GenVal[1] && (GenVal[1]->getExtValue() != ACCOUNT_ID_SIZE)) {
	    diag(GenPar[1]->getBeginLoc(), "account ID of keylet type '%0' must have %1 bytes") <<
	      KeyletTypeNames[Type] <<
	      ACCOUNT_ID_SIZE;
	  }

	  for (size_t i = 3; i < 6; ++i) {
	    if (GenVal[i] && (GenVal[i]->getExtValue() != 0)) {
	      diag(GenPar[i]->getBeginLoc(), "last 3 generic parameters of keylet type '%0' must be 0") << KeyletTypeNames[Type];
	    }
	  }

	  break;

	case 11: // KEYLET_QUALITY
	  if (GenVal[1] && (GenVal[1]->getExtValue() != KEYLET_SIZE)) {
	    diag(GenPar[1]->getBeginLoc(), "keylet of keylet type 'KEYLET_QUALITY' must have %0 bytes") << KEYLET_SIZE;
	  }

	  for (size_t i = 4; i < 6; ++i) {
	    if (GenVal[i] && (GenVal[i]->getExtValue() != 0)) {
	      diag(GenPar[i]->getBeginLoc(), "last 2 generic parameters of keylet type 'KEYLET_QUALITY' must be 0");
	    }
	  }

	  break;

	case 16: // KEYLET_DEPOSIT_PREAUTH
	  for (size_t j = 0; j < 2; ++j) {
	    size_t i = 2 * j + 1;
	    if (GenVal[i] && (GenVal[i]->getExtValue() != ACCOUNT_ID_SIZE)) {
	      diag(GenPar[i]->getBeginLoc(), "account ID of keylet type 'KEYLET_DEPOSIT_PREAUTH' must have %0 bytes") << ACCOUNT_ID_SIZE;
	    }
	  }

	  for (size_t i = 4; i < 6; ++i) {
	    if (GenVal[i] && (GenVal[i]->getExtValue() != 0)) {
	      diag(GenPar[i]->getBeginLoc(), "last 2 generic parameters of keylet type 'KEYLET_DEPOSIT_PREAUTH' must be 0");
	    }
	  }

	  break;

	case 19: // KEYLET_PAGE
	  if (GenVal[1] && (GenVal[1]->getExtValue() != KEY_SIZE)) {
	    diag(GenPar[1]->getBeginLoc(), "key of keylet type 'KEYLET_PAGE' must have %0 bytes") << KEY_SIZE;
	  }

	  for (size_t i = 4; i < 6; ++i) {
	    if (GenVal[i] && (GenVal[i]->getExtValue() != 0)) {
	      diag(GenPar[i]->getBeginLoc(), "last 2 generic parameters of keylet type 'KEYLET_PAGE' must be 0");
	    }
	  }

	  break;

	case 21: // KEYLET_PAYCHAN
	  for (size_t j = 0; j < 2; ++j) {
	    size_t i = 2 * j + 1;
	    if (GenVal[i] && (GenVal[i]->getExtValue() != ACCOUNT_ID_SIZE)) {
	      diag(GenPar[i]->getBeginLoc(), "account ID of keylet type 'KEYLET_PAYCHAN' must have %0 bytes") << ACCOUNT_ID_SIZE;
	    }
	  }

	  if (GenVal[5] && (GenVal[5]->getExtValue() != 0)) {
	    diag(GenPar[5]->getBeginLoc(), "last generic parameter of keylet type 'KEYLET_PAYCHAN' must be 0");
	  }

	  break;
      }
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
