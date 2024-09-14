#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "header.hpp"

class IRCServerTest : public ::testing::Test {
protected:
  void SetUp() override {
    server = new ();
  }

  void TearDown() override {
    delete server;
  }

  IRCServer *server;
};

TEST_F(IRCServerTest, PassCommand) {
  EXPECT_TRUE(server->handleCommand("PASS secretpassword"));
  EXPECT_FALSE(server->handleCommand("PASS incorrectpassword"));
}

TEST_F(IRCServerTest, NickCommand) {
  EXPECT_TRUE(server->handleCommand("NICK validnick"));
  EXPECT_FALSE(server->handleCommand("NICK"));              // Missing parameter
  EXPECT_FALSE(server->handleCommand("NICK invalid@nick")); // Invalid character
}

TEST_F(IRCServerTest, UserCommand) {
  EXPECT_TRUE(
      server->handleCommand("USER username hostname servername :Real Name"));
  EXPECT_FALSE(server->handleCommand("USER username")); // Missing parameters
}

TEST_F(IRCServerTest, JoinCommand) {
  EXPECT_TRUE(server->handleCommand("JOIN #channel"));
  EXPECT_TRUE(server->handleCommand("JOIN #channel,#channel2"));
  EXPECT_FALSE(server->handleCommand("JOIN")); // Missing parameter
}

TEST_F(IRCServerTest, InviteCommand) {
  EXPECT_TRUE(server->handleCommand("INVITE nickname #channel"));
  EXPECT_FALSE(server->handleCommand("INVITE")); // Missing parameters
}

TEST_F(IRCServerTest, TopicCommand) {
  EXPECT_TRUE(server->handleCommand("TOPIC #channel :New topic"));
  EXPECT_TRUE(server->handleCommand("TOPIC #channel")); // Get current topic
  EXPECT_FALSE(server->handleCommand("TOPIC"));         // Missing parameter
}

TEST_F(IRCServerTest, ModeCommand) {
  EXPECT_TRUE(server->handleCommand("MODE #channel +o nickname"));
  EXPECT_TRUE(server->handleCommand("MODE nickname +i"));
  EXPECT_FALSE(server->handleCommand("MODE")); // Missing parameters
}

TEST_F(IRCServerTest, KickCommand) {
  EXPECT_TRUE(server->handleCommand("KICK #channel nickname :Reason for kick"));
  EXPECT_FALSE(server->handleCommand("KICK")); // Missing parameters
}

TEST_F(IRCServerTest, PrivmsgCommand) {
  EXPECT_TRUE(server->handleCommand("PRIVMSG #channel :Hello, world!"));
  EXPECT_TRUE(server->handleCommand("PRIVMSG nickname :Private message"));
  EXPECT_FALSE(server->handleCommand("PRIVMSG")); // Missing parameters
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
