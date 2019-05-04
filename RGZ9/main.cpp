#include <windows.h>
#include <string>
#include <sstream>
#include <stdio.h>

#define LIBRARY_NAME "lib.dll"

LPCSTR ClassName = "main_class";
LPCSTR Title = "������� �����������";

using namespace std;
stringstream stream;
stringstream stream_error;
char info[256];
HWND hwnd;
//int fstatus = 0;
//int fstatus_old = -1;

DWORD WINAPI ThreadFunc(void*) // A function of access to dynamic library
{
    HINSTANCE hinstLib = LoadLibrary(TEXT(LIBRARY_NAME)); // ��������� ������������ ���������� � �������� �� ����������
    if(hinstLib != NULL) // ���� ���������� ����� ������������ ���������� �������
    {
		// ���� ���������� ����� ������������ ���������� �������
        int flag_HT;
		int flag_connect;
        typedef int (*ImportFunction1)(); //Import
		ImportFunction1 is_connect = (ImportFunction1)GetProcAddress(hinstLib,"is_connect");
		stream.str("");
        if(is_connect != NULL)
        {
			flag_connect = is_connect();//Receive a data to apply a picture

			stream << endl;
			sprintf(info, "\n");
			if (flag_connect)//Solution
				strcat(info, "������� �����������:    ����!\n\n");
				//stream << "������� �����������:    ����!\n\n";
            else
				//stream << "������� �����������:    ���!\n\n";
				strcat(info, "������� �����������:    ���!\n\n");
        }
		else 
		{
			stream_error.str("");
			stream_error << "������� is_connect �� ������� � " << LIBRARY_NAME;
            MessageBox(hwnd, (stream_error.str()).c_str(), "������", MB_OK | MB_ICONERROR);
		} 


        typedef int (*ImportFunction2)();
		ImportFunction2 HT = (ImportFunction2)GetProcAddress(hinstLib, "is_Hyper_Threading");
        if(HT != NULL)
        {
			flag_HT = HT();//Receive a data to apply a picture
            if(flag_HT)//Solution
				//stream << "���������� Hyper Threading:    ��������������!\n";
				strcat(info, "���������� Hyper Threading:    ��������������!\n");
            else
				//stream << "���������� Hyper Threading:    �� ��������������!\n";
				strcat(info, "���������� Hyper Threading:    �� ��������������!\n");

            //strcpy(info, (stream.str()).c_str());
        }
		else 
		{
			stream_error.str("");
			stream_error << "������� is_connect �� ������� � " << LIBRARY_NAME;
			MessageBox(hwnd, (stream_error.str()).c_str(), "������", MB_OK | MB_ICONERROR);
		}            
        FreeLibrary(hinstLib);// Free
    }
    else 
	{
		// ���� ����� � ������������ ����������� �����������
		stream_error.str("");
		stream_error << LIBRARY_NAME << "�� �������!" << endl;
		stream_error <<	"��������� ���� " << LIBRARY_NAME << " � ����� � ����������" << endl;
		stream_error << "� ��������� ��������� ��� ���.";
		MessageBox(hwnd, (stream_error.str()).c_str(), "������", MB_OK | MB_ICONERROR);
	}
	return 0;
}


LRESULT CALLBACK WindowFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) // Message processor
{
	HANDLE hThread;
	DWORD IDThread;
	PAINTSTRUCT ps;
	HDC hdc;
    switch (msg)
    {
		case WM_CREATE: 
		{
			hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
			break;
		}
		
		/*
		case WM_CTLCOLORSTATIC:// Text and background
		{
			DWORD CtrlID = GetDlgCtrlID((HWND)lParam);
			if (CtrlID == 1024)
			{
				SetTextColor((HDC)wParam, RGB(30, 200, 30));
				SetBkColor((HDC)wParam, RGB(0, 0, 0));
				return (INT_PTR)GetStockObject(BLACK_BRUSH);
			}
			break;
		}*/
		
		/*case WM_COMMAND:
		{
			if (LOWORD(wParam) == 1025)
			{
				HANDLE hThread;
				DWORD IDThread;
				hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, NULL, 0, &IDThread);
				CloseHandle(hThread);
			}
			if (LOWORD(wParam) == 1026)
				PostQuitMessage(0);
			break;
		}*/
		
		case WM_PAINT:
		{ 
			
			hdc = BeginPaint(hwnd, &ps);
			//TextOut(hdc, 10, 10, (stream.str()).c_str(), (stream.str()).length());
			TextOut(hdc, 0, 0, info, strlen(info));
			EndPaint(hwnd, &ps);
			/*if(fstatus != fstatus_old)
			{
				InvalidateRect(hwnd, 0, 0);
				UpdateWindow(hwnd);
				fstatus_old = fstatus;
			}*/
			break;
		} 
		case WM_DESTROY: 
		{
			PostQuitMessage(0);
			break;
		}
			
		default: 
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}	
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
	MSG msg;
	WNDCLASS wcl;

    /*RECT rt;
    HANDLE hThread;
    DWORD IDThread;
    HFONT font_std = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    HFONT font_mono = (HFONT)GetStockObject(OEM_FIXED_FONT);
    memset(&wcl, 0, sizeof(WNDCLASS));*/
	wcl.hInstance = hThisInst;
	wcl.lpszClassName = ClassName;
	wcl.lpfnWndProc = WindowFunc;
	wcl.style = CS_HREDRAW | CS_VREDRAW;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.lpszMenuName = NULL;
	wcl.cbWndExtra = 0;
	wcl.cbClsExtra = 0;
	wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    RegisterClass(&wcl);

    /*int window_width = 500;
    int window_height = 200;
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
    MSLabel = CreateWindow("static", "������� \"��������\" ��� �������� ", WS_CHILD | WS_VISIBLE | WS_BORDER,
        border, border, window_width - 2 * border, (window_height - button_height - 4 * border) / 2,
        hwnd, (HMENU)1024, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1024, WM_SETFONT, (WPARAM)font_mono, TRUE);
    hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
    CloseHandle(hThread);//Buttons below
    CreateWindow("btn_update", "��������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, window_width - 2 * (border + button_width), window_height - border - button_height, button_width, button_height, hwnd, (HMENU)1025, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1025, WM_SETFONT, (WPARAM)font_std, TRUE);
    CreateWindow("btn_exit", "�������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, window_width - border - button_width, window_height - border - button_height, button_width, button_height, hwnd, (HMENU)1026, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1026, WM_SETFONT, (WPARAM)font_std, TRUE);*/
	
	hwnd = CreateWindow(
		ClassName, // ��� ����
		Title,  // ����� � ��������� ����
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,  //  ����� ����
		50, // �������������� ������������ ����
		200,  // ������������ ������������ ����
		1500,  // ������ ����
		300,  // ������ ����
		HWND_DESKTOP, // ���������� (�������������) ���� ���������
		NULL, // ���������� ���� ��� ID ��������� ����
		hThisInst,  // ���������� ���������� ����������
		NULL);  // ��������� �� ������ �������� ����

    ShowWindow(hwnd, nWinMode);
    UpdateWindow(hwnd);
    
    while(GetMessage(&msg, NULL, 0, 0))//Loop
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
