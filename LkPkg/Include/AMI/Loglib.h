#ifndef LK_LOGLIB_H
#define LK_LOGLIB_H

#include <Uefi.h>
#include <Guid/FileInfo.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleFileSystem.h>

// #include "Setup.h"

#define DEBUG
#define LOG

EFI_STATUS LogInitial(EFI_HANDLE ImageHandle);
EFI_STATUS LogTip(CHAR8 *Message);
EFI_STATUS LogError(EFI_STATUS Code);
EFI_STATUS Close();
#endif