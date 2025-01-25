enum result_type
{
    result_bytes,
    result_cpu_ticks,
    result_page_faults,
    result_count,
};

struct test_results
{
    u64 min[result_count];
    u64 max[result_count];
    u64 avg[result_count];
};

enum state
{
    off,
    on,
    done,
    count
};

struct repeat
{
    state state;
    u64 accum[result_count];
    u64 total[result_count];
    u64 reset_runtime_after_min_updated;
    u64 maxRunCount;
    u64 durationInSec;
    u64 balance;
    u64 run_count;
    u64 expected_bytes;
    u64 start;
    u32 print_change;
    test_results result;
};