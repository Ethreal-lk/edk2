#ifndef OC_MISC_LIB_H
#define OC_MISC_LIB_H

#include <Uefi.h>
UINTN
CountProtocolInstances (
  IN  EFI_GUID  *Protocol,
  OUT EFI_HANDLE  **HandleBuffer);


EFI_STATUS
HandleProtocolFallback (
  IN  EFI_HANDLE  Handle,
  IN  EFI_GUID    *Protocol,
  OUT VOID        **Interface
  );
#endif // OC_MISC_LIB_H