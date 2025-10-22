#include "mock_error_reporter.h"

#include <cpplox/scanner.h>
#include <cpplox/token.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

namespace cpploxTests {

class ScannerTest : public testing::Test {
protected:
    testing::StrictMock<MockErrorReporter> mErrorReporter;

    const double kEps { 1e-9 };
};

TEST_F(ScannerTest, Keywords)
{
    std::string source { "and class else false fun for \
        if nil or print return super this true var while" };

    cpplox::Scanner scanner(source, &mErrorReporter);
    std::vector<cpplox::Token> tokens = scanner.ScanTokens();

    using Type = cpplox::Token::Type;

    ASSERT_EQ(tokens.size(), 17);

    EXPECT_EQ(tokens[0].mType, Type::kAnd);
    EXPECT_EQ(tokens[1].mType, Type::kClass);
    EXPECT_EQ(tokens[2].mType, Type::kElse);
    EXPECT_EQ(tokens[3].mType, Type::kFalse);
    EXPECT_EQ(tokens[4].mType, Type::kFun);
    EXPECT_EQ(tokens[5].mType, Type::kFor);
    EXPECT_EQ(tokens[6].mType, Type::kIf);
    EXPECT_EQ(tokens[7].mType, Type::kNil);
    EXPECT_EQ(tokens[8].mType, Type::kOr);
    EXPECT_EQ(tokens[9].mType, Type::kPrint);
    EXPECT_EQ(tokens[10].mType, Type::kReturn);
    EXPECT_EQ(tokens[11].mType, Type::kSuper);
    EXPECT_EQ(tokens[12].mType, Type::kThis);
    EXPECT_EQ(tokens[13].mType, Type::kTrue);
    EXPECT_EQ(tokens[14].mType, Type::kVar);
    EXPECT_EQ(tokens[15].mType, Type::kWhile);
    EXPECT_EQ(tokens[16].mType, Type::kEof);
}

TEST_F(ScannerTest, NumberLiterals)
{
    std::string source { "11.234 1.44123456 0 0.00 69.69 987654321 987654321.1234" };

    cpplox::Scanner scanner(source, &mErrorReporter);
    std::vector<cpplox::Token> tokens = scanner.ScanTokens();

    using Type = cpplox::Token::Type;

    ASSERT_EQ(tokens.size(), 8);

    for (int i = 0; i < 7; i++) {
        ASSERT_EQ(tokens[i].mType, Type::kNumber);
        ASSERT_TRUE(tokens[i].mObject.has_value());
    }
    EXPECT_EQ(tokens[7].mType, Type::kEof);

    EXPECT_NEAR(std::get<double>(tokens[0].mObject->mData), 11.234, kEps);
    EXPECT_NEAR(std::get<double>(tokens[1].mObject->mData), 1.44123456, kEps);
    EXPECT_NEAR(std::get<double>(tokens[2].mObject->mData), 0, kEps);
    EXPECT_NEAR(std::get<double>(tokens[3].mObject->mData), 0, kEps);
    EXPECT_NEAR(std::get<double>(tokens[4].mObject->mData), 69.69, kEps);
    EXPECT_NEAR(std::get<double>(tokens[5].mObject->mData), 987654321, kEps);
    EXPECT_NEAR(std::get<double>(tokens[6].mObject->mData), 987654321.1234, kEps);
}

TEST_F(ScannerTest, Identifiers)
{
    std::string source { "and1 _abcd for_nil while23_ " };

    cpplox::Scanner scanner(source, &mErrorReporter);
    std::vector<cpplox::Token> tokens = scanner.ScanTokens();

    using Type = cpplox::Token::Type;

    ASSERT_EQ(tokens.size(), 5);

    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(tokens[i].mType, Type::kIdentifier);
        ASSERT_TRUE(tokens[i].mObject.has_value());
    }
    EXPECT_EQ(tokens[4].mType, Type::kEof);

    EXPECT_EQ(std::get<std::string>(tokens[0].mObject->mData), "and1");
    EXPECT_EQ(std::get<std::string>(tokens[1].mObject->mData), "_abcd");
    EXPECT_EQ(std::get<std::string>(tokens[2].mObject->mData), "for_nil");
    EXPECT_EQ(std::get<std::string>(tokens[3].mObject->mData), "while23_");
}

TEST_F(ScannerTest, StringLiterals)
{
    std::string source { "\"test123.4\" \"123 4#$.;|]''\"" };

    cpplox::Scanner scanner(source, &mErrorReporter);
    std::vector<cpplox::Token> tokens = scanner.ScanTokens();

    using Type = cpplox::Token::Type;

    ASSERT_EQ(tokens.size(), 3);
    for (int i = 0; i < 2; i++) {
        ASSERT_EQ(tokens[i].mType, Type::kString);
        ASSERT_TRUE(tokens[i].mObject.has_value());
    }
    EXPECT_EQ(tokens[2].mType, Type::kEof);

    EXPECT_EQ(std::get<std::string>(tokens[0].mObject->mData), "test123.4");
    EXPECT_EQ(std::get<std::string>(tokens[1].mObject->mData), "123 4#$.;|]''");
}

TEST_F(ScannerTest, Tokens)
{
    std::string source { "({}),.-+;/* -! != = == > >= < <=" };

    cpplox::Scanner scanner(source, &mErrorReporter);
    std::vector<cpplox::Token> tokens = scanner.ScanTokens();

    using Type = cpplox::Token::Type;

    ASSERT_EQ(tokens.size(), 21);

    EXPECT_EQ(tokens[0].mType, Type::kLeftParen);
    EXPECT_EQ(tokens[1].mType, Type::kLeftBrace);
    EXPECT_EQ(tokens[2].mType, Type::kRightBrace);
    EXPECT_EQ(tokens[3].mType, Type::kRightParen);
    EXPECT_EQ(tokens[4].mType, Type::kComma);
    EXPECT_EQ(tokens[5].mType, Type::kDot);
    EXPECT_EQ(tokens[6].mType, Type::kMinus);
    EXPECT_EQ(tokens[7].mType, Type::kPlus);
    EXPECT_EQ(tokens[8].mType, Type::kSemicolon);
    EXPECT_EQ(tokens[9].mType, Type::kSlash);
    EXPECT_EQ(tokens[10].mType, Type::kStar);

    EXPECT_EQ(tokens[11].mType, Type::kMinus);
    EXPECT_EQ(tokens[12].mType, Type::kBang);
    EXPECT_EQ(tokens[13].mType, Type::kBangEqual);
    EXPECT_EQ(tokens[14].mType, Type::kEqual);
    EXPECT_EQ(tokens[15].mType, Type::kEqualEqual);
    EXPECT_EQ(tokens[16].mType, Type::kGreater);
    EXPECT_EQ(tokens[17].mType, Type::kGreaterEqual);
    EXPECT_EQ(tokens[18].mType, Type::kLess);
    EXPECT_EQ(tokens[19].mType, Type::kLessEqual);

    EXPECT_EQ(tokens[20].mType, Type::kEof);
}

TEST_F(ScannerTest, MixedTokens)
{
    std::string source { "print var a= -1234.12+nil*for_nil3/0" };

    cpplox::Scanner scanner(source, &mErrorReporter);
    std::vector<cpplox::Token> tokens = scanner.ScanTokens();

    using Type = cpplox::Token::Type;

    ASSERT_EQ(tokens.size(), 13);

    EXPECT_EQ(tokens[0].mType, Type::kPrint);

    EXPECT_EQ(tokens[1].mType, Type::kVar);

    EXPECT_EQ(tokens[2].mType, Type::kIdentifier);
    ASSERT_TRUE(tokens[2].mObject.has_value());
    EXPECT_EQ(std::get<std::string>(tokens[2].mObject->mData), "a");
    EXPECT_EQ(tokens[3].mType, Type::kEqual);

    auto compareNumber { [&](int idx, double number) {
        EXPECT_EQ(tokens[idx].mType, Type::kNumber);
        ASSERT_TRUE(tokens[idx].mObject.has_value());
        EXPECT_NEAR(std::get<double>(tokens[idx].mObject->mData), number, kEps);
    } };

    EXPECT_EQ(tokens[4].mType, Type::kMinus);
    compareNumber(5, 1234.12);
    EXPECT_EQ(tokens[6].mType, Type::kPlus);
    EXPECT_EQ(tokens[7].mType, Type::kNil);
    EXPECT_EQ(tokens[8].mType, Type::kStar);
    EXPECT_EQ(tokens[9].mType, Type::kIdentifier);
    ASSERT_TRUE(tokens[9].mObject.has_value());
    EXPECT_EQ(std::get<std::string>(tokens[9].mObject->mData), "for_nil3");
    EXPECT_EQ(tokens[10].mType, Type::kSlash);
    compareNumber(11, 0);

    EXPECT_EQ(tokens[12].mType, Type::kEof);
}

}
