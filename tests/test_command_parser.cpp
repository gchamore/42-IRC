#include <gtest/gtest.h>
#include "../includes/CommandParser.hpp"

TEST(CommandParserTest, ParseNickCommand)
{
    std::string rawMessage = ":JohnDoe NICK JohnDoe";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix, "JohnDoe");
    EXPECT_EQ(command.command, "NICK");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "JohnDoe");
}

TEST(CommandParserTest, ParseUserCommand)
{
    std::string rawMessage = ":JohnDoe USER johndoe";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix, "JohnDoe");
    EXPECT_EQ(command.command, "USER");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "johndoe");
}

TEST(CommandParserTest, ParseJoinCommand)
{
    std::string rawMessage = ":JohnDoe JOIN #general";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix, "JohnDoe");
    EXPECT_EQ(command.command, "JOIN");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "#general");
}

TEST(CommandParserTest, ParsePrivmsgCommand)
{
    std::string rawMessage = ":JohnDoe PRIVMSG #general :Hello everyone!";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix, "JohnDoe");
    EXPECT_EQ(command.command, "PRIVMSG");
    ASSERT_EQ(command.params.size(), 2);
    EXPECT_EQ(command.params[0], "#general");
    EXPECT_EQ(command.params[1], "Hello everyone!");
}

TEST(CommandParserTest, ParseInvalidCommand)
{
    std::string rawMessage = ":JohnDoe INVALIDCOMMAND";
    try {
        CommandParser::parse(rawMessage);
        FAIL() << "Expected std::invalid_argument";
    } catch (const std::invalid_argument& err)
    {
        EXPECT_EQ(std::string(err.what()), "Invalid command");
    } catch (...)
    {
        FAIL() << "Expected std::invalid_argument";
    }
}

TEST(CommandParserTest, ParseEmptyMessage)
{
    std::string rawMessage = "";
    try {
        CommandParser::parse(rawMessage);
        FAIL() << "Expected std::invalid_argument";
    } catch (const std::invalid_argument& err)
    {
        EXPECT_EQ(err.what(), std::string("Empty message"));
    } catch (...)
    {
        FAIL() << "Expected std::invalid_argument";
    }
}

TEST(CommandParserTest, ParseNoPrefix)
{
    std::string rawMessage = "NICK JohnDoe";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix, "");
    EXPECT_EQ(command.command, "NICK");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "JohnDoe");
}

TEST(CommandParserTest, ParseCommandWithMultipleParams)
{
    std::string rawMessage = ":JohnDoe COMMAND param1 param2 param3";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix, "JohnDoe");
    EXPECT_EQ(command.command, "COMMAND");
    ASSERT_EQ(command.params.size(), 3);
    EXPECT_EQ(command.params[0], "param1");
    EXPECT_EQ(command.params[1], "param2");
    EXPECT_EQ(command.params[2], "param3");
}

TEST(CommandParserTest, ParseCommandWithTrailingParam)
{
    std::string rawMessage = ":JohnDoe COMMAND param1 param2 :trailing param";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix, "JohnDoe");
    EXPECT_EQ(command.command, "COMMAND");
    ASSERT_EQ(command.params.size(), 3);
    EXPECT_EQ(command.params[0], "param1");
    EXPECT_EQ(command.params[1], "param2");
    EXPECT_EQ(command.params[2], "trailing param");
}

TEST(CommandParserTest, ParseCommandWithEmptyParams)
{
    std::string rawMessage = ":JohnDoe COMMAND";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix, "JohnDoe");
    EXPECT_EQ(command.command, "COMMAND");
    EXPECT_TRUE(command.params.empty());
}

TEST(CommandParserTest, ParseCommandWithOnlyTrailingParam)
{
    std::string rawMessage = ":JohnDoe COMMAND :trailing param";
    CommandParser::ParsedCommand command = CommandParser::parse(rawMessage);
    EXPECT_EQ(command.prefix, "JohnDoe");
    EXPECT_EQ(command.command, "COMMAND");
    ASSERT_EQ(command.params.size(), 1);
    EXPECT_EQ(command.params[0], "trailing param");
}
