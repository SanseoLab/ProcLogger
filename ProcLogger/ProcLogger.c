#include "ProcLogger.h"


#pragma alloc_text(INIT, DriverEntry)


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	NTSTATUS Status = STATUS_SUCCESS;

	DriverObject->DriverUnload = DriverUnload;

	DbgPrint("[ ProcLogger ] Driver Loaded\n");

	// installing Process Logger
	InstallProcLogger();

	// installing Register Monitor
	InstallRegMonitor(DriverObject);

	// installing Self Protection
	InstallSelfProtect();

	return Status;
}


VOID DriverUnload(PDRIVER_OBJECT DriverObject) {

	UNREFERENCED_PARAMETER(DriverObject);

	// Uninstalling Process Logger
	UnInstallProcLogger();

	// Uninstalling Register Monitor
	UnInstallRegMonitor();

	// Uninstalling Self Protection
	UnInstallSelfProtect();

	DbgPrint("[ ProcLogger ] Driver Unloaded\n");
}
