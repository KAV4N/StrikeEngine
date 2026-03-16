#include <gtest/gtest.h>
#include "StrikeEngine/Core/ParserUtils.h"
#include <glm/glm.hpp>

using namespace Strike::ParserUtils;

/* ---------- trim ---------- */

TEST(TrimTest, EmptyString) { EXPECT_EQ(detail::trim(""), ""); }
TEST(TrimTest, SingleSpace) { EXPECT_EQ(detail::trim(" "), ""); }
TEST(TrimTest, WhitespaceOnly) { EXPECT_EQ(detail::trim("\t\r\n"), ""); }
TEST(TrimTest, MixedWhitespaceOnly) { EXPECT_EQ(detail::trim("   \t   "), ""); }

TEST(TrimTest, NoWhitespaceHello) { EXPECT_EQ(detail::trim("hello"), "hello"); }
TEST(TrimTest, NoWhitespaceNumbers) { EXPECT_EQ(detail::trim("123"), "123"); }

TEST(TrimTest, LeadingSpaces) { EXPECT_EQ(detail::trim("   hello"), "hello"); }
TEST(TrimTest, LeadingMixedWhitespace) { EXPECT_EQ(detail::trim("\t\r\nhello"), "hello"); }

TEST(TrimTest, TrailingSpaces) { EXPECT_EQ(detail::trim("hello   "), "hello"); }
TEST(TrimTest, TrailingMixedWhitespace) { EXPECT_EQ(detail::trim("hello\t\r\n"), "hello"); }

TEST(TrimTest, BothSidesSpaces) { EXPECT_EQ(detail::trim("   hello   "), "hello"); }
TEST(TrimTest, BothSidesMixedWhitespace) { EXPECT_EQ(detail::trim("\t hello \n"), "hello"); }

TEST(TrimTest, MixedCharacters) { EXPECT_EQ(detail::trim(" \t\r\n  world  \t\r\n "), "world"); }


/* ---------- parseFloat ---------- */

TEST(ParseFloatTest, SimpleFloat)
{
    auto res = detail::parseFloat("1.23");
    ASSERT_TRUE(res.has_value());
    EXPECT_FLOAT_EQ(*res, 1.23f);
}

TEST(ParseFloatTest, NegativeWithSpaces)
{
    auto res = detail::parseFloat(" -42 ");
    ASSERT_TRUE(res.has_value());
    EXPECT_FLOAT_EQ(*res, -42.0f);
}

TEST(ParseFloatTest, Zero)
{
    auto res = detail::parseFloat("0");
    ASSERT_TRUE(res.has_value());
    EXPECT_FLOAT_EQ(*res, 0.0f);
}

TEST(ParseFloatTest, ScientificNotation)
{
    auto res = detail::parseFloat("1.2e3");
    ASSERT_TRUE(res.has_value());
    EXPECT_FLOAT_EQ(*res, 1200.0f);
}

TEST(ParseFloatTest, TrimmedFloat)
{
    auto res = detail::parseFloat("   3.14159   ");
    ASSERT_TRUE(res.has_value());
    EXPECT_FLOAT_EQ(*res, 3.14159f);
}

TEST(ParseFloatTest, EmptyString) { EXPECT_FALSE(detail::parseFloat("").has_value()); }
TEST(ParseFloatTest, OnlySpaces) { EXPECT_FALSE(detail::parseFloat("   ").has_value()); }
TEST(ParseFloatTest, Letters) { EXPECT_FALSE(detail::parseFloat("abc").has_value()); }
TEST(ParseFloatTest, PartialNumber) { EXPECT_FALSE(detail::parseFloat("1.2abc").has_value()); }
TEST(ParseFloatTest, CommaNumber) { EXPECT_FALSE(detail::parseFloat("1,2").has_value()); }
TEST(ParseFloatTest, FloatSuffix) { EXPECT_FALSE(detail::parseFloat("1.2f").has_value()); }

TEST(ParseFloatTest, Infinity)
{
    auto res = detail::parseFloat("inf");
    ASSERT_TRUE(res.has_value());
    EXPECT_TRUE(std::isinf(*res));
}


/* ---------- splitComma ---------- */

TEST(SplitCommaTest, Empty)
{
    auto t = detail::splitComma("");
    ASSERT_EQ(t.size(), 1);
    EXPECT_EQ(t[0], "");
}

TEST(SplitCommaTest, SingleToken)
{
    auto t = detail::splitComma("a");
    ASSERT_EQ(t.size(), 1);
    EXPECT_EQ(t[0], "a");
}

TEST(SplitCommaTest, TwoTokens)
{
    auto t = detail::splitComma("a,b");
    ASSERT_EQ(t.size(), 2);
    EXPECT_EQ(t[0], "a");
    EXPECT_EQ(t[1], "b");
}

TEST(SplitCommaTest, ThreeNumbers)
{
    auto t = detail::splitComma("1,2,3");
    ASSERT_EQ(t.size(), 3);
    EXPECT_EQ(t[0], "1");
    EXPECT_EQ(t[1], "2");
    EXPECT_EQ(t[2], "3");
}

TEST(SplitCommaTest, OnlyComma)
{
    auto t = detail::splitComma(",");
    ASSERT_EQ(t.size(), 2);
    EXPECT_EQ(t[0], "");
    EXPECT_EQ(t[1], "");
}

TEST(SplitCommaTest, EmptyMiddle)
{
    auto t = detail::splitComma("a,,b");
    ASSERT_EQ(t.size(), 3);
    EXPECT_EQ(t[0], "a");
    EXPECT_EQ(t[1], "");
    EXPECT_EQ(t[2], "b");
}

TEST(SplitCommaTest, TrailingComma)
{
    auto t = detail::splitComma("1,2,");
    ASSERT_EQ(t.size(), 3);
    EXPECT_EQ(t[2], "");
}

TEST(SplitCommaTest, TwoCommas)
{
    auto t = detail::splitComma(",,");
    ASSERT_EQ(t.size(), 3);
}

TEST(SplitCommaTest, PreserveSpaces)
{
    auto t = detail::splitComma(" 1 , 2 ");
    ASSERT_EQ(t.size(), 2);
    EXPECT_EQ(t[0], " 1 ");
    EXPECT_EQ(t[1], " 2 ");
}


/* ---------- parseVec2 ---------- */

TEST(ParseVec2Test, EmptyFallback)
{
    EXPECT_EQ(parseVec2(""), glm::vec2(0.0f));
}

TEST(ParseVec2Test, SingleValue)
{
    EXPECT_EQ(parseVec2("5"), glm::vec2(5.0f, 0.0f));
}

TEST(ParseVec2Test, TwoValues)
{
    EXPECT_EQ(parseVec2("1,2"), glm::vec2(1.0f, 2.0f));
}

TEST(ParseVec2Test, ExtraValuesIgnored)
{
    EXPECT_EQ(parseVec2("1,2,3,4"), glm::vec2(1.0f, 2.0f));
}

TEST(ParseVec2Test, InvalidFirst)
{
    EXPECT_EQ(parseVec2("abc,4"), glm::vec2(0.0f, 4.0f));
}

TEST(ParseVec2Test, InvalidSecond)
{
    EXPECT_EQ(parseVec2("5,xyz"), glm::vec2(5.0f, 0.0f));
}

TEST(ParseVec2Test, LeadingComma)
{
    EXPECT_EQ(parseVec2(",7"), glm::vec2(0.0f, 7.0f));
}

TEST(ParseVec2Test, TrailingComma)
{
    EXPECT_EQ(parseVec2("8,"), glm::vec2(8.0f, 0.0f));
}

TEST(ParseVec2Test, CustomFallback)
{
    EXPECT_EQ(parseVec2("invalid", glm::vec2(10,20)), glm::vec2(10,20));
}


/* ---------- parseVec3 ---------- */

TEST(ParseVec3Test, Empty)
{
    EXPECT_EQ(parseVec3(""), glm::vec3(0.0f));
}

TEST(ParseVec3Test, OneComponent)
{
    EXPECT_EQ(parseVec3("10"), glm::vec3(10.0f,0.0f,0.0f));
}

TEST(ParseVec3Test, TwoComponents)
{
    EXPECT_EQ(parseVec3("1,2"), glm::vec3(1.0f,2.0f,0.0f));
}

TEST(ParseVec3Test, ThreeComponents)
{
    EXPECT_EQ(parseVec3("1,2,3"), glm::vec3(1.0f,2.0f,3.0f));
}

TEST(ParseVec3Test, InvalidMiddle)
{
    EXPECT_EQ(parseVec3("1,b,3"), glm::vec3(1.0f,0.0f,3.0f));
}

TEST(ParseVec3Test, CommaEdge)
{
    EXPECT_EQ(parseVec3(",,5"), glm::vec3(0.0f,0.0f,5.0f));
}


/* ---------- parseUVec3 ---------- */

TEST(ParseUVec3Test, DefaultFallback)
{
    EXPECT_EQ(parseUVec3(""), glm::uvec3(255u));
}

TEST(ParseUVec3Test, FullValid)
{
    EXPECT_EQ(parseUVec3("10,20,30"), glm::uvec3(10u,20u,30u));
}

TEST(ParseUVec3Test, TwoValues)
{
    EXPECT_EQ(parseUVec3("100,200"), glm::uvec3(100u,200u,255u));
}

TEST(ParseUVec3Test, FloatTruncation)
{
    EXPECT_EQ(parseUVec3("1.9,2.1,3.9"), glm::uvec3(1u,2u,3u));
}

TEST(ParseUVec3Test, InvalidMiddle)
{
    EXPECT_EQ(parseUVec3("70,xyz,80"), glm::uvec3(70u,255u,80u));
}

TEST(ParseUVec3Test, LeadingComma)
{
    EXPECT_EQ(parseUVec3(",200,300"), glm::uvec3(255u,200u,300u));
}

TEST(ParseUVec3Test, CustomFallback)
{
    EXPECT_EQ(parseUVec3("bad", glm::uvec3(128u)), glm::uvec3(128u));
}