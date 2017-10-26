#include <ntifs.h>

extern UCHAR *PsGetProcessImageFileName(IN PEPROCESS Process);

LPSTR				GetProcessNameFromPid(HANDLE pid);
