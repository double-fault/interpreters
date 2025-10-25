#pragma once

#include "gmock/gmock.h"
#include <cpplox/ierror_reporter.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace cpploxTests {

class MockErrorReporter : public cpplox::IErrorReporter {
public:
    MOCK_METHOD(void, Report, (int line, std::string_view message), (override));
    MOCK_METHOD(bool, HadErrors, (), (override));
};

}
