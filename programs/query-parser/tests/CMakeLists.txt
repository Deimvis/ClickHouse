if (ENABLE_TESTS)
    macro (grep_gtest_sources BASE_DIR DST_VAR)
        # Cold match files that are not in tests/ directories
        file(GLOB_RECURSE "${DST_VAR}" CONFIGURE_DEPENDS RELATIVE "${BASE_DIR}" "gtest*.cpp")
    endmacro()

    # attach all dbms gtest sources
    grep_gtest_sources("${ClickHouse_SOURCE_DIR}/programs/query-parser/tests" query_parser_gtest_sources)
    clickhouse_add_executable(query_parser_unit_tests ${query_parser_gtest_sources})

    # gtest framework has substandard code
    target_compile_options(query_parser_unit_tests PRIVATE
        -Wno-sign-compare
    )

    target_link_libraries(query_parser_unit_tests PRIVATE
        ch_contrib::gmock_all
        ch_contrib::gtest
    )
    
    target_link_libraries(query_parser_unit_tests PUBLIC clickhouse-query-parser-lib)

    if (TARGET ch_contrib::simdjson)
        target_link_libraries(query_parser_unit_tests PRIVATE ch_contrib::simdjson)
    endif()

    if (TARGET ch_contrib::yaml_cpp)
        target_link_libraries(query_parser_unit_tests PRIVATE ch_contrib::yaml_cpp)
    endif()

    if (TARGET ch_contrib::azure_sdk)
        target_link_libraries(query_parser_unit_tests PRIVATE ch_contrib::azure_sdk)
    endif()

    if (TARGET ch_contrib::parquet)
        target_link_libraries(query_parser_unit_tests PRIVATE ch_contrib::parquet)
    endif()
endif ()