#include <windows.h>
#include <string>
#include <sstream>

#define DLLNAME "lib.dll"

LPCSTR ClassName = "main_class";
LPCSTR Title = "Сетевое подключение";

using namespace std;
HWND hwnd, MSLabel;
int fstatus = 0, fstatus_old = -1;
void thread() // A function of access to dynamic library
{
    HINSTANCE hinstLib = LoadLibrary(TEXT(DLLNAME)); // Загружаем динамическию библиотеку и получаем ее дескриптор
    if(hinstLib != NULL) // если дескриптор получен
    {
        int htt, ifc; //Variables of status of connection and htt-support
        stringstream stream; //An occurrence of stream class to operate on strings
        typedef int (*ifconnected_)(); //Import
        ifconnected_ ifconnected = (ifconnected_)GetProcAddress(hinstLib,"is_connect");
        if(ifconnected != NULL)
        {
			ifc = ifconnected();//Receive a data to apply a picture
            if(ifconnected)//Solution
				stream << "LEVEL III:" << endl << "    Network connection" << endl << "    is detected!" << endl;
            else
				stream << "LEVEL III:" << endl << "    Network connection" << endl << "    is not detected!" << endl;
        }
        else // If something wrong with dynamic library
            MessageBox(hwnd, "int ifconnected() not found in LIB", "Error", MB_OK | MB_ICONERROR);
        typedef int (*htt_support_)();//HTT section
        htt_support_ htt_support = (htt_support_)GetProcAddress(hinstLib, "htt_support");
        if(htt_support != NULL)
        {
            htt = htt_support();//Receive a data to apply a picture
			//lbl << ifc << " " << htt << endl;
			stream << "LEVEL IV:" << endl;
            if(htt)//Solution
				stream << "    Hyper Threading" << endl << "    is supported!";
            else
				stream << "    Hyper Threading" << endl << "    is not supported!";
            SetWindowText(MSLabel, (stream.str()).c_str());
        }
        else// If something wrong with dynamic library
            MessageBox(hwnd, "int htt_support() not found in LIB", "Error", MB_OK | MB_ICONERROR);
        FreeLibrary(hinstLib);// Free
    }
    else //If dynamic library did not found
        MessageBox(hwnd, "LIB did not found! Put it into the program folder and click \"Run\"!", "Error", MB_OK | MB_ICONERROR);
}


LRESULT CALLBACK WindowFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) // Message processor
{
    int wmId, wmEvent;
    static HDC memBit;
    static HBITMAP hBitmap;
    static BITMAP bm;
    switch (msg)
    {
    case WM_CTLCOLORSTATIC:// Text and background
    {
        DWORD CtrlID = GetDlgCtrlID((HWND)lParam);
        if (CtrlID == 1024)
        {
            SetTextColor((HDC)wParam, RGB(30, 200, 30));
            SetBkColor((HDC)wParam, RGB(0, 0, 0));
            return (INT_PTR)GetStockObject(BLACK_BRUSH);
        }
    }
    break;
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 1025)
        {
            HANDLE hThread;
            DWORD IDThread;
            hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, NULL, 0, &IDThread); //Function in thread
            CloseHandle(hThread);
        }
        if (LOWORD(wParam) == 1026)
            PostQuitMessage(0);
    }
    break;
    case WM_PAINT:
	{ //Output a bitmap
		PAINTSTRUCT ps;
		HDC hdc; //contrxt
        hdc = BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        if(fstatus != fstatus_old)
        {
            InvalidateRect(hwnd, 0, 0);
            UpdateWindow(hwnd);
            fstatus_old = fstatus;
        }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR str, int nWinShow)
{
    WNDCLASS wnd;
    RECT rt;
    HANDLE hThread;
    DWORD IDThread;
    HFONT font_std = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    HFONT font_mono = (HFONT)GetStockObject(OEM_FIXED_FONT);
    memset(&wnd, 0, sizeof(WNDCLASS));
    wnd.style = CS_HREDRAW | CS_VREDRAW;
    wnd.lpfnWndProc = WindowFunc;
    wnd.hInstance = hInstance;
    wnd.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wnd.lpszClassName = ClassName;
    wnd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(100));
    RegisterClass(&wnd);
    int window_width = 200;
    int window_height = 300;
    int button_width = 90;
    int button_height = 30;
    int border = 3;
    HDC hDCScreen = GetDC(NULL);
    hwnd = CreateWindow(ClassName, Title, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        (GetDeviceCaps(hDCScreen, HORZRES) - window_width) / 2, (GetDeviceCaps(hDCScreen, VERTRES) - window_height) / 2,
        window_width, window_height, NULL, NULL, hInstance, NULL);
    GetClientRect(hwnd, &rt);
    window_width = rt.right;
    window_height = rt.bottom;
    MSLabel = CreateWindow("static", "Нажмите \"Обновить\" для загрузки ", WS_CHILD | WS_VISIBLE | WS_BORDER,
        border, border, window_width - 2 * border, (window_height - button_height - 4 * border) / 2,
        hwnd, (HMENU)1024, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1024, WM_SETFONT, (WPARAM)font_mono, TRUE);
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, NULL, 0, &IDThread);
    CloseHandle(hThread);//Buttons below
    CreateWindow("btn_update", "Обновить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, window_width - 2 * (border + button_width), window_height - border - button_height, button_width, button_height, hwnd, (HMENU)1025, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1025, WM_SETFONT, (WPARAM)font_std, TRUE);
    CreateWindow("btn_exit", "Закрыть", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, window_width - border - button_width, window_height - border - button_height, button_width, button_height, hwnd, (HMENU)1026, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1026, WM_SETFONT, (WPARAM)font_std, TRUE);
    ShowWindow(hwnd, nWinShow);
    UpdateWindow(hwnd);
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))//Loop
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
