#include <gl/gl.h>

#define GL_STREAM_DRAW                  0x88E0
#define GL_STATIC_DRAW                  0x88E4
#define GL_DYNAMIC_DRAW                 0x88E8
#define GL_ARRAY_BUFFER                 0x8892
#define GL_ELEMENT_ARRAY_BUFFER         0x8893
#define GL_FRAGMENT_SHADER              0x8B30
#define GL_VERTEX_SHADER                0x8B31
#define GL_COMPILE_STATUS               0x8B81
#define GL_LINK_STATUS                  0x8B82
#define GL_FLOAT                        0x1406
#define GL_TEXTURE0                     0x84C0

                             //////  //   //  //
                            //      /// ///  //
                           /////   // / //  //
                          //      //   //  //////
                          
#define GL_EXTENSIONS_LIST \
X(void,   glGenBuffers,                 (i32 n, u32 *buffers))\
X(void,   glBufferData,                 (GLenum target, u64 size, const void *data, GLenum usage))\
X(void,   glBindBuffer,                 (GLenum target, u32 buffer))\
X(u32,    glCreateShader,               (GLenum type))\
X(void,   glShaderSource,               (u32 shader, i32 count, const c8 *const*string, const i32 *length))\
X(void,   glCompileShader,              (u32 shader))\
X(u32,    glCreateProgram,              (void))\
X(void,   glAttachShader,               (u32 program, u32 shader))\
X(void,   glLinkProgram,                (u32 program))\
X(void,   glGetShaderiv,                (u32 shader, GLenum pname, i32 *params))\
X(void,   glGetShaderInfoLog,           (u32 shader, i32 bufSize, i32 *length, c8 *infoLog))\
X(void,   glGetProgramiv,               (u32 program, GLenum pname, i32 *params))\
X(void,   glGetProgramInfoLog,          (u32 program, i32 bufSize, i32 *length, c8 *infoLog))\
X(void,   glVertexAttribPointer,        (u32 index, i32 size, GLenum type, b8 normalized, i32 stride, const void *pointer))\
X(void,   glEnableVertexAttribArray,    (u32 index))\
X(void,   glGenVertexArrays,            (i32 n, u32 *arrays))\
X(void,   glBindVertexArray,            (u32 array))\
X(void,   glUseProgram,                 (u32 program))\
X(i32,    glGetUniformLocation,         (u32 program, const c8 *name))\
X(void,   glUniform1fv,                 (i32 location, i32 count, const GLfloat *value))\
X(void,   glUniform2fv,                 (i32 location, i32 count, const GLfloat *value))\
X(void,   glUniform3fv,                 (i32 location, i32 count, const GLfloat *value))\
X(void,   glUniform4fv,                 (i32 location, i32 count, const GLfloat *value))\
X(void,   glUniformMatrix4fv,           (i32 location, i32 count, b8 transpose, const GLfloat *value))\
X(void,   glActiveTexture,              (GLenum texture))\
X(void,   glDeleteBuffers,              (i32 n, const u32 *buffers))\
X(void,   glDeleteVertexArrays,         (i32 n, const u32 *arrays))

#define X(returns, name, args)          returns (*name) args = 0;

GL_EXTENSIONS_LIST

#undef X

void win32_load_gl_extensions()
{
    #define X(returns, name, args)      name = (void*)wglGetProcAddress(#name);\
                                        assert(#name);

    GL_EXTENSIONS_LIST
}