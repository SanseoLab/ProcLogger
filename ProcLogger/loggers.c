#include "loggers.h"


#define MY_TAG 'Tag1'


VOID ProcessNotifyCallbackEx(
	PEPROCESS  Process,
	HANDLE  ProcessId,
	PPS_CREATE_NOTIFY_INFO  CreateInfo)
{


	///* Declation of Variables *///

	/* Create or Exit */
	BOOLEAN boolCreateExit = FALSE;
	PUNICODE_STRING CreateOrExit = NULL;

	UNICODE_STRING CreateAndExit[] = {
		RTL_CONSTANT_STRING(L"CREATE"),
		RTL_CONSTANT_STRING(L"EXIT")
	};

	/* Get Time */
	LARGE_INTEGER CurrentGMTTime;
	LARGE_INTEGER CurrentLocalTime;
	TIME_FIELDS CurrentLocalTimeField;

	/* Get Integrity Level */
	ULONG dwIntegrityLevel = 0;
	PACCESS_TOKEN pAccessToken;
	PUNICODE_STRING ilevel = NULL;

	UNICODE_STRING IntegrityLevel[] = {
		RTL_CONSTANT_STRING(L"LOW"),
		RTL_CONSTANT_STRING(L"MEDIUM"),
		RTL_CONSTANT_STRING(L"HIGH"),
		RTL_CONSTANT_STRING(L"SYSTEM")
	};

	/* To Log */
	UNICODE_STRING			LogFileName;
	OBJECT_ATTRIBUTES		ObjAttr;
	NTSTATUS				Status = STATUS_SUCCESS;
	HANDLE					hLogFile;
	IO_STATUS_BLOCK			IOStatus;
	PCHAR					LogEntryText;
	ULONG					LogEntryTextLen;
	LARGE_INTEGER			liFileOffset;




	///* Routines *///

	/* Get Time */
	KeQuerySystemTime(&CurrentGMTTime);
	ExSystemTimeToLocalTime(&CurrentGMTTime, &CurrentLocalTime);
	RtlTimeToTimeFields(&CurrentLocalTime, &CurrentLocalTimeField);


	/* Get Integrity Level */
	pAccessToken = PsReferencePrimaryToken(Process);
	SeQueryInformationToken(pAccessToken, TokenIntegrityLevel, (PVOID*)&dwIntegrityLevel);

	if (dwIntegrityLevel == SECURITY_MANDATORY_LOW_RID) {
		ilevel = &IntegrityLevel[0];		// Low Integrity
	}
	else if (dwIntegrityLevel >= SECURITY_MANDATORY_MEDIUM_RID && dwIntegrityLevel < SECURITY_MANDATORY_HIGH_RID) {
		ilevel = &IntegrityLevel[1];		// Medium Integrity
	}
	else if (dwIntegrityLevel >= SECURITY_MANDATORY_HIGH_RID) {
		ilevel = &IntegrityLevel[2];		// High Integrity
	}
	else if (dwIntegrityLevel >= SECURITY_MANDATORY_SYSTEM_RID) {
		ilevel = &IntegrityLevel[3];		// System Integrity
	}


	/* Create or Exit */
	if (CreateInfo != NULL) {
		boolCreateExit = TRUE;
	}


	/* To Log */

	RtlInitUnicodeString(&LogFileName, L"\\??\\C:\\ProcLogger.log");

	InitializeObjectAttributes(
		&ObjAttr,
		&LogFileName,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		NULL, NULL);

	if (!NT_SUCCESS(Status = ZwCreateFile(
		&hLogFile,
		GENERIC_WRITE,
		&ObjAttr,
		&IOStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE,
		NULL, 0)))
	{
		DbgPrint("[ ProcLogger ] [ Log ] [ Error ] zwWriteFile Error (0x%p)\n", (void*)Status);
		return;
	}

	// Allocating memory in kernel
	LogEntryText = ExAllocatePoolWithTag(PagedPool, 1024, MY_TAG);

	// For Creation
	if (boolCreateExit) {
		CreateOrExit = &CreateAndExit[0];

		// See DebugView.exe
		DbgPrint("[ ProcLogger ] [ Log ] [ %d:%d:%d:%d:%d:%d ] [ %wZ ] [ %wZ ]  [ %s (%Ix) ] [ %s (%Ix) ] [ %s (%Ix) ] [ %wZ ] \n",
			CurrentLocalTimeField.Year,
			CurrentLocalTimeField.Month,
			CurrentLocalTimeField.Day,
			CurrentLocalTimeField.Hour,
			CurrentLocalTimeField.Minute,
			CurrentLocalTimeField.Second,
			CreateOrExit,
			CreateInfo->ImageFileName,
			GetProcessNameFromPid(ProcessId),
			(ULONG_PTR)ProcessId,
			GetProcessNameFromPid(CreateInfo->ParentProcessId),
			(ULONG_PTR)CreateInfo->ParentProcessId,
			GetProcessNameFromPid(CreateInfo->CreatingThreadId.UniqueProcess),
			(ULONG_PTR)CreateInfo->CreatingThreadId.UniqueProcess,
			ilevel
		);

		// Set logs
		sprintf(
			LogEntryText,
			"[ %d:%d:%d:%d:%d:%d ] [ %wZ ] [ %wZ ]  [ %s (%Ix) ] [ %s (%Ix) ] [ %s (%Ix) ] [ %wZ ] \r\n",
			CurrentLocalTimeField.Year,
			CurrentLocalTimeField.Month,
			CurrentLocalTimeField.Day,
			CurrentLocalTimeField.Hour,
			CurrentLocalTimeField.Minute,
			CurrentLocalTimeField.Second,
			CreateOrExit,
			CreateInfo->ImageFileName,
			GetProcessNameFromPid(ProcessId),
			(ULONG_PTR)ProcessId,
			GetProcessNameFromPid(CreateInfo->ParentProcessId),
			(ULONG_PTR)CreateInfo->ParentProcessId,
			GetProcessNameFromPid(CreateInfo->CreatingThreadId.UniqueProcess),
			(ULONG_PTR)CreateInfo->CreatingThreadId.UniqueProcess,
			ilevel
		);

		// For Exit
	}
	else {
		CreateOrExit = &CreateAndExit[1];

		// See DebugView.exe
		DbgPrint("[ ProcLogger ] [ Log ] [ %d:%d:%d:%d:%d:%d ] [ %wZ ] [ N/A ] [ %s (%Ix) ] [ N/A ] [ N/A ] [ %wZ ] \n",
			CurrentLocalTimeField.Year,
			CurrentLocalTimeField.Month,
			CurrentLocalTimeField.Day,
			CurrentLocalTimeField.Hour,
			CurrentLocalTimeField.Minute,
			CurrentLocalTimeField.Second,
			CreateOrExit,
			GetProcessNameFromPid(ProcessId),
			(ULONG_PTR)ProcessId,
			ilevel
		);

		// Set logs
		sprintf(
			LogEntryText,
			"[ %d:%d:%d:%d:%d:%d ] [ %wZ ] [ N/A ] [ %s (%Ix) ] [ N/A ] [ N/A ] [ %wZ ] \r\n",
			CurrentLocalTimeField.Year,
			CurrentLocalTimeField.Month,
			CurrentLocalTimeField.Day,
			CurrentLocalTimeField.Hour,
			CurrentLocalTimeField.Minute,
			CurrentLocalTimeField.Second,
			CreateOrExit,
			GetProcessNameFromPid(ProcessId),
			(ULONG_PTR)ProcessId,
			ilevel
		);
	}

	LogEntryTextLen = strlen(LogEntryText);

	liFileOffset.HighPart = -1;
	liFileOffset.LowPart = FILE_WRITE_TO_END_OF_FILE;

	// Writing logs
	Status = ZwWriteFile(
		hLogFile,
		NULL, NULL, NULL,
		&IOStatus,
		LogEntryText,
		LogEntryTextLen,
		&liFileOffset,
		NULL);

	if (!NT_SUCCESS(Status)) {
		DbgPrint("[ ProcLogger ] [ Log ] [ Error ] zwWriteFile Fail (0x%x)", Status);
		ZwClose(hLogFile);
		return;
	}


	/* End */

	// Free memory in kernel
	ExFreePoolWithTag(LogEntryText, MY_TAG);

	ZwClose(hLogFile);

	return;
}




NTSTATUS InstallProcLogger() {
	NTSTATUS Status = STATUS_SUCCESS;

	if (!NT_SUCCESS(Status = PsSetCreateProcessNotifyRoutineEx(ProcessNotifyCallbackEx, FALSE)))
	{
		DbgPrint("[ ProcLogger ] [ Log ] [ ERROR ] Resistering PsSetCreateProcessNotifyRoutineEx Failed : (%x)\n", Status);
		return Status;
	}
	else {
		DbgPrint("[ ProcLogger ] [ Log ] [ SUCCESS ] Resistering PsSetCreateProcessNotifyRoutineEx Success\n");
	}

	return STATUS_SUCCESS;

}




VOID UnInstallProcLogger() {

	NTSTATUS Status = STATUS_SUCCESS;

	if (!NT_SUCCESS(Status = PsSetCreateProcessNotifyRoutineEx(ProcessNotifyCallbackEx, TRUE))) {
		DbgPrint("[ ProcLogger ] [ Log ] [ ERROR ] Unresistering PsSetCreateProcessNotifyRoutineEx Failed : (%x)\n", Status);
	}
	else {
		DbgPrint("[ ProcLogger ] [ Log ] [ SUCCESS ] Unresistering PsSetCreateProcessNotifyRoutineEx Success\n");
	}

}
