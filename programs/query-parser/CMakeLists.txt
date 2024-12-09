set (CLICKHOUSE_QUERY_PARSER_SOURCES ASTCustomAttributes.cpp ASTSerialization.cpp ASTNodeBuilder.cpp JsonSerialization.cpp Macros.cpp QueryParser.cpp)

set (CLICKHOUSE_QUERY_PARSER_LINK
    PRIVATE
        dbms
        clickhouse_parsers
)

clickhouse_program_add_library(query-parser)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
add_library(clickhouse_query_parser_SO SHARED ${CLICKHOUSE_QUERY_PARSER_SOURCES})
set_property(TARGET clickhouse_query_parser_SO PROPERTY POSITION_INDEPENDENT_CODE ON)
set_target_properties(clickhouse_query_parser_SO PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  C_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)
target_link_libraries(clickhouse_query_parser_SO
    PRIVATE
        dbms
        clickhouse_parsers
)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()
