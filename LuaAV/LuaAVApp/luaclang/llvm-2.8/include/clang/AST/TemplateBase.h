//===-- TemplateBase.h - Core classes for C++ templates ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file provides definitions which are common for all kinds of
//  template representation.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_AST_TEMPLATEBASE_H
#define LLVM_CLANG_AST_TEMPLATEBASE_H

#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/ErrorHandling.h"
#include "clang/AST/Type.h"
#include "clang/AST/TemplateName.h"

namespace llvm {
  class FoldingSetNodeID;
}

namespace clang {

class Decl;
class DiagnosticBuilder;
class Expr;
class TypeSourceInfo;

/// \brief Represents a template argument within a class template
/// specialization.
class TemplateArgument {
  union {
    uintptr_t TypeOrValue;
    struct {
      char Value[sizeof(llvm::APSInt)];
      void *Type;
    } Integer;
    struct {
      TemplateArgument *Args;
      unsigned NumArgs;
      bool CopyArgs;
    } Args;
  };

public:
  /// \brief The type of template argument we're storing.
  enum ArgKind {
    /// \brief Represents an empty template argument, e.g., one that has not
    /// been deduced.
    Null = 0,
    /// The template argument is a type. Its value is stored in the
    /// TypeOrValue field.
    Type,
    /// The template argument is a declaration that was provided for a pointer
    /// or reference non-type template parameter.
    Declaration,
    /// The template argument is an integral value stored in an llvm::APSInt
    /// that was provided for an integral non-type template parameter. 
    Integral,
    /// The template argument is a template name that was provided for a 
    /// template template parameter.
    Template,
    /// The template argument is a value- or type-dependent expression
    /// stored in an Expr*.
    Expression,
    /// The template argument is actually a parameter pack. Arguments are stored
    /// in the Args struct.
    Pack
  } Kind;

  /// \brief Construct an empty, invalid template argument.
  TemplateArgument() : TypeOrValue(0), Kind(Null) { }

  /// \brief Construct a template type argument.
  TemplateArgument(QualType T) : Kind(Type) {
    TypeOrValue = reinterpret_cast<uintptr_t>(T.getAsOpaquePtr());
  }

  /// \brief Construct a template argument that refers to a
  /// declaration, which is either an external declaration or a
  /// template declaration.
  TemplateArgument(Decl *D) : Kind(Declaration) {
    // FIXME: Need to be sure we have the "canonical" declaration!
    TypeOrValue = reinterpret_cast<uintptr_t>(D);
  }

  /// \brief Construct an integral constant template argument.
  TemplateArgument(const llvm::APSInt &Value, QualType Type) : Kind(Integral) {
    new (Integer.Value) llvm::APSInt(Value);
    Integer.Type = Type.getAsOpaquePtr();
  }

  /// \brief Construct a template argument that is a template.
  ///
  /// This form of template argument is generally used for template template
  /// parameters. However, the template name could be a dependent template
  /// name that ends up being instantiated to a function template whose address
  /// is taken.
  TemplateArgument(TemplateName Name) : Kind(Template) {
    TypeOrValue = reinterpret_cast<uintptr_t>(Name.getAsVoidPointer());
  }
  
  /// \brief Construct a template argument that is an expression.
  ///
  /// This form of template argument only occurs in template argument
  /// lists used for dependent types and for expression; it will not
  /// occur in a non-dependent, canonical template argument list.
  TemplateArgument(Expr *E) : Kind(Expression) {
    TypeOrValue = reinterpret_cast<uintptr_t>(E);
  }

  /// \brief Copy constructor for a template argument.
  TemplateArgument(const TemplateArgument &Other) : Kind(Other.Kind) {
    if (Kind == Integral) {
      new (Integer.Value) llvm::APSInt(*Other.getAsIntegral());
      Integer.Type = Other.Integer.Type;
    } else if (Kind == Pack) {
      Args.NumArgs = Other.Args.NumArgs;
      Args.Args = new TemplateArgument[Args.NumArgs];
      for (unsigned I = 0; I != Args.NumArgs; ++I)
        Args.Args[I] = Other.Args.Args[I];
    }
    else
      TypeOrValue = Other.TypeOrValue;
  }

  TemplateArgument& operator=(const TemplateArgument& Other) {
    // FIXME: Does not provide the strong guarantee for exception
    // safety.
    using llvm::APSInt;

    // FIXME: Handle Packs
    assert(Kind != Pack && "FIXME: Handle packs");
    assert(Other.Kind != Pack && "FIXME: Handle packs");

    if (Kind == Other.Kind && Kind == Integral) {
      // Copy integral values.
      *this->getAsIntegral() = *Other.getAsIntegral();
      Integer.Type = Other.Integer.Type;
    } else {
      // Destroy the current integral value, if that's what we're holding.
      if (Kind == Integral)
        getAsIntegral()->~APSInt();

      Kind = Other.Kind;

      if (Other.Kind == Integral) {
        new (Integer.Value) llvm::APSInt(*Other.getAsIntegral());
        Integer.Type = Other.Integer.Type;
      } else
        TypeOrValue = Other.TypeOrValue;
    }

    return *this;
  }

  ~TemplateArgument() {
    using llvm::APSInt;

    if (Kind == Integral)
      getAsIntegral()->~APSInt();
    else if (Kind == Pack && Args.CopyArgs)
      delete[] Args.Args;
  }

  /// \brief Return the kind of stored template argument.
  ArgKind getKind() const { return Kind; }

  /// \brief Determine whether this template argument has no value.
  bool isNull() const { return Kind == Null; }

  /// \brief Retrieve the template argument as a type.
  QualType getAsType() const {
    if (Kind != Type)
      return QualType();

    return QualType::getFromOpaquePtr(reinterpret_cast<void*>(TypeOrValue));
  }

  /// \brief Retrieve the template argument as a declaration.
  Decl *getAsDecl() const {
    if (Kind != Declaration)
      return 0;
    return reinterpret_cast<Decl *>(TypeOrValue);
  }

  /// \brief Retrieve the template argument as a template name.
  TemplateName getAsTemplate() const {
    if (Kind != Template)
      return TemplateName();
    
    return TemplateName::getFromVoidPointer(
                                        reinterpret_cast<void *> (TypeOrValue));
  }
  
  /// \brief Retrieve the template argument as an integral value.
  llvm::APSInt *getAsIntegral() {
    if (Kind != Integral)
      return 0;
    return reinterpret_cast<llvm::APSInt*>(&Integer.Value[0]);
  }

  const llvm::APSInt *getAsIntegral() const {
    return const_cast<TemplateArgument*>(this)->getAsIntegral();
  }

  /// \brief Retrieve the type of the integral value.
  QualType getIntegralType() const {
    if (Kind != Integral)
      return QualType();

    return QualType::getFromOpaquePtr(Integer.Type);
  }

  void setIntegralType(QualType T) {
    assert(Kind == Integral &&
           "Cannot set the integral type of a non-integral template argument");
    Integer.Type = T.getAsOpaquePtr();
  }

  /// \brief Retrieve the template argument as an expression.
  Expr *getAsExpr() const {
    if (Kind != Expression)
      return 0;

    return reinterpret_cast<Expr *>(TypeOrValue);
  }

  /// \brief Iterator that traverses the elements of a template argument pack.
  typedef const TemplateArgument * pack_iterator;

  /// \brief Iterator referencing the first argument of a template argument
  /// pack.
  pack_iterator pack_begin() const {
    assert(Kind == Pack);
    return Args.Args;
  }

  /// \brief Iterator referencing one past the last argument of a template
  /// argument pack.
  pack_iterator pack_end() const {
    assert(Kind == Pack);
    return Args.Args + Args.NumArgs;
  }

  /// \brief The number of template arguments in the given template argument
  /// pack.
  unsigned pack_size() const {
    assert(Kind == Pack);
    return Args.NumArgs;
  }

  /// Determines whether two template arguments are superficially the
  /// same.
  bool structurallyEquals(const TemplateArgument &Other) const;

  /// \brief Construct a template argument pack.
  void setArgumentPack(TemplateArgument *Args, unsigned NumArgs, bool CopyArgs);

  /// \brief Used to insert TemplateArguments into FoldingSets.
  void Profile(llvm::FoldingSetNodeID &ID, ASTContext &Context) const;
};

/// Location information for a TemplateArgument.
struct TemplateArgumentLocInfo {
private:
  union {
    Expr *Expression;
    TypeSourceInfo *Declarator;
    struct {
      unsigned QualifierRange[2];
      unsigned TemplateNameLoc;
    } Template;
  };

#ifndef NDEBUG
  enum Kind {
    K_None,
    K_TypeSourceInfo,
    K_Expression,
    K_Template
  } Kind;
#endif

public:
  TemplateArgumentLocInfo()
    : Expression(0)
#ifndef NDEBUG
      , Kind(K_None) 
#endif
    {}
  
  TemplateArgumentLocInfo(TypeSourceInfo *TInfo)
    : Declarator(TInfo)
#ifndef NDEBUG
      , Kind(K_TypeSourceInfo) 
#endif
    {}
  
  TemplateArgumentLocInfo(Expr *E)
    : Expression(E)
#ifndef NDEBUG
      , Kind(K_Expression) 
#endif
    {}
  
  TemplateArgumentLocInfo(SourceRange QualifierRange, 
                          SourceLocation TemplateNameLoc)
#ifndef NDEBUG
    : Kind(K_Template)
#endif
  {
    Template.QualifierRange[0] = QualifierRange.getBegin().getRawEncoding();
    Template.QualifierRange[1] = QualifierRange.getEnd().getRawEncoding();
    Template.TemplateNameLoc = TemplateNameLoc.getRawEncoding();
  }

  TypeSourceInfo *getAsTypeSourceInfo() const {
    assert(Kind == K_TypeSourceInfo);
    return Declarator;
  }

  Expr *getAsExpr() const {
    assert(Kind == K_Expression);
    return Expression;
  }

  SourceRange getTemplateQualifierRange() const {
    assert(Kind == K_Template);
    return SourceRange(
                SourceLocation::getFromRawEncoding(Template.QualifierRange[0]),
                SourceLocation::getFromRawEncoding(Template.QualifierRange[1]));
  }
  
  SourceLocation getTemplateNameLoc() const {
    assert(Kind == K_Template);
    return SourceLocation::getFromRawEncoding(Template.TemplateNameLoc);
  }
  
#ifndef NDEBUG
  void validateForArgument(const TemplateArgument &Arg) {
    switch (Arg.getKind()) {
    case TemplateArgument::Type:
      assert(Kind == K_TypeSourceInfo);
      break;
    case TemplateArgument::Expression:
    case TemplateArgument::Declaration:
      assert(Kind == K_Expression);
      break;
    case TemplateArgument::Template:
      assert(Kind == K_Template);
      break;
    case TemplateArgument::Integral:
    case TemplateArgument::Pack:
      assert(Kind == K_None);
      break;
    case TemplateArgument::Null:
      llvm_unreachable("source info for null template argument?");
    }
  }
#endif
};

/// Location wrapper for a TemplateArgument.  TemplateArgument is to
/// TemplateArgumentLoc as Type is to TypeLoc.
class TemplateArgumentLoc {
  TemplateArgument Argument;
  TemplateArgumentLocInfo LocInfo;

public:
  TemplateArgumentLoc() {}

  TemplateArgumentLoc(const TemplateArgument &Argument,
                      TemplateArgumentLocInfo Opaque)
    : Argument(Argument), LocInfo(Opaque) {
  }

  TemplateArgumentLoc(const TemplateArgument &Argument, TypeSourceInfo *TInfo)
    : Argument(Argument), LocInfo(TInfo) {
    assert(Argument.getKind() == TemplateArgument::Type);
  }

  TemplateArgumentLoc(const TemplateArgument &Argument, Expr *E)
    : Argument(Argument), LocInfo(E) {
    assert(Argument.getKind() == TemplateArgument::Expression);
  }

  TemplateArgumentLoc(const TemplateArgument &Argument, 
                      SourceRange QualifierRange,
                      SourceLocation TemplateNameLoc)
    : Argument(Argument), LocInfo(QualifierRange, TemplateNameLoc) {
    assert(Argument.getKind() == TemplateArgument::Template);
  }
  
  /// \brief - Fetches the primary location of the argument.
  SourceLocation getLocation() const {
    if (Argument.getKind() == TemplateArgument::Template)
      return getTemplateNameLoc();
    
    return getSourceRange().getBegin();
  }

  /// \brief - Fetches the full source range of the argument.
  SourceRange getSourceRange() const;

  const TemplateArgument &getArgument() const {
    return Argument;
  }

  TemplateArgumentLocInfo getLocInfo() const {
    return LocInfo;
  }

  TypeSourceInfo *getTypeSourceInfo() const {
    assert(Argument.getKind() == TemplateArgument::Type);
    return LocInfo.getAsTypeSourceInfo();
  }

  Expr *getSourceExpression() const {
    assert(Argument.getKind() == TemplateArgument::Expression);
    return LocInfo.getAsExpr();
  }

  Expr *getSourceDeclExpression() const {
    assert(Argument.getKind() == TemplateArgument::Declaration);
    return LocInfo.getAsExpr();
  }
  
  SourceRange getTemplateQualifierRange() const {
    assert(Argument.getKind() == TemplateArgument::Template);
    return LocInfo.getTemplateQualifierRange();
  }
  
  SourceLocation getTemplateNameLoc() const {
    assert(Argument.getKind() == TemplateArgument::Template);
    return LocInfo.getTemplateNameLoc();
  }  
};

/// A convenient class for passing around template argument
/// information.  Designed to be passed by reference.
class TemplateArgumentListInfo {
  llvm::SmallVector<TemplateArgumentLoc, 8> Arguments;
  SourceLocation LAngleLoc;
  SourceLocation RAngleLoc;

public:
  TemplateArgumentListInfo() {}

  TemplateArgumentListInfo(SourceLocation LAngleLoc,
                           SourceLocation RAngleLoc)
    : LAngleLoc(LAngleLoc), RAngleLoc(RAngleLoc) {}

  SourceLocation getLAngleLoc() const { return LAngleLoc; }
  SourceLocation getRAngleLoc() const { return RAngleLoc; }

  void setLAngleLoc(SourceLocation Loc) { LAngleLoc = Loc; }
  void setRAngleLoc(SourceLocation Loc) { RAngleLoc = Loc; }

  unsigned size() const { return Arguments.size(); }

  const TemplateArgumentLoc *getArgumentArray() const {
    return Arguments.data();
  }

  const TemplateArgumentLoc &operator[](unsigned I) const {
    return Arguments[I];
  }

  void addArgument(const TemplateArgumentLoc &Loc) {
    Arguments.push_back(Loc);
  }
};

const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                    const TemplateArgument &Arg);

inline TemplateSpecializationType::iterator
    TemplateSpecializationType::end() const {
  return getArgs() + getNumArgs();
}

inline DependentTemplateSpecializationType::iterator
    DependentTemplateSpecializationType::end() const {
  return getArgs() + getNumArgs();
}

inline const TemplateArgument &
    TemplateSpecializationType::getArg(unsigned Idx) const {
  assert(Idx < getNumArgs() && "Template argument out of range");
  return getArgs()[Idx];
}

inline const TemplateArgument &
    DependentTemplateSpecializationType::getArg(unsigned Idx) const {
  assert(Idx < getNumArgs() && "Template argument out of range");
  return getArgs()[Idx];
}
  
} // end namespace clang

#endif
