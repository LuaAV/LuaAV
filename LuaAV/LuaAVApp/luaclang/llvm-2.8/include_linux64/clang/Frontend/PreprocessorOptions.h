//===--- PreprocessorOptionms.h ---------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_FRONTEND_PREPROCESSOROPTIONS_H_
#define LLVM_CLANG_FRONTEND_PREPROCESSOROPTIONS_H_

#include "llvm/ADT/StringRef.h"
#include <cassert>
#include <string>
#include <utility>
#include <vector>

namespace llvm {
  class MemoryBuffer;
}

namespace clang {

class Preprocessor;
class LangOptions;

/// PreprocessorOptions - This class is used for passing the various options
/// used in preprocessor initialization to InitializePreprocessor().
class PreprocessorOptions {
public:
  std::vector<std::pair<std::string, bool/*isUndef*/> > Macros;
  std::vector<std::string> Includes;
  std::vector<std::string> MacroIncludes;

  unsigned UsePredefines : 1; /// Initialize the preprocessor with the compiler
                              /// and target specific predefines.

  unsigned DetailedRecord : 1; /// Whether we should maintain a detailed
                               /// record of all macro definitions and
                               /// instantiations.
  
  /// The implicit PCH included at the start of the translation unit, or empty.
  std::string ImplicitPCHInclude;

  /// \brief When true, disables most of the normal validation performed on
  /// precompiled headers.
  bool DisablePCHValidation;
  
  /// \brief If non-zero, the implicit PCH include is actually a precompiled
  /// preamble that covers this number of bytes in the main source file.
  ///
  /// The boolean indicates whether the preamble ends at the start of a new
  /// line.
  std::pair<unsigned, bool> PrecompiledPreambleBytes;
  
  /// The implicit PTH input included at the start of the translation unit, or
  /// empty.
  std::string ImplicitPTHInclude;

  /// If given, a PTH cache file to use for speeding up header parsing.
  std::string TokenCache;

  /// \brief The set of file remappings, which take existing files on
  /// the system (the first part of each pair) and gives them the
  /// contents of other files on the system (the second part of each
  /// pair).
  std::vector<std::pair<std::string, std::string> >  RemappedFiles;

  /// \brief The set of file-to-buffer remappings, which take existing files
  /// on the system (the first part of each pair) and gives them the contents
  /// of the specified memory buffer (the second part of each pair).
  std::vector<std::pair<std::string, const llvm::MemoryBuffer *> > 
    RemappedFileBuffers;
  
  /// \brief Whether the compiler instance should retain (i.e., not free)
  /// the buffers associated with remapped files.
  ///
  /// This flag defaults to false; it can be set true only through direct
  /// manipulation of the compiler invocation object, in cases where the 
  /// compiler invocation and its buffers will be reused.
  bool RetainRemappedFileBuffers;
  
  typedef std::vector<std::pair<std::string, std::string> >::iterator
    remapped_file_iterator;
  typedef std::vector<std::pair<std::string, std::string> >::const_iterator
    const_remapped_file_iterator;
  remapped_file_iterator remapped_file_begin() { 
    return RemappedFiles.begin();
  }
  const_remapped_file_iterator remapped_file_begin() const {
    return RemappedFiles.begin();
  }
  remapped_file_iterator remapped_file_end() { 
    return RemappedFiles.end();
  }
  const_remapped_file_iterator remapped_file_end() const { 
    return RemappedFiles.end();
  }

  typedef std::vector<std::pair<std::string, const llvm::MemoryBuffer *> >::
                                  iterator remapped_file_buffer_iterator;
  typedef std::vector<std::pair<std::string, const llvm::MemoryBuffer *> >::
                            const_iterator const_remapped_file_buffer_iterator;
  remapped_file_buffer_iterator remapped_file_buffer_begin() {
    return RemappedFileBuffers.begin();
  }
  const_remapped_file_buffer_iterator remapped_file_buffer_begin() const {
    return RemappedFileBuffers.begin();
  }
  remapped_file_buffer_iterator remapped_file_buffer_end() {
    return RemappedFileBuffers.end();
  }
  const_remapped_file_buffer_iterator remapped_file_buffer_end() const {
    return RemappedFileBuffers.end();
  }
  
public:
  PreprocessorOptions() : UsePredefines(true), DetailedRecord(false),
                          DisablePCHValidation(false),
                          PrecompiledPreambleBytes(0, true),
                          RetainRemappedFileBuffers(false) { }

  void addMacroDef(llvm::StringRef Name) {
    Macros.push_back(std::make_pair(Name, false));
  }
  void addMacroUndef(llvm::StringRef Name) {
    Macros.push_back(std::make_pair(Name, true));
  }
  void addRemappedFile(llvm::StringRef From, llvm::StringRef To) {
    RemappedFiles.push_back(std::make_pair(From, To));
  }
  
  remapped_file_iterator eraseRemappedFile(remapped_file_iterator Remapped) {
    return RemappedFiles.erase(Remapped);
  }
  
  void addRemappedFile(llvm::StringRef From, const llvm::MemoryBuffer * To) {
    RemappedFileBuffers.push_back(std::make_pair(From, To));
  }
  
  remapped_file_buffer_iterator
  eraseRemappedFile(remapped_file_buffer_iterator Remapped) {
    return RemappedFileBuffers.erase(Remapped);
  }
  
  void clearRemappedFiles() {
    RemappedFiles.clear();
    RemappedFileBuffers.clear();
  }
};

} // end namespace clang

#endif
