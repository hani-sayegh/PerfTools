#include <array>
#include <cstdio>
#include <assert.h>
#include <stdlib.h>
#include "./common/all_common.h"

extern "C" void mov32x8(u64 Count, u8 *Data, u64 repeat);
#pragma comment(lib, "max_bandwidth")

using func_pointer = void (*)(u64 Count, u8 *Data, u64 mask);

struct func_info
{
    func_pointer fn;
    const char *label;
};

#define TEST_FN(fn) \
    {               \
        fn, #fn     \
    }

void print_bitpattern(u64 x)
{
    u8 buff[1000];
    u16 len = 0;
    while (x)
    {
        if (x & 1)
        {
            arr(buff, len) = 1;
        }
        else
        {
            arr(buff, len) = 0;
        }
        ++len;
        x >>= 1;
    }
    --len;
    for (u16 i = 0; i <= len; ++i)
    {
        printf(buff[len - i] ? "1" : "0");
    }
    printf("\n");
}

void print_byte_info(u64 bytes)
{
    u64 kb = 1024;
    u64 mb = kb * kb;
    u64 gb = kb * mb;
    printf("bytes(%llu) kilo(%llu) mega(%llu) giga(%llu)\n",
           bytes,
           bytes / kb,
           bytes / mb,
           bytes / gb);
}

int main()
{
    InitializeOSPlatform();
    func_info funcs[] = {
        TEST_FN(mov32x8),
    };

    u64 one_gb = 1024 * 1024 * 1024;

    buffer b = create_buffer(one_gb);
    for (u64 i = 0; i < b.count; ++i)
    {
        b.data[i] = (u8)i;
    }

    repeat testers[ArrayCount(funcs)][1024] = {};
    u64 bytes_used [1024] = {};

    for (u64 block_size = 1024, test_idx = 0; block_size <= one_gb; block_size *= 2, ++test_idx)
    {
        print_byte_info(block_size);
        bytes_used[test_idx] = block_size;
        for (u16 func_idx = 0; func_idx < ArrayCount(funcs); ++func_idx)
        {
            repeat *tester = testers[func_idx] + test_idx;
            auto func = funcs[func_idx];
            u64 mem_processed = one_gb;
            u64 times = mem_processed / block_size;
            initialize(tester, mem_processed);
            printf("Processing: %s\n", func.label);
            while (continue_test(tester))
            {
                begin_test(tester);
                func.fn(block_size, b.data, times);
                end_test(tester, mem_processed);
            }
            print_min(tester);
            print_max(tester);
            print_info(tester);
            printf("\n\n\n");
        }
    }
    for (u64 i = 0; i < ArrayCount(funcs); ++i)
    {
        printf("Memory(KB), Bandwidth (GB/S)");
        printf("\n");
        for (u64 j = 0; j < 1024; ++j)
        {
            repeat *x = testers[i] + j;
            if (x->run_count)
            {
                printf("%llu, %f ", bytes_used[j], calc_bandwidth(x->result.min));
                printf("\n");
            }
        }
    }
    return 0;
}