#pragma once
#include <intrin.h>
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "bcrypt.lib")

using namespace std;

struct os_platform
{
    b32 Initialized;
    HANDLE ProcessHandle;
    u64 cpu_freq;
};
static os_platform GlobalMetrics;

static u64 GetOSTimerFreq(void)
{
    LARGE_INTEGER Freq;
    QueryPerformanceFrequency(&Freq);
    return Freq.QuadPart;
}

static u64 ReadOSTimer(void)
{
    LARGE_INTEGER Value;
    QueryPerformanceCounter(&Value);
    return Value.QuadPart;
}

inline u64 ReadCPUTimer(void)
{
    // NOTE(casey): If you were on ARM, you would need to replace __rdtsc
    // with one of their performance counter read instructions, depending
    // on which ones are available on your platform.

    return __rdtsc();
}

static u64 ReadOSPageFaultCount(void)
{
    PROCESS_MEMORY_COUNTERS_EX MemoryCounters = {};
    MemoryCounters.cb = sizeof(MemoryCounters);
    GetProcessMemoryInfo(GlobalMetrics.ProcessHandle, (PROCESS_MEMORY_COUNTERS *)&MemoryCounters, sizeof(MemoryCounters));
    
    u64 Result = MemoryCounters.PageFaultCount;
    return Result;
}

// static void *OSAllocate(size_t ByteCount)
// {
//     void *Result = VirtualAlloc(0, ByteCount, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
//     return Result;
// }

// static void OSFree(size_t ByteCount, void *BaseAddress)
// {
//     (void)ByteCount; // NOTE(casey): On Windows, you don't pass the size when deallocating
//     VirtualFree(BaseAddress, 0, MEM_RELEASE);
// }

static u64 EstimateCPUFreq(u32 ms)
{
    u64 osFreq = GetOSTimerFreq();
    u64 waitTime = osFreq * ms / 1000;

    u64 osStart, osEnd, osElapsed = 0;
    u64 CpuStart = ReadCPUTimer();

    osStart = ReadOSTimer();
    while (osElapsed < waitTime)
    {
        osEnd = ReadOSTimer();
        osElapsed = osEnd - osStart;
    }

    u64 CpuEnd = ReadCPUTimer();
    u64 CpuElapsed = CpuEnd - CpuStart;

    return CpuElapsed * osFreq / osElapsed;
}

static void InitializeOSPlatform(void)
{
    if(!GlobalMetrics.Initialized)
    {
        GlobalMetrics.Initialized = true;
        GlobalMetrics.ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
        GlobalMetrics.cpu_freq = EstimateCPUFreq(100);
    }
}

static u64 GetMaxOSRandomCount(void)
{
    return 0xffffffff;
}

static b32 ReadOSRandomBytes(u64 Count, void *Dest)
{
    b32 Result = false;
    if(Count < GetMaxOSRandomCount())
    {
        Result = (BCryptGenRandom(0, (BYTE *)Dest, (u32)Count, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0);
    }
    
    return Result;
}

inline void FillWithRandomBytes(buffer Dest)
{
    u64 MaxRandCount = GetMaxOSRandomCount();
    u64 AtOffset = 0;
    while(AtOffset < Dest.count)
    {
        u64 ReadCount = Dest.count - AtOffset;
        if(ReadCount > MaxRandCount)
        {
            ReadCount = MaxRandCount;
        }
        
        ReadOSRandomBytes(ReadCount, Dest.data + AtOffset);
        AtOffset += ReadCount;
    }
}