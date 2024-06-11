#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "rapidjson/document.h"

#include <Common/SettingsChanges.h>
#include <Parsers/ASTAlterNamedCollectionQuery.h>
#include <Parsers/ASTExpressionList.h>

#include "gtest_helpers.hpp"


TEST(JsonSerialization, ASTAlterNamedCollectionQuery) {
  auto v = std::make_shared<DB::ASTAlterNamedCollectionQuery>();
  v->collection_name = "collection";
  auto changes = DB::SettingsChanges{};
  changes.push_back(DB::SettingChange("fail", true));
  v->changes = changes;
  v->delete_keys = std::vector<std::string>{"password"};
  v->if_exists = true;
  v->overridability = std::unordered_map<std::string, bool>{{"Tsoi is alive", true}};
  std::string json = SerializeToJSON(v);
  rapidjson::Document document;
  document.Parse(json.c_str());
  EXPECT_TRUE(document.IsObject());
  EXPECT_TRUE(document.HasMember("id"));
  EXPECT_TRUE(document["id"].IsString());
  EXPECT_TRUE(document.HasMember("attributes"));
  EXPECT_TRUE(document["attributes"].IsObject());

  EXPECT_TRUE(document["attributes"].HasMember("overridability"));
  EXPECT_TRUE(document["attributes"]["overridability"].IsObject());
  EXPECT_TRUE(document["attributes"]["overridability"].HasMember("Tsoi is alive"));
  EXPECT_TRUE(document["attributes"]["overridability"]["Tsoi is alive"].IsBool());
  EXPECT_EQ(document["attributes"]["overridability"]["Tsoi is alive"].GetBool(), true);

  EXPECT_TRUE(document["attributes"].HasMember("if_exists"));
  EXPECT_TRUE(document["attributes"]["if_exists"].IsBool());
  EXPECT_EQ(document["attributes"]["if_exists"].GetBool(), true);

  EXPECT_TRUE(document["attributes"].HasMember("delete_keys"));
  EXPECT_TRUE(document["attributes"]["delete_keys"].IsArray());
  EXPECT_EQ(document["attributes"]["delete_keys"].GetArray().Size(), 1);
  EXPECT_TRUE(document["attributes"]["delete_keys"].GetArray()[0].IsString());
  EXPECT_STREQ(document["attributes"]["delete_keys"].GetArray()[0].GetString(), "password");

  EXPECT_TRUE(document["attributes"].HasMember("collection_name"));
  EXPECT_TRUE(document["attributes"]["collection_name"].IsString());
  EXPECT_STREQ(document["attributes"]["collection_name"].GetString(), "collection");

  EXPECT_TRUE(document["attributes"].HasMember("changes"));
  EXPECT_TRUE(document["attributes"]["changes"].IsArray());
  EXPECT_EQ(document["attributes"]["changes"].GetArray().Size(), 1);
  EXPECT_TRUE(document["attributes"]["changes"].GetArray()[0].IsObject());
  EXPECT_TRUE(document["attributes"]["changes"].GetArray()[0].GetObject().HasMember("fail"));
  EXPECT_TRUE(document["attributes"]["changes"].GetArray()[0].GetObject()["fail"].IsString());
  EXPECT_STREQ(document["attributes"]["changes"].GetArray()[0].GetObject()["fail"].GetString(), "Bool_1");

  EXPECT_TRUE(document["attributes"].HasMember("label"));
  EXPECT_TRUE(document["attributes"]["label"].IsString());
  EXPECT_STREQ(document["attributes"]["label"].GetString(), "AlterNamedCollectionQuery");
}

TEST(JsonSerialization, ExpressionList) {
  auto v = std::make_shared<DB::ASTExpressionList>();
  v->separator = 'x';
  std::string json = SerializeToJSON(v);
  rapidjson::Document document;
  document.Parse(json.c_str());
  EXPECT_TRUE(document.IsObject());
  EXPECT_TRUE(document.HasMember("attributes"));
  EXPECT_TRUE(document["attributes"].IsObject());
  EXPECT_TRUE(document["attributes"].HasMember("separator"));
  EXPECT_TRUE(document["attributes"]["separator"].IsString());
  EXPECT_STREQ(document["attributes"]["separator"].GetString(), "x");
}
