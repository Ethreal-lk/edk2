#include <Uefi.h>
#include  <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include  <AMI/ProtocolLib.h>
#define DELAY_1S 1000000

EFI_GRAPHICS_OUTPUT_PROTOCOL          *mGraphicsOutput;


// VOID
// NotifyWaitFunc(
//   IN  EFI_EVENT                Event,
//   IN  VOID                     *Context)
// {
//   static UINTN count =0;
//   EFI_STATUS Status;
//   count++;
//   gBS->Stall(DELAY_1S);
//   Print(L"NotifyWaitFunc:  Context = %s ount = %d\n" ,Context, count);
//   if ((count % 5) ==0) {
//     Status = gBS->SignalEvent(Event);
//   }

// }
// VOID
// NotifySignalFunc(
//   IN  EFI_EVENT                Event,
//   IN  VOID                     *Context)
// {

//   Print(L"NotifySignalFunc:Context = %s  \n",  Context);
// }


// VOID TestNotifyEvent(VOID)
// {
//   EFI_EVENT myWaitEvent;
//   EFI_EVENT mySignalEvent;
//   UINTN     Index;
//   EFI_STATUS Status;

// Status = gBS->CreateEvent(
//             EVT_NOTIFY_SIGNAL,
//             TPL_NOTIFY,
//             (EFI_EVENT_NOTIFY)NotifySignalFunc,
//             (VOID*)L"Signal!",
//             &mySignalEvent);
// Print(L"CreateEvent Status = %r\n", Status);
// Status = gBS->CreateEvent(
//             EVT_NOTIFY_WAIT,
//             TPL_NOTIFY,
//             (EFI_EVENT_NOTIFY)NotifyWaitFunc,
//             (VOID*)L"wait!",
//             &myWaitEvent);
// Print(L"CreateEVT_NOTIFY_WAITEvent Status = %r\n", Status);
  
//   Status = gBS->WaitForEvent(1, &myWaitEvent,&Index);
//   Print(L"WaitForEvent Status = %r\n", Status);
//   Status = gBS->SignalEvent(mySignalEvent);
//   Print(L"Status = %r\n", Status);

//   // Status = gBS->CloseEvent(myWaitEvent);
//   Status = gBS->CloseEvent(mySignalEvent);

// }
INT32 Num_rand(void)
{
  INT32 hi,lo, x;
  static UINT32 next = 1;
  if (next ==0 ){
    next = 123459876;
  }
  hi = next / 127773;
  lo = next %1277773;
  x = 16807 * lo - 2836 * hi;
  if (x  < 0){
    x += 0x7fffffff;
  }
  return ((next) = x) % (((UINT32)0x7fffffff+1));
}
VOID rectblock(
  UINTN x1,
  UINTN y1,
  UINTN x2,
  UINTN y2,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL * color)
{
  UINTN minx, miny, maxx, maxy;
  EFI_STATUS  Status;


  minx = (x1 < x2) ? x1 : x2;
  miny = (y1 < y2) ? y1 : y2;
  maxx = (x1 > x2) ? x1 : x2;
  maxy = (y1 > y2) ? y1 : y2;
  Status =   mGraphicsOutput->Blt(mGraphicsOutput,
                      color,
                      EfiBltVideoFill,
                      0,0,
                      minx,miny,
                      (maxx - minx + 1), (maxy - miny + 1),
                      0);
  Print(L"rectblock Blt Status = %r\n", Status);
}

VOID RandomBlock(
  UINT32  Width,
  UINT32  Height,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltArray,
  UINT32 BltArraySize
)
{

  EFI_EVENT     myEvent;
  EFI_STATUS    Status;
  UINTN         repeats = 0;
  UINTN         index = 0;
  EFI_HANDLE   *HandleBuffer;
  UINTN         HandleCount;
  UINT32        rand_x1, rand_y1, randColor;

  Status = gBS->CreateEvent(
      EVT_TIMER,
      TPL_CALLBACK,
      (EFI_EVENT_NOTIFY)NULL,
      (VOID*)NULL,
      &myEvent);
  Status = gBS->SetTimer(myEvent, TimerPeriodic, 2 * 1000 * 1000);// 200ms Ò»´Î
  HandleCount = CountProtocolInstances(&gEfiGraphicsOutputProtocolGuid, &HandleBuffer);
  Status = gBS->HandleProtocol(
    HandleBuffer[0],
    &gEfiGraphicsOutputProtocolGuid,
    (VOID **)&mGraphicsOutput);

  while (1)
  {
    Status = gBS->WaitForEvent(1,&myEvent,&index);
    rand_x1 = (Num_rand() % Width);
    rand_y1 = (Num_rand() % Height);
    randColor = (Num_rand() % BltArraySize);
    Print(L"repeats = %d rand_x1 = %d rand_y1 = %d randColor = %d\n ",repeats,rand_x1, rand_y1, randColor);
    rectblock(rand_x1,rand_y1, rand_x1 + 50, rand_y1 + 100,&BltArray[randColor]);
    if (repeats ++ == 100) {
      break;
    }
  }
  Status = gBS->CloseEvent(myEvent);

}


EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL color[] = {
                                          {0, 0, 255, 0},     // red 
                                          {250, 250, 255, 0}, // snow
                                          {255, 255 ,0},       // Cyan1
                                          {173, 222,255,0},
                                          {185, 218,255},
                                          {250, 255, 245}
      
                                      };
  RandomBlock(80,60,color,6);
  return EFI_SUCCESS;
}