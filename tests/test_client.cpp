#include <gtest/gtest.h>
#include "../includes/Client.hpp"

TEST(ClientTest, NicknameTest)
{
    Client client(1);
    client.setNickname("TestNick");
    EXPECT_EQ(client.getNickname(), "TestNick");
}

TEST(ClientTest, UsernameTest)
{
    Client client(1);
    client.setUsername("TestUser");
    EXPECT_EQ(client.getUsername(), "TestUser");
}

TEST(ClientTest, AuthenticationTest)
{
    Client client(1);
    EXPECT_FALSE(client.authenticated());
    client.authenticate();
    EXPECT_TRUE(client.authenticated());
}

TEST(ClientTest, InvalidNicknameTest)
{
    Client client(1);
    EXPECT_THROW(client.setNickname(""), std::invalid_argument);
}

TEST(ClientTest, InvalidUsernameTest)
{
    Client client(1);
    EXPECT_THROW(client.setUsername(""), std::invalid_argument);
}

TEST(ClientTest, SetNicknameTwice)
{
    Client client(1);
    client.setNickname("FirstNick");
    client.setNickname("SecondNick");
    EXPECT_EQ(client.getNickname(), "SecondNick");
}

TEST(ClientTest, SetUsernameTwice)
{
    Client client(1);
    client.setUsername("FirstUser");
    client.setUsername("SecondUser");
    EXPECT_EQ(client.getUsername(), "SecondUser");
}

TEST(ClientTest, GetFDTest)
{
    Client client(42);
    EXPECT_EQ(client.getFD(), 42);
}

TEST(ClientTest, AuthenticateTwice)
{
    Client client(1);
    client.authenticate();
    client.authenticate();
    EXPECT_TRUE(client.authenticated());
}

TEST(ClientTest, NicknameNotSet)
{
    Client client(1);
    EXPECT_EQ(client.getNickname(), "");
}

TEST(ClientTest, UsernameNotSet)
{
    Client client(1);
    EXPECT_EQ(client.getUsername(), "");
}

TEST(ClientTest, NegativeSocketFD)
{
    EXPECT_THROW(Client client(-1), std::invalid_argument);
}
