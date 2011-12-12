//===--- DeclSpec.h - Parsed declaration specifiers -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the classes used to store parsed information about
// declaration-specifiers and declarators.
//
//   static const int volatile x, *y, *(*(*z)[10])(const void *x);
//   ------------------------- -  --  ---------------------------
//     declaration-specifiers  \  |   /
//                            declarators
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_SEMA_DECLSPEC_H
#define LLVM_CLANG_SEMA_DECLSPEC_H

#include "clang/Sema/AttributeList.h"
#include "clang/Sema/Ownership.h"
#include "clang/Lex/Token.h"
#include "clang/Basic/OperatorKinds.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/SmallVector.h"

namespace clang {
  class LangOptions;
  class Diagnostic;
  class IdentifierInfo;
  class NestedNameSpecifier;
  class Preprocessor;
  class Declarator;
  struct TemplateIdAnnotation;

/// CXXScopeSpec - Represents a C++ nested-name-specifier or a global scope
/// specifier.  These can be in 3 states:
///   1) Not present, identified by isEmpty()
///   2) Present, identified by isNotEmpty()
///      2.a) Valid, idenified by isValid()
///      2.b) Invalid, identified by isInvalid().
///
/// isSet() is deprecated because it mostly corresponded to "valid" but was
/// often used as if it meant "present".
///
/// The actual scope is described by getScopeRep().
class CXXScopeSpec {
  SourceRange Range;
  NestedNameSpecifier *ScopeRep;

public:
  CXXScopeSpec() : Range(), ScopeRep() { }

  const SourceRange &getRange() const { return Range; }
  void setRange(const SourceRange &R) { Range = R; }
  void setBeginLoc(SourceLocation Loc) { Range.setBegin(Loc); }
  void setEndLoc(SourceLocation Loc) { Range.setEnd(Loc); }
  SourceLocation getBeginLoc() const { return Range.getBegin(); }
  SourceLocation getEndLoc() const { return Range.getEnd(); }

  NestedNameSpecifier *getScopeRep() const { return ScopeRep; }
  void setScopeRep(NestedNameSpecifier *S) { ScopeRep = S; }

  /// No scope specifier.
  bool isEmpty() const { return !Range.isValid(); }
  /// A scope specifier is present, but may be valid or invalid.
  bool isNotEmpty() const { return !isEmpty(); }

  /// An error occured during parsing of the scope specifier.
  bool isInvalid() const { return isNotEmpty() && ScopeRep == 0; }
  /// A scope specifier is present, and it refers to a real scope.
  bool isValid() const { return isNotEmpty() && ScopeRep != 0; }

  /// Deprecated.  Some call sites intend isNotEmpty() while others intend
  /// isValid().
  bool isSet() const { return ScopeRep != 0; }

  void clear() {
    Range = SourceRange();
    ScopeRep = 0;
  }
};

/// DeclSpec - This class captures information about "declaration specifiers",
/// which encompasses storage-class-specifiers, type-specifiers,
/// type-qualifiers, and function-specifiers.
class DeclSpec {
public:
  // storage-class-specifier
  // Note: The order of these enumerators is important for diagnostics.
  enum SCS {
    SCS_unspecified = 0,
    SCS_typedef,
    SCS_extern,
    SCS_static,
    SCS_auto,
    SCS_register,
    SCS_private_extern,
    SCS_mutable
  };

  // Import type specifier width enumeration and constants.
  typedef TypeSpecifierWidth TSW;
  static const TSW TSW_unspecified = clang::TSW_unspecified;
  static const TSW TSW_short = clang::TSW_short;
  static const TSW TSW_long = clang::TSW_long;
  static const TSW TSW_longlong = clang::TSW_longlong;
  
  enum TSC {
    TSC_unspecified,
    TSC_imaginary,
    TSC_complex
  };

  // Import type specifier sign enumeration and constants.
  typedef TypeSpecifierSign TSS;
  static const TSS TSS_unspecified = clang::TSS_unspecified;
  static const TSS TSS_signed = clang::TSS_signed;
  static const TSS TSS_unsigned = clang::TSS_unsigned;

  // Import type specifier type enumeration and constants.
  typedef TypeSpecifierType TST;
  static const TST TST_unspecified = clang::TST_unspecified;
  static const TST TST_void = clang::TST_void;
  static const TST TST_char = clang::TST_char;
  static const TST TST_wchar = clang::TST_wchar;
  static const TST TST_char16 = clang::TST_char16;
  static const TST TST_char32 = clang::TST_char32;
  static const TST TST_int = clang::TST_int;
  static const TST TST_float = clang::TST_float;
  static const TST TST_double = clang::TST_double;
  static const TST TST_bool = clang::TST_bool;
  static const TST TST_decimal32 = clang::TST_decimal32;
  static const TST TST_decimal64 = clang::TST_decimal64;
  static const TST TST_decimal128 = clang::TST_decimal128;
  static const TST TST_enum = clang::TST_enum;
  static const TST TST_union = clang::TST_union;
  static const TST TST_struct = clang::TST_struct;
  static const TST TST_class = clang::TST_class;
  static const TST TST_typename = clang::TST_typename;
  static const TST TST_typeofType = clang::TST_typeofType;
  static const TST TST_typeofExpr = clang::TST_typeofExpr;
  static const TST TST_decltype = clang::TST_decltype;
  static const TST TST_auto = clang::TST_auto;
  static const TST TST_error = clang::TST_error;

  // type-qualifiers
  enum TQ {   // NOTE: These flags must be kept in sync with Qualifiers::TQ.
    TQ_unspecified = 0,
    TQ_const       = 1,
    TQ_restrict    = 2,
    TQ_volatile    = 4
  };

  /// ParsedSpecifiers - Flags to query which specifiers were applied.  This is
  /// returned by getParsedSpecifiers.
  enum ParsedSpecifiers {
    PQ_None                  = 0,
    PQ_StorageClassSpecifier = 1,
    PQ_TypeSpecifier         = 2,
    PQ_TypeQualifier         = 4,
    PQ_FunctionSpecifier     = 8
  };

private:

  // storage-class-specifier
  /*SCS*/unsigned StorageClassSpec : 3;
  bool SCS_thread_specified : 1;
  bool SCS_extern_in_linkage_spec : 1;

  // type-specifier
  /*TSW*/unsigned TypeSpecWidth : 2;
  /*TSC*/unsigned TypeSpecComplex : 2;
  /*TSS*/unsigned TypeSpecSign : 2;
  /*TST*/unsigned TypeSpecType : 5;
  bool TypeAltiVecVector : 1;
  bool TypeAltiVecPixel : 1;
  bool TypeAltiVecBool : 1;
  bool TypeSpecOwned : 1;

  // type-qualifiers
  unsigned TypeQualifiers : 3;  // Bitwise OR of TQ.

  // function-specifier
  bool FS_inline_specified : 1;
  bool FS_virtual_specified : 1;
  bool FS_explicit_specified : 1;

  // friend-specifier
  bool Friend_specified : 1;

  // constexpr-specifier
  bool Constexpr_specified : 1;

  /*SCS*/unsigned StorageClassSpecAsWritten : 3;

  union {
    UnionParsedType TypeRep;
    Decl *DeclRep;
    Expr *ExprRep;
  };

  // attributes.
  AttributeList *AttrList;

  // Scope specifier for the type spec, if applicable.
  CXXScopeSpec TypeScope;

  // List of protocol qualifiers for objective-c classes.  Used for
  // protocol-qualified interfaces "NString<foo>" and protocol-qualified id
  // "id<foo>".
  Decl * const *ProtocolQualifiers;
  unsigned NumProtocolQualifiers;
  SourceLocation ProtocolLAngleLoc;
  SourceLocation *ProtocolLocs;

  // SourceLocation info.  These are null if the item wasn't specified or if
  // the setting was synthesized.
  SourceRange Range;

  SourceLocation StorageClassSpecLoc, SCS_threadLoc;
  SourceLocation TSWLoc, TSCLoc, TSSLoc, TSTLoc, AltiVecLoc;
  SourceRange TypeofParensRange;
  SourceLocation TQ_constLoc, TQ_restrictLoc, TQ_volatileLoc;
  SourceLocation FS_inlineLoc, FS_virtualLoc, FS_explicitLoc;
  SourceLocation FriendLoc, ConstexprLoc;

  WrittenBuiltinSpecs writtenBS;
  void SaveWrittenBuiltinSpecs();
  void SaveStorageSpecifierAsWritten();

  static bool isTypeRep(TST T) {
    return (T == TST_typename || T == TST_typeofType);
  }
  static bool isExprRep(TST T) {
    return (T == TST_typeofExpr || T == TST_decltype);
  }
  static bool isDeclRep(TST T) {
    return (T == TST_enum || T == TST_struct ||
            T == TST_union || T == TST_class);
  }

  DeclSpec(const DeclSpec&);       // DO NOT IMPLEMENT
  void operator=(const DeclSpec&); // DO NOT IMPLEMENT
public:

  DeclSpec()
    : StorageClassSpec(SCS_unspecified),
      SCS_thread_specified(false),
      SCS_extern_in_linkage_spec(false),
      TypeSpecWidth(TSW_unspecified),
      TypeSpecComplex(TSC_unspecified),
      TypeSpecSign(TSS_unspecified),
      TypeSpecType(TST_unspecified),
      TypeAltiVecVector(false),
      TypeAltiVecPixel(false),
      TypeAltiVecBool(false),
      TypeSpecOwned(false),
      TypeQualifiers(TSS_unspecified),
      FS_inline_specified(false),
      FS_virtual_specified(false),
      FS_explicit_specified(false),
      Friend_specified(false),
      Constexpr_specified(false),
      StorageClassSpecAsWritten(SCS_unspecified),
      AttrList(0),
      ProtocolQualifiers(0),
      NumProtocolQualifiers(0),
      ProtocolLocs(0),
      writtenBS() {
  }
  ~DeclSpec() {
    delete AttrList;
    delete [] ProtocolQualifiers;
    delete [] ProtocolLocs;
  }
  // storage-class-specifier
  SCS getStorageClassSpec() const { return (SCS)StorageClassSpec; }
  bool isThreadSpecified() const { return SCS_thread_specified; }
  bool isExternInLinkageSpec() const { return SCS_extern_in_linkage_spec; }
  void setExternInLinkageSpec(bool Value) {
    SCS_extern_in_linkage_spec = Value;
  }

  SourceLocation getStorageClassSpecLoc() const { return StorageClassSpecLoc; }
  SourceLocation getThreadSpecLoc() const { return SCS_threadLoc; }

  void ClearStorageClassSpecs() {
    StorageClassSpec     = DeclSpec::SCS_unspecified;
    SCS_thread_specified = false;
    SCS_extern_in_linkage_spec = false;
    StorageClassSpecLoc  = SourceLocation();
    SCS_threadLoc        = SourceLocation();
  }

  // type-specifier
  TSW getTypeSpecWidth() const { return (TSW)TypeSpecWidth; }
  TSC getTypeSpecComplex() const { return (TSC)TypeSpecComplex; }
  TSS getTypeSpecSign() const { return (TSS)TypeSpecSign; }
  TST getTypeSpecType() const { return (TST)TypeSpecType; }
  bool isTypeAltiVecVector() const { return TypeAltiVecVector; }
  bool isTypeAltiVecPixel() const { return TypeAltiVecPixel; }
  bool isTypeAltiVecBool() const { return TypeAltiVecBool; }
  bool isTypeSpecOwned() const { return TypeSpecOwned; }
  ParsedType getRepAsType() const {
    assert(isTypeRep((TST) TypeSpecType) && "DeclSpec does not store a type");
    return TypeRep;
  }
  Decl *getRepAsDecl() const {
    assert(isDeclRep((TST) TypeSpecType) && "DeclSpec does not store a decl");
    return DeclRep;
  }
  Expr *getRepAsExpr() const {
    assert(isExprRep((TST) TypeSpecType) && "DeclSpec does not store an expr");
    return ExprRep;
  }
  CXXScopeSpec &getTypeSpecScope() { return TypeScope; }
  const CXXScopeSpec &getTypeSpecScope() const { return TypeScope; }

  const SourceRange &getSourceRange() const { return Range; }
  SourceLocation getTypeSpecWidthLoc() const { return TSWLoc; }
  SourceLocation getTypeSpecComplexLoc() const { return TSCLoc; }
  SourceLocation getTypeSpecSignLoc() const { return TSSLoc; }
  SourceLocation getTypeSpecTypeLoc() const { return TSTLoc; }
  SourceLocation getAltiVecLoc() const { return AltiVecLoc; }

  SourceRange getTypeofParensRange() const { return TypeofParensRange; }
  void setTypeofParensRange(SourceRange range) { TypeofParensRange = range; }

  /// getSpecifierName - Turn a type-specifier-type into a string like "_Bool"
  /// or "union".
  static const char *getSpecifierName(DeclSpec::TST T);
  static const char *getSpecifierName(DeclSpec::TQ Q);
  static const char *getSpecifierName(DeclSpec::TSS S);
  static const char *getSpecifierName(DeclSpec::TSC C);
  static const char *getSpecifierName(DeclSpec::TSW W);
  static const char *getSpecifierName(DeclSpec::SCS S);

  // type-qualifiers

  /// getTypeQualifiers - Return a set of TQs.
  unsigned getTypeQualifiers() const { return TypeQualifiers; }
  SourceLocation getConstSpecLoc() const { return TQ_constLoc; }
  SourceLocation getRestrictSpecLoc() const { return TQ_restrictLoc; }
  SourceLocation getVolatileSpecLoc() const { return TQ_volatileLoc; }

  // function-specifier
  bool isInlineSpecified() const { return FS_inline_specified; }
  SourceLocation getInlineSpecLoc() const { return FS_inlineLoc; }

  bool isVirtualSpecified() const { return FS_virtual_specified; }
  SourceLocation getVirtualSpecLoc() const { return FS_virtualLoc; }

  bool isExplicitSpecified() const { return FS_explicit_specified; }
  SourceLocation getExplicitSpecLoc() const { return FS_explicitLoc; }

  void ClearFunctionSpecs() {
    FS_inline_specified = false;
    FS_inlineLoc = SourceLocation();
    FS_virtual_specified = false;
    FS_virtualLoc = SourceLocation();
    FS_explicit_specified = false;
    FS_explicitLoc = SourceLocation();
  }

  /// hasTypeSpecifier - Return true if any type-specifier has been found.
  bool hasTypeSpecifier() const {
    return getTypeSpecType() != DeclSpec::TST_unspecified ||
           getTypeSpecWidth() != DeclSpec::TSW_unspecified ||
           getTypeSpecComplex() != DeclSpec::TSC_unspecified ||
           getTypeSpecSign() != DeclSpec::TSS_unspecified;
  }

  /// getParsedSpecifiers - Return a bitmask of which flavors of specifiers this
  /// DeclSpec includes.
  ///
  unsigned getParsedSpecifiers() const;

  SCS getStorageClassSpecAsWritten() const {
    return (SCS)StorageClassSpecAsWritten;
  }

  /// isEmpty - Return true if this declaration specifier is completely empty:
  /// no tokens were parsed in the production of it.
  bool isEmpty() const {
    return getParsedSpecifiers() == DeclSpec::PQ_None;
  }

  void SetRangeStart(SourceLocation Loc) { Range.setBegin(Loc); }
  void SetRangeEnd(SourceLocation Loc) { Range.setEnd(Loc); }

  /// These methods set the specified attribute of the DeclSpec and
  /// return false if there was no error.  If an error occurs (for
  /// example, if we tried to set "auto" on a spec with "extern"
  /// already set), they return true and set PrevSpec and DiagID
  /// such that
  ///   Diag(Loc, DiagID) << PrevSpec;
  /// will yield a useful result.
  ///
  /// TODO: use a more general approach that still allows these
  /// diagnostics to be ignored when desired.
  bool SetStorageClassSpec(SCS S, SourceLocation Loc, const char *&PrevSpec,
                           unsigned &DiagID);
  bool SetStorageClassSpecThread(SourceLocation Loc, const char *&PrevSpec,
                                 unsigned &DiagID);
  bool SetTypeSpecWidth(TSW W, SourceLocation Loc, const char *&PrevSpec,
                        unsigned &DiagID);
  bool SetTypeSpecComplex(TSC C, SourceLocation Loc, const char *&PrevSpec,
                          unsigned &DiagID);
  bool SetTypeSpecSign(TSS S, SourceLocation Loc, const char *&PrevSpec,
                       unsigned &DiagID);
  bool SetTypeSpecType(TST T, SourceLocation Loc, const char *&PrevSpec,
                       unsigned &DiagID);
  bool SetTypeSpecType(TST T, SourceLocation Loc, const char *&PrevSpec,
                       unsigned &DiagID, ParsedType Rep);
  bool SetTypeSpecType(TST T, SourceLocation Loc, const char *&PrevSpec,
                       unsigned &DiagID, Decl *Rep, bool Owned);
  bool SetTypeSpecType(TST T, SourceLocation Loc, const char *&PrevSpec,
                       unsigned &DiagID, Expr *Rep);
  bool SetTypeAltiVecVector(bool isAltiVecVector, SourceLocation Loc,
                       const char *&PrevSpec, unsigned &DiagID);
  bool SetTypeAltiVecPixel(bool isAltiVecPixel, SourceLocation Loc,
                       const char *&PrevSpec, unsigned &DiagID);
  bool SetTypeSpecError();
  void UpdateDeclRep(Decl *Rep) {
    assert(isDeclRep((TST) TypeSpecType));
    DeclRep = Rep;
  }
  void UpdateTypeRep(ParsedType Rep) {
    assert(isTypeRep((TST) TypeSpecType));
    TypeRep = Rep;
  }
  void UpdateExprRep(Expr *Rep) {
    assert(isExprRep((TST) TypeSpecType));
    ExprRep = Rep;
  }

  bool SetTypeQual(TQ T, SourceLocation Loc, const char *&PrevSpec,
                   unsigned &DiagID, const LangOptions &Lang);

  bool SetFunctionSpecInline(SourceLocation Loc, const char *&PrevSpec,
                             unsigned &DiagID);
  bool SetFunctionSpecVirtual(SourceLocation Loc, const char *&PrevSpec,
                              unsigned &DiagID);
  bool SetFunctionSpecExplicit(SourceLocation Loc, const char *&PrevSpec,
                               unsigned &DiagID);

  bool SetFriendSpec(SourceLocation Loc, const char *&PrevSpec,
                     unsigned &DiagID);

  bool SetConstexprSpec(SourceLocation Loc, const char *&PrevSpec,
                        unsigned &DiagID);

  bool isFriendSpecified() const { return Friend_specified; }
  SourceLocation getFriendSpecLoc() const { return FriendLoc; }

  bool isConstexprSpecified() const { return Constexpr_specified; }
  SourceLocation getConstexprSpecLoc() const { return ConstexprLoc; }

  /// AddAttributes - contatenates two attribute lists.
  /// The GCC attribute syntax allows for the following:
  ///
  /// short __attribute__(( unused, deprecated ))
  /// int __attribute__(( may_alias, aligned(16) )) var;
  ///
  /// This declares 4 attributes using 2 lists. The following syntax is
  /// also allowed and equivalent to the previous declaration.
  ///
  /// short __attribute__((unused)) __attribute__((deprecated))
  /// int __attribute__((may_alias)) __attribute__((aligned(16))) var;
  ///
  void AddAttributes(AttributeList *alist) {
    AttrList = addAttributeLists(AttrList, alist);
  }
  void SetAttributes(AttributeList *AL) { AttrList = AL; }
  const AttributeList *getAttributes() const { return AttrList; }
  AttributeList *getAttributes() { return AttrList; }

  /// TakeAttributes - Return the current attribute list and remove them from
  /// the DeclSpec so that it doesn't own them.
  AttributeList *TakeAttributes() {
    AttributeList *AL = AttrList;
    AttrList = 0;
    return AL;
  }

  typedef Decl * const *ProtocolQualifierListTy;
  ProtocolQualifierListTy getProtocolQualifiers() const {
    return ProtocolQualifiers;
  }
  SourceLocation *getProtocolLocs() const { return ProtocolLocs; }
  unsigned getNumProtocolQualifiers() const {
    return NumProtocolQualifiers;
  }
  SourceLocation getProtocolLAngleLoc() const { return ProtocolLAngleLoc; }
  void setProtocolQualifiers(Decl * const *Protos, unsigned NP,
                             SourceLocation *ProtoLocs,
                             SourceLocation LAngleLoc);

  /// Finish - This does final analysis of the declspec, issuing diagnostics for
  /// things like "_Imaginary" (lacking an FP type).  After calling this method,
  /// DeclSpec is guaranteed self-consistent, even if an error occurred.
  void Finish(Diagnostic &D, Preprocessor &PP);

  const WrittenBuiltinSpecs& getWrittenBuiltinSpecs() const {
    return writtenBS;
  }

  /// isMissingDeclaratorOk - This checks if this DeclSpec can stand alone,
  /// without a Declarator. Only tag declspecs can stand alone.
  bool isMissingDeclaratorOk();
};

/// ObjCDeclSpec - This class captures information about
/// "declaration specifiers" specific to objective-c
class ObjCDeclSpec {
public:
  /// ObjCDeclQualifier - Qualifier used on types in method declarations
  enum ObjCDeclQualifier {
    DQ_None = 0x0,
    DQ_In = 0x1,
    DQ_Inout = 0x2,
    DQ_Out = 0x4,
    DQ_Bycopy = 0x8,
    DQ_Byref = 0x10,
    DQ_Oneway = 0x20
  };

  /// PropertyAttributeKind - list of property attributes.
  enum ObjCPropertyAttributeKind { DQ_PR_noattr = 0x0,
    DQ_PR_readonly = 0x01,
    DQ_PR_getter = 0x02,
    DQ_PR_assign = 0x04,
    DQ_PR_readwrite = 0x08,
    DQ_PR_retain = 0x10,
    DQ_PR_copy = 0x20,
    DQ_PR_nonatomic = 0x40,
    DQ_PR_setter = 0x80
  };


  ObjCDeclSpec()
    : objcDeclQualifier(DQ_None), PropertyAttributes(DQ_PR_noattr),
      GetterName(0), SetterName(0) { }
  ObjCDeclQualifier getObjCDeclQualifier() const { return objcDeclQualifier; }
  void setObjCDeclQualifier(ObjCDeclQualifier DQVal) {
    objcDeclQualifier = (ObjCDeclQualifier) (objcDeclQualifier | DQVal);
  }

  ObjCPropertyAttributeKind getPropertyAttributes() const {
    return ObjCPropertyAttributeKind(PropertyAttributes);
  }
  void setPropertyAttributes(ObjCPropertyAttributeKind PRVal) {
    PropertyAttributes =
      (ObjCPropertyAttributeKind)(PropertyAttributes | PRVal);
  }

  const IdentifierInfo *getGetterName() const { return GetterName; }
  IdentifierInfo *getGetterName() { return GetterName; }
  void setGetterName(IdentifierInfo *name) { GetterName = name; }

  const IdentifierInfo *getSetterName() const { return SetterName; }
  IdentifierInfo *getSetterName() { return SetterName; }
  void setSetterName(IdentifierInfo *name) { SetterName = name; }
private:
  // FIXME: These two are unrelated and mutially exclusive. So perhaps
  // we can put them in a union to reflect their mutual exclusiveness
  // (space saving is negligible).
  ObjCDeclQualifier objcDeclQualifier : 6;

  // NOTE: VC++ treats enums as signed, avoid using ObjCPropertyAttributeKind
  unsigned PropertyAttributes : 8;
  IdentifierInfo *GetterName;    // getter name of NULL if no getter
  IdentifierInfo *SetterName;    // setter name of NULL if no setter
};

/// \brief Represents a C++ unqualified-id that has been parsed. 
class UnqualifiedId {
private:
  const UnqualifiedId &operator=(const UnqualifiedId &); // DO NOT IMPLEMENT
  
public:
  /// \brief Describes the kind of unqualified-id parsed.
  enum IdKind {
    /// \brief An identifier.
    IK_Identifier,
    /// \brief An overloaded operator name, e.g., operator+.
    IK_OperatorFunctionId,
    /// \brief A conversion function name, e.g., operator int.
    IK_ConversionFunctionId,
    /// \brief A user-defined literal name, e.g., operator "" _i.
    IK_LiteralOperatorId,
    /// \brief A constructor name.
    IK_ConstructorName,
    /// \brief A constructor named via a template-id.
    IK_ConstructorTemplateId,
    /// \brief A destructor name.
    IK_DestructorName,
    /// \brief A template-id, e.g., f<int>.
    IK_TemplateId
  } Kind;

  /// \brief Anonymous union that holds extra data associated with the
  /// parsed unqualified-id.
  union {
    /// \brief When Kind == IK_Identifier, the parsed identifier, or when Kind
    /// == IK_UserLiteralId, the identifier suffix.
    IdentifierInfo *Identifier;
    
    /// \brief When Kind == IK_OperatorFunctionId, the overloaded operator
    /// that we parsed.
    struct {
      /// \brief The kind of overloaded operator.
      OverloadedOperatorKind Operator;
      
      /// \brief The source locations of the individual tokens that name
      /// the operator, e.g., the "new", "[", and "]" tokens in 
      /// operator new []. 
      ///
      /// Different operators have different numbers of tokens in their name,
      /// up to three. Any remaining source locations in this array will be
      /// set to an invalid value for operators with fewer than three tokens.
      unsigned SymbolLocations[3];
    } OperatorFunctionId;
    
    /// \brief When Kind == IK_ConversionFunctionId, the type that the 
    /// conversion function names.
    UnionParsedType ConversionFunctionId;

    /// \brief When Kind == IK_ConstructorName, the class-name of the type
    /// whose constructor is being referenced.
    UnionParsedType ConstructorName;
    
    /// \brief When Kind == IK_DestructorName, the type referred to by the
    /// class-name.
    UnionParsedType DestructorName;
    
    /// \brief When Kind == IK_TemplateId or IK_ConstructorTemplateId,
    /// the template-id annotation that contains the template name and
    /// template arguments.
    TemplateIdAnnotation *TemplateId;
  };
  
  /// \brief The location of the first token that describes this unqualified-id,
  /// which will be the location of the identifier, "operator" keyword,
  /// tilde (for a destructor), or the template name of a template-id.
  SourceLocation StartLocation;
  
  /// \brief The location of the last token that describes this unqualified-id.
  SourceLocation EndLocation;
  
  UnqualifiedId() : Kind(IK_Identifier), Identifier(0) { }
  
  /// \brief Do not use this copy constructor. It is temporary, and only
  /// exists because we are holding FieldDeclarators in a SmallVector when we
  /// don't actually need them.
  ///
  /// FIXME: Kill this copy constructor.
  UnqualifiedId(const UnqualifiedId &Other) 
    : Kind(IK_Identifier), Identifier(Other.Identifier), 
      StartLocation(Other.StartLocation), EndLocation(Other.EndLocation) {
    assert(Other.Kind == IK_Identifier && "Cannot copy non-identifiers");
  }

  /// \brief Destroy this unqualified-id.
  ~UnqualifiedId() { clear(); }
  
  /// \brief Clear out this unqualified-id, setting it to default (invalid) 
  /// state.
  void clear();
  
  /// \brief Determine whether this unqualified-id refers to a valid name.
  bool isValid() const { return StartLocation.isValid(); }

  /// \brief Determine whether this unqualified-id refers to an invalid name.
  bool isInvalid() const { return !isValid(); }
  
  /// \brief Determine what kind of name we have.
  IdKind getKind() const { return Kind; }
  
  /// \brief Specify that this unqualified-id was parsed as an identifier.
  ///
  /// \param Id the parsed identifier.
  /// \param IdLoc the location of the parsed identifier.
  void setIdentifier(const IdentifierInfo *Id, SourceLocation IdLoc) {
    Kind = IK_Identifier;
    Identifier = const_cast<IdentifierInfo *>(Id);
    StartLocation = EndLocation = IdLoc;
  }
  
  /// \brief Specify that this unqualified-id was parsed as an 
  /// operator-function-id.
  ///
  /// \param OperatorLoc the location of the 'operator' keyword.
  ///
  /// \param Op the overloaded operator.
  ///
  /// \param SymbolLocations the locations of the individual operator symbols
  /// in the operator.
  void setOperatorFunctionId(SourceLocation OperatorLoc, 
                             OverloadedOperatorKind Op,
                             SourceLocation SymbolLocations[3]);
  
  /// \brief Specify that this unqualified-id was parsed as a 
  /// conversion-function-id.
  ///
  /// \param OperatorLoc the location of the 'operator' keyword.
  ///
  /// \param Ty the type to which this conversion function is converting.
  ///
  /// \param EndLoc the location of the last token that makes up the type name.
  void setConversionFunctionId(SourceLocation OperatorLoc, 
                               ParsedType Ty,
                               SourceLocation EndLoc) {
    Kind = IK_ConversionFunctionId;
    StartLocation = OperatorLoc;
    EndLocation = EndLoc;
    ConversionFunctionId = Ty;
  }

  /// \brief Specific that this unqualified-id was parsed as a
  /// literal-operator-id.
  ///
  /// \param Id the parsed identifier.
  ///
  /// \param OpLoc the location of the 'operator' keyword.
  ///
  /// \param IdLoc the location of the identifier.
  void setLiteralOperatorId(const IdentifierInfo *Id, SourceLocation OpLoc,
                              SourceLocation IdLoc) {
    Kind = IK_LiteralOperatorId;
    Identifier = const_cast<IdentifierInfo *>(Id);
    StartLocation = OpLoc;
    EndLocation = IdLoc;
  }
  
  /// \brief Specify that this unqualified-id was parsed as a constructor name.
  ///
  /// \param ClassType the class type referred to by the constructor name.
  ///
  /// \param ClassNameLoc the location of the class name.
  ///
  /// \param EndLoc the location of the last token that makes up the type name.
  void setConstructorName(ParsedType ClassType, 
                          SourceLocation ClassNameLoc,
                          SourceLocation EndLoc) {
    Kind = IK_ConstructorName;
    StartLocation = ClassNameLoc;
    EndLocation = EndLoc;
    ConstructorName = ClassType;
  }

  /// \brief Specify that this unqualified-id was parsed as a
  /// template-id that names a constructor.
  ///
  /// \param TemplateId the template-id annotation that describes the parsed
  /// template-id. This UnqualifiedId instance will take ownership of the
  /// \p TemplateId and will free it on destruction.
  void setConstructorTemplateId(TemplateIdAnnotation *TemplateId);

  /// \brief Specify that this unqualified-id was parsed as a destructor name.
  ///
  /// \param TildeLoc the location of the '~' that introduces the destructor
  /// name.
  ///
  /// \param ClassType the name of the class referred to by the destructor name.
  void setDestructorName(SourceLocation TildeLoc,
                         ParsedType ClassType,
                         SourceLocation EndLoc) {
    Kind = IK_DestructorName;
    StartLocation = TildeLoc;
    EndLocation = EndLoc;
    DestructorName = ClassType;
  }
  
  /// \brief Specify that this unqualified-id was parsed as a template-id.
  ///
  /// \param TemplateId the template-id annotation that describes the parsed
  /// template-id. This UnqualifiedId instance will take ownership of the
  /// \p TemplateId and will free it on destruction.
  void setTemplateId(TemplateIdAnnotation *TemplateId);

  /// \brief Return the source range that covers this unqualified-id.
  SourceRange getSourceRange() const { 
    return SourceRange(StartLocation, EndLocation); 
  }
};
  
/// CachedTokens - A set of tokens that has been cached for later
/// parsing.
typedef llvm::SmallVector<Token, 4> CachedTokens;

/// DeclaratorChunk - One instance of this struct is used for each type in a
/// declarator that is parsed.
///
/// This is intended to be a small value object.
struct DeclaratorChunk {
  enum {
    Pointer, Reference, Array, Function, BlockPointer, MemberPointer
  } Kind;

  /// Loc - The place where this type was defined.
  SourceLocation Loc;
  /// EndLoc - If valid, the place where this chunck ends.
  SourceLocation EndLoc;

  struct PointerTypeInfo {
    /// The type qualifiers: const/volatile/restrict.
    unsigned TypeQuals : 3;
    AttributeList *AttrList;
    void destroy() {
      delete AttrList;
    }
  };

  struct ReferenceTypeInfo {
    /// The type qualifier: restrict. [GNU] C++ extension
    bool HasRestrict : 1;
    /// True if this is an lvalue reference, false if it's an rvalue reference.
    bool LValueRef : 1;
    AttributeList *AttrList;
    void destroy() {
      delete AttrList;
    }
  };

  struct ArrayTypeInfo {
    /// The type qualifiers for the array: const/volatile/restrict.
    unsigned TypeQuals : 3;

    /// True if this dimension included the 'static' keyword.
    bool hasStatic : 1;

    /// True if this dimension was [*].  In this case, NumElts is null.
    bool isStar : 1;

    /// This is the size of the array, or null if [] or [*] was specified.
    /// Since the parser is multi-purpose, and we don't want to impose a root
    /// expression class on all clients, NumElts is untyped.
    Expr *NumElts;
    void destroy() {}
  };

  /// ParamInfo - An array of paraminfo objects is allocated whenever a function
  /// declarator is parsed.  There are two interesting styles of arguments here:
  /// K&R-style identifier lists and parameter type lists.  K&R-style identifier
  /// lists will have information about the identifier, but no type information.
  /// Parameter type lists will have type info (if the actions module provides
  /// it), but may have null identifier info: e.g. for 'void foo(int X, int)'.
  struct ParamInfo {
    IdentifierInfo *Ident;
    SourceLocation IdentLoc;
    Decl *Param;

    /// DefaultArgTokens - When the parameter's default argument
    /// cannot be parsed immediately (because it occurs within the
    /// declaration of a member function), it will be stored here as a
    /// sequence of tokens to be parsed once the class definition is
    /// complete. Non-NULL indicates that there is a default argument.
    CachedTokens *DefaultArgTokens;

    ParamInfo() {}
    ParamInfo(IdentifierInfo *ident, SourceLocation iloc,
              Decl *param,
              CachedTokens *DefArgTokens = 0)
      : Ident(ident), IdentLoc(iloc), Param(param),
        DefaultArgTokens(DefArgTokens) {}
  };

  struct TypeAndRange {
    ParsedType Ty;
    SourceRange Range;
  };

  struct FunctionTypeInfo {
    /// hasPrototype - This is true if the function had at least one typed
    /// argument.  If the function is () or (a,b,c), then it has no prototype,
    /// and is treated as a K&R-style function.
    bool hasPrototype : 1;

    /// isVariadic - If this function has a prototype, and if that
    /// proto ends with ',...)', this is true. When true, EllipsisLoc
    /// contains the location of the ellipsis.
    bool isVariadic : 1;

    /// The type qualifiers: const/volatile/restrict.
    /// The qualifier bitmask values are the same as in QualType.
    unsigned TypeQuals : 3;

    /// hasExceptionSpec - True if the function has an exception specification.
    bool hasExceptionSpec : 1;

    /// hasAnyExceptionSpec - True if the function has a throw(...) specifier.
    bool hasAnyExceptionSpec : 1;

    /// DeleteArgInfo - If this is true, we need to delete[] ArgInfo.
    bool DeleteArgInfo : 1;

    /// When isVariadic is true, the location of the ellipsis in the source.
    unsigned EllipsisLoc;

    /// NumArgs - This is the number of formal arguments provided for the
    /// declarator.
    unsigned NumArgs;

    /// NumExceptions - This is the number of types in the exception-decl, if
    /// the function has one.
    unsigned NumExceptions;

    /// ThrowLoc - When hasExceptionSpec is true, the location of the throw
    /// keyword introducing the spec.
    unsigned ThrowLoc;

    /// ArgInfo - This is a pointer to a new[]'d array of ParamInfo objects that
    /// describe the arguments for this function declarator.  This is null if
    /// there are no arguments specified.
    ParamInfo *ArgInfo;

    /// Exceptions - This is a pointer to a new[]'d array of TypeAndRange
    /// objects that contain the types in the function's exception
    /// specification and their locations.
    TypeAndRange *Exceptions;

    /// freeArgs - reset the argument list to having zero arguments.  This is
    /// used in various places for error recovery.
    void freeArgs() {
      if (DeleteArgInfo) {
        delete[] ArgInfo;
        DeleteArgInfo = false;
      }
      NumArgs = 0;
    }

    void destroy() {
      if (DeleteArgInfo)
        delete[] ArgInfo;
      delete[] Exceptions;
    }

    /// isKNRPrototype - Return true if this is a K&R style identifier list,
    /// like "void foo(a,b,c)".  In a function definition, this will be followed
    /// by the argument type definitions.
    bool isKNRPrototype() const {
      return !hasPrototype && NumArgs != 0;
    }
    
    SourceLocation getEllipsisLoc() const {
      return SourceLocation::getFromRawEncoding(EllipsisLoc);
    }
    SourceLocation getThrowLoc() const {
      return SourceLocation::getFromRawEncoding(ThrowLoc);
    }
  };

  struct BlockPointerTypeInfo {
    /// For now, sema will catch these as invalid.
    /// The type qualifiers: const/volatile/restrict.
    unsigned TypeQuals : 3;
    AttributeList *AttrList;
    void destroy() {
      delete AttrList;
    }
  };

  struct MemberPointerTypeInfo {
    /// The type qualifiers: const/volatile/restrict.
    unsigned TypeQuals : 3;
    AttributeList *AttrList;
    // CXXScopeSpec has a constructor, so it can't be a direct member.
    // So we need some pointer-aligned storage and a bit of trickery.
    union {
      void *Aligner;
      char Mem[sizeof(CXXScopeSpec)];
    } ScopeMem;
    CXXScopeSpec &Scope() {
      return *reinterpret_cast<CXXScopeSpec*>(ScopeMem.Mem);
    }
    const CXXScopeSpec &Scope() const {
      return *reinterpret_cast<const CXXScopeSpec*>(ScopeMem.Mem);
    }
    void destroy() {
      delete AttrList;
      Scope().~CXXScopeSpec();
    }
  };

  union {
    PointerTypeInfo       Ptr;
    ReferenceTypeInfo     Ref;
    ArrayTypeInfo         Arr;
    FunctionTypeInfo      Fun;
    BlockPointerTypeInfo  Cls;
    MemberPointerTypeInfo Mem;
  };

  void destroy() {
    switch (Kind) {
    default: assert(0 && "Unknown decl type!");
    case DeclaratorChunk::Function:      return Fun.destroy();
    case DeclaratorChunk::Pointer:       return Ptr.destroy();
    case DeclaratorChunk::BlockPointer:  return Cls.destroy();
    case DeclaratorChunk::Reference:     return Ref.destroy();
    case DeclaratorChunk::Array:         return Arr.destroy();
    case DeclaratorChunk::MemberPointer: return Mem.destroy();
    }
  }

  /// getAttrs - If there are attributes applied to this declaratorchunk, return
  /// them.
  const AttributeList *getAttrs() const {
    switch (Kind) {
    default: assert(0 && "Unknown declarator kind!");
    case Pointer:       return Ptr.AttrList;
    case Reference:     return Ref.AttrList;
    case MemberPointer: return Mem.AttrList;
    case Array:         return 0;
    case Function:      return 0;
    case BlockPointer:  return Cls.AttrList;
    }
  }


  /// getPointer - Return a DeclaratorChunk for a pointer.
  ///
  static DeclaratorChunk getPointer(unsigned TypeQuals, SourceLocation Loc,
                                    AttributeList *AL) {
    DeclaratorChunk I;
    I.Kind          = Pointer;
    I.Loc           = Loc;
    I.Ptr.TypeQuals = TypeQuals;
    I.Ptr.AttrList  = AL;
    return I;
  }

  /// getReference - Return a DeclaratorChunk for a reference.
  ///
  static DeclaratorChunk getReference(unsigned TypeQuals, SourceLocation Loc,
                                      AttributeList *AL, bool lvalue) {
    DeclaratorChunk I;
    I.Kind            = Reference;
    I.Loc             = Loc;
    I.Ref.HasRestrict = (TypeQuals & DeclSpec::TQ_restrict) != 0;
    I.Ref.LValueRef   = lvalue;
    I.Ref.AttrList  = AL;
    return I;
  }

  /// getArray - Return a DeclaratorChunk for an array.
  ///
  static DeclaratorChunk getArray(unsigned TypeQuals, bool isStatic,
                                  bool isStar, Expr *NumElts,
                                  SourceLocation LBLoc, SourceLocation RBLoc) {
    DeclaratorChunk I;
    I.Kind          = Array;
    I.Loc           = LBLoc;
    I.EndLoc        = RBLoc;
    I.Arr.TypeQuals = TypeQuals;
    I.Arr.hasStatic = isStatic;
    I.Arr.isStar    = isStar;
    I.Arr.NumElts   = NumElts;
    return I;
  }

  /// DeclaratorChunk::getFunction - Return a DeclaratorChunk for a function.
  /// "TheDeclarator" is the declarator that this will be added to.
  static DeclaratorChunk getFunction(bool hasProto, bool isVariadic,
                                     SourceLocation EllipsisLoc,
                                     ParamInfo *ArgInfo, unsigned NumArgs,
                                     unsigned TypeQuals, bool hasExceptionSpec,
                                     SourceLocation ThrowLoc,
                                     bool hasAnyExceptionSpec,
                                     ParsedType *Exceptions,
                                     SourceRange *ExceptionRanges,
                                     unsigned NumExceptions,
                                     SourceLocation LPLoc, SourceLocation RPLoc,
                                     Declarator &TheDeclarator);

  /// getBlockPointer - Return a DeclaratorChunk for a block.
  ///
  static DeclaratorChunk getBlockPointer(unsigned TypeQuals, SourceLocation Loc,
                                         AttributeList *AL) {
    DeclaratorChunk I;
    I.Kind          = BlockPointer;
    I.Loc           = Loc;
    I.Cls.TypeQuals = TypeQuals;
    I.Cls.AttrList  = AL;
    return I;
  }

  static DeclaratorChunk getMemberPointer(const CXXScopeSpec &SS,
                                          unsigned TypeQuals,
                                          SourceLocation Loc,
                                          AttributeList *AL) {
    DeclaratorChunk I;
    I.Kind          = MemberPointer;
    I.Loc           = Loc;
    I.Mem.TypeQuals = TypeQuals;
    I.Mem.AttrList  = AL;
    new (I.Mem.ScopeMem.Mem) CXXScopeSpec(SS);
    return I;
  }
};

/// Declarator - Information about one declarator, including the parsed type
/// information and the identifier.  When the declarator is fully formed, this
/// is turned into the appropriate Decl object.
///
/// Declarators come in two types: normal declarators and abstract declarators.
/// Abstract declarators are used when parsing types, and don't have an
/// identifier.  Normal declarators do have ID's.
///
/// Instances of this class should be a transient object that lives on the
/// stack, not objects that are allocated in large quantities on the heap.
class Declarator {
public:
  enum TheContext {
    FileContext,         // File scope declaration.
    PrototypeContext,    // Within a function prototype.
    KNRTypeListContext,  // K&R type definition list for formals.
    TypeNameContext,     // Abstract declarator for types.
    MemberContext,       // Struct/Union field.
    BlockContext,        // Declaration within a block in a function.
    ForContext,          // Declaration within first part of a for loop.
    ConditionContext,    // Condition declaration in a C++ if/switch/while/for.
    TemplateParamContext,// Within a template parameter list.
    CXXCatchContext,     // C++ catch exception-declaration
    BlockLiteralContext  // Block literal declarator.
  };

private:
  const DeclSpec &DS;
  CXXScopeSpec SS;
  UnqualifiedId Name;
  SourceRange Range;

  /// Context - Where we are parsing this declarator.
  ///
  TheContext Context;

  /// DeclTypeInfo - This holds each type that the declarator includes as it is
  /// parsed.  This is pushed from the identifier out, which means that element
  /// #0 will be the most closely bound to the identifier, and
  /// DeclTypeInfo.back() will be the least closely bound.
  llvm::SmallVector<DeclaratorChunk, 8> DeclTypeInfo;

  /// InvalidType - Set by Sema::GetTypeForDeclarator().
  bool InvalidType : 1;

  /// GroupingParens - Set by Parser::ParseParenDeclarator().
  bool GroupingParens : 1;

  /// AttrList - Attributes.
  AttributeList *AttrList;

  /// AsmLabel - The asm label, if specified.
  Expr *AsmLabel;

  /// InlineParams - This is a local array used for the first function decl
  /// chunk to avoid going to the heap for the common case when we have one
  /// function chunk in the declarator.
  DeclaratorChunk::ParamInfo InlineParams[16];
  bool InlineParamsUsed;

  /// Extension - true if the declaration is preceded by __extension__.
  bool Extension : 1;

  friend struct DeclaratorChunk;

public:
  Declarator(const DeclSpec &ds, TheContext C)
    : DS(ds), Range(ds.getSourceRange()), Context(C),
      InvalidType(DS.getTypeSpecType() == DeclSpec::TST_error),
      GroupingParens(false), AttrList(0), AsmLabel(0),
      InlineParamsUsed(false), Extension(false) {
  }

  ~Declarator() {
    clear();
  }

  /// getDeclSpec - Return the declaration-specifier that this declarator was
  /// declared with.
  const DeclSpec &getDeclSpec() const { return DS; }

  /// getMutableDeclSpec - Return a non-const version of the DeclSpec.  This
  /// should be used with extreme care: declspecs can often be shared between
  /// multiple declarators, so mutating the DeclSpec affects all of the
  /// Declarators.  This should only be done when the declspec is known to not
  /// be shared or when in error recovery etc.
  DeclSpec &getMutableDeclSpec() { return const_cast<DeclSpec &>(DS); }

  /// getCXXScopeSpec - Return the C++ scope specifier (global scope or
  /// nested-name-specifier) that is part of the declarator-id.
  const CXXScopeSpec &getCXXScopeSpec() const { return SS; }
  CXXScopeSpec &getCXXScopeSpec() { return SS; }

  /// \brief Retrieve the name specified by this declarator.
  UnqualifiedId &getName() { return Name; }
  
  TheContext getContext() const { return Context; }

  /// getSourceRange - Get the source range that spans this declarator.
  const SourceRange &getSourceRange() const { return Range; }

  void SetSourceRange(SourceRange R) { Range = R; }
  /// SetRangeBegin - Set the start of the source range to Loc, unless it's
  /// invalid.
  void SetRangeBegin(SourceLocation Loc) {
    if (!Loc.isInvalid())
      Range.setBegin(Loc);
  }
  /// SetRangeEnd - Set the end of the source range to Loc, unless it's invalid.
  void SetRangeEnd(SourceLocation Loc) {
    if (!Loc.isInvalid())
      Range.setEnd(Loc);
  }
  /// ExtendWithDeclSpec - Extend the declarator source range to include the
  /// given declspec, unless its location is invalid. Adopts the range start if
  /// the current range start is invalid.
  void ExtendWithDeclSpec(const DeclSpec &DS) {
    const SourceRange &SR = DS.getSourceRange();
    if (Range.getBegin().isInvalid())
      Range.setBegin(SR.getBegin());
    if (!SR.getEnd().isInvalid())
      Range.setEnd(SR.getEnd());
  }

  /// clear - Reset the contents of this Declarator.
  void clear() {
    SS.clear();
    Name.clear();
    Range = DS.getSourceRange();
    
    for (unsigned i = 0, e = DeclTypeInfo.size(); i != e; ++i)
      DeclTypeInfo[i].destroy();
    DeclTypeInfo.clear();
    delete AttrList;
    AttrList = 0;
    AsmLabel = 0;
    InlineParamsUsed = false;
  }

  /// mayOmitIdentifier - Return true if the identifier is either optional or
  /// not allowed.  This is true for typenames, prototypes, and template
  /// parameter lists.
  bool mayOmitIdentifier() const {
    return Context == TypeNameContext || Context == PrototypeContext ||
           Context == TemplateParamContext || Context == CXXCatchContext ||
           Context == BlockLiteralContext;
  }

  /// mayHaveIdentifier - Return true if the identifier is either optional or
  /// required.  This is true for normal declarators and prototypes, but not
  /// typenames.
  bool mayHaveIdentifier() const {
    return Context != TypeNameContext && Context != BlockLiteralContext;
  }

  /// mayBeFollowedByCXXDirectInit - Return true if the declarator can be
  /// followed by a C++ direct initializer, e.g. "int x(1);".
  bool mayBeFollowedByCXXDirectInit() const {
    return !hasGroupingParens() &&
           (Context == FileContext  ||
            Context == BlockContext ||
            Context == ForContext);
  }

  /// isPastIdentifier - Return true if we have parsed beyond the point where
  /// the
  bool isPastIdentifier() const { return Name.isValid(); }

  /// hasName - Whether this declarator has a name, which might be an
  /// identifier (accessible via getIdentifier()) or some kind of
  /// special C++ name (constructor, destructor, etc.).
  bool hasName() const { 
    return Name.getKind() != UnqualifiedId::IK_Identifier || Name.Identifier;
  }

  IdentifierInfo *getIdentifier() const { 
    if (Name.getKind() == UnqualifiedId::IK_Identifier)
      return Name.Identifier;
    
    return 0;
  }
  SourceLocation getIdentifierLoc() const { return Name.StartLocation; }

  /// \brief Set the name of this declarator to be the given identifier.
  void SetIdentifier(IdentifierInfo *Id, SourceLocation IdLoc) {
    Name.setIdentifier(Id, IdLoc);
  }
  
  /// AddTypeInfo - Add a chunk to this declarator. Also extend the range to
  /// EndLoc, which should be the last token of the chunk.
  void AddTypeInfo(const DeclaratorChunk &TI, SourceLocation EndLoc) {
    DeclTypeInfo.push_back(TI);
    if (!EndLoc.isInvalid())
      SetRangeEnd(EndLoc);
  }

  /// getNumTypeObjects() - Return the number of types applied to this
  /// declarator.
  unsigned getNumTypeObjects() const { return DeclTypeInfo.size(); }

  /// Return the specified TypeInfo from this declarator.  TypeInfo #0 is
  /// closest to the identifier.
  const DeclaratorChunk &getTypeObject(unsigned i) const {
    assert(i < DeclTypeInfo.size() && "Invalid type chunk");
    return DeclTypeInfo[i];
  }
  DeclaratorChunk &getTypeObject(unsigned i) {
    assert(i < DeclTypeInfo.size() && "Invalid type chunk");
    return DeclTypeInfo[i];
  }

  void DropFirstTypeObject()
  {
    assert(!DeclTypeInfo.empty() && "No type chunks to drop.");
    DeclTypeInfo.front().destroy();
    DeclTypeInfo.erase(DeclTypeInfo.begin());
  }

  /// isFunctionDeclarator - Once this declarator is fully parsed and formed,
  /// this method returns true if the identifier is a function declarator.
  bool isFunctionDeclarator() const {
    return !DeclTypeInfo.empty() &&
           DeclTypeInfo[0].Kind == DeclaratorChunk::Function;
  }

  /// AddAttributes - simply adds the attribute list to the Declarator.
  /// These examples both add 3 attributes to "var":
  ///  short int var __attribute__((aligned(16),common,deprecated));
  ///  short int x, __attribute__((aligned(16)) var
  ///                                 __attribute__((common,deprecated));
  ///
  /// Also extends the range of the declarator.
  void AddAttributes(AttributeList *alist, SourceLocation LastLoc) {
    AttrList = addAttributeLists(AttrList, alist);

    if (!LastLoc.isInvalid())
      SetRangeEnd(LastLoc);
  }

  const AttributeList *getAttributes() const { return AttrList; }
  AttributeList *getAttributes() { return AttrList; }

  /// hasAttributes - do we contain any attributes?
  bool hasAttributes() const {
    if (getAttributes() || getDeclSpec().getAttributes()) return true;
    for (unsigned i = 0, e = getNumTypeObjects(); i != e; ++i)
      if (getTypeObject(i).getAttrs())
        return true;
    return false;
  }

  void setAsmLabel(Expr *E) { AsmLabel = E; }
  Expr *getAsmLabel() const { return AsmLabel; }

  void setExtension(bool Val = true) { Extension = Val; }
  bool getExtension() const { return Extension; }

  void setInvalidType(bool Val = true) { InvalidType = Val; }
  bool isInvalidType() const {
    return InvalidType || DS.getTypeSpecType() == DeclSpec::TST_error;
  }

  void setGroupingParens(bool flag) { GroupingParens = flag; }
  bool hasGroupingParens() const { return GroupingParens; }
};

/// FieldDeclarator - This little struct is used to capture information about
/// structure field declarators, which is basically just a bitfield size.
struct FieldDeclarator {
  Declarator D;
  Expr *BitfieldSize;
  explicit FieldDeclarator(DeclSpec &DS) : D(DS, Declarator::MemberContext) {
    BitfieldSize = 0;
  }
};
  
} // end namespace clang

#endif
