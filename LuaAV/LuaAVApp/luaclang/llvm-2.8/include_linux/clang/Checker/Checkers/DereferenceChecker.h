//== NullDerefChecker.h - Null dereference checker --------------*- C++ -*--==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This defines NullDerefChecker and UndefDerefChecker, two builtin checks
// in GRExprEngine that check for null and undefined pointers at loads
// and stores.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_DEREFCHECKER
#define LLVM_CLANG_DEREFCHECKER

#include <utility>

namespace clang {

class GRExprEngine;
class ExplodedNode;

std::pair<ExplodedNode * const *, ExplodedNode * const *>
GetImplicitNullDereferences(GRExprEngine &Eng);

} // end clang namespace

#endif
