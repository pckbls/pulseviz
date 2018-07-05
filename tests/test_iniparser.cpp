#include "gtest/gtest.h"
#include "../src/iniparser.h"
#include <fstream>

namespace
{

TEST(IniParserTest, ParseValidIni)
{
    IniParser parser;

    std::stringstream ss(R"ini(
        [one] ; comment after section
        foo = "bar"
        integer = 1337 ; comment after assignment
        float = 42.24
        a_long_string = ihgseäüö!"§$%&/()=?`´|*+~_.,{[]}³²><^°

        [two]
        ; section is empty
    )ini");
    ASSERT_NO_THROW(parser.parse(ss));

    ASSERT_EQ(parser.getData().size(), 2);

    auto items = parser.getData().at("one");
    ASSERT_EQ(items.size(), 4);
    ASSERT_EQ(items.at("foo"), "\"bar\"");
    ASSERT_EQ(items.at("integer"), "1337");
    ASSERT_EQ(items.at("float"), "42.24");
    ASSERT_EQ(
        parser.getOption("one", "a_long_string"),
        "ihgseäüö!\"§$%&/()=?`´|*+~_.,{[]}³²><^°"
    );

    items = parser.getData().at("two");
    ASSERT_EQ(items.size(), 0);

    ASSERT_EQ(parser.getOption("one", "integer"), "1337");
    ASSERT_THROW(parser.getOption("three", "foo"), IniParserDataException);
    ASSERT_THROW(parser.getOption("two", "foo"), IniParserDataException);
}

TEST(IniParserTest, AssignmentWithoutSectionShouldFail)
{
    IniParser parser;

    std::stringstream ss(R"ini(
        foo = "bar"
        [one]
    )ini");

    try
    {
        parser.parse(ss);
    }
    catch (IniParserException& e)
    {
        ASSERT_EQ(e.getLine(), 2);
        ASSERT_EQ(e.getLineContent(), "        foo = \"bar\"");
        return;
    }

    FAIL() << "Expected an exception to be thrown.";
}

TEST(IniParserTest, MultipleAssignmentsShouldOverwriteValue)
{
    IniParser parser;

    std::stringstream ss(R"ini(
        [one]
        foo = bar
        foo = baz
    )ini");

    ASSERT_NO_THROW(parser.parse(ss));
    ASSERT_EQ(parser.getOption("one", "foo"), "baz");
}

TEST(IniParserTest, DenyDataAccessAfterFailedParseAttempt)
{
    IniParser parser;

    std::stringstream ss(R"ini(
        [one]
        foo = "bar"
        reage
    )ini");
    ASSERT_THROW(parser.parse(ss), IniParserException);

    ASSERT_THROW(parser.getData(), IniParserDataException);
}

TEST(IniParserTest, CallingParseMultipleTimesDiscardsOldData)
{
    IniParser parser;

    std::stringstream ss("[one]\nfoo = bar");
    parser.parse(ss);
    ASSERT_EQ(parser.getOption("one", "foo"), "bar");

    ss.str("");
    parser.parse(ss);
    ASSERT_THROW(parser.getOption("one", "foo"), IniParserDataException);
}

TEST(IniParserTest, SuccessfullyParseTheDefaultConfig)
{
    // Our parser should be capable of parsing the example configuration file
    // shipped with pulseviz.
    IniParser parser;
    std::ifstream stream(std::string(SHARE_PATH) + "/pulseviz.ini");
    ASSERT_TRUE(stream.is_open());
    ASSERT_NO_THROW(parser.parse(stream));
}

TEST(IniParserTest, GetOptionAsBool)
{
    IniParser parser;

    std::stringstream ss(R"ini(
        [bools]
        true_1 = true
        true_2 = 1
        false_1 = false
        false_2 = 0

        [invalid_bools]
        invalid_1 = a
        invalid_2 = 2
        invalid_3 = -1
        invalid_4 = 1a
    )ini");
    ASSERT_NO_THROW(parser.parse(ss));

    ASSERT_TRUE(parser.getOptionAsBool("bools", "true_1"));
    ASSERT_TRUE(parser.getOptionAsBool("bools", "true_2"));
    ASSERT_FALSE(parser.getOptionAsBool("bools", "false_1"));
    ASSERT_FALSE(parser.getOptionAsBool("bools", "false_2"));
    ASSERT_THROW(parser.getOptionAsBool("invalid_bools", "invalid_1"), IniParserTypeConversionException);
    ASSERT_THROW(parser.getOptionAsBool("invalid_bools", "invalid_2"), IniParserTypeConversionException);
    ASSERT_THROW(parser.getOptionAsBool("invalid_bools", "invalid_3"), IniParserTypeConversionException);
    ASSERT_THROW(parser.getOptionAsBool("invalid_bools", "invalid_4"), IniParserTypeConversionException);
}

TEST(IniParserTest, GetOptionAsUnsignedInteger)
{
    IniParser parser;

    std::stringstream ss(R"ini(
        [uints]
        uint_1 = 0
        uint_2 = 1337
        uint_3 = -1
        uint_4 = a
        uint_5 = 1.1
        uint_6 = 1a
    )ini");
    ASSERT_NO_THROW(parser.parse(ss));

    ASSERT_EQ(parser.getOptionAsUnsignedInteger("uints", "uint_1"), 0);
    ASSERT_EQ(parser.getOptionAsUnsignedInteger("uints", "uint_2"), 1337);
    ASSERT_THROW(parser.getOptionAsUnsignedInteger("uints", "uint_3"), IniParserTypeConversionException);
    ASSERT_THROW(parser.getOptionAsUnsignedInteger("uints", "uint_4"), IniParserTypeConversionException);
    ASSERT_THROW(parser.getOptionAsUnsignedInteger("uints", "uint_5"), IniParserTypeConversionException);
    ASSERT_THROW(parser.getOptionAsUnsignedInteger("uints", "uint_6"), IniParserTypeConversionException);
}

TEST(IniParserTest, GetOptionAsFloat)
{
    IniParser parser;

    std::stringstream ss(R"ini(
        [floats]
        float_1 = 0
        float_2 = 1337.0
        float_3 = -1
        float_4 = a
        float_5 =
        float_6 = 1a
    )ini");
    ASSERT_NO_THROW(parser.parse(ss));

    ASSERT_EQ(parser.getOptionAsFloat("floats", "float_1"), 0.0);
    ASSERT_EQ(parser.getOptionAsFloat("floats", "float_2"), 1337.0);
    ASSERT_EQ(parser.getOptionAsFloat("floats", "float_3"), -1.0);
    ASSERT_THROW(parser.getOptionAsFloat("floats", "float_4"), IniParserTypeConversionException);
    ASSERT_THROW(parser.getOptionAsFloat("floats", "float_5"), IniParserTypeConversionException);
    ASSERT_THROW(parser.getOptionAsFloat("floats", "float_6"), IniParserTypeConversionException);
}

} // namespace
