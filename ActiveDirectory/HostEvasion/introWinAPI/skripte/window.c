#include <windows.h>
#include <Lmcons.h>  
#include <stdio.h>
#include <stdlib.h>
const char g_szClassName[] = "myWindowClass";
void executeCurl(const char* username){
    char naredba[256];
    int maxLength = sizeof(naredba) - 1;
    snprintf(naredba, maxLength, "curl http://X:P/%s",username);
    system(naredba);
}

// Window procedura
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            char username[UNLEN + 1];
            DWORD username_len = UNLEN + 1;
            char* poruka="Pozdrav";
            if (GetUserNameA(username, &username_len)) {
                printf("%s\n",username);
                executeCurl(username);
                TextOutA(hdc, 10, 10, username, lstrlenA(username));
                TextOutA(hdc, 10, 45, poruka, lstrlen(poruka));

            } else {
                TextOutA(hdc, 10, 10, "Ne mogu dohvatiti ime korisnika", 100);
            }

            EndPaint(hwnd, &ps);
        }
        break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{


    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Korak jedan, registracija klase
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Windows registracija nije uspjela", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Ime prozora",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 240,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}