#include <Uefi.h>
#include  <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include  <AMI/ProtocolLib.h>


EFI_STATUS GetKey(EFI_INPUT_KEY *Key)
{
    UINTN Index;
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);
    return gST->ConIn->ReadKeyStroke(gST->ConIn,Key);
}
EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;

    Status = GetKey(&Key);
    Print(L"Key.ScanCode = 0x%04x, Key.UnicodeChar = 0x%04x\n",Key.ScanCode, Key.UnicodeChar);
    return EFI_SUCCESS;
}


