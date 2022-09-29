#ifndef WinSysApiUtils_H
#define WinSysApiUtils_H

#include <windows.h>
#include <psapi.h>

#include <QApplication>

struct MemoryUsageInfo {
    bool valid = false;
    quint64 workingSetSize = 0;
    quint64 peakWorkingSetSize = 0;
    quint64 quotaPeakPagedPoolUsage = 0;
    quint64 quotaPagedPoolUsage = 0;
    quint64 quotaPeakNonPagedPoolUsage = 0;
    quint64 quotaNonPagedPoolUsage = 0;
    quint64 pagefileUsage = 0;
    quint64 peakPagefileUsage = 0;
    quint32 cb = 0;
};

class WinSysApiUtils
{
public:
    // MEMORY USAGE INFO
    static MemoryUsageInfo getMemoryUsageInfo();
    static quint64 getPrivateMemoryUsage();
};

#endif // WinSysApiUtils_H
