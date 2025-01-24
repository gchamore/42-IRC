#include <gtest/gtest.h>
#include "../includes/CommandParser.hpp"

TEST(CommandParserTest, ParseNickCommand)
{
    std::string rawMessage = ":JohnDoe NICK JohnDoe\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix.nickname, "JohnDoe");
    EXPECT_EQ(command.command, "NICK");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "JohnDoe");
}

TEST(CommandParserTest, ParseUserCommand)
{
    std::string rawMessage = ":JohnDoe USER johndoe\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix.nickname, "JohnDoe");
    EXPECT_EQ(command.command, "USER");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "johndoe");
}

TEST(CommandParserTest, ParseJoinCommand)
{
    std::string rawMessage = ":JohnDoe JOIN #general\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix.nickname, "JohnDoe");
    EXPECT_EQ(command.command, "JOIN");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "#general");
}

TEST(CommandParserTest, ParsePrivmsgCommand)
{
    std::string rawMessage = ":JohnDoe PRIVMSG #general :Hello everyone!\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix.nickname, "JohnDoe");
    EXPECT_EQ(command.command, "PRIVMSG");
    ASSERT_EQ(command.params.size(), 2);
    EXPECT_EQ(command.params[0], "#general");
    EXPECT_EQ(command.params[1], "Hello everyone!");
}

TEST(CommandParserTest, ParseInvalidCommand)
{
    std::string rawMessage = ":JohnDoe INVALIDCOMMAND\r\n";
    try {
        CommandParser::parse(rawMessage);
        FAIL() << "Expected CommandParser::ParseError";
    } catch (const CommandParser::ParseError& err)
    {
        EXPECT_EQ(std::string(err.what()), "Invalid command format");
    } catch (...)
    {
        FAIL() << "Expected CommandParser::ParseError";
    }
}

TEST(CommandParserTest, ParseEmptyMessage)
{
    std::string rawMessage = "";
    try {
        CommandParser::parse(rawMessage);
        FAIL() << "Expected CommandParser::ParseError";
    } catch (const CommandParser::ParseError& err)
    {
        EXPECT_EQ(err.what(), std::string("Empty message"));
    } catch (...)
    {
        FAIL() << "Expected CommandParser::ParseError";
    }
}

TEST(CommandParserTest, ParseNoPrefix)
{
    std::string rawMessage = "NICK JohnDoe\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_TRUE(command.prefix.empty());
    EXPECT_EQ(command.command, "NICK");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "JohnDoe");
}

TEST(CommandParserTest, ParseCommandWithMultipleParams)
{
    std::string rawMessage = ":JohnDoe COMMAND param1 param2 param3\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix.nickname, "JohnDoe");
    EXPECT_EQ(command.command, "COMMAND");
    ASSERT_EQ(command.params.size(), 3);
    EXPECT_EQ(command.params[0], "param1");
    EXPECT_EQ(command.params[1], "param2");
    EXPECT_EQ(command.params[2], "param3");
}

TEST(CommandParserTest, ParseCommandWithTrailingParam)
{
    std::string rawMessage = ":JohnDoe COMMAND param1 param2 :trailing param\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix.nickname, "JohnDoe");
    EXPECT_EQ(command.command, "COMMAND");
    ASSERT_EQ(command.params.size(), 3);
    EXPECT_EQ(command.params[0], "param1");
    EXPECT_EQ(command.params[1], "param2");
    EXPECT_EQ(command.params[2], "trailing param");
}

TEST(CommandParserTest, ParseCommandWithEmptyParams)
{
    std::string rawMessage = ":JohnDoe COMMAND\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix.nickname, "JohnDoe");
    EXPECT_EQ(command.command, "COMMAND");
    EXPECT_TRUE(command.params.empty());
}

TEST(CommandParserTest, ParseCommandWithOnlyTrailingParam)
{
    std::string rawMessage = ":JohnDoe COMMAND :trailing param\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix.nickname, "JohnDoe");
    EXPECT_EQ(command.command, "COMMAND");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "trailing param");
}

TEST(CommandParserTest, ParseCommandWithMaxParams)
{
    std::string rawMessage = ":JohnDoe COMMAND param1 param2 param3 param4 param5 param6 param7 param8 param9 param10 param11 param12 param13 param14 param15\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix.nickname, "JohnDoe");
    EXPECT_EQ(command.command, "COMMAND");
    ASSERT_EQ(command.params.size(), 15);
    EXPECT_EQ(command.params[14], "param15");
}

TEST(CommandParserTest, ParseCommandWithTooManyParams)
{
    std::string rawMessage = ":JohnDoe COMMAND param1 param2 param3 param4 param5 param6 param7 param8 param9 param10 param11 param12 param13 param14 param15 param16\r\n";
    try {
        CommandParser::parse(rawMessage);
        FAIL() << "Expected CommandParser::ParseError";
    } catch (const CommandParser::ParseError& err)
    {
        EXPECT_EQ(err.what(), std::string("Too many parameters"));
    } catch (...)
    {
        FAIL() << "Expected CommandParser::ParseError";
    }
}

TEST(CommandParserTest, ParseCommandWithMaxPrefixLength)
{
    std::string longPrefix(511, 'a'); // Adjust length to 511 to account for the colon
    std::string rawMessage = ":" + longPrefix + " COMMAND param1\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix.nickname, longPrefix);
    EXPECT_EQ(command.command, "COMMAND");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "param1");
}

TEST(CommandParserTest, ParseCommandWithTooLongPrefix)
{
    std::string longPrefix(512, 'a'); // Adjust length to 512 to account for the colon
    std::string rawMessage = ":" + longPrefix + " COMMAND param1\r\n";
    try {
        CommandParser::parse(rawMessage);
        FAIL() << "Expected CommandParser::ParseError";
    } catch (const CommandParser::ParseError& err)
    {
        EXPECT_EQ(err.what(), std::string("Prefix too long"));
    } catch (...)
    {
        FAIL() << "Expected CommandParser::ParseError";
    }
}

TEST(CommandParserTest, ParseCommandWithMaxCommandLength)
{
    std::string longCommand(510, 'a');
    std::string rawMessage = longCommand + " param1\r\n";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.command, longCommand);
}

TEST(CommandParserTest, ParseCommandWithTooLongCommand)
{
    std::string longCommand(511, 'a');
    std::string rawMessage = longCommand + " param1\r\n";
    try {
        CommandParser::parse(rawMessage);
        FAIL() << "Expected CommandParser::ParseError";
    } catch (const CommandParser::ParseError& err)
    {
        EXPECT_EQ(err.what(), std::string("Invalid command format"));
    } catch (...)
    {
        FAIL() << "Expected CommandParser::ParseError";
    }
}

TEST(CommandParserTest, ParseCommandWithoutCRLF)
{
    std::string rawMessage = ":JohnDoe COMMAND param1";
    try {
        CommandParser::parse(rawMessage);
        FAIL() << "Expected CommandParser::ParseError";
    } catch (const CommandParser::ParseError& err)
    {
        EXPECT_EQ(err.what(), std::string("Invalid message termination"));
    } catch (...)
    {
        FAIL() << "Expected CommandParser::ParseError";
    }
}

