
// >>THIS FILE IS AUTO GENERATED<< (DON'T MODIFY ITS CONTENT)

#pragma once

#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, N, ...) N

#define REGISTER_ATTR(ATTR_NAME)     attributes.emplace_back(#ATTR_NAME, SerializeAttributeValue(ast->ATTR_NAME));

#define FOREACH_ARG(FUNC, ...)     FOREACH_ARG_IMPL(COUNT_ARGS(__VA_ARGS__), FUNC, __VA_ARGS__)

#define FOREACH_ARG_IMPL(N, FUNC, ...)     FOREACH_ARG_IMPL_(N, FUNC, __VA_ARGS__)

#define FOREACH_ARG_IMPL_(N, FUNC, ...)     FOREACH_ARG_##N(FUNC, __VA_ARGS__)

#define FOREACH_ARG_1(FUNC, A) FUNC(A)
#define FOREACH_ARG_2(FUNC, A, ...)   FUNC(A) FOREACH_ARG_1(FUNC, __VA_ARGS__)
#define FOREACH_ARG_3(FUNC, A, ...)   FUNC(A) FOREACH_ARG_2(FUNC, __VA_ARGS__)
#define FOREACH_ARG_4(FUNC, A, ...)   FUNC(A) FOREACH_ARG_3(FUNC, __VA_ARGS__)
#define FOREACH_ARG_5(FUNC, A, ...)   FUNC(A) FOREACH_ARG_4(FUNC, __VA_ARGS__)
#define FOREACH_ARG_6(FUNC, A, ...)   FUNC(A) FOREACH_ARG_5(FUNC, __VA_ARGS__)
#define FOREACH_ARG_7(FUNC, A, ...)   FUNC(A) FOREACH_ARG_6(FUNC, __VA_ARGS__)
#define FOREACH_ARG_8(FUNC, A, ...)   FUNC(A) FOREACH_ARG_7(FUNC, __VA_ARGS__)
#define FOREACH_ARG_9(FUNC, A, ...)   FUNC(A) FOREACH_ARG_8(FUNC, __VA_ARGS__)
#define FOREACH_ARG_10(FUNC, A, ...)  FUNC(A) FOREACH_ARG_9(FUNC, __VA_ARGS__)
#define FOREACH_ARG_11(FUNC, A, ...)  FUNC(A) FOREACH_ARG_10(FUNC, __VA_ARGS__)
#define FOREACH_ARG_12(FUNC, A, ...)  FUNC(A) FOREACH_ARG_11(FUNC, __VA_ARGS__)
#define FOREACH_ARG_13(FUNC, A, ...)  FUNC(A) FOREACH_ARG_12(FUNC, __VA_ARGS__)
#define FOREACH_ARG_14(FUNC, A, ...)  FUNC(A) FOREACH_ARG_13(FUNC, __VA_ARGS__)
#define FOREACH_ARG_15(FUNC, A, ...)  FUNC(A) FOREACH_ARG_14(FUNC, __VA_ARGS__)
#define FOREACH_ARG_16(FUNC, A, ...)  FUNC(A) FOREACH_ARG_15(FUNC, __VA_ARGS__)
#define FOREACH_ARG_17(FUNC, A, ...)  FUNC(A) FOREACH_ARG_16(FUNC, __VA_ARGS__)
#define FOREACH_ARG_18(FUNC, A, ...)  FUNC(A) FOREACH_ARG_17(FUNC, __VA_ARGS__)
#define FOREACH_ARG_19(FUNC, A, ...)  FUNC(A) FOREACH_ARG_18(FUNC, __VA_ARGS__)
#define FOREACH_ARG_20(FUNC, A, ...)  FUNC(A) FOREACH_ARG_19(FUNC, __VA_ARGS__)
#define FOREACH_ARG_21(FUNC, A, ...)  FUNC(A) FOREACH_ARG_20(FUNC, __VA_ARGS__)
#define FOREACH_ARG_22(FUNC, A, ...)  FUNC(A) FOREACH_ARG_21(FUNC, __VA_ARGS__)
#define FOREACH_ARG_23(FUNC, A, ...)  FUNC(A) FOREACH_ARG_22(FUNC, __VA_ARGS__)
#define FOREACH_ARG_24(FUNC, A, ...)  FUNC(A) FOREACH_ARG_23(FUNC, __VA_ARGS__)
#define FOREACH_ARG_25(FUNC, A, ...)  FUNC(A) FOREACH_ARG_24(FUNC, __VA_ARGS__)
#define FOREACH_ARG_26(FUNC, A, ...)  FUNC(A) FOREACH_ARG_25(FUNC, __VA_ARGS__)
#define FOREACH_ARG_27(FUNC, A, ...)  FUNC(A) FOREACH_ARG_26(FUNC, __VA_ARGS__)
#define FOREACH_ARG_28(FUNC, A, ...)  FUNC(A) FOREACH_ARG_27(FUNC, __VA_ARGS__)
#define FOREACH_ARG_29(FUNC, A, ...)  FUNC(A) FOREACH_ARG_28(FUNC, __VA_ARGS__)
#define FOREACH_ARG_30(FUNC, A, ...)  FUNC(A) FOREACH_ARG_29(FUNC, __VA_ARGS__)
#define FOREACH_ARG_31(FUNC, A, ...)  FUNC(A) FOREACH_ARG_30(FUNC, __VA_ARGS__)
#define FOREACH_ARG_32(FUNC, A, ...)  FUNC(A) FOREACH_ARG_31(FUNC, __VA_ARGS__)
#define FOREACH_ARG_33(FUNC, A, ...)  FUNC(A) FOREACH_ARG_32(FUNC, __VA_ARGS__)
#define FOREACH_ARG_34(FUNC, A, ...)  FUNC(A) FOREACH_ARG_33(FUNC, __VA_ARGS__)
#define FOREACH_ARG_35(FUNC, A, ...)  FUNC(A) FOREACH_ARG_34(FUNC, __VA_ARGS__)
#define FOREACH_ARG_36(FUNC, A, ...)  FUNC(A) FOREACH_ARG_35(FUNC, __VA_ARGS__)
#define FOREACH_ARG_37(FUNC, A, ...)  FUNC(A) FOREACH_ARG_36(FUNC, __VA_ARGS__)
#define FOREACH_ARG_38(FUNC, A, ...)  FUNC(A) FOREACH_ARG_37(FUNC, __VA_ARGS__)
#define FOREACH_ARG_39(FUNC, A, ...)  FUNC(A) FOREACH_ARG_38(FUNC, __VA_ARGS__)
#define FOREACH_ARG_40(FUNC, A, ...)  FUNC(A) FOREACH_ARG_39(FUNC, __VA_ARGS__)
#define FOREACH_ARG_41(FUNC, A, ...)  FUNC(A) FOREACH_ARG_40(FUNC, __VA_ARGS__)
#define FOREACH_ARG_42(FUNC, A, ...)  FUNC(A) FOREACH_ARG_41(FUNC, __VA_ARGS__)
#define FOREACH_ARG_43(FUNC, A, ...)  FUNC(A) FOREACH_ARG_42(FUNC, __VA_ARGS__)
#define FOREACH_ARG_44(FUNC, A, ...)  FUNC(A) FOREACH_ARG_43(FUNC, __VA_ARGS__)
#define FOREACH_ARG_45(FUNC, A, ...)  FUNC(A) FOREACH_ARG_44(FUNC, __VA_ARGS__)
#define FOREACH_ARG_46(FUNC, A, ...)  FUNC(A) FOREACH_ARG_45(FUNC, __VA_ARGS__)
#define FOREACH_ARG_47(FUNC, A, ...)  FUNC(A) FOREACH_ARG_46(FUNC, __VA_ARGS__)
#define FOREACH_ARG_48(FUNC, A, ...)  FUNC(A) FOREACH_ARG_47(FUNC, __VA_ARGS__)
#define FOREACH_ARG_49(FUNC, A, ...)  FUNC(A) FOREACH_ARG_48(FUNC, __VA_ARGS__)
#define FOREACH_ARG_50(FUNC, A, ...)  FUNC(A) FOREACH_ARG_49(FUNC, __VA_ARGS__)

#define DO_NOTHING(A) (void)(A);
