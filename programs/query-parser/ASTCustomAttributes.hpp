#pragma once

#include <vector>

#include<Parsers/ASTAlterNamedCollectionQuery.h>
#include<Parsers/ASTAlterQuery.h>
#include<Parsers/ASTAssignment.h>
#include<Parsers/ASTAsterisk.h>
#include<Parsers/ASTBackupQuery.h>
#include<Parsers/ASTCheckQuery.h>
#include<Parsers/ASTCollation.h>
#include<Parsers/ASTColumnDeclaration.h>
#include<Parsers/ASTColumnsMatcher.h>
#include<Parsers/ASTColumnsTransformers.h>
#include<Parsers/ASTConstraintDeclaration.h>
#include<Parsers/ASTCreateFunctionQuery.h>
#include<Parsers/ASTCreateIndexQuery.h>
#include<Parsers/ASTCreateNamedCollectionQuery.h>
#include<Parsers/ASTCreateQuery.h>
#include<Parsers/ASTDatabaseOrNone.h>
#include<Parsers/ASTDeleteQuery.h>
#include<Parsers/ASTDescribeCacheQuery.h>
#include<Parsers/ASTDictionaryAttributeDeclaration.h>
#include<Parsers/ASTDictionary.h>
#include<Parsers/ASTDropFunctionQuery.h>
#include<Parsers/ASTDropIndexQuery.h>
#include<Parsers/ASTDropNamedCollectionQuery.h>
#include<Parsers/ASTDropQuery.h>
#include<Parsers/ASTExplainQuery.h>
#include<Parsers/ASTExpressionList.h>
#include<Parsers/ASTExternalDDLQuery.h>
#include<Parsers/ASTForeignKeyDeclaration.h>
#include<Parsers/ASTFunction.h>
#include<Parsers/ASTFunctionWithKeyValueArguments.h>
#include<Parsers/ASTHelpers.h>
#include<Parsers/ASTIdentifier_fwd.h>
#include<Parsers/ASTIdentifier.h>
#include<Parsers/ASTIndexDeclaration.h>
#include<Parsers/ASTInsertQuery.h>
#include<Parsers/ASTInterpolateElement.h>
#include<Parsers/ASTKillQueryQuery.h>
#include<Parsers/ASTLiteral.h>
#include<Parsers/ASTNameTypePair.h>
#include<Parsers/ASTOptimizeQuery.h>
#include<Parsers/ASTOrderByElement.h>
#include<Parsers/ASTPartition.h>
#include<Parsers/ASTProjectionDeclaration.h>
#include<Parsers/ASTProjectionSelectQuery.h>
#include<Parsers/ASTQualifiedAsterisk.h>
#include<Parsers/ASTQueryParameter.h>
#include<Parsers/ASTQueryWithOnCluster.h>
#include<Parsers/ASTQueryWithOutput.h>
#include<Parsers/ASTQueryWithTableAndOutput.h>
#include<Parsers/ASTRefreshStrategy.h>
#include<Parsers/ASTRenameQuery.h>
#include<Parsers/ASTSampleRatio.h>
#include<Parsers/ASTSelectIntersectExceptQuery.h>
#include<Parsers/ASTSelectQuery.h>
#include<Parsers/ASTSelectWithUnionQuery.h>
#include<Parsers/ASTSetQuery.h>
#include<Parsers/ASTShowColumnsQuery.h>
#include<Parsers/ASTShowEngineQuery.h>
#include<Parsers/ASTShowFunctionsQuery.h>
#include<Parsers/ASTShowIndexesQuery.h>
#include<Parsers/ASTShowProcesslistQuery.h>
#include<Parsers/ASTShowSettingQuery.h>
#include<Parsers/ASTShowTablesQuery.h>
#include<Parsers/ASTSQLSecurity.h>
#include<Parsers/ASTStatisticsDeclaration.h>
#include<Parsers/ASTSubquery.h>
#include<Parsers/ASTSystemQuery.h>
#include<Parsers/ASTTableOverrides.h>
#include<Parsers/ASTTablesInSelectQuery.h>
#include<Parsers/ASTTimeInterval.h>
#include<Parsers/ASTTransactionControl.h>
#include<Parsers/ASTTTLElement.h>
#include<Parsers/ASTUndropQuery.h>
#include<Parsers/ASTUseQuery.h>
#include<Parsers/ASTWatchQuery.h>
#include<Parsers/ASTWindowDefinition.h>
#include<Parsers/ASTWithAlias.h>
#include<Parsers/ASTWithElement.h>

#include "Macros.hpp"
#include "ASTNodeBuilder.hpp"


#define AST_CUSTOM_ATTRIBUTES(AST_CLASS, ...) \
    inline std::vector<Attribute> GetCustomAttributesImpl(const AST_CLASS* ast) {   \
        std::vector<Attribute> attributes; \
        FOREACH_ARG(REGISTER_ATTR, __VA_ARGS__) \
        return attributes; \
    }

inline std::vector<Attribute> GetCustomAttributesImpl(const DB::IAST*) {
    return {};
}

AST_CUSTOM_ATTRIBUTES(DB::ASTAlterNamedCollectionQuery, collection_name, changes, delete_keys, if_exists, overridability)
// AST_CUSTOM_ATTRIBUTES(DB::ASTAlterQuery)
AST_CUSTOM_ATTRIBUTES(DB::ASTAssignment, column_name)
// AST_CUSTOM_ATTRIBUTES(DB::ASTAsterisk)
// AST_CUSTOM_ATTRIBUTES(DB::ASTBackupQuery)
// AST_CUSTOM_ATTRIBUTES(DB::ASTCheckQuery)
// AST_CUSTOM_ATTRIBUTES(DB::ASTCollation)
AST_CUSTOM_ATTRIBUTES(DB::ASTColumnDeclaration, name, null_modifier, default_specifier, ephemeral_default, primary_key_specifier)
// AST_CUSTOM_ATTRIBUTES(DB::ASTColumnsMatcher)
// AST_CUSTOM_ATTRIBUTES(DB::ASTColumnsTransformers)
AST_CUSTOM_ATTRIBUTES(DB::ASTConstraintDeclaration, name)
AST_CUSTOM_ATTRIBUTES(DB::ASTCreateFunctionQuery, or_replace, if_not_exists)
AST_CUSTOM_ATTRIBUTES(DB::ASTCreateIndexQuery, if_not_exists, unique)
AST_CUSTOM_ATTRIBUTES(DB::ASTCreateNamedCollectionQuery, collection_name, changes, if_not_exists, overridability)
AST_CUSTOM_ATTRIBUTES(DB::ASTCreateQuery, attach, if_not_exists, is_ordinary_view, is_live_view, is_window_view, is_populate, is_create_empty, replace_view, has_uuid, as_database, as_table, is_dictionary, is_watermark_strictly_ascending, is_watermark_ascending, is_watermark_bounded, allowed_lateness, attach_short_syntax, attach_from_path, replace_table, create_or_replace)
AST_CUSTOM_ATTRIBUTES(DB::ASTDatabaseOrNone, none, database_name)
// AST_CUSTOM_ATTRIBUTES(DB::ASTDeleteQuery)
AST_CUSTOM_ATTRIBUTES(DB::ASTDescribeCacheQuery, cache_name)
AST_CUSTOM_ATTRIBUTES(DB::ASTDictionaryAttributeDeclaration, name, hierarchical, bidirectional, injective, is_object_id)
// AST_CUSTOM_ATTRIBUTES(DB::ASTDictionary)
AST_CUSTOM_ATTRIBUTES(DB::ASTDropFunctionQuery, function_name, if_exists)
AST_CUSTOM_ATTRIBUTES(DB::ASTDropIndexQuery, if_exists)
AST_CUSTOM_ATTRIBUTES(DB::ASTDropNamedCollectionQuery, collection_name, if_exists)
AST_CUSTOM_ATTRIBUTES(DB::ASTDropQuery, if_exists, no_ddl_lock, has_all_tables, is_dictionary, is_view, sync, permanently)
// AST_CUSTOM_ATTRIBUTES(DB::ASTExplainQuery, kind)
AST_CUSTOM_ATTRIBUTES(DB::ASTExpressionList, separator)
// AST_CUSTOM_ATTRIBUTES(DB::ASTExternalDDLQuery)
AST_CUSTOM_ATTRIBUTES(DB::ASTForeignKeyDeclaration, name)
AST_CUSTOM_ATTRIBUTES(DB::ASTFunction, name, is_window_function, compute_after_window_functions, is_lambda_function, prefer_subquery_to_function_formatting, window_name, no_empty_args)
AST_CUSTOM_ATTRIBUTES(DB::ASTFunctionWithKeyValueArguments, name, has_brackets)
// AST_CUSTOM_ATTRIBUTES(DB::ASTHelpers)
// AST_CUSTOM_ATTRIBUTES(DB::ASTIdentifier_fwd)
AST_CUSTOM_ATTRIBUTES(DB::ASTIdentifier, full_name, name_parts)
AST_CUSTOM_ATTRIBUTES(DB::ASTIndexDeclaration, name, granularity, part_of_create_index_query)
AST_CUSTOM_ATTRIBUTES(DB::ASTInsertQuery, format, async_insert_flush)
AST_CUSTOM_ATTRIBUTES(DB::ASTInterpolateElement, column)
AST_CUSTOM_ATTRIBUTES(DB::ASTKillQueryQuery, sync, test)
AST_CUSTOM_ATTRIBUTES(DB::ASTLiteral, value, unique_column_name, use_legacy_column_name_of_tuple)
AST_CUSTOM_ATTRIBUTES(DB::ASTNameTypePair, name)
AST_CUSTOM_ATTRIBUTES(DB::ASTOptimizeQuery, final, deduplicate, cleanup)
AST_CUSTOM_ATTRIBUTES(DB::ASTOrderByElement, direction, nulls_direction, nulls_direction_was_explicitly_specified, with_fill)
AST_CUSTOM_ATTRIBUTES(DB::ASTPartition, fields_count, all)
AST_CUSTOM_ATTRIBUTES(DB::ASTProjectionDeclaration, name)
// AST_CUSTOM_ATTRIBUTES(DB::ASTProjectionSelectQuery)
// AST_CUSTOM_ATTRIBUTES(DB::ASTQualifiedAsterisk)
AST_CUSTOM_ATTRIBUTES(DB::ASTQueryParameter, name, type)
AST_CUSTOM_ATTRIBUTES(DB::ASTQueryWithOnCluster, cluster)
AST_CUSTOM_ATTRIBUTES(DB::ASTQueryWithOutput, is_into_outfile_with_stdout, is_outfile_append, is_outfile_truncate)
AST_CUSTOM_ATTRIBUTES(DB::ASTQueryWithTableAndOutput, temporary)
// AST_CUSTOM_ATTRIBUTES(DB::ASTRefreshStrategy)
AST_CUSTOM_ATTRIBUTES(DB::ASTRenameQuery, exchange, database, dictionary, rename_if_cannot_exchange)
// AST_CUSTOM_ATTRIBUTES(DB::ASTSampleRatio)
// AST_CUSTOM_ATTRIBUTES(DB::ASTSelectIntersectExceptQuery)
AST_CUSTOM_ATTRIBUTES(DB::ASTSelectQuery, recursive_with, distinct, group_by_all, group_by_with_totals, group_by_with_rollup, group_by_with_cube, group_by_with_constant_keys, group_by_with_grouping_sets, order_by_all, limit_with_ties)
AST_CUSTOM_ATTRIBUTES(DB::ASTSelectWithUnionQuery, union_mode, list_of_modes, is_normalized, set_of_modes)
AST_CUSTOM_ATTRIBUTES(DB::ASTSetQuery, is_standalone, print_in_format, changes, default_settings, query_parameters)
AST_CUSTOM_ATTRIBUTES(DB::ASTShowColumnsQuery, extended, full, not_like, case_insensitive_like, database, table, like)
// AST_CUSTOM_ATTRIBUTES(DB::ASTShowEngineQuery)
AST_CUSTOM_ATTRIBUTES(DB::ASTShowFunctionsQuery, case_insensitive_like, like)
AST_CUSTOM_ATTRIBUTES(DB::ASTShowIndexesQuery, extended, database, table)
// AST_CUSTOM_ATTRIBUTES(DB::ASTShowProcesslistQuery)
// AST_CUSTOM_ATTRIBUTES(DB::ASTShowSettingQuery)
AST_CUSTOM_ATTRIBUTES(DB::ASTShowTablesQuery, databases, clusters, cluster, dictionaries, m_settings, merges, changed, temporary, caches, full, cluster_str, like, not_like, case_insensitive_like)
AST_CUSTOM_ATTRIBUTES(DB::ASTSQLSecurity, is_definer_current_user)
// AST_CUSTOM_ATTRIBUTES(DB::ASTStatisticsDeclaration)
AST_CUSTOM_ATTRIBUTES(DB::ASTSubquery, cte_name)
AST_CUSTOM_ATTRIBUTES(DB::ASTSystemQuery, target_model, target_function, replica, shard, replica_zk_path, is_drop_whole_replica, storage_policy, volume, disk, seconds, filesystem_cache_name, key_to_drop, offset_to_drop, backup_name, schema_cache_storage, schema_cache_format, fail_point_name, src_replicas, fake_time_for_view)
AST_CUSTOM_ATTRIBUTES(DB::ASTTableOverride, table_name, is_standalone)
// AST_CUSTOM_ATTRIBUTES(DB::ASTTablesInSelectQuery)
// AST_CUSTOM_ATTRIBUTES(DB::ASTTimeInterval)
AST_CUSTOM_ATTRIBUTES(DB::ASTTransactionControl, snapshot)
AST_CUSTOM_ATTRIBUTES(DB::ASTTTLElement, destination_name, if_exists)
// AST_CUSTOM_ATTRIBUTES(DB::ASTUndropQuery)
// AST_CUSTOM_ATTRIBUTES(DB::ASTUseQuery)
AST_CUSTOM_ATTRIBUTES(DB::ASTWatchQuery, is_watch_events)
AST_CUSTOM_ATTRIBUTES(DB::ASTWindowDefinition, parent_window_name, frame_is_default, frame_begin_preceding, frame_end_preceding)
AST_CUSTOM_ATTRIBUTES(DB::ASTWithAlias, alias, prefer_alias_to_column_name)
AST_CUSTOM_ATTRIBUTES(DB::ASTWithElement, name)

std::vector<Attribute> GetCustomAttributes(const DB::IAST* ast);
