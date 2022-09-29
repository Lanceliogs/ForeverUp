#include "winsysapiutils.h"

MemoryUsageInfo WinSysApiUtils::getMemoryUsageInfo()
{
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;
    DWORD pid = qApp->applicationPid();
    MemoryUsageInfo mui;

    // Print information about the memory usage of the process.
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (NULL == hProcess)
        return mui;

    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        mui.valid = true;
        mui.workingSetSize = pmc.WorkingSetSize;
        mui.peakWorkingSetSize = pmc.PeakWorkingSetSize;
        mui.quotaPeakPagedPoolUsage = pmc.QuotaPeakPagedPoolUsage;
        mui.quotaPagedPoolUsage = pmc.QuotaPagedPoolUsage;
        mui.quotaPeakNonPagedPoolUsage = pmc.QuotaPeakNonPagedPoolUsage;
        mui.quotaNonPagedPoolUsage = pmc.QuotaNonPagedPoolUsage;
        mui.pagefileUsage = pmc.PagefileUsage;
        mui.peakPagefileUsage = pmc.PeakPagefileUsage;
        mui.cb = pmc.cb;
    }
    CloseHandle(hProcess);

    return mui;
}

quint64 WinSysApiUtils::getPrivateMemoryUsage()
{
    DWORD usage = 0;
    unsigned char *p = NULL;
    MEMORY_BASIC_INFORMATION info;
    while (VirtualQuery(p, &info, sizeof(info)) == sizeof(info)) {
        if ((info.State == MEM_COMMIT) && (info.Type == MEM_PRIVATE))
            usage += info.RegionSize;
        p += info.RegionSize;
    }
    return usage;
}
