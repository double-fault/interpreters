#pragma once

#include <ir/ierror_reporter.h>
#include <ir/ir.h>
#include <map>

namespace vm {

class Vm final {
public:
    Vm(ir::ObjectFunction* main, ir::IErrorReporter* errorReporter);
    void Run();

private:
    struct CallFrame {
        ir::ObjectFunction* mFunction;
        int mIp;
        int mBp;
    };

    struct Byte {
        uint8_t mByte;
        int mLine;
    };

    void Global(Byte byte);
    void Local(Byte byte);
    void Constant(Byte byte);
    void Negate(Byte byte);
    void Binary(Byte byte);
    void Print(Byte byte);
    void Popn(Byte byte);

    // bytecode
    Byte NextByte();
    Byte PeekByte();
    bool HasMoreBytes();

    // value stack
    void Push(ir::Value);
    ir::Value Pop();

    bool IsTrue(ir::Value value);
    bool CheckType(ir::Value::Type, ir::Value, int line);
    bool CheckType(ir::Value::Type, std::initializer_list<ir::Value>, int line);

    ir::ObjectFunction* mMain;
    ir::IErrorReporter* mErrorReporter;
    // ir::ObjectFunction* mCurrentFunction;
    ir::Chunk* mChunk;
    CallFrame* mFrame;

    std::vector<ir::Value> mValueStack;
    int mStackPointer { 0 };
    std::vector<CallFrame> mCallStack;
    std::map<std::string, ir::Value> mGlobals;
};

}
