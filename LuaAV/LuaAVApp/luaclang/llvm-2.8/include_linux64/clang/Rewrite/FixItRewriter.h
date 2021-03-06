//===--- FixItRewriter.h - Fix-It Rewriter Diagnostic Client ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a diagnostic client adaptor that performs rewrites as
// suggested by code modification hints attached to diagnostics. It
// then forwards any diagnostics to the adapted diagnostic client.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_REWRITE_FIX_IT_REWRITER_H
#define LLVM_CLANG_REWRITE_FIX_IT_REWRITER_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Rewrite/Rewriter.h"
#include "llvm/ADT/SmallVector.h"

namespace llvm { class raw_ostream; }

namespace clang {

class SourceManager;
class FileEntry;

class FixItOptions {
public:
  virtual ~FixItOptions();

  /// \brief This file is about to be rewritten. Return the name of the file
  /// that is okay to write to.
  virtual std::string RewriteFilename(const std::string &Filename) = 0;

  /// \brief Whether to abort fixing a file when not all errors could be fixed.
  bool FixWhatYouCan;
};

class FixItRewriter : public DiagnosticClient {
  /// \brief The diagnostics machinery.
  Diagnostic &Diags;

  /// \brief The rewriter used to perform the various code
  /// modifications.
  Rewriter Rewrite;

  /// \brief The diagnostic client that performs the actual formatting
  /// of error messages.
  DiagnosticClient *Client;

  /// \brief Turn an input path into an output path. NULL implies overwriting
  /// the original.
  FixItOptions *FixItOpts;

  /// \brief The number of rewriter failures.
  unsigned NumFailures;

public:
  typedef Rewriter::buffer_iterator iterator;

  /// \brief Initialize a new fix-it rewriter.
  FixItRewriter(Diagnostic &Diags, SourceManager &SourceMgr,
                const LangOptions &LangOpts, FixItOptions *FixItOpts);

  /// \brief Destroy the fix-it rewriter.
  ~FixItRewriter();

  /// \brief Check whether there are modifications for a given file.
  bool IsModified(FileID ID) const {
    return Rewrite.getRewriteBufferFor(ID) != NULL;
  }

  // Iteration over files with changes.
  iterator buffer_begin() { return Rewrite.buffer_begin(); }
  iterator buffer_end() { return Rewrite.buffer_end(); }

  /// \brief Write a single modified source file.
  ///
  /// \returns true if there was an error, false otherwise.
  bool WriteFixedFile(FileID ID, llvm::raw_ostream &OS);

  /// \brief Write the modified source files.
  ///
  /// \returns true if there was an error, false otherwise.
  bool WriteFixedFiles();

  /// IncludeInDiagnosticCounts - This method (whose default implementation
  /// returns true) indicates whether the diagnostics handled by this
  /// DiagnosticClient should be included in the number of diagnostics
  /// reported by Diagnostic.
  virtual bool IncludeInDiagnosticCounts() const;

  /// HandleDiagnostic - Handle this diagnostic, reporting it to the user or
  /// capturing it to a log as needed.
  virtual void HandleDiagnostic(Diagnostic::Level DiagLevel,
                                const DiagnosticInfo &Info);

  /// \brief Emit a diagnostic via the adapted diagnostic client.
  void Diag(FullSourceLoc Loc, unsigned DiagID);
};

}

#endif // LLVM_CLANG_REWRITE_FIX_IT_REWRITER_H
