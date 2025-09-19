#pragma once

#include "gmock/gmock.h"
#include <ierror_reporter.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace cpploxTests {

class MockErrorReporter : public cpplox::IErrorReporter {
public:
    MOCK_METHOD(void, Report, (int line, std::string_view message), (override));
    MOCK_METHOD(bool, HadErrors, (), (override));
};

}

