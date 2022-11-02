#include <Uefi.h>
#include  <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include  <AMI/ProtocolLib.h>
#include <Protocol/SimpleTextInEx.h>
#include  <AMI/loglib.h>



EFI_STATUS GetKey(EFI_INPUT_KEY *Key)
{
    UINTN Index;
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);
    return gST->ConIn->ReadKeyStroke(gST->ConIn,Key);
}
EFI_STATUS HotkeyNotifyFunc(
    IN EFI_KEY_DATA  *Hotkey)
{   
    EFI_STATUS          Status;
    UINTN               TimerCount = 0;
    EFI_TIME            Time;


    while (1)
    {   
        TimerCount++;
        if (TimerCount % 10 ==0) {
            break;
        }
        Status = gBS->Stall(1000000);
        Print(L"stall status %r timercount = %d\n", Status, TimerCount);
        Status = gRT->GetTime(&Time,NULL);
        Print(L"second is %d\n", Time.Second);
    }
    Print(L"HOT KEY PRESS!\n");
    return EFI_SUCCESS;
}

EFI_STATUS HotKeySample(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *TextInputEx,
    IN EFI_KEY_DATA  *Hotkey)
{
    EFI_STATUS      Status;
    EFI_KEY_DATA    KeyData = {0};
    EFI_HANDLE      HotkeyNotifyHandle;
    UINTN           Index;

    Hotkey->KeyState.KeyShiftState |= EFI_SHIFT_STATE_VALID;   // ���Ƽ���Ч 
    Hotkey->KeyState.KeyToggleState |= EFI_TOGGLE_STATE_VALID | EFI_KEY_STATE_EXPOSED; // �л�״̬����Ч
    Status = TextInputEx->RegisterKeyNotify(
                TextInputEx,
                Hotkey,
                HotkeyNotifyFunc,
                (VOID**)&HotkeyNotifyHandle
                );
    while (KeyData.Key.ScanCode != SCAN_ESC)
    {
        Status = gBS->WaitForEvent(1,&(TextInputEx->WaitForKeyEx), &Index);
        Status = TextInputEx->ReadKeyStrokeEx(TextInputEx,&KeyData);
        Print(L"KeyData.Key.ScanCode = 0x%04x, KeyData.Key.UnicodeChar = 0x%04x\n",KeyData.Key.ScanCode, KeyData.Key.UnicodeChar);
        Print(L"KeyData.KeyState.KeyShiftState = 0x%x, KeyData.KeyState.KeyToggleState = 0x%x\n",KeyData.KeyState.KeyShiftState, KeyData.KeyState.KeyToggleState);
    }
    Status = TextInputEx->UnregisterKeyNotify(TextInputEx, HotkeyNotifyHandle);
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS      Status;
    // EFI_INPUT_KEY   Key;
    UINTN           HandleCount;
    EFI_HANDLE      *Handles;
    UINTN           Index;
    // EFI_KEY_DATA                      KeyData;

    #ifdef LOG
        Print(L"UefiMain Entry!!!!\n");
    #endif
    Status = LogInitial(ImageHandle);

    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *TextInputEx;
    EFI_KEY_DATA myHotKey = {0,0};
    myHotKey.Key.UnicodeChar = 'a';
    myHotKey.KeyState.KeyShiftState = EFI_LEFT_CONTROL_PRESSED;


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
    Status = HotKeySample(TextInputEx,&myHotKey);



    // Status = gBS->WaitForEvent(1, &TextInputEx->WaitForKeyEx, &Index);
    // if (EFI_ERROR (Status)) {
    //     Print(L"WaitForKeyEx Status %r\n", Status);
    // }
    // Status = TextInputEx->ReadKeyStrokeEx(TextInputEx, &KeyData);
    // Print(L"KeyData.Key.ScanCode = 0x%04x, KeyData.Key.UnicodeChar = 0x%04x\n",KeyData.Key.ScanCode, KeyData.Key.UnicodeChar);
    // Print(L"KeyData.KeyState.KeyShiftState = 0x%x, KeyData.KeyState.KeyToggleState = 0x%x\n",KeyData.KeyState.KeyShiftState, KeyData.KeyState.KeyToggleState);
    // // Status = GetKey(&Key);
    // // Print(L"Key.ScanCode = 0x%04x, Key.UnicodeChar = 0x%04x\n",Key.ScanCode, Key.UnicodeChar);

    return EFI_SUCCESS;
}


