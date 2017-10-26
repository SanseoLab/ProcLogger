#include <ntifs.h>
#include <stdio.h>
#include "utils.h"


extern UCHAR	*PsGetProcessImageFileName(IN PEPROCESS Process);


VOID			ProcessNotifyCallbackEx(
	PEPROCESS  Process,
	HANDLE  ProcessId,
	PPS_CREATE_NOTIFY_INFO  CreateInfo);
NTSTATUS		InstallProcLogger();
VOID			UnInstallProcLogger();

