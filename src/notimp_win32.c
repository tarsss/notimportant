#define OS_PAGE_SIZE 4096

#include <windows.h>
#include <gl/gl.h>
#include "notimp.c"

int running = 1;

void win32_assert()
{
    u32 error = GetLastError();

    if(error != 0)
    {
        c8* message = 0;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                      FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      error,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR) &message, 0,0);
        assert(0);
    }
}

void *os_mem_reserve(u64 size_bytes)
{
    return VirtualAlloc(0, size_bytes, MEM_RESERVE, PAGE_READWRITE);
}

void os_mem_commit(void *p, u64 size_bytes)
{
    VirtualAlloc(p, size_bytes, MEM_COMMIT, PAGE_READWRITE);
}

void *os_read_file(c8 *path, arena *arena, u64 *out_size)
{
    HANDLE file = CreateFileA(path, 
                        GENERIC_READ, 
                        FILE_SHARE_READ,
                        0,
                        OPEN_EXISTING, 
                        FILE_ATTRIBUTE_NORMAL, 
                        0);
    win32_assert();
    u32 size = GetFileSize(file, 0); // todo: epic fail with a large file
    win32_assert();
    if(out_size)
    {
        *out_size = size;
    }

    void *p = arena_push_dont_zero(arena, size, 8);  

    ReadFile(file,
             p,
             size,
             0,
             0);
    win32_assert();
    CloseHandle(file);
    win32_assert();
    return p;
}

LRESULT win32_window_callback(HWND window,
                              UINT message,
                              WPARAM wParam,
                              LPARAM lParam)
{
    switch (message)
    {
        case WM_KEYDOWN:
        {
            if(wParam == VK_ESCAPE)
            {
                running = 0;
            }
            break;
        }
        case WM_CLOSE:
        {
            DestroyWindow(window);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(window, message, wParam, lParam);
        }
    }
    return 0;
}

int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nShowCmd)
{

    ////////////////////////////////////////////////////////////////////////////////
    // create a window
    ////////////////////////////////////////////////////////////////////////////////
    
    WNDCLASS wndclass = {0};
    wndclass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = win32_window_callback;
    wndclass.hInstance = hInstance;
    wndclass.hCursor = LoadCursor(0, IDC_ARROW);
    wndclass.hbrBackground = 0;
    wndclass.lpszClassName = "notimportant";
    RegisterClass(&wndclass);

    HWND window = CreateWindowEx(0, wndclass.lpszClassName, "notimportant",
                                 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 0, 0, hInstance, 0);

    ////////////////////////////////////////////////////////////////////////////////
    // sad semi modern opengl initialization
    ////////////////////////////////////////////////////////////////////////////////

    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    
    HDC hdc = GetDC(window);
    int iPixelFormat = ChoosePixelFormat(hdc, &pfd);
    assert(SetPixelFormat(hdc, iPixelFormat, &pfd));
    HGLRC oldRenderingContext = wglCreateContext(hdc);
    assert(wglMakeCurrent(hdc, oldRenderingContext))

    typedef HGLRC wgl_create_context_attribs_ARB(HDC hDC, 
                                                 HGLRC hShareContext, 
                                                 const int *attribList);
    wgl_create_context_attribs_ARB *wglCreateContextAttribsARB;
    wglCreateContextAttribsARB = (void*)wglGetProcAddress("wglCreateContextAttribsARB");

    #define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
    #define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
    #define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
    #define WGL_CONTEXT_FLAGS_ARB                   0x2094
    #define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

    // Accepted as bits in the attribute value for WGL_CONTEXT_FLAGS in
    // <*attribList>:

    #define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
    #define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

    // Accepted as bits in the attribute value for
    // WGL_CONTEXT_PROFILE_MASK_ARB in <*attribList>:

    #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
    #define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

    int attribs[] = 
    { 
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB,
        WGL_CONTEXT_DEBUG_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0 
    };

    HGLRC modernContext = wglCreateContextAttribsARB(hdc, 0, attribs);
    assert(modernContext);
    wglDeleteContext(oldRenderingContext);
    assert(wglMakeCurrent(hdc, modernContext));

    ////////////////////////////////////////////////////////////////////////////////
    // main loop nyaa
    ////////////////////////////////////////////////////////////////////////////////

    setup();

    while(running)
    {
        MSG message = {0};
        while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            if(message.message == WM_QUIT)
            {
                running = 0;
            }
            DispatchMessage(&message);
        }

        update();

        glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        SwapBuffers(hdc);
        glFinish();
    }
}