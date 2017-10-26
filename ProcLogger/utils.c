#include "utils.h"


char * GetProcessNameFromPid(HANDLE pid)
{
	PEPROCESS Process;
	if (PsLookupProcessByProcessId(pid, &Process) == STATUS_INVALID_PARAMETER)
	{
		return "??";
	}
	return (CHAR*)PsGetProcessImageFileName(Process);
}
