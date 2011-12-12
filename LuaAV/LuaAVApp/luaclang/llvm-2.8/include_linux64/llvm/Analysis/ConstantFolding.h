//===-- ConstantFolding.h - Fold instructions into constants --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares routines for folding instructions into constants.
//
// Also, to supplement the basic VMCore ConstantExpr simplifications,
// this file declares some additional folding routines that can make use of
// TargetData information. These functions cannot go in VMCore due to library
// dependency issues.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ANALYSIS_CONSTANTFOLDING_H
#define LLVM_ANALYSIS_CONSTANTFOLDING_H

namespace llvm {
  class Constant;
  class ConstantExpr;
  class Instruction;
  class TargetData;
  class Function;
  class Type;

/// ConstantFoldInstruction - Attempt to constant fold the specified
/// instruction.  If successful, the constant result is returned, if not, null
/// is returned.  Note that this function can only fail when attempting to fold
/// instructions like loads and stores, which have no constant expression form.
///
Constant *ConstantFoldInstruction(Instruction *I, const TargetData *TD = 0);

/// ConstantFoldConstantExpression - Attempt to fold the constant expression
/// using the specified TargetData.  If successful, the constant result is
/// result is returned, if not, null is returned.
Constant *ConstantFoldConstantExpression(const ConstantExpr *CE,
                                         const TargetData *TD = 0);

/// ConstantFoldInstOperands - Attempt to constant fold an instruction with the
/// specified operands.  If successful, the constant result is returned, if not,
/// null is returned.  Note that this function can fail when attempting to 
/// fold instructions like loads and stores, which have no constant expression 
/// form.
///
Constant *ConstantFoldInstOperands(unsigned Opcode, const Type *DestTy,
                                   Constant *const *Ops, unsigned NumOps,
                                   const TargetData *TD = 0);

/// ConstantFoldCompareInstOperands - Attempt to constant fold a compare
/// instruction (icmp/fcmp) with the specified operands.  If it fails, it
/// returns a constant expression of the specified operands.
///
Constant *ConstantFoldCompareInstOperands(unsigned Predicate,
                                          Constant *LHS, Constant *RHS,
                                          const TargetData *TD = 0);

/// ConstantFoldLoadFromConstPtr - Return the value that a load from C would
/// produce if it is constant and determinable.  If this is not determinable,
/// return null.
Constant *ConstantFoldLoadFromConstPtr(Constant *C, const TargetData *TD = 0);

/// ConstantFoldLoadThroughGEPConstantExpr - Given a constant and a
/// getelementptr constantexpr, return the constant value being addressed by the
/// constant expression, or null if something is funny and we can't decide.
Constant *ConstantFoldLoadThroughGEPConstantExpr(Constant *C, ConstantExpr *CE);
  
/// canConstantFoldCallTo - Return true if its even possible to fold a call to
/// the specified function.
bool canConstantFoldCallTo(const Function *F);

/// ConstantFoldCall - Attempt to constant fold a call to the specified function
/// with the specified arguments, returning null if unsuccessful.
Constant *
ConstantFoldCall(Function *F, Constant *const *Operands, unsigned NumOperands);
}

#endif
