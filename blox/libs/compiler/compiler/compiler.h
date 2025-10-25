#pragma once

#include "compiler/token.h"
#include "ir/object.h"
#include "scanner.h"

#include <functional>
#include <ir/ierror_reporter.h>
#include <ir/ir.h>
#include <memory>

namespace compiler {

// TODO: while benchmarking is copying the source around noticeable?
// If so, switch to some sort of streaming string source
class Compiler final {
public:
    Compiler(std::string_view source, ir::IErrorReporter* errorReporter);
    std::unique_ptr<ir::ObjectFunction> Compile(); // -> function main()

private:
    enum class Precedence {
        kNone = 0,
        kAssignment,
        kOr,
        kAnd,
        kEquality,
        kComparison,
        kTerm,
        kFactor,
        kUnary,
        kCall,
        kPrimary
    };

    // std::function<void(Token)> -> std::function<void(Compiler,Token)> -> actual function
    // Too many layers of indirection, does this create a measurable slowdown?
    // (std::bind is being used)
    struct ParseRule {
        std::function<void(Token)> mPrefix;
        std::function<void(Token)> mInfix;
        Precedence mPrecedence;
    };

    // TODO: We want to emit an opcode for EOF?
    void ParseWithPrecedence(Precedence minPrecedence);
    void Expression();
    void Number(Token);
    void Unary(Token);
    void Binary(Token);
    void Grouping(Token);

    ParseRule GetRule(Token token);

    Scanner mScanner;
    ir::IErrorReporter* mErrorReporter;
    std::vector<ParseRule> mParseRules;

    std::unique_ptr<ir::ObjectFunction> mMain;
    ir::ObjectFunction* mCurrentFunction;
    ir::Chunk* mCurrentChunk;
};

}
