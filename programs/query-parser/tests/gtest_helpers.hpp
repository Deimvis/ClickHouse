#pragma once

#include "../ASTSerialization.hpp"
#include "../JsonSerialization.hpp"

#define CHECK(TYPE, VALUE, EXPECTED)                   \
  TYPE v = (VALUE);                                    \
  EXPECT_STREQ(SerializeToJSON(v).c_str(), #EXPECTED) << "Mismatch in " << __FILE__ << ":" << __LINE__


template <typename T>
void check(const T& v, const char* expected) {
  EXPECT_STREQ(SerializeToJSON(v).c_str(), expected);
}
