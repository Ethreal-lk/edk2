#include  <AMI/loglib.h>
#include "draw_logo.h"
#include <Uefi/UefiSpec.h>
#include <Uefi/UefiMultiPhase.h>




EFI_GRAPHICS_OUTPUT_BLT_PIXEL Grey = {166, 166, 166, 0};
UINTN GetValue(
    IN UINT8 *StartAddress,
    IN UINTN Offset,
    IN UINTN Size
)
{

    UINT8 *ByteStart = StartAddress + Offset;
    UINTN Result = 0;
    for(UINTN i = 0; i < Size; i++)
    {
        Result += *(ByteStart + i) << i * 8;
    }
    return Result;
}
EFI_STATUS GetGopHandle(
    IN EFI_HANDLE ImageHandle,
    OUT EFI_GRAPHICS_OUTPUT_PROTOCOL **Gop
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN HandleCount = 0;
    EFI_HANDLE *HandleBuffer;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
    );
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to LocateHanleBuffer of GraphicsOutputProtocol.\n");
        return Status;
    }
    Print(L"SUCCESS:Get %d handles that supported GraphicsOutputProtocol.\n", HandleCount);
    
    #endif
    Status = gBS->OpenProtocol(
        HandleBuffer[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)Gop,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to open first handle that supported GraphicsOutputProtocol.\n");
        return Status;
    }

    Print(L"SUCCESS:GraphicsOutputProtocol is opened with first handle.\n");
    
    #endif

    return Status;
}


EFI_STATUS SetVideoMode(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *ModeInfo;
    UINTN ModeInfoSize = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
    UINTN H = 0;
    UINTN V = 0;
    UINTN ModeIndex = 0;

    #ifdef DEBUG
        Print(L"Gop address second_1: 0x%x.\n", Gop);
    #endif

    for(UINTN i = 0; i < Gop->Mode->MaxMode; i++)
    {
        Status = Gop->QueryMode(Gop, i, &ModeInfoSize, &ModeInfo);
        H = ModeInfo->HorizontalResolution;
        V = ModeInfo->VerticalResolution;
        if(((H == 1024) && (V == 768)) || ((H == 1440) && (V == 900)))
        {    
            ModeIndex = i;            
        }
    }

    Status = Gop->SetMode(Gop, ModeIndex);    

    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to SetMode.\n");
        return Status;
    }
    Print(L"SUCCESS:SetMode to Index:%d.\n", ModeIndex);
    
    #endif
    
    return Status;
}

EFI_STATUS VideoInit(
    IN EFI_HANDLE ImageHandle,
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL **Gop,
    OUT VIDEO_CONFIG *VideoConfig
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    Status = GetGopHandle(ImageHandle, Gop);
    #ifdef DEBUG
        Print(L"Gop address second:0x%x.\n", *Gop);
    #endif
    Status = SetVideoMode((*Gop));

    VideoConfig->FrameBufferBase = (*Gop)->Mode->FrameBufferBase;
    VideoConfig->FrameBufferSize = (*Gop)->Mode->FrameBufferSize;
    VideoConfig->HorizontalResolution = (*Gop)->Mode->Info->HorizontalResolution;
    VideoConfig->VerticalResolution = (*Gop)->Mode->Info->VerticalResolution;
    VideoConfig->PixelsPerScanLine = (*Gop)->Mode->Info->PixelsPerScanLine;
    
    return Status;
}
EFI_STATUS GetFileHandle(
    IN EFI_HANDLE ImageHandle,
    IN CHAR16 *FileName,
    OUT EFI_FILE_PROTOCOL **FileHandle
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN HandleCount = 0;
    EFI_HANDLE *HandleBuffer;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
    );
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to LocateHanleBuffer of SimpleFileSystemProtocol.\n");
        return Status;
    }
    Print(L"SUCCESS:Get %d handles that supported SimpleFileSystemProtocol.\n", HandleCount);
    #endif
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
    Status = gBS->OpenProtocol(
        HandleBuffer[0],
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID **)&FileSystem,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to open first handle that supported SimpleFileSystemProtocol.\n");
        return Status;
    }

    Print(L"SUCCESS:SimpleFileSystemProtocol is opened with first handle.\n");
    #endif
    
    EFI_FILE_PROTOCOL *Root;
    Status = FileSystem->OpenVolume(
        FileSystem,
        &Root
    );
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to open volume.\n");
        return Status;
    }

    Print(L"SUCCESS:Volume is opened.\n");
    #endif
    Status = Root->Open(
        Root,
        FileHandle,
        FileName, // L"\\Logo.bmp"
        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );


    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to open file.\n");
        return Status;
    }
    Print(L"SUCCESS:File is opened.\n");
    #endif

    return Status;
} 
EFI_STATUS ReadFile(
    IN EFI_FILE_PROTOCOL *File,
    OUT UINT8           **FileBase
)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    EFI_FILE_INFO   *FileInfo;
    UINT8           *Buffer;

    UINTN InfoSize = sizeof(EFI_FILE_INFO) + 128;
    Status = gBS->AllocatePool(
        EfiLoaderData,
        InfoSize,
        (VOID **)&FileInfo
    );

    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to AllocatePool for FileInfo.\n");
        return Status;
    }
    Print(L"SUCCESS:Memory for FileInfo is ready.\n");
    #endif

    Status = File->GetInfo(
        File,
        &gEfiFileInfoGuid,
        &InfoSize,
        FileInfo
    );
    
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to GetInfo of Bmp.\n");
        return Status;
    }
    Print(L"SUCCESS:FileInfo is getted.\n");
    #endif


    Status = gBS->AllocatePool(EfiReservedMemoryType, (UINTN)FileInfo->FileSize,(VOID**)&Buffer);
    #ifdef DEBUG
        if(EFI_ERROR(Status))
        {
            Print(L"ERROR:Failed to AllocatePool for File.\n");
            return Status;
        }
        Print(L"SUCCESS:Memory for File is ready. Memory address = 0x%0x\n", Buffer);
    #endif

    // UINTN FilePageSize = (UINTN)((FileInfo->FileSize >> 12) + 1);
    // Print(L"FilePageSize = %d\n", FilePageSize);
    
    // EFI_PHYSICAL_ADDRESS FileBufferAddress;
    // Status = gBS->AllocatePages(
    //     AllocateAnyPages,
    //     EfiLoaderData,
    //     FilePageSize,
    //     &FileBufferAddress
    // );

    // #ifdef DEBUG
    // if(EFI_ERROR(Status))
    // {
    //     Print(L"ERROR:Failed to AllocatePages for File.\n");
    //     return Status;
    // }
    // Print(L"SUCCESS:Memory for File is ready.\n");
    // #endif
    UINTN ReadSize = (UINTN)FileInfo->FileSize;
    Status = File->Read(
        File,
        &ReadSize,
        Buffer 
    );
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to Read File.\n");
        return Status;
    }
    Print(L"SUCCESS:File is readed,size=%d.\n", ReadSize);
    #endif
    gBS->FreePool(FileInfo);
    *FileBase = Buffer;
    return Status;
}
UINT8* BmpTransform(
    IN  UINT8       *BmpBase,
    OUT BMP_CONFIG  *BmpConfig
)    
{  
    EFI_STATUS Status = EFI_SUCCESS;
    // Not used, just for example
    struct bmp_header *bheader = (struct bmp_header *)BmpBase;
    Print(L"File size = %08lx.\n", bheader->file_size);
    BmpConfig->Size = GetValue(BmpBase, 0x02, 4);
    BmpConfig->PageSize = (BmpConfig->Size >> 12) + 1;
    BmpConfig->Offset = GetValue(BmpBase, 0x0A, 4);
    
    BmpConfig->Width = GetValue(BmpBase, 0x12, 4);
    BmpConfig->Height = GetValue(BmpBase, 0x16, 4);
    UINT8           *PixelBuffer;
    Status = gBS->AllocatePool(EfiReservedMemoryType, BmpConfig->Size,(VOID**)&PixelBuffer);
    #ifdef DEBUG
        if(EFI_ERROR(Status))
        {
            Print(L"ERROR:Failed to AllocatePages for PixelArea.\n");
        }
        Print(L"SUCCESS:Memory for PixelArea is ready. PixelBuffer memory address = 0x%0x\n", PixelBuffer);
        Print(L"SUCESS :Allocate PixelBuffer memory size = %d\n", BmpConfig->Size);
    #endif
    // EFI_PHYSICAL_ADDRESS PixelStart;
    // Status = gBS->AllocatePages(
    //     AllocateAnyPages,
    //     EfiLoaderData,
    //     BmpConfig->PageSize,
    //     &PixelStart
    // );

    // #ifdef DEBUG
    // if(EFI_ERROR(Status))
    // {
    //     Print(L"ERROR:Failed to AllocatePages for PixelArea.\n");
    //     return Status;
    // }
    // Print(L"SUCCESS:Memory for PixelArea is ready.\n");
    // #endif

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelFromFile= NULL;
    PixelFromFile = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)(BmpBase 
                                                    + BmpConfig->Offset 
                                                    + BmpConfig->Width * BmpConfig->Height * 4);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelToBuffer = NULL;
    
    PixelToBuffer= (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)PixelBuffer;

    for(UINTN i = 0; i < BmpConfig->Height; i++)
    {
        PixelFromFile -= BmpConfig->Width;
        for(UINTN j = 0; j < BmpConfig->Width; j++)
        {
            *PixelToBuffer = *PixelFromFile;
            PixelToBuffer++;
            PixelFromFile++;
        }
        PixelFromFile -= BmpConfig->Width;
    }

    return PixelBuffer;
}

EFI_STATUS DrawLogo(
    IN EFI_HANDLE ImageHandle,
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    CHAR16 *FileName = L"\\Logo.bmp"; 
    UINTN Hor = Gop->Mode->Info->HorizontalResolution;
    UINTN Ver = Gop->Mode->Info->VerticalResolution;
    UINT8 * PixelStart;
    #ifdef DEBUG
        Print(L"DrawLogo: Hor = %d  Ver = %d \n", Hor, Ver);
    #endif
    if(Hor * 3 == Ver * 4)
    {
        // FileName = L"\\Narrow.BMP";
        // Print(L"Narrow BMP\n");
    }
    EFI_FILE_PROTOCOL *Logo;
    Status = GetFileHandle(ImageHandle, FileName, &Logo);

    UINT8  *LogoAddress;
    Status = ReadFile(Logo, &LogoAddress);
    Print(L"lk LogoAddress = 0x%0x\n", LogoAddress);
    BMP_CONFIG BmpConfig;
    PixelStart = BmpTransform(LogoAddress, &BmpConfig);
    Print(L"BmpConfig->PixelStart = 0x%0x\n", PixelStart);

    // UINTN X = (Hor - BmpConfig.Width) / 2;
    // UINTN Y = (Ver - BmpConfig.Height) / 2;

    // Status = DrawBmp(Gop, BmpConfig, X, Y);
    
    return Status;
}
EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    VIDEO_CONFIG VideoConfig;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop = NULL;


    #ifdef LOG
    Status = LogInitial(ImageHandle);
    if (EFI_ERROR(Status)) {
        LogError(Status);
    } else {
        LogTip("Log is good now.\n");
    }
    #endif

    #ifdef DEBUG
    Print(L"Gop address first:0x%x.\n", Gop);
    #endif
    Status = VideoInit(ImageHandle, &Gop, &VideoConfig);
    #ifdef DEBUG
    Print(L"Gop address third:0x%x.\n", Gop);
    #endif
    #ifdef LOG
    if(EFI_ERROR(Status))
    {
        LogError(Status);
    }else
    {
        LogTip("Video is good now.\n");
    }
    #endif  


    Status = DrawLogo(ImageHandle, Gop);
    


    return Status;


}