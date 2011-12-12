//===--- TypeTraits.h - C++ Type Traits Support Enumerations ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines enumerations for the type traits support.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TYPETRAITS_H
#define LLVM_CLANG_TYPETRAITS_H

namespace clang {

  /// UnaryTypeTrait - Names for the unary type traits.
  enum UnaryTypeTrait {
    UTT_HasNothrowAssign,
    UTT_HasNothrowCopy,
    UTT_HasNothrowConstructor,
    UTT_HasTrivialAssign,
    UTT_HasTrivialCopy,
    UTT_HasTrivialConstructor,
    UTT_HasTrivialDestructor,
    UTT_HasVirtualDestructor,
    UTT_IsAbstract,
    UTT_IsClass,
    UTT_IsEmpty,
    UTT_IsEnum,
    UTT_IsPOD,
    UTT_IsPolymorphic,
    UTT_IsUnion,
    UTT_IsLiteral
  };

}

#endif
