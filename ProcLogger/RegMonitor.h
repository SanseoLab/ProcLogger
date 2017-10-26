#include <ntifs.h>
#include <stdio.h>
#include "utils.h"


BOOLEAN		CheckProcess(VOID);
BOOLEAN		CheckPolicy(PUNICODE_STRING KeyFullPath);
NTSTATUS	RegPreOpenKey(PVOID RootObject, PUNICODE_STRING CompleteName);
NTSTATUS	RegistryFilterCallback(
				IN PVOID               CallbackContext,
				IN PVOID               Argument1,
				IN PVOID               Argument2
);
NTSTATUS	InstallRegMonitor(IN PDRIVER_OBJECT DriverObject);
VOID		UnInstallRegMonitor();

