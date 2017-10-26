#include <ntifs.h>
#include "utils.h"


#define PROCESS_TERMINATE      (0x0001)
#define PROCESS_VM_READ        (0x0010)
#define PROCESS_VM_WRITE       (0x0020)
#define PROCESS_VM_OPERATION   (0x0008) 


OB_PREOP_CALLBACK_STATUS	ObjectPreCallBack(IN PVOID RegistrationContext, IN POB_PRE_OPERATION_INFORMATION OperationInformation);
VOID				ObjectPostCallBack(IN  PVOID RegistrationContext, IN  POB_POST_OPERATION_INFORMATION OperationInformation);
NTSTATUS			InstallSelfProtect();
VOID				UnInstallSelfProtect();
