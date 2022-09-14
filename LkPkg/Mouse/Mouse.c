#include <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimplePointer.h>
#include  <AMI/ProtocolLib.h>
/***
    Print a welcoming message.

    Establishes the main structure of the application.

    @retval  0         The application exited normally.
    @retval  Other     An error occurred.
***/

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    UINTN           HandleCount;
    EFI_HANDLE      *HandleBuffer;
    UINTN           index;
    EFI_STATUS      Status;
    EFI_SIMPLE_POINTER_PROTOCOL    *SimpleMouse;

    HandleCount = CountProtocolInstances(&gEfiSimplePointerProtocolGuid, &HandleBuffer);
    Print(L"HandleCount = %d\n", HandleCount);
    for (index = 0; index < HandleCount; index++) {
        Status = gBS->HandleProtocol(HandleBuffer[index], &gEfiSimplePointerProtocolGuid,(VOID**)&SimpleMouse);
        Print(L"HandleProtocol = %r\n", Status);
        Print(L"SimpleMouse->Mode->LeftButton = 0x%x, SimpleMouse->Mode->RightButton = 0x%x\n", SimpleMouse->Mode->LeftButton, SimpleMouse->Mode->RightButton);
        Print(L"SimpleMouse->Mode->ResolutionX = %d, SimpleMouse->Mode->ResolutionY = %d SimpleMouse->Mode->ResolutionZ = %d\n", \
        SimpleMouse->Mode->ResolutionX, SimpleMouse->Mode->ResolutionY, SimpleMouse->Mode->ResolutionZ);
    } 
    return EFI_SUCCESS;
}