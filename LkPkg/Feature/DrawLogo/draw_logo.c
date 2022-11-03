#include  <AMI/loglib.h>
#include "draw_logo.h"



EFI_GRAPHICS_OUTPUT_BLT_PIXEL Grey = {166, 166, 166, 0};

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
EFI_STATUS DrawLogo(
    IN EFI_HANDLE ImageHandle,
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    CHAR16 *FileName = L"\\Logo.bmp"; 
    UINTN Hor = Gop->Mode->Info->HorizontalResolution;
    UINTN Ver = Gop->Mode->Info->VerticalResolution;
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
    // EFI_PHYSICAL_ADDRESS LogoAddress;
    // Status = ReadFile(Logo, &LogoAddress);

    // BMP_CONFIG BmpConfig;
    // Status = BmpTransform(LogoAddress, &BmpConfig);

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