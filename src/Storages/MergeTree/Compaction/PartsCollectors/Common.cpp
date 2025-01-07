#include <Storages/MergeTree/Compaction/PartsCollectors/Common.h>

#include <Disks/IStoragePolicy.h>

namespace DB
{

PartsRanges constructPartsRanges(
    std::vector<MergeTreeDataPartsVector> && ranges, const StorageMetadataPtr & metadata_snapshot,
    const StoragePolicyPtr & storage_policy, const time_t & current_time)
{
    const bool has_volumes_with_disabled_merges = storage_policy->hasAnyVolumeWithDisabledMerges();

    PartsRanges properties_ranges;
    properties_ranges.reserve(ranges.size());

    for (const auto & range : ranges)
    {
        PartsRange properties_range;
        properties_ranges.reserve(range.size());

        for (const auto & part : range)
            properties_range.push_back(buildPartProperties(part, metadata_snapshot, storage_policy, current_time, has_volumes_with_disabled_merges));

        properties_ranges.push_back(std::move(properties_range));
    }

    return properties_ranges;
}

MergeTreeDataPartsVector filterByPartitions(
    MergeTreeDataPartsVector && parts, const std::optional<PartitionIdsHint> & partitions_to_keep)
{
    if (!partitions_to_keep)
        return parts;

    std::erase_if(parts, [&partitions_to_keep](const auto & part) { return !partitions_to_keep->contains(part->info.partition_id); });

    return parts;
}

}
