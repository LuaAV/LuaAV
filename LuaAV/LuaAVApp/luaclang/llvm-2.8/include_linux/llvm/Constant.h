//===-- llvm/Constant.h - Constant class definition -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the Constant class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CONSTANT_H
#define LLVM_CONSTANT_H

#include "llvm/User.h"

namespace llvm {
  class APInt;

  template<typename T> class SmallVectorImpl;
  class LLVMContext;

/// This is an important base class in LLVM. It provides the common facilities
/// of all constant values in an LLVM program. A constant is a value that is
/// immutable at runtime. Functions are constants because their address is
/// immutable. Same with global variables. 
/// 
/// All constants share the capabilities provided in this class. All constants
/// can have a null value. They can have an operand list. Constants can be
/// simple (integer and floating point values), complex (arrays and structures),
/// or expression based (computations yielding a constant value composed of 
/// only certain operators and other constant values).
/// 
/// Note that Constants are immutable (once created they never change) 
/// and are fully shared by structural equivalence.  This means that two 
/// structurally equivalent constants will always have the same address.  
/// Constants are created on demand as needed and never deleted: thus clients 
/// don't have to worry about the lifetime of the objects.
/// @brief LLVM Constant Representation
class Constant : public User {
  void operator=(const Constant &);     // Do not implement
  Constant(const Constant &);           // Do not implement
  
protected:
  Constant(const Type *ty, ValueTy vty, Use *Ops, unsigned NumOps)
    : User(ty, vty, Ops, NumOps) {}

  void destroyConstantImpl();
  
  void setOperand(unsigned i, Value *V) {
    User::setOperand(i, V);
  }
public:
  /// isNullValue - Return true if this is the value that would be returned by
  /// getNullValue.
  virtual bool isNullValue() const = 0;

  /// isNegativeZeroValue - Return true if the value is what would be returned 
  /// by getZeroValueForNegation.
  virtual bool isNegativeZeroValue() const { return isNullValue(); }

  /// canTrap - Return true if evaluation of this constant could trap.  This is
  /// true for things like constant expressions that could divide by zero.
  bool canTrap() const;

  /// isConstantUsed - Return true if the constant has users other than constant
  /// exprs and other dangling things.
  bool isConstantUsed() const;
  
  enum PossibleRelocationsTy {
    NoRelocation = 0,
    LocalRelocation = 1,
    GlobalRelocations = 2
  };
  
  /// getRelocationInfo - This method classifies the entry according to
  /// whether or not it may generate a relocation entry.  This must be
  /// conservative, so if it might codegen to a relocatable entry, it should say
  /// so.  The return values are:
  /// 
  ///  NoRelocation: This constant pool entry is guaranteed to never have a
  ///     relocation applied to it (because it holds a simple constant like
  ///     '4').
  ///  LocalRelocation: This entry has relocations, but the entries are
  ///     guaranteed to be resolvable by the static linker, so the dynamic
  ///     linker will never see them.
  ///  GlobalRelocations: This entry may have arbitrary relocations.
  ///
  /// FIXME: This really should not be in VMCore.
  PossibleRelocationsTy getRelocationInfo() const;
  
  // Specialize get/setOperand for Users as their operands are always
  // constants or BasicBlocks as well.
  User *getOperand(unsigned i) {
    return static_cast<User*>(User::getOperand(i));
  }
  const User *getOperand(unsigned i) const {
    return static_cast<const User*>(User::getOperand(i));
  }
  
  /// getVectorElements - This method, which is only valid on constant of vector
  /// type, returns the elements of the vector in the specified smallvector.
  /// This handles breaking down a vector undef into undef elements, etc.  For
  /// constant exprs and other cases we can't handle, we return an empty vector.
  void getVectorElements(SmallVectorImpl<Constant*> &Elts) const;

  /// destroyConstant - Called if some element of this constant is no longer
  /// valid.  At this point only other constants may be on the use_list for this
  /// constant.  Any constants on our Use list must also be destroy'd.  The
  /// implementation must be sure to remove the constant from the list of
  /// available cached constants.  Implementations should call
  /// destroyConstantImpl as the last thing they do, to destroy all users and
  /// delete this.
  virtual void destroyConstant() { assert(0 && "Not reached!"); }

  //// Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const Constant *) { return true; }
  static inline bool classof(const GlobalValue *) { return true; }
  static inline bool classof(const Value *V) {
    return V->getValueID() >= ConstantFirstVal &&
           V->getValueID() <= ConstantLastVal;
  }

  /// replaceUsesOfWithOnConstant - This method is a special form of
  /// User::replaceUsesOfWith (which does not work on constants) that does work
  /// on constants.  Basically this method goes through the trouble of building
  /// a new constant that is equivalent to the current one, with all uses of
  /// From replaced with uses of To.  After this construction is completed, all
  /// of the users of 'this' are replaced to use the new constant, and then
  /// 'this' is deleted.  In general, you should not call this method, instead,
  /// use Value::replaceAllUsesWith, which automatically dispatches to this
  /// method as needed.
  ///
  virtual void replaceUsesOfWithOnConstant(Value *, Value *, Use *) {
    // Provide a default implementation for constants (like integers) that
    // cannot use any other values.  This cannot be called at runtime, but needs
    // to be here to avoid link errors.
    assert(getNumOperands() == 0 && "replaceUsesOfWithOnConstant must be "
           "implemented for all constants that have operands!");
    assert(0 && "Constants that do not have operands cannot be using 'From'!");
  }
  
  static Constant* getNullValue(const Type* Ty);
  
  /// @returns the value for an integer constant of the given type that has all
  /// its bits set to true.
  /// @brief Get the all ones value
  static Constant* getAllOnesValue(const Type* Ty);

  /// getIntegerValue - Return the value for an integer or pointer constant,
  /// or a vector thereof, with the given scalar value.
  static Constant* getIntegerValue(const Type* Ty, const APInt &V);
};

} // End llvm namespace

#endif
