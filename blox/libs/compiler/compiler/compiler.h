#pragma once

#include "scanner.h"
#include "token.h"

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
        std::function<void(Precedence minPrecedence)> mPrefix;
        std::function<void(Precedence minPrecedence)> mInfix;
        Precedence mPrecedence;
    };

    struct LocalVariable {
        std::string_view mName;
        int mDepth;
    };

    const static int kLocalVariablesCount { 256 };

    // TODO: We want to emit an opcode for EOF?
    void ParseWithPrecedence(Precedence minPrecedence);

    void Declaration();
    void DeclarationVariable();
    void Statement();
    void StatementPrint();
    void StatementExpression();
    void StatementBlock();
    void Expression();

    void Identifier(Precedence);
    void Number(Precedence);
    void String(Precedence);
    void Nil(Precedence);
    void True(Precedence);
    void False(Precedence);
    void Unary(Precedence);
    void Binary(Precedence);
    void Grouping(Precedence);
    void Return(Precedence);

    void BeginScope(Token token);
    // TODO: Will using an std::optional here cause much of a slowdown?
    int ResolveLocal(std::string_view name); // -> -1 on failure
    void EndScope(Token token);
    std::optional<uint8_t> AddIdentifier(Token token);
    ParseRule GetRule(Token token);
    bool Consume(Token::Type type);

    Scanner mScanner;
    ir::IErrorReporter* mErrorReporter;
    std::vector<ParseRule> mParseRules;

    std::unique_ptr<ir::ObjectFunction> mMain;
    ir::ObjectFunction* mCurrentFunction;
    ir::Chunk* mCurrentChunk;

    std::vector<LocalVariable> mLocals;
    int mScopeDepth;
};

}
