#include <windows.h>

int running = 1;

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
    WNDCLASS wndclass = {0};
    wndclass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = win32_window_callback;
    wndclass.hInstance = hInstance;
    wndclass.hCursor = LoadCursor(0, IDC_ARROW);
    wndclass.hbrBackground = 0;
    wndclass.lpszClassName = "notimportant";
    RegisterClass(&wndclass);

    CreateWindowEx(0, wndclass.lpszClassName, "notimportant",
                    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    0, 0, hInstance, 0);
    
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
    }
}