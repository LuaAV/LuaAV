//===-- Transform/Utils/BasicBlockUtils.h - BasicBlock Utils ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This family of functions perform manipulations on basic blocks, and
// instructions contained within basic blocks.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_BASICBLOCK_H
#define LLVM_TRANSFORMS_UTILS_BASICBLOCK_H

// FIXME: Move to this file: BasicBlock::removePredecessor, BB::splitBasicBlock

#include "llvm/BasicBlock.h"
#include "llvm/Support/CFG.h"

namespace llvm {

class Instruction;
class Pass;
class AliasAnalysis;

/// DeleteDeadBlock - Delete the specified block, which must have no
/// predecessors.
void DeleteDeadBlock(BasicBlock *BB);
  
  
/// FoldSingleEntryPHINodes - We know that BB has one predecessor.  If there are
/// any single-entry PHI nodes in it, fold them away.  This handles the case
/// when all entries to the PHI nodes in a block are guaranteed equal, such as
/// when the block has exactly one predecessor.
void FoldSingleEntryPHINodes(BasicBlock *BB);

/// DeleteDeadPHIs - Examine each PHI in the given block and delete it if it
/// is dead. Also recursively delete any operands that become dead as
/// a result. This includes tracing the def-use list from the PHI to see if
/// it is ultimately unused or if it reaches an unused cycle. Return true
/// if any PHIs were deleted.
bool DeleteDeadPHIs(BasicBlock *BB);

/// MergeBlockIntoPredecessor - Attempts to merge a block into its predecessor,
/// if possible.  The return value indicates success or failure.
bool MergeBlockIntoPredecessor(BasicBlock* BB, Pass* P = 0);

// ReplaceInstWithValue - Replace all uses of an instruction (specified by BI)
// with a value, then remove and delete the original instruction.
//
void ReplaceInstWithValue(BasicBlock::InstListType &BIL,
                          BasicBlock::iterator &BI, Value *V);

// ReplaceInstWithInst - Replace the instruction specified by BI with the
// instruction specified by I.  The original instruction is deleted and BI is
// updated to point to the new instruction.
//
void ReplaceInstWithInst(BasicBlock::InstListType &BIL,
                         BasicBlock::iterator &BI, Instruction *I);

// ReplaceInstWithInst - Replace the instruction specified by From with the
// instruction specified by To.
//
void ReplaceInstWithInst(Instruction *From, Instruction *To);

/// FindFunctionBackedges - Analyze the specified function to find all of the
/// loop backedges in the function and return them.  This is a relatively cheap
/// (compared to computing dominators and loop info) analysis.
///
/// The output is added to Result, as pairs of <from,to> edge info.
void FindFunctionBackedges(const Function &F,
      SmallVectorImpl<std::pair<const BasicBlock*,const BasicBlock*> > &Result);
  

// RemoveSuccessor - Change the specified terminator instruction such that its
// successor #SuccNum no longer exists.  Because this reduces the outgoing
// degree of the current basic block, the actual terminator instruction itself
// may have to be changed.  In the case where the last successor of the block is
// deleted, a return instruction is inserted in its place which can cause a
// suprising change in program behavior if it is not expected.
//
void RemoveSuccessor(TerminatorInst *TI, unsigned SuccNum);

/// GetSuccessorNumber - Search for the specified successor of basic block BB
/// and return its position in the terminator instruction's list of
/// successors.  It is an error to call this with a block that is not a
/// successor.
unsigned GetSuccessorNumber(BasicBlock *BB, BasicBlock *Succ);

/// isCriticalEdge - Return true if the specified edge is a critical edge.
/// Critical edges are edges from a block with multiple successors to a block
/// with multiple predecessors.
///
bool isCriticalEdge(const TerminatorInst *TI, unsigned SuccNum,
                    bool AllowIdenticalEdges = false);

/// SplitCriticalEdge - If this edge is a critical edge, insert a new node to
/// split the critical edge.  This will update DominatorTree and
/// DominatorFrontier information if it is available, thus calling this pass
/// will not invalidate either of them. This returns the new block if the edge
/// was split, null otherwise.
///
/// If MergeIdenticalEdges is true (not the default), *all* edges from TI to the
/// specified successor will be merged into the same critical edge block.  
/// This is most commonly interesting with switch instructions, which may 
/// have many edges to any one destination.  This ensures that all edges to that
/// dest go to one block instead of each going to a different block, but isn't 
/// the standard definition of a "critical edge".
///
/// It is invalid to call this function on a critical edge that starts at an
/// IndirectBrInst.  Splitting these edges will almost always create an invalid
/// program because the address of the new block won't be the one that is jumped
/// to.
///
BasicBlock *SplitCriticalEdge(TerminatorInst *TI, unsigned SuccNum,
                              Pass *P = 0, bool MergeIdenticalEdges = false);

inline BasicBlock *SplitCriticalEdge(BasicBlock *BB, succ_iterator SI,
                                     Pass *P = 0) {
  return SplitCriticalEdge(BB->getTerminator(), SI.getSuccessorIndex(), P);
}

/// SplitCriticalEdge - If the edge from *PI to BB is not critical, return
/// false.  Otherwise, split all edges between the two blocks and return true.
/// This updates all of the same analyses as the other SplitCriticalEdge
/// function.  If P is specified, it updates the analyses
/// described above.
inline bool SplitCriticalEdge(BasicBlock *Succ, pred_iterator PI, Pass *P = 0) {
  bool MadeChange = false;
  TerminatorInst *TI = (*PI)->getTerminator();
  for (unsigned i = 0, e = TI->getNumSuccessors(); i != e; ++i)
    if (TI->getSuccessor(i) == Succ)
      MadeChange |= !!SplitCriticalEdge(TI, i, P);
  return MadeChange;
}

/// SplitCriticalEdge - If an edge from Src to Dst is critical, split the edge
/// and return true, otherwise return false.  This method requires that there be
/// an edge between the two blocks.  If P is specified, it updates the analyses
/// described above.
inline BasicBlock *SplitCriticalEdge(BasicBlock *Src, BasicBlock *Dst,
                                     Pass *P = 0,
                                     bool MergeIdenticalEdges = false) {
  TerminatorInst *TI = Src->getTerminator();
  unsigned i = 0;
  while (1) {
    assert(i != TI->getNumSuccessors() && "Edge doesn't exist!");
    if (TI->getSuccessor(i) == Dst)
      return SplitCriticalEdge(TI, i, P, MergeIdenticalEdges);
    ++i;
  }
}

/// SplitEdge -  Split the edge connecting specified block. Pass P must 
/// not be NULL. 
BasicBlock *SplitEdge(BasicBlock *From, BasicBlock *To, Pass *P);

/// SplitBlock - Split the specified block at the specified instruction - every
/// thing before SplitPt stays in Old and everything starting with SplitPt moves
/// to a new block.  The two blocks are joined by an unconditional branch and
/// the loop info is updated.
///
BasicBlock *SplitBlock(BasicBlock *Old, Instruction *SplitPt, Pass *P);
 
/// SplitBlockPredecessors - This method transforms BB by introducing a new
/// basic block into the function, and moving some of the predecessors of BB to
/// be predecessors of the new block.  The new predecessors are indicated by the
/// Preds array, which has NumPreds elements in it.  The new block is given a
/// suffix of 'Suffix'.  This function returns the new block.
///
/// This currently updates the LLVM IR, AliasAnalysis, DominatorTree,
/// DominanceFrontier, LoopInfo, and LCCSA but no other analyses.
/// In particular, it does not preserve LoopSimplify (because it's
/// complicated to handle the case where one of the edges being split
/// is an exit of a loop with other exits).
///
BasicBlock *SplitBlockPredecessors(BasicBlock *BB, BasicBlock *const *Preds,
                                   unsigned NumPreds, const char *Suffix,
                                   Pass *P = 0);
  
} // End llvm namespace

#endif
