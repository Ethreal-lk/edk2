#include <Uefi.h>
#include  <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include  <AMI/ProtocolLib.h>
#include <Protocol/SimpleTextInEx.h>


EFI_STATUS GetKey(EFI_INPUT_KEY *Key)
{
    UINTN Index;
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);
    return gST->ConIn->ReadKeyStroke(gST->ConIn,Key);
}
EFI_STATUS HotkeyNotifyFunc(
    IN EFI_KEY_DATA  Hotkey)
{
    Print(L"HOT KEY PRESS!\n");
    return EFI_SUCCESS;
}

EFI_STATUS HotKeySample(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *TextInputEx,
    IN EFI_KEY_DATA  Hotkey)
{
    
}
EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS      Status;
    // EFI_INPUT_KEY   Key;
    UINTN           HandleCount;
    EFI_HANDLE      *Handles;
    UINTN           Index;
    EFI_KEY_DATA                      KeyData;
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *TextInputEx;

    HandleCount = CountProtocolInstances(&gEfiSimpleTextInputExProtocolGuid, &Handles);
    Print(L"HandleCount = %d\n", HandleCount);
    for (Index = 0; Index < HandleCount; Index++) {
        Status = HandleProtocolFallback(Handles[Index], &gEfiSimpleTextInputExProtocolGuid, (VOID**)&TextInputEx);
        Print(L"HandleProtocolFallback status = %r  index =%d\n", Status, Index);
        if (EFI_ERROR(Status)) {
            continue;
        }
        break;
    }
    Status = gBS->WaitForEvent(1, &TextInputEx->WaitForKeyEx, &Index);
    if (EFI_ERROR (Status)) {
        Print(L"WaitForKeyEx Status %r\n", Status);
    }
    Status = TextInputEx->ReadKeyStrokeEx(TextInputEx, &KeyData);
    Print(L"KeyData.Key.ScanCode = 0x%04x, KeyData.Key.UnicodeChar = 0x%04x\n",KeyData.Key.ScanCode, KeyData.Key.UnicodeChar);
    Print(L"KeyData.KeyState.KeyShiftState = 0x%x, KeyData.KeyState.KeyToggleState = 0x%x\n",KeyData.KeyState.KeyShiftState, KeyData.KeyState.KeyToggleState);
    // Status = GetKey(&Key);
    // Print(L"Key.ScanCode = 0x%04x, Key.UnicodeChar = 0x%04x\n",Key.ScanCode, Key.UnicodeChar);

    return EFI_SUCCESS;
}


