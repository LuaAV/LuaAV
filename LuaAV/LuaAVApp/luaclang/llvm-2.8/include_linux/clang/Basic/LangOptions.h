//===--- LangOptions.h - C Language Family Language Options -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the LangOptions interface.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LANGOPTIONS_H
#define LLVM_CLANG_LANGOPTIONS_H

#include <string>

namespace clang {

/// LangOptions - This class keeps track of the various options that can be
/// enabled, which controls the dialect of C that is accepted.
class LangOptions {
public:
  unsigned Trigraphs         : 1;  // Trigraphs in source files.
  unsigned BCPLComment       : 1;  // BCPL-style '//' comments.
  unsigned Bool              : 1;  // 'bool', 'true', 'false' keywords.
  unsigned DollarIdents      : 1;  // '$' allowed in identifiers.
  unsigned AsmPreprocessor   : 1;  // Preprocessor in asm mode.
  unsigned GNUMode           : 1;  // True in gnu99 mode false in c99 mode (etc)
  unsigned GNUKeywords       : 1;  // True if GNU-only keywords are allowed
  unsigned ImplicitInt       : 1;  // C89 implicit 'int'.
  unsigned Digraphs          : 1;  // C94, C99 and C++
  unsigned HexFloats         : 1;  // C99 Hexadecimal float constants.
  unsigned C99               : 1;  // C99 Support
  unsigned Microsoft         : 1;  // Microsoft extensions.
  unsigned Borland           : 1;  // Borland extensions.
  unsigned CPlusPlus         : 1;  // C++ Support
  unsigned CPlusPlus0x       : 1;  // C++0x Support
  unsigned CXXOperatorNames  : 1;  // Treat C++ operator names as keywords.

  unsigned ObjC1             : 1;  // Objective-C 1 support enabled.
  unsigned ObjC2             : 1;  // Objective-C 2 support enabled.
  unsigned ObjCNonFragileABI : 1;  // Objective-C modern abi enabled
  unsigned ObjCNonFragileABI2 : 1;  // Objective-C enhanced modern abi enabled

  unsigned PascalStrings     : 1;  // Allow Pascal strings
  unsigned WritableStrings   : 1;  // Allow writable strings
  unsigned ConstStrings      : 1;  // Add const qualifier to strings (-Wwrite-strings)
  unsigned LaxVectorConversions : 1;
  unsigned AltiVec           : 1;  // Support AltiVec-style vector initializers.
  unsigned Exceptions        : 1;  // Support exception handling.
  unsigned SjLjExceptions    : 1;  // Use setjmp-longjump exception handling.
  unsigned RTTI              : 1;  // Support RTTI information.

  unsigned NeXTRuntime       : 1; // Use NeXT runtime.
  unsigned Freestanding      : 1; // Freestanding implementation
  unsigned NoBuiltin         : 1; // Do not use builtin functions (-fno-builtin)

  unsigned ThreadsafeStatics : 1; // Whether static initializers are protected
                                  // by locks.
  unsigned POSIXThreads      : 1; // Compiling with POSIX thread support
                                  // (-pthread)
  unsigned Blocks            : 1; // block extension to C
  unsigned EmitAllDecls      : 1; // Emit all declarations, even if
                                  // they are unused.
  unsigned MathErrno         : 1; // Math functions must respect errno
                                  // (modulo the platform support).

  unsigned HeinousExtensions : 1; // Extensions that we really don't like and
                                  // may be ripped out at any time.

  unsigned Optimize          : 1; // Whether __OPTIMIZE__ should be defined.
  unsigned OptimizeSize      : 1; // Whether __OPTIMIZE_SIZE__ should be
                                  // defined.
  unsigned Static            : 1; // Should __STATIC__ be defined (as
                                  // opposed to __DYNAMIC__).
  unsigned PICLevel          : 2; // The value for __PIC__, if non-zero.

  unsigned GNUInline         : 1; // Should GNU inline semantics be
                                  // used (instead of C99 semantics).
  unsigned NoInline          : 1; // Should __NO_INLINE__ be defined.

  unsigned ObjCGCBitmapPrint : 1; // Enable printing of gc's bitmap layout
                                  // for __weak/__strong ivars.

  unsigned AccessControl     : 1; // Whether C++ access control should
                                  // be enabled.
  unsigned CharIsSigned      : 1; // Whether char is a signed or unsigned type
  unsigned ShortWChar        : 1; // Force wchar_t to be unsigned short int.

  unsigned OpenCL            : 1; // OpenCL C99 language extensions.
  
  unsigned AssumeSaneOperatorNew : 1; // Whether to add __attribute__((malloc))
                                      // to the declaration of C++'s new
                                      // operators
  unsigned ElideConstructors : 1; // Whether C++ copy constructors should be
                                  // elided if possible.
  unsigned CatchUndefined    : 1; // Generate code to check for undefined ops.
  unsigned DumpRecordLayouts : 1; /// Dump the layout of IRgen'd records.
  unsigned DumpVTableLayouts : 1; /// Dump the layouts of emitted vtables.
  unsigned NoConstantCFStrings : 1;  // Do not do CF strings
  unsigned InlineVisibilityHidden : 1; // Whether inline C++ methods have
                                       // hidden visibility by default.  

  unsigned SpellChecking : 1; // Whether to perform spell-checking for error
                              // recovery.
  // FIXME: This is just a temporary option, for testing purposes.
  unsigned NoBitFieldTypeAlign : 1;

private:
  // We declare multibit enums as unsigned because MSVC insists on making enums
  // signed.  Set/Query these values using accessors.
  unsigned GC : 2;                // Objective-C Garbage Collection modes.
  unsigned SymbolVisibility  : 3; // Symbol's visibility.
  unsigned StackProtector    : 2; // Whether stack protectors are on.
  unsigned SignedOverflowBehavior : 2; // How to handle signed integer overflow.

public:
  unsigned InstantiationDepth;    // Maximum template instantiation depth.

  std::string ObjCConstantStringClass;

  enum GCMode { NonGC, GCOnly, HybridGC };
  enum StackProtectorMode { SSPOff, SSPOn, SSPReq };
  enum VisibilityMode {
    Default,
    Protected,
    Hidden
  };
  
  enum SignedOverflowBehaviorTy {
    SOB_Undefined,  // Default C standard behavior.
    SOB_Defined,    // -fwrapv
    SOB_Trapping    // -ftrapv
  };

  LangOptions() {
    Trigraphs = BCPLComment = Bool = DollarIdents = AsmPreprocessor = 0;
    GNUMode = GNUKeywords = ImplicitInt = Digraphs = 0;
    HexFloats = 0;
    GC = ObjC1 = ObjC2 = ObjCNonFragileABI = ObjCNonFragileABI2 = 0;
    NoConstantCFStrings = 0; InlineVisibilityHidden = 0;
    C99 = Microsoft = Borland = CPlusPlus = CPlusPlus0x = 0;
    CXXOperatorNames = PascalStrings = WritableStrings = ConstStrings = 0;
    Exceptions = SjLjExceptions = Freestanding = NoBuiltin = 0;
    NeXTRuntime = 1;
    RTTI = 1;
    LaxVectorConversions = 1;
    HeinousExtensions = 0;
    AltiVec = OpenCL = StackProtector = 0;

    SymbolVisibility = (unsigned) Default;
      
    ThreadsafeStatics = 1;
    POSIXThreads = 0;
    Blocks = 0;
    EmitAllDecls = 0;
    MathErrno = 1;
    SignedOverflowBehavior = SOB_Undefined;
    
    AssumeSaneOperatorNew = 1;
    AccessControl = 1;
    ElideConstructors = 1;

    SignedOverflowBehavior = 0;
    ObjCGCBitmapPrint = 0;

    InstantiationDepth = 1024;

    Optimize = 0;
    OptimizeSize = 0;

    Static = 0;
    PICLevel = 0;

    GNUInline = 0;
    NoInline = 0;

    CharIsSigned = 1;
    ShortWChar = 0;
    CatchUndefined = 0;
    DumpRecordLayouts = 0;
    DumpVTableLayouts = 0;
    SpellChecking = 1;
    NoBitFieldTypeAlign = 0;
  }

  GCMode getGCMode() const { return (GCMode) GC; }
  void setGCMode(GCMode m) { GC = (unsigned) m; }

  StackProtectorMode getStackProtectorMode() const {
    return static_cast<StackProtectorMode>(StackProtector);
  }
  void setStackProtectorMode(StackProtectorMode m) {
    StackProtector = static_cast<unsigned>(m);
  }

  VisibilityMode getVisibilityMode() const {
    return (VisibilityMode) SymbolVisibility;
  }
  void setVisibilityMode(VisibilityMode v) { SymbolVisibility = (unsigned) v; }
  
  SignedOverflowBehaviorTy getSignedOverflowBehavior() const {
    return (SignedOverflowBehaviorTy)SignedOverflowBehavior;
  }
  void setSignedOverflowBehavior(SignedOverflowBehaviorTy V) {
    SignedOverflowBehavior = (unsigned)V;
  }
};

}  // end namespace clang

#endif
