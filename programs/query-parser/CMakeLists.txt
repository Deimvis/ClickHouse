set (CLICKHOUSE_QUERY_PARSER_SOURCES ASTCustomAttributes.cpp ASTSerialization.cpp ASTNodeBuilder.cpp JsonSerialization.cpp Macros.cpp QueryParser.cpp)

set (CLICKHOUSE_QUERY_PARSER_LINK
    PRIVATE
        clickhouse_aggregate_functions
        clickhouse_common_config
        clickhouse_common_io
        clickhouse_common_zookeeper
        clickhouse_functions
        clickhouse_parsers
        clickhouse_storages_system
        clickhouse_table_functions
)

clickhouse_program_add_library(query-parser)

# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# add_library(clickhouse_query_parser_SO SHARED ${CLICKHOUSE_QUERY_PARSER_SOURCES})

# set_property(TARGET clickhouse_query_parser_SO PROPERTY POSITION_INDEPENDENT_CODE ON)

# target_link_libraries(clickhouse_query_parser_SO
#     PRIVATE
#         clickhouse_aggregate_functions
#         clickhouse_common_config
#         clickhouse_common_io
#         clickhouse_common_zookeeper
#         clickhouse_functions
#         clickhouse_parsers
#         clickhouse_storages_system
#         clickhouse_table_functions
# )

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()
