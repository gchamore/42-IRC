#include <gtest/gtest.h>
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"

TEST(ChannelTest, AddMemberTest)
{
    Channel channel("#general");
    Client client(1);
    client.setNickname("TestNick");
    channel.addMember(&client);
    const std::vector<Client*>& members = channel.getMembers();
    ASSERT_EQ(members.size(), 1);
    EXPECT_EQ(members[0]->getNickname(), "TestNick");
}

TEST(ChannelTest, RemoveMemberTest)
{
    Channel channel("#general");
    Client client(1);
    client.setNickname("TestNick");
    channel.addMember(&client);
    channel.removeMember(&client);
    const std::vector<Client*>& members = channel.getMembers();
    EXPECT_TRUE(members.empty());
}

TEST(ChannelTest, MultipleMembersTest)
{
    Channel channel("#general");
    Client client1(1);
    client1.setNickname("TestNick1");
    Client client2(2);
    client2.setNickname("TestNick2");
    channel.addMember(&client1);
    channel.addMember(&client2);
    const std::vector<Client*>& members = channel.getMembers();
    ASSERT_EQ(members.size(), 2);
    EXPECT_EQ(members[0]->getNickname(), "TestNick1");
    EXPECT_EQ(members[1]->getNickname(), "TestNick2");
}

TEST(ChannelTest, AddSameMemberTwice)
{
    Channel channel("#general");
    Client client(1);
    client.setNickname("TestNick");
    channel.addMember(&client);
    channel.addMember(&client);
    const std::vector<Client*>& members = channel.getMembers();
    ASSERT_EQ(members.size(), 1) << "Member added twice";
}

TEST(ChannelTest, RemoveNonExistentMember)
{
    Channel channel("#general");
    Client client(1);
    client.setNickname("TestNick");
    channel.removeMember(&client);
    const std::vector<Client*>& members = channel.getMembers();
    EXPECT_TRUE(members.empty());
}

TEST(ChannelTest, AddAndRemoveMultipleMembers)
{
    Channel channel("#general");
    Client client1(1);
    client1.setNickname("TestNick1");
    Client client2(2);
    client2.setNickname("TestNick2");
    channel.addMember(&client1);
    channel.addMember(&client2);
    channel.removeMember(&client1);
    const std::vector<Client*>& members = channel.getMembers();
    ASSERT_EQ(members.size(), 1);
    EXPECT_EQ(members[0]->getNickname(), "TestNick2");
}

TEST(ChannelTest, ChannelNameTest)
{
    Channel channel("#general");
    EXPECT_EQ(channel.getName(), "#general");
}

TEST(ChannelTest, AddNullMember)
{
    Channel channel("#general");
    EXPECT_THROW(channel.addMember(nullptr), std::invalid_argument);
}

TEST(ChannelTest, RemoveNullMember)
{
    Channel channel("#general");
    EXPECT_NO_THROW(channel.removeMember(nullptr));
}
