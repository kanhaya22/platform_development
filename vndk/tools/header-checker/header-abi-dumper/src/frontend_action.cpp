// Copyright (C) 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "frontend_action.h"

#include "ast_processing.h"
#include "diagnostic_consumer.h"
#include "fake_decl_source.h"
#include "header_abi_util.h"
#include "ir_representation.h"

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Preprocessor.h>

#include <llvm/ADT/STLExtras.h>

HeaderCheckerFrontendAction::HeaderCheckerFrontendAction(
    HeaderCheckerOptions &options)
    : options_(options) {}

std::unique_ptr<clang::ASTConsumer>
HeaderCheckerFrontendAction::CreateASTConsumer(clang::CompilerInstance &ci,
                                               llvm::StringRef header_file) {
  // Create AST consumers.
  return llvm::make_unique<HeaderASTConsumer>(&ci, options_);
}

bool HeaderCheckerFrontendAction::BeginInvocation(clang::CompilerInstance &ci) {
  if (options_.suppress_errors_) {
    ci.getFrontendOpts().SkipFunctionBodies = true;
    clang::DiagnosticsEngine &diagnostics = ci.getDiagnostics();
    diagnostics.setClient(
        new HeaderCheckerDiagnosticConsumer(diagnostics.takeClient()),
        /* ShouldOwnClient */ true);
  }
  return true;
}

bool HeaderCheckerFrontendAction::BeginSourceFileAction(
    clang::CompilerInstance &ci) {
  if (options_.suppress_errors_) {
    ci.setExternalSemaSource(new FakeDeclSource(ci));
    ci.getPreprocessor().SetSuppressIncludeNotFoundError(true);
  }
  return true;
}
