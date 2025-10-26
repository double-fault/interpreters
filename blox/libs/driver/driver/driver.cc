#include "driver.h"
#include "error_reporter.h"

#include <compiler/compiler.h>
#include <ir/ierror_reporter.h>
#include <ir/ir.h>
#include <vm/vm.h>

namespace driver {

bool Driver::Run(std::string_view source)
{
    std::unique_ptr<ir::IErrorReporter> errorReporter = std::make_unique<ErrorReporter>();

    compiler::Compiler compiler(source, errorReporter.get());
    std::unique_ptr<ir::ObjectFunction> main = compiler.Compile();

    if (errorReporter->HadErrors()) {
        return false;
    }

    main->mChunk.Print();

    vm::Vm vm(main.get(), errorReporter.get());
    vm.Run();

    return !errorReporter->HadErrors();
}

}
