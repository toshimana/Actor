#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Actor.h>

using namespace ::testing;

// テスト用Actor
class TestActor : public Actor
{

};

class ActorTest : public ::testing::Test
{
};

TEST_F( ActorTest, spawnTest )
{

}