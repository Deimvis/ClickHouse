#include <gtest/gtest.h>

#include <string>
#include <unordered_set>
#include <vector>

#include "gtest_helpers.hpp"

TEST(JsonSerialization, int) {
  CHECK(int, 42, 42);
}


TEST(JsonSerialization, string) {
  CHECK(std::string, "hello, world", "hello, world");
}

TEST(JsonSerialization, vectorInt) {
  check(std::vector<int>{1, 2, 3}, R"([1,2,3])");
}

TEST(JsonSerialization, vectorString) {
  check(std::vector<std::string>{"1", "2", "3"}, R"(["1","2","3"])");
}

TEST(JsonSerialization, usetInt) {
  auto json = SerializeToJSON(std::unordered_set<int>{1, 2, 3});
  EXPECT_EQ(json.length(), 7);
  EXPECT_EQ(json.front(), '[');
  EXPECT_EQ(json.back(), ']');
  EXPECT_TRUE((json.find('1') != std::string::npos));
  EXPECT_TRUE((json.find('2') != std::string::npos));
  EXPECT_TRUE((json.find('3') != std::string::npos));
  EXPECT_EQ(json.find(','), 2);
  EXPECT_EQ(json.rfind(','), 4);
}

TEST(JsonSerialization, umapStringInt) {
  check(std::unordered_map<std::string, int>{{"ans", 42}}, R"({"ans":42})");
}
