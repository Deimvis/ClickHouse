#include "PageCache.h"

#include <unistd.h>
#include <sys/mman.h>
#include <Common/Allocator.h>
#include <Common/logger_useful.h>
#include <Common/MemoryTrackerBlockerInThread.h>
#include <Common/formatReadable.h>
#include <Common/ProfileEvents.h>
#include <Common/SipHash.h>
#include <base/hex.h>
#include <base/errnoToString.h>
#include <base/getPageSize.h>
#include <IO/ReadBufferFromFile.h>
#include <IO/ReadHelpers.h>

namespace ProfileEvents
{
    extern const Event PageCacheHits;
    extern const Event PageCacheMisses;
    extern const Event PageCacheWeightLost;
    extern const Event PageCacheResized;
}

namespace DB
{

namespace ErrorCodes
{
}

template class CacheBase<UInt128, PageCacheCell, UInt128TrivialHash, PageCacheWeightFunction>;

UInt128 PageCacheKey::hash() const
{
    SipHash hash(offset);
    hash.update(size);
    hash.update(path.data(), path.size());
    if (!file_version.empty())
    {
        hash.update("\0", 1);
        hash.update(file_version.data(), file_version.size());
    }
    return hash.get128();
}

std::string PageCacheKey::toString() const
{
    return fmt::format("{}:{}:{}{}{}", path, offset, size, file_version.empty() ? "" : ":", file_version);
}

PageCache::PageCache(size_t default_block_size_, std::chrono::milliseconds history_window_, const String & cache_policy, double size_ratio, size_t min_size_in_bytes_, size_t max_size_in_bytes_, double free_memory_ratio_)
    : Base(cache_policy, min_size_in_bytes_, 0, size_ratio)
    , default_block_size(default_block_size_)
    , min_size_in_bytes(min_size_in_bytes_)
    , max_size_in_bytes(max_size_in_bytes_)
    , free_memory_ratio(free_memory_ratio_)
    , history_window(history_window_)
{
}

PageCache::MappedPtr PageCache::getOrSet(const PageCacheKey & key, bool detached_if_missing, bool inject_eviction, std::function<void(const MappedPtr &)> load)
{
    /// Prevent MemoryTracker from calling autoResize() while we may be holding the mutex.
    MemoryTrackerBlockerInThread blocker(VariableContext::Global);

    Key key_hash = key.hash();

    if (inject_eviction && thread_local_rng() % 10 == 0)
        Base::remove(key_hash);

    MappedPtr result;
    bool miss = false;
    if (detached_if_missing)
    {
        result = Base::get(key_hash);
        if (!result)
        {
            blocker.reset(); // allow throwing out-of-memory exception when allocating or loading cell

            miss = true;
            result = std::make_shared<PageCacheCell>(key, /*temporary*/ true);
            load(result);
        }
    }
    else
    {
        std::tie(result, miss) = Base::getOrSet(key_hash, [&]() -> MappedPtr
        {
            /// At this point CacheBase is not holding the mutex, so it's ok to let MemoryTracker
            /// call autoResize().
            blocker.reset();

            MappedPtr cell;
            try
            {
                cell = std::make_shared<PageCacheCell>(key, /*temporary*/ false);
                load(cell);
            }
            catch (...)
            {
                blocker = MemoryTrackerBlockerInThread(VariableContext::Global);
                throw;
            }

            blocker = MemoryTrackerBlockerInThread(VariableContext::Global);
            return cell;
        });
    }
    chassert(result);

    if (miss)
        ProfileEvents::increment(ProfileEvents::PageCacheMisses);
    else
        ProfileEvents::increment(ProfileEvents::PageCacheHits);

    return result;
}

void PageCache::onRemoveOverflowWeightLoss(size_t weight_loss)
{
    ProfileEvents::increment(ProfileEvents::PageCacheWeightLost, weight_loss);
}

void PageCache::autoResize(size_t memory_usage, size_t memory_limit)
{
    /// Avoid recursion when called from MemoryTracker.
    MemoryTrackerBlockerInThread blocker(VariableContext::Global);

    size_t cache_size = sizeInBytes();

    size_t peak;
    {
        std::lock_guard lock(mutex);
        size_t usage_excluding_cache = memory_usage - std::min(cache_size, memory_usage);

        if (history_window.count() <= 0)
        {
            peak = usage_excluding_cache;
        }
        else
        {
            int64_t now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            int64_t bucket = now / history_window.count();
            if (bucket > cur_bucket + 1)
                peak_memory_buckets = {0, 0};
            else if (bucket > cur_bucket)
                peak_memory_buckets = {0, peak_memory_buckets[0]};
            cur_bucket = bucket;
            peak_memory_buckets[0] = std::max(peak_memory_buckets[0], usage_excluding_cache);
            peak = std::max(peak_memory_buckets[0], peak_memory_buckets[1]);
        }
    }

    size_t reduced_limit = size_t(memory_limit * (1. - std::min(free_memory_ratio, 1.)));
    size_t target_size = reduced_limit - std::min(peak, reduced_limit);
    target_size = std::min(max_size_in_bytes, std::max(min_size_in_bytes, target_size));

    setMaxSizeInBytes(target_size);

    ProfileEvents::increment(ProfileEvents::PageCacheResized);
}

void PageCache::clear()
{
    MemoryTrackerBlockerInThread blocker(VariableContext::Global);
    Base::clear();
}

size_t PageCache::sizeInBytes() const
{
    MemoryTrackerBlockerInThread blocker(VariableContext::Global);
    return Base::sizeInBytes();
}

size_t PageCache::count() const
{
    MemoryTrackerBlockerInThread blocker(VariableContext::Global);
    return Base::count();
}

size_t PageCache::maxSizeInBytes() const
{
    MemoryTrackerBlockerInThread blocker(VariableContext::Global);
    return Base::maxSizeInBytes();
}

PageCacheCell::PageCacheCell(PageCacheKey key_, bool temporary) : key(std::move(key_)), m_size(key.size), m_temporary(temporary)
{
    /// Don't attribute page cache memory to the query that happened to allocate it.
    std::optional<MemoryTrackerBlockerInThread> blocker;
    if (!m_temporary)
        blocker.emplace();

    /// Allow throwing out-of-memory exceptions from here.
    m_data = reinterpret_cast<char *>(Allocator<false>().alloc(m_size));
}

PageCacheCell::~PageCacheCell()
{
    std::optional<MemoryTrackerBlockerInThread> blocker;
    if (!m_temporary)
        blocker.emplace();
    Allocator<false>().free(m_data, m_size);
}

}
