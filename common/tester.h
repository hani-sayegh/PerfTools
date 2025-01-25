#include "cpu.h"
#include "tester_types.h"

[[noreturn]] void Error(char const *Message)
{
    fprintf(stderr, "ERROR: %s\n", Message);
    exit(0);
}

f64 calc_bandwidth(u64 data[result_count])
{
    u64 ticks = data[result_cpu_ticks];
    f64 timeSec = DIV_F64(ticks, GlobalMetrics.cpu_freq);
    return DIV_F64(data[result_bytes], (timeSec * GB));
}

void print(u64 data[result_count], const char *label)
{
    u64 ticks = data[result_cpu_ticks];
    f64 timeSec = DIV_F64(ticks, GlobalMetrics.cpu_freq);
    f64 Gigabyte = (1024.f * 1024.0f * 1024.0f);
    f64 bandGB = DIV_F64(data[result_bytes], (timeSec * Gigabyte));
    f64 band_byte_per_sec = (f64)data[result_bytes] / (timeSec);
    u64 page_faults = data[result_page_faults];

    printf("%s", label);

    printf("BandwidthGB( %f ) Raw( %f )", bandGB, band_byte_per_sec);
    if (page_faults != 0)
    {
        f64 bytes_per_pagefault = ((f64)data[result_bytes]) / (f64)page_faults;
        printf("Pagefaults(%llu) BytesPerPagefault(%f)", page_faults, bytes_per_pagefault);
    }
    printf("\n");
}

void print_min(repeat *tester)
{
    print(tester->result.min, "BEST: ");
}

void print_max(repeat *tester)
{
    print(tester->result.max, "WORST: ");
}

bool continue_test(repeat *tester)
{
    u8 c1 = tester->run_count < tester->maxRunCount;
    return c1 && tester->state != done;
}

void begin_test(repeat *tester)
{
    ++tester->run_count;
    ++tester->balance;
    if (tester->balance != 1)
    {
        Error("start too many");
    }

    tester->accum[result_page_faults] -= ReadOSPageFaultCount();
    tester->accum[result_cpu_ticks] -= ReadCPUTimer();
}

void print_info(repeat *tester)
{
    printf("Run Count: %llu\n", tester->run_count);
    print_min(tester);
    print_max(tester);
}

void end_test(repeat *tester, u64 bytes)
{
    u64 end = ReadCPUTimer();
    tester->total[result_cpu_ticks] += tester->accum[result_cpu_ticks] += end;
    --tester->balance;
    if (tester->balance != 0)
    {
        Error("invalid end balance");
    }

    tester->total[result_page_faults] += tester->accum[result_page_faults] += ReadOSPageFaultCount();
    tester->total[result_bytes] += tester->accum[result_bytes] += bytes;
    if (tester->accum[result_bytes] != tester->expected_bytes)
    {
        Error("Invalid byte count");
    }

    if (tester->accum[result_cpu_ticks] < tester->result.min[result_cpu_ticks])
    {
        if (tester->reset_runtime_after_min_updated)
        {
            tester->start = end;
        }

        for (u32 i = 0; i < result_count; ++i)
        {
            tester->result.min[i] = tester->accum[i];
        }

        if (tester->print_change)
        {
            print_min(tester);
        }
    }

    if (tester->accum[result_cpu_ticks] > tester->result.max[result_cpu_ticks])
    {
        for (u32 i = 0; i < result_count; ++i)
        {
            tester->result.max[i] = tester->accum[i];
        }
        if (tester->print_change)
        {
            print_max(tester);
        }
    }
    u64 diff = end - tester->start;

    for (u32 i = 0; i < result_count; ++i)
    {
        tester->accum[i] = 0;
    }
    if (diff > GlobalMetrics.cpu_freq * tester->durationInSec)
    {
        tester->state = done;
    }
}

#define Ignore (u64) - 1
void initialize(repeat *r, u64 count)
{
    for (u32 i = 0; i < result_count; ++i)
    {
        r->accum[i] = 0;
        r->total[i] = 0;
        if (r->state == off)
        {
            r->result.min[i] = (u64)-1;
            r->result.max[i] = 0;
        }
    }
    r->reset_runtime_after_min_updated = 1;
    r->print_change = 0;
    r->maxRunCount = Ignore;
    r->durationInSec = 10;
    r->balance = 0;
    r->run_count = 0;
    r->start = ReadCPUTimer();
    r->expected_bytes = count;
    if (r->state == off)
    {
        r->state = on;
    }
}