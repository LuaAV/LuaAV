//===-- llvm/ADT/Triple.h - Target triple helper class ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ADT_TRIPLE_H
#define LLVM_ADT_TRIPLE_H

#include "llvm/ADT/StringRef.h"
#include <string>

// Some system headers or GCC predefined macros conflict with identifiers in
// this file.  Undefine them here.
#undef mips
#undef sparc

namespace llvm {
class StringRef;
class Twine;

/// Triple - Helper class for working with target triples.
///
/// Target triples are strings in the canonical form:
///   ARCHITECTURE-VENDOR-OPERATING_SYSTEM
/// or
///   ARCHITECTURE-VENDOR-OPERATING_SYSTEM-ENVIRONMENT
///
/// This class is used for clients which want to support arbitrary
/// target triples, but also want to implement certain special
/// behavior for particular targets. This class isolates the mapping
/// from the components of the target triple to well known IDs.
///
/// At its core the Triple class is designed to be a wrapper for a triple
/// string; the constructor does not change or normalize the triple string.
/// Clients that need to handle the non-canonical triples that users often
/// specify should use the normalize method.
///
/// See autoconf/config.guess for a glimpse into what triples look like in
/// practice.
class Triple {
public:
  enum ArchType {
    UnknownArch,
    
    alpha,   // Alpha: alpha
    arm,     // ARM; arm, armv.*, xscale
    bfin,    // Blackfin: bfin
    cellspu, // CellSPU: spu, cellspu
    mips,    // MIPS: mips, mipsallegrex
    mipsel,  // MIPSEL: mipsel, mipsallegrexel, psp
    msp430,  // MSP430: msp430
    pic16,   // PIC16: pic16
    ppc,     // PPC: powerpc
    ppc64,   // PPC64: powerpc64, ppu
    sparc,   // Sparc: sparc
    sparcv9, // Sparcv9: Sparcv9
    systemz, // SystemZ: s390x
    tce,     // TCE (http://tce.cs.tut.fi/): tce
    thumb,   // Thumb: thumb, thumbv.*
    x86,     // X86: i[3-9]86
    x86_64,  // X86-64: amd64, x86_64
    xcore,   // XCore: xcore
    mblaze,  // MBlaze: mblaze

    InvalidArch
  };
  enum VendorType {
    UnknownVendor,

    Apple, 
    PC
  };
  enum OSType {
    UnknownOS,

    AuroraUX,
    Cygwin,
    Darwin,
    DragonFly,
    FreeBSD,
    Linux,
    Lv2,        // PS3
    MinGW32,
    MinGW64,
    NetBSD,
    OpenBSD,
    Psp,
    Solaris,
    Win32,
    Haiku,
    Minix
  };
  
private:
  std::string Data;

  /// The parsed arch type (or InvalidArch if uninitialized).
  mutable ArchType Arch;

  /// The parsed vendor type.
  mutable VendorType Vendor;

  /// The parsed OS type.
  mutable OSType OS;

  bool isInitialized() const { return Arch != InvalidArch; }
  static ArchType ParseArch(StringRef ArchName);
  static VendorType ParseVendor(StringRef VendorName);
  static OSType ParseOS(StringRef OSName);
  void Parse() const;

public:
  /// @name Constructors
  /// @{
  
  Triple() : Data(), Arch(InvalidArch) {}
  explicit Triple(StringRef Str) : Data(Str), Arch(InvalidArch) {}
  explicit Triple(StringRef ArchStr, StringRef VendorStr, StringRef OSStr)
    : Data(ArchStr), Arch(InvalidArch) {
    Data += '-';
    Data += VendorStr;
    Data += '-';
    Data += OSStr;
  }

  /// @}
  /// @name Normalization
  /// @{

  /// normalize - Turn an arbitrary machine specification into the canonical
  /// triple form (or something sensible that the Triple class understands if
  /// nothing better can reasonably be done).  In particular, it handles the
  /// common case in which otherwise valid components are in the wrong order.
  static std::string normalize(StringRef Str);

  /// @}
  /// @name Typed Component Access
  /// @{
  
  /// getArch - Get the parsed architecture type of this triple.
  ArchType getArch() const { 
    if (!isInitialized()) Parse(); 
    return Arch;
  }
  
  /// getVendor - Get the parsed vendor type of this triple.
  VendorType getVendor() const { 
    if (!isInitialized()) Parse(); 
    return Vendor;
  }
  
  /// getOS - Get the parsed operating system type of this triple.
  OSType getOS() const { 
    if (!isInitialized()) Parse(); 
    return OS;
  }

  /// hasEnvironment - Does this triple have the optional environment
  /// (fourth) component?
  bool hasEnvironment() const {
    return getEnvironmentName() != "";
  }

  /// @}
  /// @name Direct Component Access
  /// @{

  const std::string &str() const { return Data; }

  const std::string &getTriple() const { return Data; }

  /// getArchName - Get the architecture (first) component of the
  /// triple.
  StringRef getArchName() const;

  /// getVendorName - Get the vendor (second) component of the triple.
  StringRef getVendorName() const;

  /// getOSName - Get the operating system (third) component of the
  /// triple.
  StringRef getOSName() const;

  /// getEnvironmentName - Get the optional environment (fourth)
  /// component of the triple, or "" if empty.
  StringRef getEnvironmentName() const;

  /// getOSAndEnvironmentName - Get the operating system and optional
  /// environment components as a single string (separated by a '-'
  /// if the environment component is present).
  StringRef getOSAndEnvironmentName() const;

  
  /// getDarwinNumber - Parse the 'darwin number' out of the specific target
  /// triple.  For example, if we have darwin8.5 return 8,5,0.  If any entry is
  /// not defined, return 0's.  This requires that the triple have an OSType of
  /// darwin before it is called.
  void getDarwinNumber(unsigned &Maj, unsigned &Min, unsigned &Revision) const;
  
  /// getDarwinMajorNumber - Return just the major version number, this is
  /// specialized because it is a common query.
  unsigned getDarwinMajorNumber() const {
    unsigned Maj, Min, Rev;
    getDarwinNumber(Maj, Min, Rev);
    return Maj;
  }
  
  /// @}
  /// @name Mutators
  /// @{

  /// setArch - Set the architecture (first) component of the triple
  /// to a known type.
  void setArch(ArchType Kind);

  /// setVendor - Set the vendor (second) component of the triple to a
  /// known type.
  void setVendor(VendorType Kind);

  /// setOS - Set the operating system (third) component of the triple
  /// to a known type.
  void setOS(OSType Kind);

  /// setTriple - Set all components to the new triple \arg Str.
  void setTriple(const Twine &Str);

  /// setArchName - Set the architecture (first) component of the
  /// triple by name.
  void setArchName(StringRef Str);

  /// setVendorName - Set the vendor (second) component of the triple
  /// by name.
  void setVendorName(StringRef Str);

  /// setOSName - Set the operating system (third) component of the
  /// triple by name.
  void setOSName(StringRef Str);

  /// setEnvironmentName - Set the optional environment (fourth)
  /// component of the triple by name.
  void setEnvironmentName(StringRef Str);

  /// setOSAndEnvironmentName - Set the operating system and optional
  /// environment components with a single string.
  void setOSAndEnvironmentName(StringRef Str);

  /// getArchNameForAssembler - Get an architecture name that is understood by
  /// the target assembler.
  const char *getArchNameForAssembler();

  /// @}
  /// @name Static helpers for IDs.
  /// @{

  /// getArchTypeName - Get the canonical name for the \arg Kind
  /// architecture.
  static const char *getArchTypeName(ArchType Kind);

  /// getArchTypePrefix - Get the "prefix" canonical name for the \arg Kind
  /// architecture. This is the prefix used by the architecture specific
  /// builtins, and is suitable for passing to \see
  /// Intrinsic::getIntrinsicForGCCBuiltin().
  ///
  /// \return - The architecture prefix, or 0 if none is defined.
  static const char *getArchTypePrefix(ArchType Kind);

  /// getVendorTypeName - Get the canonical name for the \arg Kind
  /// vendor.
  static const char *getVendorTypeName(VendorType Kind);

  /// getOSTypeName - Get the canonical name for the \arg Kind vendor.
  static const char *getOSTypeName(OSType Kind);

  /// @}
  /// @name Static helpers for converting alternate architecture names.
  /// @{

  /// getArchTypeForLLVMName - The canonical type for the given LLVM
  /// architecture name (e.g., "x86").
  static ArchType getArchTypeForLLVMName(StringRef Str);

  /// getArchTypeForDarwinArchName - Get the architecture type for a "Darwin"
  /// architecture name, for example as accepted by "gcc -arch" (see also
  /// arch(3)).
  static ArchType getArchTypeForDarwinArchName(StringRef Str);

  /// @}
};

} // End llvm namespace


#endif
