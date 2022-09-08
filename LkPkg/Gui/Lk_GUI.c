#include <Uefi.h>
#include  <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#define DELAY_1S 1000000


VOID
NotifyWaitFunc(
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context)
{
  static UINTN count =0;
  EFI_STATUS Status;
  count++;
  gBS->Stall(DELAY_1S);
  Print(L"NotifyWaitFunc:  Context = %s ount = %d\n" ,Context, count);
  if ((count % 5) ==0) {
    Status = gBS->SignalEvent(Event);
  }

}
VOID
NotifySignalFunc(
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context)
{

  Print(L"NotifySignalFunc:Context = %s  \n",  Context);
}


VOID TestNotifyEvent(VOID)
{
  EFI_EVENT myWaitEvent;
  EFI_EVENT mySignalEvent;
  UINTN     Index;
  EFI_STATUS Status;

Status = gBS->CreateEvent(
            EVT_NOTIFY_SIGNAL,
            TPL_NOTIFY,
            (EFI_EVENT_NOTIFY)NotifySignalFunc,
            (VOID*)L"Signal!",
            &mySignalEvent);
Print(L"CreateEvent Status = %r\n", Status);
Status = gBS->CreateEvent(
            EVT_NOTIFY_WAIT,
            TPL_NOTIFY,
            (EFI_EVENT_NOTIFY)NotifyWaitFunc,
            (VOID*)L"wait!",
            &myWaitEvent);
Print(L"CreateEVT_NOTIFY_WAITEvent Status = %r\n", Status);
  
  Status = gBS->WaitForEvent(1, &myWaitEvent,&Index);
  Print(L"WaitForEvent Status = %r\n", Status);
  Status = gBS->SignalEvent(mySignalEvent);
  Print(L"Status = %r\n", Status);

  // Status = gBS->CloseEvent(myWaitEvent);
  Status = gBS->CloseEvent(mySignalEvent);

}
EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  TestNotifyEvent();
  return EFI_SUCCESS;
}