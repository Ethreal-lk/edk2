#include <Library/BaseMemoryLib.h>
#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>
#include <Guid/FileInfo.h>

struct bmp_header {
    UINT8 sig[2];
    UINT32 file_size;
    UINT16 reserved1;
    UINT16 reserved2;
    UINT32 header_size;
    UINT32 info_header_size;
    UINT32 width;
    UINT32 height;
    UINT16 plane_num;
    UINT16 color_bit;
    UINT32 compression_type;
    UINT32 compression_size;
    UINT32 horizontal_pixel;
    UINT32 vertical_pixel;
    UINT32 color_num;
    UINT32 essentail_num;
};
typedef struct
{
    EFI_PHYSICAL_ADDRESS       FrameBufferBase;
    UINTN                      FrameBufferSize;
    UINT32                     HorizontalResolution;
    UINT32                     VerticalResolution;
    UINT32                     PixelsPerScanLine;
} VIDEO_CONFIG;

typedef struct
{
    UINTN Size;
    UINTN PageSize;
    UINTN Width;
    UINTN Height;
    UINTN Offset;
    UINT64 PixelStart;
} BMP_CONFIG;

