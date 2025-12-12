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

void gl_assert_no_error()
{
    u32 gl_error = glGetError();
    assert(gl_error == 0);
}

typedef struct
{
    u32             program;

} shader;

shader *load_shader(c8 *name, arena_t *arena, arena_t *scratch)
{
    shader *ls = arena_push_struct(arena, shader);
    string8 name_vert = cstring_add(name, ".vert", scratch);
    string8 name_frag = cstring_add(name, ".frag", scratch);

    c8 *vert_source = (c8*)os_read_file(name_vert.cstring_ptr, scratch, 0);
    c8 *frag_source = (c8*)os_read_file(name_frag.cstring_ptr, scratch, 0);

    // compile vertex...
    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vert_source, 0);
    glCompileShader(vertex_shader);

    // log vertex...
    i32 compile_status;
    c8 log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    glGetShaderInfoLog(vertex_shader, 512, 0, log);
    assert(compile_status);

    // compile fragment...
    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &frag_source, 0);
    glCompileShader(fragment_shader);
    
    // log fragment...
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    glGetShaderInfoLog(fragment_shader, 512, 0, log);
    assert(compile_status);

    // link...
    u32 shader = glCreateProgram();
    glAttachShader(shader, vertex_shader);
    glAttachShader(shader, fragment_shader);
    glLinkProgram(shader);
    
    // log link...
    i32 link_status;
    glGetProgramiv(shader, GL_LINK_STATUS, &link_status);
    glGetProgramInfoLog(shader, 512, 0, log);
    assert(link_status);

    ls->program = shader;

    gl_assert_no_error();
    return ls;
}

u32 renderer_upload_bitmap(bitmap_header *bitmap, void *data)
{
    u32 texture_handle;

    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D,
                0,
                GL_RGBA8,
                bitmap->Width,
                bitmap->Height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data);

    gl_assert_no_error();
    
    return texture_handle;
}

typedef struct
{
    vector3 pos;
    vector2 uv;

} text_vertex;

arena   *test_arena;
u32     tex;
shader  *text_shader;

void setup() 
{
    test_arena = arena_alloc(1LL << 32, 1LL << 16);
    u64 size;
    void *file = os_read_file("c:/notimportant/src/notimp.c", test_arena, &size);
    void *pixels;
    bitmap_header *bmp = load_bitmap("c:/notimportant/resources/font.bmp", test_arena, &pixels);
    text_shader = load_shader("c:/notimportant/resources/text", test_arena, test_arena);
    tex = renderer_upload_bitmap(bmp, pixels);
}

typedef struct
{
    u32 screen_w, screen_h;

} input, input_t;

void update(input *input)
{
    glViewport(0, 0, input->screen_w, input->screen_h);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    text_vertex vertices[4] = 
    {
        {
            .pos = (vector3) { 0.0f, 0.0f, 0.0f}, 
            .uv = (vector2) { 0.0f, 0.0f }
        },
        {
            .pos = (vector3) { 1.0f, 0.0f, 0.0f}, 
            .uv = (vector2) { 1.0f, 0.0f }
        },
        {
            .pos = (vector3) { 1.0f, 1.0f, 0.0f}, 
            .uv = (vector2) { 1.0f, 1.0f }
        },
        {
            .pos = (vector3) { 0.0f, 1.0f, 0.0f }, 
            .uv = (vector2) { 0.0f, 1.0f }
        },
    };
    u32 indices[6] = 
    {
        0, 1, 2, 2, 3, 0
    };

    u32 vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(text_vertex) * 4, &vertices, GL_STREAM_DRAW);
    gl_assert_no_error();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * 6, &indices, GL_STREAM_DRAW);
    gl_assert_no_error();

    glVertexAttribPointer(0, 
        3,
        GL_FLOAT,
        0,
        sizeof(vector3) + sizeof(vector2), 
        (void *)0);
    
    glVertexAttribPointer(1, 
        2,
        GL_FLOAT,
        0,
        sizeof(vector3) + sizeof(vector2), 
        (void *)sizeof(vector3));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glUseProgram(text_shader->program);

    i32 loc = glGetUniformLocation(text_shader->program, "color");
    glUniform4fv(loc, 1, ((f32[]) { 1.0f, 1.0f, 1.0f, 1.0f } ));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    gl_assert_no_error();
}