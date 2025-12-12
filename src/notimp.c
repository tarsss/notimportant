#include "notimp.h"

#pragma pack(push, 1)
typedef struct
{
    u16     FileType;        /* File type, always 4D42h ("BM") */
	u32     FileSize;        /* Size of the file in bytes */
	u16     Reserved1;       /* Always 0 */
	u16     Reserved2;       /* Always 0 */
	u32     BitmapOffset;    /* Starting position of image data in bytes */
	u32     Size;            /* Size of this header in bytes */
	u32     Width;           /* Image width in pixels */
	u32     Height;          /* Image height in pixels */
	u16     Planes;          /* Number of color planes */
	u16     BitsPerPixel;    /* Number of bits per pixel */
	u32     Compression;     /* Compression methods used */
	u32     SizeOfBitmap;    /* Size of bitmap in bytes */
} bitmap_header;
#pragma pack(pop)

bitmap_header *load_bitmap(c8 *path, arena *arena, void **pixels)
{
    bitmap_header *bmp = os_read_file(path, arena, 0);

    *pixels = ((u8*)bmp) + bmp->BitmapOffset;

    // we want a raw uncompressed bitmap for this throwaway thing
    assert(bmp->Compression == 3);

    // bgra -> rgba. im sure its so slow its almost 5% of the time it takes vscode to type a character
    u32 num_pixels = bmp->Width * bmp->Height;
    for(u32 i = 0; i < num_pixels; i++)
    {
        u8* ptr = ((u8*)*pixels) + i * 4;

        u8 b = *(ptr + 0);
        u8 g = *(ptr + 1);
        u8 r = *(ptr + 2);
        u8 a = *(ptr + 3);
        
        *(ptr + 0) = r;
        *(ptr + 1) = g;
        *(ptr + 2) = b;
        *(ptr + 3) = a;
    }

    return bmp;
}

void setup()
{
    arena *test_arena = arena_alloc(1LL << 32, 1LL << 16);
    u64 size;
    void *file = os_read_file("c:/notimportant/src/notimp.c", test_arena, &size);
    void *pixels;
    bitmap_header *bmp = load_bitmap("c:/notimportant/resources/font.bmp", test_arena, &pixels);
}

void update()
{

}