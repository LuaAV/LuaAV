//===-- InstructionSimplify.h - Fold instructions into simpler forms ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares routines for folding instructions into simpler forms that
// do not require creating new instructions.  For example, this does constant
// folding, and can handle identities like (X&0)->0.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ANALYSIS_INSTRUCTIONSIMPLIFY_H
#define LLVM_ANALYSIS_INSTRUCTIONSIMPLIFY_H

namespace llvm {
  class Instruction;
  class Value;
  class TargetData;

  /// SimplifyAddInst - Given operands for an Add, see if we can
  /// fold the result.  If not, this returns null.
  Value *SimplifyAddInst(Value *LHS, Value *RHS, bool isNSW, bool isNUW,
                         const TargetData *TD = 0);
  
  /// SimplifyAndInst - Given operands for an And, see if we can
  /// fold the result.  If not, this returns null.
  Value *SimplifyAndInst(Value *LHS, Value *RHS,
                         const TargetData *TD = 0);

  /// SimplifyOrInst - Given operands for an Or, see if we can
  /// fold the result.  If not, this returns null.
  Value *SimplifyOrInst(Value *LHS, Value *RHS,
                        const TargetData *TD = 0);
  
  /// SimplifyICmpInst - Given operands for an ICmpInst, see if we can
  /// fold the result.  If not, this returns null.
  Value *SimplifyICmpInst(unsigned Predicate, Value *LHS, Value *RHS,
                          const TargetData *TD = 0);
  
  /// SimplifyFCmpInst - Given operands for an FCmpInst, see if we can
  /// fold the result.  If not, this returns null.
  Value *SimplifyFCmpInst(unsigned Predicate, Value *LHS, Value *RHS,
                          const TargetData *TD = 0);
  
  /// SimplifySelectInst - Given operands for a SelectInst, see if we can fold
  /// the result.  If not, this returns null.
  Value *SimplifySelectInst(Value *Cond, Value *TrueVal, Value *FalseVal,
                            const TargetData *TD = 0);

  /// SimplifyGEPInst - Given operands for an GetElementPtrInst, see if we can
  /// fold the result.  If not, this returns null.
  Value *SimplifyGEPInst(Value * const *Ops, unsigned NumOps,
                         const TargetData *TD = 0);
  
  //=== Helper functions for higher up the class hierarchy.
  
  
  /// SimplifyCmpInst - Given operands for a CmpInst, see if we can
  /// fold the result.  If not, this returns null.
  Value *SimplifyCmpInst(unsigned Predicate, Value *LHS, Value *RHS,
                         const TargetData *TD = 0);
  
  /// SimplifyBinOp - Given operands for a BinaryOperator, see if we can
  /// fold the result.  If not, this returns null.
  Value *SimplifyBinOp(unsigned Opcode, Value *LHS, Value *RHS, 
                       const TargetData *TD = 0);
  
  /// SimplifyInstruction - See if we can compute a simplified version of this
  /// instruction.  If not, this returns null.
  Value *SimplifyInstruction(Instruction *I, const TargetData *TD = 0);
  
  
  /// ReplaceAndSimplifyAllUses - Perform From->replaceAllUsesWith(To) and then
  /// delete the From instruction.  In addition to a basic RAUW, this does a
  /// recursive simplification of the updated instructions.  This catches
  /// things where one simplification exposes other opportunities.  This only
  /// simplifies and deletes scalar operations, it does not change the CFG.
  ///
  void ReplaceAndSimplifyAllUses(Instruction *From, Value *To,
                                 const TargetData *TD = 0);
} // end namespace llvm

#endif

