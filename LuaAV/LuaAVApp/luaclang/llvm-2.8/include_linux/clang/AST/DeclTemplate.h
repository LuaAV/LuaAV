//===-- DeclTemplate.h - Classes for representing C++ templates -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the C++ template declaration subclasses.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_AST_DECLTEMPLATE_H
#define LLVM_CLANG_AST_DECLTEMPLATE_H

#include "clang/AST/DeclCXX.h"
#include "clang/AST/TemplateBase.h"
#include "llvm/ADT/PointerUnion.h"
#include <limits>

namespace clang {

class TemplateParameterList;
class TemplateDecl;
class RedeclarableTemplateDecl;
class FunctionTemplateDecl;
class ClassTemplateDecl;
class ClassTemplatePartialSpecializationDecl;
class TemplateTypeParmDecl;
class NonTypeTemplateParmDecl;
class TemplateTemplateParmDecl;

/// \brief Stores a template parameter of any kind.
typedef llvm::PointerUnion3<TemplateTypeParmDecl*, NonTypeTemplateParmDecl*,
                            TemplateTemplateParmDecl*> TemplateParameter;

/// TemplateParameterList - Stores a list of template parameters for a
/// TemplateDecl and its derived classes.
class TemplateParameterList {
  /// The location of the 'template' keyword.
  SourceLocation TemplateLoc;

  /// The locations of the '<' and '>' angle brackets.
  SourceLocation LAngleLoc, RAngleLoc;

  /// The number of template parameters in this template
  /// parameter list.
  unsigned NumParams;

  TemplateParameterList(SourceLocation TemplateLoc, SourceLocation LAngleLoc,
                        NamedDecl **Params, unsigned NumParams,
                        SourceLocation RAngleLoc);

public:
  static TemplateParameterList *Create(ASTContext &C,
                                       SourceLocation TemplateLoc,
                                       SourceLocation LAngleLoc,
                                       NamedDecl **Params,
                                       unsigned NumParams,
                                       SourceLocation RAngleLoc);

  /// iterator - Iterates through the template parameters in this list.
  typedef NamedDecl** iterator;

  /// const_iterator - Iterates through the template parameters in this list.
  typedef NamedDecl* const* const_iterator;

  iterator begin() { return reinterpret_cast<NamedDecl **>(this + 1); }
  const_iterator begin() const {
    return reinterpret_cast<NamedDecl * const *>(this + 1);
  }
  iterator end() { return begin() + NumParams; }
  const_iterator end() const { return begin() + NumParams; }

  unsigned size() const { return NumParams; }

  NamedDecl* getParam(unsigned Idx) {
    assert(Idx < size() && "Template parameter index out-of-range");
    return begin()[Idx];
  }

  const NamedDecl* getParam(unsigned Idx) const {
    assert(Idx < size() && "Template parameter index out-of-range");
    return begin()[Idx];
  }

  /// \btief Returns the minimum number of arguments needed to form a
  /// template specialization. This may be fewer than the number of
  /// template parameters, if some of the parameters have default
  /// arguments or if there is a parameter pack.
  unsigned getMinRequiredArguments() const;

  /// \brief Get the depth of this template parameter list in the set of
  /// template parameter lists.
  ///
  /// The first template parameter list in a declaration will have depth 0,
  /// the second template parameter list will have depth 1, etc.
  unsigned getDepth() const;
  
  SourceLocation getTemplateLoc() const { return TemplateLoc; }
  SourceLocation getLAngleLoc() const { return LAngleLoc; }
  SourceLocation getRAngleLoc() const { return RAngleLoc; }

  SourceRange getSourceRange() const {
    return SourceRange(TemplateLoc, RAngleLoc);
  }
};

/// \brief A helper class for making template argument lists.
class TemplateArgumentListBuilder {
  TemplateArgument *StructuredArgs;
  unsigned MaxStructuredArgs;
  unsigned NumStructuredArgs;

  llvm::SmallVector<TemplateArgument, 4> FlatArgs;
  unsigned MaxFlatArgs;
  unsigned NumFlatArgs;

  bool AddingToPack;
  unsigned PackBeginIndex;

public:
  TemplateArgumentListBuilder(const TemplateParameterList *Parameters,
                              unsigned NumTemplateArgs)
  : StructuredArgs(0), MaxStructuredArgs(Parameters->size()),
  NumStructuredArgs(0), FlatArgs(0),
  MaxFlatArgs(std::max(MaxStructuredArgs, NumTemplateArgs)), NumFlatArgs(0),
  AddingToPack(false), PackBeginIndex(0) { }

  void Append(const TemplateArgument &Arg);
  void BeginPack();
  void EndPack();

  unsigned flatSize() const { return FlatArgs.size(); }
  const TemplateArgument *getFlatArguments() const { return FlatArgs.data(); }

  unsigned structuredSize() const {
    // If we don't have any structured args, just reuse the flat size.
    if (!StructuredArgs)
      return flatSize();

    return NumStructuredArgs;
  }
  const TemplateArgument *getStructuredArguments() const {
    // If we don't have any structured args, just reuse the flat args.
    if (!StructuredArgs)
      return getFlatArguments();

    return StructuredArgs;
  }
};

/// \brief A template argument list.
///
/// FIXME: In the future, this class will be extended to support
/// variadic templates and member templates, which will make some of
/// the function names below make more sense.
class TemplateArgumentList {
  /// \brief The template argument list.
  ///
  /// The integer value will be non-zero to indicate that this
  /// template argument list does own the pointer.
  llvm::PointerIntPair<const TemplateArgument *, 1> FlatArguments;

  /// \brief The number of template arguments in this template
  /// argument list.
  unsigned NumFlatArguments;

  llvm::PointerIntPair<const TemplateArgument *, 1> StructuredArguments;
  unsigned NumStructuredArguments;

  TemplateArgumentList(const TemplateArgumentList &Other); // DO NOT IMPL
  void operator=(const TemplateArgumentList &Other); // DO NOT IMPL
public:
  /// TemplateArgumentList - If this constructor is passed "true" for 'TakeArgs'
  /// it copies them into a locally new[]'d array.  If passed "false", then it
  /// just references the array passed in.  This is only safe if the builder
  /// outlives it, but saves a copy.
  TemplateArgumentList(ASTContext &Context,
                       TemplateArgumentListBuilder &Builder,
                       bool TakeArgs);

  /// TemplateArgumentList - It copies the template arguments into a locally
  /// new[]'d array.
  TemplateArgumentList(ASTContext &Context,
                       const TemplateArgument *Args, unsigned NumArgs);

  /// Produces a shallow copy of the given template argument list.  This
  /// assumes that the input argument list outlives it.  This takes the list as
  /// a pointer to avoid looking like a copy constructor, since this really
  /// really isn't safe to use that way.
  explicit TemplateArgumentList(const TemplateArgumentList *Other);

  TemplateArgumentList() : NumFlatArguments(0), NumStructuredArguments(0) { }

  /// \brief Copies the template arguments into a locally new[]'d array.
  void init(ASTContext &Context,
            const TemplateArgument *Args, unsigned NumArgs);

  /// \brief Retrieve the template argument at a given index.
  const TemplateArgument &get(unsigned Idx) const {
    assert(Idx < NumFlatArguments && "Invalid template argument index");
    return getFlatArgumentList()[Idx];
  }

  /// \brief Retrieve the template argument at a given index.
  const TemplateArgument &operator[](unsigned Idx) const { return get(Idx); }

  /// \brief Retrieve the number of template arguments in this
  /// template argument list.
  unsigned size() const { return NumFlatArguments; }

  /// \brief Retrieve the number of template arguments in the
  /// flattened template argument list.
  unsigned flat_size() const { return NumFlatArguments; }

  /// \brief Retrieve the flattened template argument list.
  const TemplateArgument *getFlatArgumentList() const {
    return FlatArguments.getPointer();
  }
};

//===----------------------------------------------------------------------===//
// Kinds of Templates
//===----------------------------------------------------------------------===//

/// TemplateDecl - The base class of all kinds of template declarations (e.g.,
/// class, function, etc.). The TemplateDecl class stores the list of template
/// parameters and a reference to the templated scoped declaration: the
/// underlying AST node.
class TemplateDecl : public NamedDecl {
protected:
  // This is probably never used.
  TemplateDecl(Kind DK, DeclContext *DC, SourceLocation L,
               DeclarationName Name)
    : NamedDecl(DK, DC, L, Name), TemplatedDecl(0), TemplateParams(0) { }

  // Construct a template decl with the given name and parameters.
  // Used when there is not templated element (tt-params, alias?).
  TemplateDecl(Kind DK, DeclContext *DC, SourceLocation L,
               DeclarationName Name, TemplateParameterList *Params)
    : NamedDecl(DK, DC, L, Name), TemplatedDecl(0), TemplateParams(Params) { }

  // Construct a template decl with name, parameters, and templated element.
  TemplateDecl(Kind DK, DeclContext *DC, SourceLocation L,
               DeclarationName Name, TemplateParameterList *Params,
               NamedDecl *Decl)
    : NamedDecl(DK, DC, L, Name), TemplatedDecl(Decl),
      TemplateParams(Params) { }
public:
  /// Get the list of template parameters
  TemplateParameterList *getTemplateParameters() const {
    return TemplateParams;
  }

  /// Get the underlying, templated declaration.
  NamedDecl *getTemplatedDecl() const { return TemplatedDecl; }

  // Implement isa/cast/dyncast/etc.
  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classof(const TemplateDecl *D) { return true; }
  static bool classof(const RedeclarableTemplateDecl *D) { return true; }
  static bool classof(const FunctionTemplateDecl *D) { return true; }
  static bool classof(const ClassTemplateDecl *D) { return true; }
  static bool classof(const TemplateTemplateParmDecl *D) { return true; }
  static bool classofKind(Kind K) {
    return K >= firstTemplate && K <= lastTemplate;
  }

  SourceRange getSourceRange() const {
    return SourceRange(TemplateParams->getTemplateLoc(),
                       TemplatedDecl->getSourceRange().getEnd());
  }

protected:
  NamedDecl *TemplatedDecl;
  TemplateParameterList* TemplateParams;
  
public:
  /// \brief Initialize the underlying templated declaration and
  /// template parameters.
  void init(NamedDecl *templatedDecl, TemplateParameterList* templateParams) {
    assert(TemplatedDecl == 0 && "TemplatedDecl already set!");
    assert(TemplateParams == 0 && "TemplateParams already set!");
    TemplatedDecl = templatedDecl;
    TemplateParams = templateParams;
  }
};

/// \brief Provides information about a function template specialization,
/// which is a FunctionDecl that has been explicitly specialization or
/// instantiated from a function template.
class FunctionTemplateSpecializationInfo : public llvm::FoldingSetNode {
public:
  /// \brief The function template specialization that this structure
  /// describes.
  FunctionDecl *Function;

  /// \brief The function template from which this function template
  /// specialization was generated.
  ///
  /// The two bits are contain the top 4 values of TemplateSpecializationKind.
  llvm::PointerIntPair<FunctionTemplateDecl *, 2> Template;

  /// \brief The template arguments used to produce the function template
  /// specialization from the function template.
  const TemplateArgumentList *TemplateArguments;

  /// \brief The template arguments as written in the sources, if provided.
  const TemplateArgumentListInfo *TemplateArgumentsAsWritten;

  /// \brief The point at which this function template specialization was
  /// first instantiated. 
  SourceLocation PointOfInstantiation;
  
  /// \brief Retrieve the template from which this function was specialized.
  FunctionTemplateDecl *getTemplate() const { return Template.getPointer(); }

  /// \brief Determine what kind of template specialization this is.
  TemplateSpecializationKind getTemplateSpecializationKind() const {
    return (TemplateSpecializationKind)(Template.getInt() + 1);
  }

  /// \brief Set the template specialization kind.
  void setTemplateSpecializationKind(TemplateSpecializationKind TSK) {
    assert(TSK != TSK_Undeclared &&
         "Cannot encode TSK_Undeclared for a function template specialization");
    Template.setInt(TSK - 1);
  }

  /// \brief Retrieve the first point of instantiation of this function
  /// template specialization.
  ///
  /// The point of instantiation may be an invalid source location if this
  /// function has yet to be instantiated.
  SourceLocation getPointOfInstantiation() const { 
    return PointOfInstantiation; 
  }
  
  /// \brief Set the (first) point of instantiation of this function template
  /// specialization.
  void setPointOfInstantiation(SourceLocation POI) {
    PointOfInstantiation = POI;
  }
  
  void Profile(llvm::FoldingSetNodeID &ID) {
    Profile(ID, TemplateArguments->getFlatArgumentList(),
            TemplateArguments->flat_size(),
            Function->getASTContext());
  }

  static void
  Profile(llvm::FoldingSetNodeID &ID, const TemplateArgument *TemplateArgs,
          unsigned NumTemplateArgs, ASTContext &Context) {
    ID.AddInteger(NumTemplateArgs);
    for (unsigned Arg = 0; Arg != NumTemplateArgs; ++Arg)
      TemplateArgs[Arg].Profile(ID, Context);
  }
};

/// \brief Provides information a specialization of a member of a class 
/// template, which may be a member function, static data member, or
/// member class.
class MemberSpecializationInfo {
  // The member declaration from which this member was instantiated, and the
  // manner in which the instantiation occurred (in the lower two bits).
  llvm::PointerIntPair<NamedDecl *, 2> MemberAndTSK;
  
  // The point at which this member was first instantiated.
  SourceLocation PointOfInstantiation;
  
public:
  explicit 
  MemberSpecializationInfo(NamedDecl *IF, TemplateSpecializationKind TSK,
                           SourceLocation POI = SourceLocation())
    : MemberAndTSK(IF, TSK - 1), PointOfInstantiation(POI) {
    assert(TSK != TSK_Undeclared && 
           "Cannot encode undeclared template specializations for members");
  }
  
  /// \brief Retrieve the member declaration from which this member was
  /// instantiated.
  NamedDecl *getInstantiatedFrom() const { return MemberAndTSK.getPointer(); }
  
  /// \brief Determine what kind of template specialization this is.
  TemplateSpecializationKind getTemplateSpecializationKind() const {
    return (TemplateSpecializationKind)(MemberAndTSK.getInt() + 1);
  }
  
  /// \brief Set the template specialization kind.
  void setTemplateSpecializationKind(TemplateSpecializationKind TSK) {
    assert(TSK != TSK_Undeclared && 
           "Cannot encode undeclared template specializations for members");
    MemberAndTSK.setInt(TSK - 1);
  }
  
  /// \brief Retrieve the first point of instantiation of this member. 
  /// If the point of instantiation is an invalid location, then this member
  /// has not yet been instantiated.
  SourceLocation getPointOfInstantiation() const { 
    return PointOfInstantiation; 
  }
  
  /// \brief Set the first point of instantiation.
  void setPointOfInstantiation(SourceLocation POI) {
    PointOfInstantiation = POI;
  }
};

/// \brief Provides information about a dependent function-template
/// specialization declaration.  Since explicit function template
/// specialization and instantiation declarations can only appear in
/// namespace scope, and you can only specialize a member of a
/// fully-specialized class, the only way to get one of these is in
/// a friend declaration like the following:
///
///   template <class T> void foo(T);
///   template <class T> class A {
///     friend void foo<>(T);
///   };
class DependentFunctionTemplateSpecializationInfo {
  union {
    // Force sizeof to be a multiple of sizeof(void*) so that the
    // trailing data is aligned.
    void *Aligner; 

    struct {
      /// The number of potential template candidates.
      unsigned NumTemplates;

      /// The number of template arguments.
      unsigned NumArgs;      
    } d;
  };

  /// The locations of the left and right angle brackets.
  SourceRange AngleLocs;

  FunctionTemplateDecl * const *getTemplates() const {
    return reinterpret_cast<FunctionTemplateDecl*const*>(this+1);
  }

  const TemplateArgumentLoc *getTemplateArgs() const {
    return reinterpret_cast<const TemplateArgumentLoc*>(
             &getTemplates()[getNumTemplates()]);
  }

public:
  DependentFunctionTemplateSpecializationInfo(
                                 const UnresolvedSetImpl &Templates,
                                 const TemplateArgumentListInfo &TemplateArgs);

  /// \brief Returns the number of function templates that this might
  /// be a specialization of.
  unsigned getNumTemplates() const {
    return d.NumTemplates;
  }

  /// \brief Returns the i'th template candidate.
  FunctionTemplateDecl *getTemplate(unsigned I) const {
    assert(I < getNumTemplates() && "template index out of range");
    return getTemplates()[I];
  }

  /// \brief Returns the number of explicit template arguments that were given.
  unsigned getNumTemplateArgs() const {
    return d.NumArgs;
  }

  /// \brief Returns the nth template argument.
  const TemplateArgumentLoc &getTemplateArg(unsigned I) const {
    assert(I < getNumTemplateArgs() && "template arg index out of range");
    return getTemplateArgs()[I];
  }

  SourceLocation getLAngleLoc() const {
    return AngleLocs.getBegin();
  }

  SourceLocation getRAngleLoc() const {
    return AngleLocs.getEnd();
  }
};
  
/// Declaration of a redeclarable template.
class RedeclarableTemplateDecl : public TemplateDecl {

  RedeclarableTemplateDecl *getPreviousDeclarationImpl() {
    return CommonOrPrev.dyn_cast<RedeclarableTemplateDecl*>();
  }

  RedeclarableTemplateDecl *getCanonicalDeclImpl();

  void setPreviousDeclarationImpl(RedeclarableTemplateDecl *Prev);

  RedeclarableTemplateDecl *getInstantiatedFromMemberTemplateImpl() {
    return getCommonPtr()->InstantiatedFromMember.getPointer();
  }

  void setInstantiatedFromMemberTemplateImpl(RedeclarableTemplateDecl *TD) {
    assert(!getCommonPtr()->InstantiatedFromMember.getPointer());
    getCommonPtr()->InstantiatedFromMember.setPointer(TD);
  }

protected:
  template <typename EntryType> struct SpecEntryTraits {
    typedef EntryType DeclType;

    static DeclType *getMostRecentDeclaration(EntryType *D) {
      return D->getMostRecentDeclaration();
    }
  };

  template <typename EntryType,
            typename _SETraits = SpecEntryTraits<EntryType>,
            typename _DeclType = typename _SETraits::DeclType>
  class SpecIterator : public std::iterator<std::forward_iterator_tag,
                                            _DeclType*, ptrdiff_t,
                                            _DeclType*, _DeclType*> {
    typedef _SETraits SETraits;
    typedef _DeclType DeclType;

    typedef typename llvm::FoldingSet<EntryType>::iterator SetIteratorType;

    SetIteratorType SetIter;

  public:
    SpecIterator() : SetIter() {}
    SpecIterator(SetIteratorType SetIter) : SetIter(SetIter) {}

    DeclType *operator*() const {
      return SETraits::getMostRecentDeclaration(&*SetIter);
    }
    DeclType *operator->() const { return **this; }

    SpecIterator &operator++() { ++SetIter; return *this; }
    SpecIterator operator++(int) {
      SpecIterator tmp(*this);
      ++(*this);
      return tmp;
    }

    bool operator==(SpecIterator Other) const {
      return SetIter == Other.SetIter;
    }
    bool operator!=(SpecIterator Other) const {
      return SetIter != Other.SetIter;
    }
  };

  template <typename EntryType>
  SpecIterator<EntryType> makeSpecIterator(llvm::FoldingSet<EntryType> &Specs,
                                           bool isEnd) {
    return SpecIterator<EntryType>(isEnd ? Specs.end() : Specs.begin());
  }

  template <class EntryType> typename SpecEntryTraits<EntryType>::DeclType*
  findSpecializationImpl(llvm::FoldingSet<EntryType> &Specs,
                         const TemplateArgument *Args, unsigned NumArgs,
                         void *&InsertPos);

  struct CommonBase {
    CommonBase() : InstantiatedFromMember(0, false) { }

    /// \brief The template from which this was most
    /// directly instantiated (or null).
    ///
    /// The boolean value indicates whether this template
    /// was explicitly specialized.
    llvm::PointerIntPair<RedeclarableTemplateDecl*, 1, bool>
      InstantiatedFromMember;

    /// \brief The latest declaration of this template.
    RedeclarableTemplateDecl *Latest;
  };

  /// \brief A pointer to the previous declaration (if this is a redeclaration)
  /// or to the data that is common to all declarations of this template.
  llvm::PointerUnion<CommonBase*, RedeclarableTemplateDecl*> CommonOrPrev;

  /// \brief Retrieves the "common" pointer shared by all (re-)declarations of
  /// the same template. Calling this routine may implicitly allocate memory
  /// for the common pointer.
  CommonBase *getCommonPtr();

  virtual CommonBase *newCommon() = 0;

  // Construct a template decl with name, parameters, and templated element.
  RedeclarableTemplateDecl(Kind DK, DeclContext *DC, SourceLocation L,
                           DeclarationName Name, TemplateParameterList *Params,
                           NamedDecl *Decl)
    : TemplateDecl(DK, DC, L, Name, Params, Decl),
      CommonOrPrev((CommonBase*)0) { }

public:
  template <class decl_type> friend class RedeclarableTemplate;

  RedeclarableTemplateDecl *getCanonicalDecl() {
    return getCanonicalDeclImpl();
  }

  /// \brief Retrieve the previous declaration of this template, or
  /// NULL if no such declaration exists.
  RedeclarableTemplateDecl *getPreviousDeclaration() {
    return getPreviousDeclarationImpl();
  }

  /// \brief Retrieve the previous declaration of this template, or
  /// NULL if no such declaration exists.
  const RedeclarableTemplateDecl *getPreviousDeclaration() const {
    return
      const_cast<RedeclarableTemplateDecl*>(this)->getPreviousDeclaration();
  }

  /// \brief Retrieve the first declaration of this template, or itself
  /// if this the first one.
  RedeclarableTemplateDecl *getFirstDeclaration() {
    return getCanonicalDecl();
  }

  /// \brief Retrieve the first declaration of this template, or itself
  /// if this the first one.
  const RedeclarableTemplateDecl *getFirstDeclaration() const {
    return
      const_cast<RedeclarableTemplateDecl*>(this)->getFirstDeclaration();
  }

  /// \brief Retrieve the most recent declaration of this template, or itself
  /// if this the most recent one.
  RedeclarableTemplateDecl *getMostRecentDeclaration() {
    return getCommonPtr()->Latest;
  }

  /// \brief Retrieve the most recent declaration of this template, or itself
  /// if this the most recent one.
  const RedeclarableTemplateDecl *getMostRecentDeclaration() const {
    return
      const_cast<RedeclarableTemplateDecl*>(this)->getMostRecentDeclaration();
  }

  /// \brief Determines whether this template was a specialization of a 
  /// member template.
  ///
  /// In the following example, the function template \c X<int>::f and the
  /// member template \c X<int>::Inner are member specializations.
  ///
  /// \code
  /// template<typename T>
  /// struct X {
  ///   template<typename U> void f(T, U);
  ///   template<typename U> struct Inner;
  /// };
  ///
  /// template<> template<typename T>
  /// void X<int>::f(int, T);
  /// template<> template<typename T>
  /// struct X<int>::Inner { /* ... */ };
  /// \endcode
  bool isMemberSpecialization() {
    return getCommonPtr()->InstantiatedFromMember.getInt();
  }
  
  /// \brief Note that this member template is a specialization.
  void setMemberSpecialization() {
    assert(getCommonPtr()->InstantiatedFromMember.getPointer() &&
           "Only member templates can be member template specializations");
    getCommonPtr()->InstantiatedFromMember.setInt(true);
  }
  
  /// \brief Retrieve the previous declaration of this template, or
  /// NULL if no such declaration exists.
  RedeclarableTemplateDecl *getInstantiatedFromMemberTemplate() {
    return getInstantiatedFromMemberTemplateImpl();
  }

  virtual RedeclarableTemplateDecl *getNextRedeclaration();

  // Implement isa/cast/dyncast/etc.
  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classof(const RedeclarableTemplateDecl *D) { return true; }
  static bool classof(const FunctionTemplateDecl *D) { return true; }
  static bool classof(const ClassTemplateDecl *D) { return true; }
  static bool classofKind(Kind K) {
    return K >= firstRedeclarableTemplate && K <= lastRedeclarableTemplate;
  }

  friend class ASTDeclReader;
  friend class ASTDeclWriter;
};

template <class decl_type>
class RedeclarableTemplate {
  RedeclarableTemplateDecl *thisDecl() {
    return static_cast<decl_type*>(this);
  }

public:
  /// \brief Retrieve the previous declaration of this function template, or
  /// NULL if no such declaration exists.
  decl_type *getPreviousDeclaration() {
    return static_cast<decl_type*>(thisDecl()->getPreviousDeclarationImpl());
  }

  /// \brief Retrieve the previous declaration of this function template, or
  /// NULL if no such declaration exists.
  const decl_type *getPreviousDeclaration() const {
    return const_cast<RedeclarableTemplate*>(this)->getPreviousDeclaration();
  }

  /// \brief Set the previous declaration of this function template.
  void setPreviousDeclaration(decl_type *Prev) {
    thisDecl()->setPreviousDeclarationImpl(Prev);
  }

  decl_type *getCanonicalDecl() {
    return static_cast<decl_type*>(thisDecl()->getCanonicalDeclImpl());
  }

  const decl_type *getCanonicalDecl() const {
    return const_cast<RedeclarableTemplate*>(this)->getCanonicalDecl();
  }

  /// \brief Retrieve the member template that this template was instantiated
  /// from.
  ///
  /// This routine will return non-NULL for member templates of
  /// class templates.  For example, given:
  ///
  /// \code
  /// template <typename T>
  /// struct X {
  ///   template <typename U> void f();
  ///   template <typename U> struct A {};
  /// };
  /// \endcode
  ///
  /// X<int>::f<float> is a CXXMethodDecl (whose parent is X<int>, a
  /// ClassTemplateSpecializationDecl) for which getPrimaryTemplate() will
  /// return X<int>::f, a FunctionTemplateDecl (whose parent is again
  /// X<int>) for which getInstantiatedFromMemberTemplate() will return
  /// X<T>::f, a FunctionTemplateDecl (whose parent is X<T>, a
  /// ClassTemplateDecl).
  ///
  /// X<int>::A<float> is a ClassTemplateSpecializationDecl (whose parent
  /// is X<int>, also a CTSD) for which getSpecializedTemplate() will
  /// return X<int>::A<U>, a ClassTemplateDecl (whose parent is again
  /// X<int>) for which getInstantiatedFromMemberTemplate() will return
  /// X<T>::A<U>, a ClassTemplateDecl (whose parent is X<T>, also a CTD).
  ///
  /// \returns NULL if this is not an instantiation of a member template.
  decl_type *getInstantiatedFromMemberTemplate() {
    return static_cast<decl_type*>(
             thisDecl()->getInstantiatedFromMemberTemplateImpl());
  }

  void setInstantiatedFromMemberTemplate(decl_type *TD) {
    thisDecl()->setInstantiatedFromMemberTemplateImpl(TD);
  }
};

template <> struct RedeclarableTemplateDecl::
SpecEntryTraits<FunctionTemplateSpecializationInfo> {
  typedef FunctionDecl DeclType;

  static DeclType *
  getMostRecentDeclaration(FunctionTemplateSpecializationInfo *I) {
    return I->Function->getMostRecentDeclaration();
  }
};

/// Declaration of a template function.
class FunctionTemplateDecl : public RedeclarableTemplateDecl,
                             public RedeclarableTemplate<FunctionTemplateDecl> {
  static void DeallocateCommon(void *Ptr);

protected:
  typedef RedeclarableTemplate<FunctionTemplateDecl> redeclarable_base;

  /// \brief Data that is common to all of the declarations of a given
  /// function template.
  struct Common : CommonBase {
    /// \brief The function template specializations for this function
    /// template, including explicit specializations and instantiations.
    llvm::FoldingSet<FunctionTemplateSpecializationInfo> Specializations;
  };

  FunctionTemplateDecl(DeclContext *DC, SourceLocation L, DeclarationName Name,
                       TemplateParameterList *Params, NamedDecl *Decl)
    : RedeclarableTemplateDecl(FunctionTemplate, DC, L, Name, Params, Decl) { }

  CommonBase *newCommon();

  Common *getCommonPtr() {
    return static_cast<Common *>(RedeclarableTemplateDecl::getCommonPtr());
  }

  friend void FunctionDecl::setFunctionTemplateSpecialization(
                                       FunctionTemplateDecl *Template,
                                       const TemplateArgumentList *TemplateArgs,
                                       void *InsertPos,
                                       TemplateSpecializationKind TSK,
                          const TemplateArgumentListInfo *TemplateArgsAsWritten,
                                       SourceLocation PointOfInstantiation);

  /// \brief Retrieve the set of function template specializations of this
  /// function template.
  llvm::FoldingSet<FunctionTemplateSpecializationInfo> &getSpecializations() {
    return getCommonPtr()->Specializations;
  }
  
public:
  /// Get the underlying function declaration of the template.
  FunctionDecl *getTemplatedDecl() const {
    return static_cast<FunctionDecl*>(TemplatedDecl);
  }

  /// Returns whether this template declaration defines the primary
  /// pattern.
  bool isThisDeclarationADefinition() const {
    return getTemplatedDecl()->isThisDeclarationADefinition();
  }

  /// \brief Return the specialization with the provided arguments if it exists,
  /// otherwise return the insertion point.
  FunctionDecl *findSpecialization(const TemplateArgument *Args,
                                   unsigned NumArgs, void *&InsertPos);

  FunctionTemplateDecl *getCanonicalDecl() {
    return redeclarable_base::getCanonicalDecl();
  }
  const FunctionTemplateDecl *getCanonicalDecl() const {
    return redeclarable_base::getCanonicalDecl();
  }

  /// \brief Retrieve the previous declaration of this function template, or
  /// NULL if no such declaration exists.
  FunctionTemplateDecl *getPreviousDeclaration() {
    return redeclarable_base::getPreviousDeclaration();
  }

  /// \brief Retrieve the previous declaration of this function template, or
  /// NULL if no such declaration exists.
  const FunctionTemplateDecl *getPreviousDeclaration() const {
    return redeclarable_base::getPreviousDeclaration();
  }

  FunctionTemplateDecl *getInstantiatedFromMemberTemplate() {
    return redeclarable_base::getInstantiatedFromMemberTemplate();
  }

  typedef SpecIterator<FunctionTemplateSpecializationInfo> spec_iterator;

  spec_iterator spec_begin() {
    return makeSpecIterator(getSpecializations(), false);
  }

  spec_iterator spec_end() {
    return makeSpecIterator(getSpecializations(), true);
  }

  /// Create a template function node.
  static FunctionTemplateDecl *Create(ASTContext &C, DeclContext *DC,
                                      SourceLocation L,
                                      DeclarationName Name,
                                      TemplateParameterList *Params,
                                      NamedDecl *Decl);

  // Implement isa/cast/dyncast support
  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classof(const FunctionTemplateDecl *D) { return true; }
  static bool classofKind(Kind K) { return K == FunctionTemplate; }

  friend class ASTDeclReader;
  friend class ASTDeclWriter;
};

//===----------------------------------------------------------------------===//
// Kinds of Template Parameters
//===----------------------------------------------------------------------===//

/// The TemplateParmPosition class defines the position of a template parameter
/// within a template parameter list. Because template parameter can be listed
/// sequentially for out-of-line template members, each template parameter is
/// given a Depth - the nesting of template parameter scopes - and a Position -
/// the occurrence within the parameter list.
/// This class is inheritedly privately by different kinds of template
/// parameters and is not part of the Decl hierarchy. Just a facility.
class TemplateParmPosition {
protected:
  // FIXME: This should probably never be called, but it's here as
  TemplateParmPosition()
    : Depth(0), Position(0)
  { /* assert(0 && "Cannot create positionless template parameter"); */ }

  TemplateParmPosition(unsigned D, unsigned P)
    : Depth(D), Position(P)
  { }

  // FIXME: These probably don't need to be ints. int:5 for depth, int:8 for
  // position? Maybe?
  unsigned Depth;
  unsigned Position;

public:
  /// Get the nesting depth of the template parameter.
  unsigned getDepth() const { return Depth; }
  void setDepth(unsigned D) { Depth = D; }

  /// Get the position of the template parameter within its parameter list.
  unsigned getPosition() const { return Position; }
  void setPosition(unsigned P) { Position = P; }

  /// Get the index of the template parameter within its parameter list.
  unsigned getIndex() const { return Position; }
};

/// TemplateTypeParmDecl - Declaration of a template type parameter,
/// e.g., "T" in
/// @code
/// template<typename T> class vector;
/// @endcode
class TemplateTypeParmDecl : public TypeDecl {
  /// \brief Whether this template type parameter was declaration with
  /// the 'typename' keyword. If false, it was declared with the
  /// 'class' keyword.
  bool Typename : 1;

  /// \brief Whether this template type parameter inherited its
  /// default argument.
  bool InheritedDefault : 1;

  /// \brief Whether this is a parameter pack.
  bool ParameterPack : 1;

  /// \brief The default template argument, if any.
  TypeSourceInfo *DefaultArgument;

  TemplateTypeParmDecl(DeclContext *DC, SourceLocation L, IdentifierInfo *Id,
                       bool Typename, QualType Type, bool ParameterPack)
    : TypeDecl(TemplateTypeParm, DC, L, Id), Typename(Typename),
      InheritedDefault(false), ParameterPack(ParameterPack), DefaultArgument() {
    TypeForDecl = Type.getTypePtr();
  }

public:
  static TemplateTypeParmDecl *Create(ASTContext &C, DeclContext *DC,
                                      SourceLocation L, unsigned D, unsigned P,
                                      IdentifierInfo *Id, bool Typename,
                                      bool ParameterPack);
  static TemplateTypeParmDecl *Create(ASTContext &C, EmptyShell Empty);

  /// \brief Whether this template type parameter was declared with
  /// the 'typename' keyword. If not, it was declared with the 'class'
  /// keyword.
  bool wasDeclaredWithTypename() const { return Typename; }

  /// \brief Determine whether this template parameter has a default
  /// argument.
  bool hasDefaultArgument() const { return DefaultArgument != 0; }

  /// \brief Retrieve the default argument, if any.
  QualType getDefaultArgument() const { return DefaultArgument->getType(); }

  /// \brief Retrieves the default argument's source information, if any.
  TypeSourceInfo *getDefaultArgumentInfo() const { return DefaultArgument; }

  /// \brief Retrieves the location of the default argument declaration.
  SourceLocation getDefaultArgumentLoc() const;

  /// \brief Determines whether the default argument was inherited
  /// from a previous declaration of this template.
  bool defaultArgumentWasInherited() const { return InheritedDefault; }

  /// \brief Set the default argument for this template parameter, and
  /// whether that default argument was inherited from another
  /// declaration.
  void setDefaultArgument(TypeSourceInfo *DefArg, bool Inherited) {
    DefaultArgument = DefArg;
    InheritedDefault = Inherited;
  }

  /// \brief Removes the default argument of this template parameter.
  void removeDefaultArgument() {
    DefaultArgument = 0;
    InheritedDefault = false;
  }
  
  /// \brief Set whether this template type parameter was declared with
  /// the 'typename' or 'class' keyword.
  void setDeclaredWithTypename(bool withTypename) { Typename = withTypename; }

  /// \brief Set whether this is a parameter pack.
  void setParameterPack(bool isParamPack) { ParameterPack = isParamPack; }

  /// \brief Retrieve the depth of the template parameter.
  unsigned getDepth() const;
  
  /// \brief Retrieve the index of the template parameter.
  unsigned getIndex() const;

  /// \brief Returns whether this is a parameter pack.
  bool isParameterPack() const { return ParameterPack; }

  // Implement isa/cast/dyncast/etc.
  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classof(const TemplateTypeParmDecl *D) { return true; }
  static bool classofKind(Kind K) { return K == TemplateTypeParm; }
};

/// NonTypeTemplateParmDecl - Declares a non-type template parameter,
/// e.g., "Size" in
/// @code
/// template<int Size> class array { };
/// @endcode
class NonTypeTemplateParmDecl
  : public VarDecl, protected TemplateParmPosition {
  /// \brief The default template argument, if any, and whether or not
  /// it was inherited.
  llvm::PointerIntPair<Expr*, 1, bool> DefaultArgumentAndInherited;

  NonTypeTemplateParmDecl(DeclContext *DC, SourceLocation L, unsigned D,
                          unsigned P, IdentifierInfo *Id, QualType T,
                          TypeSourceInfo *TInfo)
    : VarDecl(NonTypeTemplateParm, DC, L, Id, T, TInfo, SC_None, SC_None),
      TemplateParmPosition(D, P), DefaultArgumentAndInherited(0, false)
  { }

public:
  static NonTypeTemplateParmDecl *
  Create(ASTContext &C, DeclContext *DC, SourceLocation L, unsigned D,
         unsigned P, IdentifierInfo *Id, QualType T, TypeSourceInfo *TInfo);

  using TemplateParmPosition::getDepth;
  using TemplateParmPosition::setDepth;
  using TemplateParmPosition::getPosition;
  using TemplateParmPosition::setPosition;
  using TemplateParmPosition::getIndex;

  /// \brief Determine whether this template parameter has a default
  /// argument.
  bool hasDefaultArgument() const {
    return DefaultArgumentAndInherited.getPointer() != 0;
  }

  /// \brief Retrieve the default argument, if any.
  Expr *getDefaultArgument() const {
    return DefaultArgumentAndInherited.getPointer();
  }

  /// \brief Retrieve the location of the default argument, if any.
  SourceLocation getDefaultArgumentLoc() const;

  /// \brief Determines whether the default argument was inherited
  /// from a previous declaration of this template.
  bool defaultArgumentWasInherited() const {
    return DefaultArgumentAndInherited.getInt();
  }

  /// \brief Set the default argument for this template parameter, and
  /// whether that default argument was inherited from another
  /// declaration.
  void setDefaultArgument(Expr *DefArg, bool Inherited) {
    DefaultArgumentAndInherited.setPointer(DefArg);
    DefaultArgumentAndInherited.setInt(Inherited);
  }

  /// \brief Removes the default argument of this template parameter.
  void removeDefaultArgument() {
    DefaultArgumentAndInherited.setPointer(0);
    DefaultArgumentAndInherited.setInt(false);
  }

  // Implement isa/cast/dyncast/etc.
  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classof(const NonTypeTemplateParmDecl *D) { return true; }
  static bool classofKind(Kind K) { return K == NonTypeTemplateParm; }
};

/// TemplateTemplateParmDecl - Declares a template template parameter,
/// e.g., "T" in
/// @code
/// template <template <typename> class T> class container { };
/// @endcode
/// A template template parameter is a TemplateDecl because it defines the
/// name of a template and the template parameters allowable for substitution.
class TemplateTemplateParmDecl
  : public TemplateDecl, protected TemplateParmPosition {

  /// DefaultArgument - The default template argument, if any.
  TemplateArgumentLoc DefaultArgument;
  /// Whether or not the default argument was inherited.
  bool DefaultArgumentWasInherited;

  TemplateTemplateParmDecl(DeclContext *DC, SourceLocation L,
                           unsigned D, unsigned P,
                           IdentifierInfo *Id, TemplateParameterList *Params)
    : TemplateDecl(TemplateTemplateParm, DC, L, Id, Params),
      TemplateParmPosition(D, P), DefaultArgument(),
      DefaultArgumentWasInherited(false)
    { }

public:
  static TemplateTemplateParmDecl *Create(ASTContext &C, DeclContext *DC,
                                          SourceLocation L, unsigned D,
                                          unsigned P, IdentifierInfo *Id,
                                          TemplateParameterList *Params);

  using TemplateParmPosition::getDepth;
  using TemplateParmPosition::getPosition;
  using TemplateParmPosition::getIndex;

  /// \brief Determine whether this template parameter has a default
  /// argument.
  bool hasDefaultArgument() const {
    return !DefaultArgument.getArgument().isNull();
  }

  /// \brief Retrieve the default argument, if any.
  const TemplateArgumentLoc &getDefaultArgument() const {
    return DefaultArgument;
  }

  /// \brief Retrieve the location of the default argument, if any.
  SourceLocation getDefaultArgumentLoc() const;

  /// \brief Determines whether the default argument was inherited
  /// from a previous declaration of this template.
  bool defaultArgumentWasInherited() const {
    return DefaultArgumentWasInherited;
  }

  /// \brief Set the default argument for this template parameter, and
  /// whether that default argument was inherited from another
  /// declaration.
  void setDefaultArgument(const TemplateArgumentLoc &DefArg, bool Inherited) {
    DefaultArgument = DefArg;
    DefaultArgumentWasInherited = Inherited;
  }

  /// \brief Removes the default argument of this template parameter.
  void removeDefaultArgument() {
    DefaultArgument = TemplateArgumentLoc();
    DefaultArgumentWasInherited = false;
  }

  SourceRange getSourceRange() const {
    SourceLocation End = getLocation();
    if (hasDefaultArgument() && !defaultArgumentWasInherited())
      End = getDefaultArgument().getSourceRange().getEnd();
    return SourceRange(getTemplateParameters()->getTemplateLoc(), End);
  }

  // Implement isa/cast/dyncast/etc.
  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classof(const TemplateTemplateParmDecl *D) { return true; }
  static bool classofKind(Kind K) { return K == TemplateTemplateParm; }

  friend class ASTDeclReader;
  friend class ASTDeclWriter;
};

/// \brief Represents a class template specialization, which refers to
/// a class template with a given set of template arguments.
///
/// Class template specializations represent both explicit
/// specialization of class templates, as in the example below, and
/// implicit instantiations of class templates.
///
/// \code
/// template<typename T> class array;
///
/// template<>
/// class array<bool> { }; // class template specialization array<bool>
/// \endcode
class ClassTemplateSpecializationDecl
  : public CXXRecordDecl, public llvm::FoldingSetNode {

  /// \brief Structure that stores information about a class template
  /// specialization that was instantiated from a class template partial
  /// specialization.
  struct SpecializedPartialSpecialization {
    /// \brief The class template partial specialization from which this
    /// class template specialization was instantiated.
    ClassTemplatePartialSpecializationDecl *PartialSpecialization;

    /// \brief The template argument list deduced for the class template
    /// partial specialization itself.
    TemplateArgumentList *TemplateArgs;
  };

  /// \brief The template that this specialization specializes
  llvm::PointerUnion<ClassTemplateDecl *, SpecializedPartialSpecialization *>
    SpecializedTemplate;

  /// \brief Further info for explicit template specialization/instantiation.
  struct ExplicitSpecializationInfo {
    /// \brief The type-as-written.
    TypeSourceInfo *TypeAsWritten;
    /// \brief The location of the extern keyword.
    SourceLocation ExternLoc;
    /// \brief The location of the template keyword.
    SourceLocation TemplateKeywordLoc;

    ExplicitSpecializationInfo()
      : TypeAsWritten(0), ExternLoc(), TemplateKeywordLoc() {}
  };

  /// \brief Further info for explicit template specialization/instantiation.
  /// Does not apply to implicit specializations.
  ExplicitSpecializationInfo *ExplicitInfo;

  /// \brief The template arguments used to describe this specialization.
  TemplateArgumentList TemplateArgs;

  /// \brief The point where this template was instantiated (if any)
  SourceLocation PointOfInstantiation;

  /// \brief The kind of specialization this declaration refers to.
  /// Really a value of type TemplateSpecializationKind.
  unsigned SpecializationKind : 3;

protected:
  ClassTemplateSpecializationDecl(ASTContext &Context, Kind DK, TagKind TK,
                                  DeclContext *DC, SourceLocation L,
                                  ClassTemplateDecl *SpecializedTemplate,
                                  TemplateArgumentListBuilder &Builder,
                                  ClassTemplateSpecializationDecl *PrevDecl);

  explicit ClassTemplateSpecializationDecl(Kind DK);

public:
  static ClassTemplateSpecializationDecl *
  Create(ASTContext &Context, TagKind TK, DeclContext *DC, SourceLocation L,
         ClassTemplateDecl *SpecializedTemplate,
         TemplateArgumentListBuilder &Builder,
         ClassTemplateSpecializationDecl *PrevDecl);
  static ClassTemplateSpecializationDecl *
  Create(ASTContext &Context, EmptyShell Empty);

  virtual void getNameForDiagnostic(std::string &S,
                                    const PrintingPolicy &Policy,
                                    bool Qualified) const;

  ClassTemplateSpecializationDecl *getMostRecentDeclaration() {
    CXXRecordDecl *Recent
        = cast<CXXRecordDecl>(CXXRecordDecl::getMostRecentDeclaration());
    if (!isa<ClassTemplateSpecializationDecl>(Recent)) {
      // FIXME: Does injected class name need to be in the redeclarations chain?
      assert(Recent->isInjectedClassName() && Recent->getPreviousDeclaration());
      Recent = Recent->getPreviousDeclaration();
    }
    return cast<ClassTemplateSpecializationDecl>(Recent);
  }

  /// \brief Retrieve the template that this specialization specializes.
  ClassTemplateDecl *getSpecializedTemplate() const;

  /// \brief Retrieve the template arguments of the class template
  /// specialization.
  const TemplateArgumentList &getTemplateArgs() const {
    return TemplateArgs;
  }

  /// \brief Initialize the template arguments of the class template
  /// specialization.
  void initTemplateArgs(TemplateArgument *Args, unsigned NumArgs) {
    assert(TemplateArgs.flat_size() == 0 &&
           "Template arguments already initialized!");
    TemplateArgs.init(getASTContext(), Args, NumArgs);
  }

  /// \brief Determine the kind of specialization that this
  /// declaration represents.
  TemplateSpecializationKind getSpecializationKind() const {
    return static_cast<TemplateSpecializationKind>(SpecializationKind);
  }

  void setSpecializationKind(TemplateSpecializationKind TSK) {
    SpecializationKind = TSK;
  }

  /// \brief Get the point of instantiation (if any), or null if none.
  SourceLocation getPointOfInstantiation() const {
    return PointOfInstantiation;
  }

  void setPointOfInstantiation(SourceLocation Loc) {
    assert(Loc.isValid() && "point of instantiation must be valid!");
    PointOfInstantiation = Loc;
  }

  /// \brief If this class template specialization is an instantiation of
  /// a template (rather than an explicit specialization), return the
  /// class template or class template partial specialization from which it
  /// was instantiated.
  llvm::PointerUnion<ClassTemplateDecl *,
                     ClassTemplatePartialSpecializationDecl *>
  getInstantiatedFrom() const {
    if (getSpecializationKind() != TSK_ImplicitInstantiation &&
        getSpecializationKind() != TSK_ExplicitInstantiationDefinition &&
        getSpecializationKind() != TSK_ExplicitInstantiationDeclaration)
      return llvm::PointerUnion<ClassTemplateDecl *,
                                ClassTemplatePartialSpecializationDecl *>();

    if (SpecializedPartialSpecialization *PartialSpec
          = SpecializedTemplate.dyn_cast<SpecializedPartialSpecialization*>())
      return PartialSpec->PartialSpecialization;

    return const_cast<ClassTemplateDecl*>(
                             SpecializedTemplate.get<ClassTemplateDecl*>());
  }

  /// \brief Retrieve the class template or class template partial
  /// specialization which was specialized by this.
  llvm::PointerUnion<ClassTemplateDecl *,
                     ClassTemplatePartialSpecializationDecl *>
  getSpecializedTemplateOrPartial() const {
    if (SpecializedPartialSpecialization *PartialSpec
          = SpecializedTemplate.dyn_cast<SpecializedPartialSpecialization*>())
      return PartialSpec->PartialSpecialization;

    return const_cast<ClassTemplateDecl*>(
                             SpecializedTemplate.get<ClassTemplateDecl*>());
  }

  /// \brief Retrieve the set of template arguments that should be used
  /// to instantiate members of the class template or class template partial
  /// specialization from which this class template specialization was
  /// instantiated.
  ///
  /// \returns For a class template specialization instantiated from the primary
  /// template, this function will return the same template arguments as
  /// getTemplateArgs(). For a class template specialization instantiated from
  /// a class template partial specialization, this function will return the
  /// deduced template arguments for the class template partial specialization
  /// itself.
  const TemplateArgumentList &getTemplateInstantiationArgs() const {
    if (SpecializedPartialSpecialization *PartialSpec
        = SpecializedTemplate.dyn_cast<SpecializedPartialSpecialization*>())
      return *PartialSpec->TemplateArgs;

    return getTemplateArgs();
  }

  /// \brief Note that this class template specialization is actually an
  /// instantiation of the given class template partial specialization whose
  /// template arguments have been deduced.
  void setInstantiationOf(ClassTemplatePartialSpecializationDecl *PartialSpec,
                          TemplateArgumentList *TemplateArgs) {
    assert(!SpecializedTemplate.is<SpecializedPartialSpecialization*>() &&
           "Already set to a class template partial specialization!");
    SpecializedPartialSpecialization *PS
      = new (getASTContext()) SpecializedPartialSpecialization();
    PS->PartialSpecialization = PartialSpec;
    PS->TemplateArgs = TemplateArgs;
    SpecializedTemplate = PS;
  }

  /// \brief Note that this class template specialization is actually an
  /// instantiation of the given class template partial specialization whose
  /// template arguments have been deduced.
  void setInstantiationOf(ClassTemplatePartialSpecializationDecl *PartialSpec,
                          TemplateArgument *TemplateArgs,
                          unsigned NumTemplateArgs) {
    ASTContext &Ctx = getASTContext();
    setInstantiationOf(PartialSpec,
                       new (Ctx) TemplateArgumentList(Ctx, TemplateArgs,
                                                      NumTemplateArgs));
  }

  /// \brief Note that this class template specialization is an instantiation
  /// of the given class template.
  void setInstantiationOf(ClassTemplateDecl *TemplDecl) {
    assert(!SpecializedTemplate.is<SpecializedPartialSpecialization*>() &&
           "Previously set to a class template partial specialization!");
    SpecializedTemplate = TemplDecl;
  }

  /// \brief Sets the type of this specialization as it was written by
  /// the user. This will be a class template specialization type.
  void setTypeAsWritten(TypeSourceInfo *T) {
    if (!ExplicitInfo)
      ExplicitInfo = new (getASTContext()) ExplicitSpecializationInfo;
    ExplicitInfo->TypeAsWritten = T;
  }
  /// \brief Gets the type of this specialization as it was written by
  /// the user, if it was so written.
  TypeSourceInfo *getTypeAsWritten() const {
    return ExplicitInfo ? ExplicitInfo->TypeAsWritten : 0;
  }

  /// \brief Gets the location of the extern keyword, if present.
  SourceLocation getExternLoc() const {
    return ExplicitInfo ? ExplicitInfo->ExternLoc : SourceLocation();
  }
  /// \brief Sets the location of the extern keyword.
  void setExternLoc(SourceLocation Loc) {
    if (!ExplicitInfo)
      ExplicitInfo = new (getASTContext()) ExplicitSpecializationInfo;
    ExplicitInfo->ExternLoc = Loc;
  }

  /// \brief Sets the location of the template keyword.
  void setTemplateKeywordLoc(SourceLocation Loc) {
    if (!ExplicitInfo)
      ExplicitInfo = new (getASTContext()) ExplicitSpecializationInfo;
    ExplicitInfo->TemplateKeywordLoc = Loc;
  }
  /// \brief Gets the location of the template keyword, if present.
  SourceLocation getTemplateKeywordLoc() const {
    return ExplicitInfo ? ExplicitInfo->TemplateKeywordLoc : SourceLocation();
  }

  SourceLocation getInnerLocStart() const { return getTemplateKeywordLoc(); }

  void Profile(llvm::FoldingSetNodeID &ID) const {
    Profile(ID, TemplateArgs.getFlatArgumentList(), TemplateArgs.flat_size(),
            getASTContext());
  }

  static void
  Profile(llvm::FoldingSetNodeID &ID, const TemplateArgument *TemplateArgs,
          unsigned NumTemplateArgs, ASTContext &Context) {
    ID.AddInteger(NumTemplateArgs);
    for (unsigned Arg = 0; Arg != NumTemplateArgs; ++Arg)
      TemplateArgs[Arg].Profile(ID, Context);
  }

  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classofKind(Kind K) {
    return K >= firstClassTemplateSpecialization &&
           K <= lastClassTemplateSpecialization;
  }

  static bool classof(const ClassTemplateSpecializationDecl *) {
    return true;
  }

  static bool classof(const ClassTemplatePartialSpecializationDecl *) {
    return true;
  }
};

class ClassTemplatePartialSpecializationDecl
  : public ClassTemplateSpecializationDecl {
  /// \brief The list of template parameters
  TemplateParameterList* TemplateParams;

  /// \brief The source info for the template arguments as written.
  /// FIXME: redundant with TypeAsWritten?
  TemplateArgumentLoc *ArgsAsWritten;
  unsigned NumArgsAsWritten;

  /// \brief Sequence number indicating when this class template partial
  /// specialization was added to the set of partial specializations for
  /// its owning class template.
  unsigned SequenceNumber;
    
  /// \brief The class template partial specialization from which this 
  /// class template partial specialization was instantiated.
  ///
  /// The boolean value will be true to indicate that this class template
  /// partial specialization was specialized at this level.
  llvm::PointerIntPair<ClassTemplatePartialSpecializationDecl *, 1, bool>
      InstantiatedFromMember;
    
  ClassTemplatePartialSpecializationDecl(ASTContext &Context, TagKind TK,
                                         DeclContext *DC, SourceLocation L,
                                         TemplateParameterList *Params,
                                         ClassTemplateDecl *SpecializedTemplate,
                                         TemplateArgumentListBuilder &Builder,
                                         TemplateArgumentLoc *ArgInfos,
                                         unsigned NumArgInfos,
                               ClassTemplatePartialSpecializationDecl *PrevDecl,
                                         unsigned SequenceNumber)
    : ClassTemplateSpecializationDecl(Context,
                                      ClassTemplatePartialSpecialization,
                                      TK, DC, L, SpecializedTemplate, Builder,
                                      PrevDecl),
      TemplateParams(Params), ArgsAsWritten(ArgInfos),
      NumArgsAsWritten(NumArgInfos), SequenceNumber(SequenceNumber),
      InstantiatedFromMember(0, false) { }
  
  ClassTemplatePartialSpecializationDecl()
    : ClassTemplateSpecializationDecl(ClassTemplatePartialSpecialization),
      TemplateParams(0), ArgsAsWritten(0),
      NumArgsAsWritten(0), SequenceNumber(0),
      InstantiatedFromMember(0, false) { }

public:
  static ClassTemplatePartialSpecializationDecl *
  Create(ASTContext &Context, TagKind TK,DeclContext *DC, SourceLocation L,
         TemplateParameterList *Params,
         ClassTemplateDecl *SpecializedTemplate,
         TemplateArgumentListBuilder &Builder,
         const TemplateArgumentListInfo &ArgInfos,
         QualType CanonInjectedType,
         ClassTemplatePartialSpecializationDecl *PrevDecl,
         unsigned SequenceNumber);

  static ClassTemplatePartialSpecializationDecl *
  Create(ASTContext &Context, EmptyShell Empty);

  ClassTemplatePartialSpecializationDecl *getMostRecentDeclaration() {
    return cast<ClassTemplatePartialSpecializationDecl>(
                   ClassTemplateSpecializationDecl::getMostRecentDeclaration());
  }

  /// Get the list of template parameters
  TemplateParameterList *getTemplateParameters() const {
    return TemplateParams;
  }

  void initTemplateParameters(TemplateParameterList *Params) {
    assert(TemplateParams == 0 && "TemplateParams already set");
    TemplateParams = Params;
  }

  /// Get the template arguments as written.
  TemplateArgumentLoc *getTemplateArgsAsWritten() const {
    return ArgsAsWritten;
  }

  void initTemplateArgsAsWritten(const TemplateArgumentListInfo &ArgInfos);

  /// Get the number of template arguments as written.
  unsigned getNumTemplateArgsAsWritten() const {
    return NumArgsAsWritten;
  }

  /// \brief Get the sequence number for this class template partial
  /// specialization.
  unsigned getSequenceNumber() const { return SequenceNumber; }
  void setSequenceNumber(unsigned N) { SequenceNumber = N; }
    
  /// \brief Retrieve the member class template partial specialization from
  /// which this particular class template partial specialization was
  /// instantiated.
  ///
  /// \code
  /// template<typename T>
  /// struct Outer {
  ///   template<typename U> struct Inner;
  ///   template<typename U> struct Inner<U*> { }; // #1
  /// };
  ///
  /// Outer<float>::Inner<int*> ii;
  /// \endcode
  ///
  /// In this example, the instantiation of \c Outer<float>::Inner<int*> will
  /// end up instantiating the partial specialization 
  /// \c Outer<float>::Inner<U*>, which itself was instantiated from the class 
  /// template partial specialization \c Outer<T>::Inner<U*>. Given 
  /// \c Outer<float>::Inner<U*>, this function would return
  /// \c Outer<T>::Inner<U*>.
  ClassTemplatePartialSpecializationDecl *getInstantiatedFromMember() {
    ClassTemplatePartialSpecializationDecl *First
      = cast<ClassTemplatePartialSpecializationDecl>(getFirstDeclaration());
    return First->InstantiatedFromMember.getPointer();
  }
  
  void setInstantiatedFromMember(
                          ClassTemplatePartialSpecializationDecl *PartialSpec) {
    ClassTemplatePartialSpecializationDecl *First
      = cast<ClassTemplatePartialSpecializationDecl>(getFirstDeclaration());
    First->InstantiatedFromMember.setPointer(PartialSpec);
  }
    
  /// \brief Determines whether this class template partial specialization 
  /// template was a specialization of a member partial specialization.
  ///
  /// In the following example, the member template partial specialization
  /// \c X<int>::Inner<T*> is a member specialization.
  ///
  /// \code
  /// template<typename T>
  /// struct X {
  ///   template<typename U> struct Inner;
  ///   template<typename U> struct Inner<U*>;
  /// };
  ///
  /// template<> template<typename T>
  /// struct X<int>::Inner<T*> { /* ... */ };
  /// \endcode
  bool isMemberSpecialization() {
    ClassTemplatePartialSpecializationDecl *First
      = cast<ClassTemplatePartialSpecializationDecl>(getFirstDeclaration());
    return First->InstantiatedFromMember.getInt();
  }
  
  /// \brief Note that this member template is a specialization.
  void setMemberSpecialization() {
    ClassTemplatePartialSpecializationDecl *First
      = cast<ClassTemplatePartialSpecializationDecl>(getFirstDeclaration());
    assert(First->InstantiatedFromMember.getPointer() &&
           "Only member templates can be member template specializations");
    return First->InstantiatedFromMember.setInt(true);
  }

  /// Retrieves the injected specialization type for this partial
  /// specialization.  This is not the same as the type-decl-type for
  /// this partial specialization, which is an InjectedClassNameType.
  QualType getInjectedSpecializationType() const {
    assert(getTypeForDecl() && "partial specialization has no type set!");
    return cast<InjectedClassNameType>(getTypeForDecl())
             ->getInjectedSpecializationType();
  }
    
  // FIXME: Add Profile support!

  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classofKind(Kind K) {
    return K == ClassTemplatePartialSpecialization;
  }

  static bool classof(const ClassTemplatePartialSpecializationDecl *) {
    return true;
  }
};

/// Declaration of a class template.
class ClassTemplateDecl : public RedeclarableTemplateDecl,
                          public RedeclarableTemplate<ClassTemplateDecl> {
  static void DeallocateCommon(void *Ptr);
  
protected:
  typedef RedeclarableTemplate<ClassTemplateDecl> redeclarable_base;

  /// \brief Data that is common to all of the declarations of a given
  /// class template.
  struct Common : CommonBase {
    /// \brief The class template specializations for this class
    /// template, including explicit specializations and instantiations.
    llvm::FoldingSet<ClassTemplateSpecializationDecl> Specializations;

    /// \brief The class template partial specializations for this class
    /// template.
    llvm::FoldingSet<ClassTemplatePartialSpecializationDecl>
      PartialSpecializations;

    /// \brief The injected-class-name type for this class template.
    QualType InjectedClassNameType;
  };

  /// \brief Retrieve the set of specializations of this class template.
  llvm::FoldingSet<ClassTemplateSpecializationDecl> &getSpecializations() {
    return getCommonPtr()->Specializations;
  }

  /// \brief Retrieve the set of partial specializations of this class
  /// template.
  llvm::FoldingSet<ClassTemplatePartialSpecializationDecl> &
  getPartialSpecializations() {
    return getCommonPtr()->PartialSpecializations;
  }

  ClassTemplateDecl(DeclContext *DC, SourceLocation L, DeclarationName Name,
                    TemplateParameterList *Params, NamedDecl *Decl)
    : RedeclarableTemplateDecl(ClassTemplate, DC, L, Name, Params, Decl) { }

  CommonBase *newCommon();

  Common *getCommonPtr() {
    return static_cast<Common *>(RedeclarableTemplateDecl::getCommonPtr());
  }

public:
  /// Get the underlying class declarations of the template.
  CXXRecordDecl *getTemplatedDecl() const {
    return static_cast<CXXRecordDecl *>(TemplatedDecl);
  }

  /// Returns whether this template declaration defines the primary
  /// class pattern.
  bool isThisDeclarationADefinition() const {
    return getTemplatedDecl()->isThisDeclarationADefinition();
  }

  /// Create a class template node.
  static ClassTemplateDecl *Create(ASTContext &C, DeclContext *DC,
                                   SourceLocation L,
                                   DeclarationName Name,
                                   TemplateParameterList *Params,
                                   NamedDecl *Decl,
                                   ClassTemplateDecl *PrevDecl);

  /// \brief Return the specialization with the provided arguments if it exists,
  /// otherwise return the insertion point.
  ClassTemplateSpecializationDecl *
  findSpecialization(const TemplateArgument *Args, unsigned NumArgs,
                     void *&InsertPos);

  /// \brief Insert the specified specialization knowing that it is not already
  /// in. InsertPos must be obtained from findSpecialization.
  void AddSpecialization(ClassTemplateSpecializationDecl *D, void *InsertPos) {
    getSpecializations().InsertNode(D, InsertPos);
  }

  ClassTemplateDecl *getCanonicalDecl() {
    return redeclarable_base::getCanonicalDecl();
  }
  const ClassTemplateDecl *getCanonicalDecl() const {
    return redeclarable_base::getCanonicalDecl();
  }

  /// \brief Retrieve the previous declaration of this class template, or
  /// NULL if no such declaration exists.
  ClassTemplateDecl *getPreviousDeclaration() {
    return redeclarable_base::getPreviousDeclaration();
  }

  /// \brief Retrieve the previous declaration of this class template, or
  /// NULL if no such declaration exists.
  const ClassTemplateDecl *getPreviousDeclaration() const {
    return redeclarable_base::getPreviousDeclaration();
  }

  ClassTemplateDecl *getInstantiatedFromMemberTemplate() {
    return redeclarable_base::getInstantiatedFromMemberTemplate();
  }

  /// \brief Return the partial specialization with the provided arguments if it
  /// exists, otherwise return the insertion point.
  ClassTemplatePartialSpecializationDecl *
  findPartialSpecialization(const TemplateArgument *Args, unsigned NumArgs,
                            void *&InsertPos);

  /// \brief Insert the specified partial specialization knowing that it is not
  /// already in. InsertPos must be obtained from findPartialSpecialization.
  void AddPartialSpecialization(ClassTemplatePartialSpecializationDecl *D,
                                void *InsertPos) {
    getPartialSpecializations().InsertNode(D, InsertPos);
  }

  /// \brief Return the next partial specialization sequence number.
  unsigned getNextPartialSpecSequenceNumber() {
    return getPartialSpecializations().size();
  }

  /// \brief Retrieve the partial specializations as an ordered list.
  void getPartialSpecializations(
          llvm::SmallVectorImpl<ClassTemplatePartialSpecializationDecl *> &PS);
  
  /// \brief Find a class template partial specialization with the given
  /// type T.
  ///
  /// \param T a dependent type that names a specialization of this class
  /// template.
  ///
  /// \returns the class template partial specialization that exactly matches
  /// the type \p T, or NULL if no such partial specialization exists.
  ClassTemplatePartialSpecializationDecl *findPartialSpecialization(QualType T);
  
  /// \brief Find a class template partial specialization which was instantiated
  /// from the given member partial specialization.
  ///
  /// \param D a member class template partial specialization.
  ///
  /// \returns the class template partial specialization which was instantiated
  /// from the given member partial specialization, or NULL if no such partial
  /// specialization exists.
  ClassTemplatePartialSpecializationDecl *
  findPartialSpecInstantiatedFromMember(
                                     ClassTemplatePartialSpecializationDecl *D);

  /// \brief Retrieve the template specialization type of the
  /// injected-class-name for this class template.
  ///
  /// The injected-class-name for a class template \c X is \c
  /// X<template-args>, where \c template-args is formed from the
  /// template arguments that correspond to the template parameters of
  /// \c X. For example:
  ///
  /// \code
  /// template<typename T, int N>
  /// struct array {
  ///   typedef array this_type; // "array" is equivalent to "array<T, N>"
  /// };
  /// \endcode
  QualType getInjectedClassNameSpecialization();

  typedef SpecIterator<ClassTemplateSpecializationDecl> spec_iterator;

  spec_iterator spec_begin() {
    return makeSpecIterator(getSpecializations(), false);
  }

  spec_iterator spec_end() {
    return makeSpecIterator(getSpecializations(), true);
  }

  typedef SpecIterator<ClassTemplatePartialSpecializationDecl>
          partial_spec_iterator;

  partial_spec_iterator partial_spec_begin() {
    return makeSpecIterator(getPartialSpecializations(), false);
  }

  partial_spec_iterator partial_spec_end() {
    return makeSpecIterator(getPartialSpecializations(), true);
  }

  // Implement isa/cast/dyncast support
  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classof(const ClassTemplateDecl *D) { return true; }
  static bool classofKind(Kind K) { return K == ClassTemplate; }

  friend class ASTDeclReader;
  friend class ASTDeclWriter;
};

/// Declaration of a friend template.  For example:
///
/// template <typename T> class A {
///   friend class MyVector<T>; // not a friend template
///   template <typename U> friend class B; // not a friend template
///   template <typename U> friend class Foo<T>::Nested; // friend template
/// };
/// NOTE: This class is not currently in use.  All of the above
/// will yield a FriendDecl, not a FriendTemplateDecl.
class FriendTemplateDecl : public Decl {
public:
  typedef llvm::PointerUnion<NamedDecl*,TypeSourceInfo*> FriendUnion;

private:
  // The number of template parameters;  always non-zero.
  unsigned NumParams;

  // The parameter list.
  TemplateParameterList **Params;

  // The declaration that's a friend of this class.
  FriendUnion Friend;

  // Location of the 'friend' specifier.
  SourceLocation FriendLoc;


  FriendTemplateDecl(DeclContext *DC, SourceLocation Loc,
                     unsigned NParams, 
                     TemplateParameterList **Params,
                     FriendUnion Friend,
                     SourceLocation FriendLoc)
    : Decl(Decl::FriendTemplate, DC, Loc),
      NumParams(NParams),
      Params(Params),
      Friend(Friend),
      FriendLoc(FriendLoc)
  {}

  FriendTemplateDecl(EmptyShell Empty)
    : Decl(Decl::FriendTemplate, Empty),
      NumParams(0),
      Params(0)
  {}

public:
  static FriendTemplateDecl *Create(ASTContext &Context,
                                    DeclContext *DC, SourceLocation Loc,
                                    unsigned NParams, 
                                    TemplateParameterList **Params,
                                    FriendUnion Friend,
                                    SourceLocation FriendLoc);

  static FriendTemplateDecl *Create(ASTContext &Context, EmptyShell Empty);

  /// If this friend declaration names a templated type (or
  /// a dependent member type of a templated type), return that
  /// type;  otherwise return null.
  TypeSourceInfo *getFriendType() const {
    return Friend.dyn_cast<TypeSourceInfo*>();
  }

  /// If this friend declaration names a templated function (or
  /// a member function of a templated type), return that type;
  /// otherwise return null.
  NamedDecl *getFriendDecl() const {
    return Friend.dyn_cast<NamedDecl*>();
  }

  /// Retrieves the location of the 'friend' keyword.
  SourceLocation getFriendLoc() const {
    return FriendLoc;
  }

  TemplateParameterList *getTemplateParameterList(unsigned i) const {
    assert(i <= NumParams);
    return Params[i];
  }

  unsigned getNumTemplateParameters() const {
    return NumParams;
  }

  // Implement isa/cast/dyncast/etc.
  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classofKind(Kind K) { return K == Decl::FriendTemplate; }
  static bool classof(const FriendTemplateDecl *D) { return true; }

  friend class ASTDeclReader;
};

/// Implementation of inline functions that require the template declarations
inline AnyFunctionDecl::AnyFunctionDecl(FunctionTemplateDecl *FTD)
  : Function(FTD) { }

} /* end of namespace clang */

#endif
