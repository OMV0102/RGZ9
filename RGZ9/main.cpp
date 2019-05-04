#include <windows.h>
#include <string>
#include <sstream>
#include <stdio.h>

#define LIBRARY_NAME "lib.dll"

LPCSTR ClassName = "main_class";
LPCSTR Title = "Сетевое подключение";

using namespace std;
string text;
string string_error;
HWND hwnd;
HWND hwnd_label;
//int fstatus = 0;
//int fstatus_old = -1;

DWORD WINAPI ThreadFunc(void*) // A function of access to dynamic library
{
    HINSTANCE hinstLib = LoadLibrary(TEXT(LIBRARY_NAME)); // Загружаем динамическию библиотеку и получаем ее дескриптор
    if(hinstLib != NULL) // если дескриптор файла динамической библиоткеи получен
    {
		// Если дескриптор файла динамической библиотеки получен
        int flag_HT;
		int flag_connect;
        typedef int (*ImportFunction1)(); //Import
		ImportFunction1 is_connect = (ImportFunction1)GetProcAddress(hinstLib,"is_connect");
		
        if(is_connect != NULL)
        {
			flag_connect = is_connect();//Receive a data to apply a picture
										
			text = "\n";
			if (flag_connect)//Solution
				text += "Сетевое подключение:    Есть!\n\n";
            else
				text += "Сетевое подключение:    Нет!\n\n";
        }
		else 
		{
			string_error = "";
			string_error += "Функция is_connect не найдена в "; string_error += LIBRARY_NAME;
            MessageBox(hwnd, string_error.c_str(), "Ошибка", MB_OK | MB_ICONERROR);
		} 


        typedef int (*ImportFunction2)();
		ImportFunction2 HT = (ImportFunction2)GetProcAddress(hinstLib, "is_Hyper_Threading");
        if(HT != NULL)
        {
			flag_HT = HT();
            if(flag_HT)
				text += "Технология Hyper Threading:    поддерживается!\n";
            else
				text += "Технология Hyper Threading:    НЕ поддерживается!\n";
			SetWindowText(hwnd_label, text.c_str());
            //strcpy(info, (stream.str()).c_str());
        }
		else 
		{
			string_error = "";
			string_error += "Функция is_Hyper_Threading не найдена в "; string_error += LIBRARY_NAME;
			MessageBox(hwnd, string_error.c_str(), "Ошибка", MB_OK | MB_ICONERROR);
		}            
        FreeLibrary(hinstLib);// Free
    }
    else 
	{
		// Если файла с динамической библиотекой отсутствует
		string_error = "";
		string_error += LIBRARY_NAME; string_error += " не найдена!\n";
		string_error += "Поместите файл "; string_error += LIBRARY_NAME; string_error += " в папку с программой\n";
		string_error += "и запустите программу еще раз.";
		MessageBox(hwnd, string_error.c_str(), "Ошибка", MB_OK | MB_ICONERROR);
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
			//TextOut(hdc, 15, 15, info, strlen(info));
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
			
		case WM_CTLCOLORSTATIC:
		{
			DWORD CtrlID = GetDlgCtrlID((HWND)lParam);
			if (CtrlID == 1024)
			{
				SetTextColor((HDC)wParam, RGB(30, 200, 30));
				SetBkColor((HDC)wParam, RGB(0, 0, 0));
				return (INT_PTR)GetStockObject(BLACK_BRUSH);
			}
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
    MSLabel = CreateWindow("static", "Нажмите \"Обновить\" для загрузки ", WS_CHILD | WS_VISIBLE | WS_BORDER,
        border, border, window_width - 2 * border, (window_height - button_height - 4 * border) / 2,
        hwnd, (HMENU)1024, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1024, WM_SETFONT, (WPARAM)font_mono, TRUE);
    hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
    CloseHandle(hThread);//Buttons below
    CreateWindow("btn_update", "Обновить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, window_width - 2 * (border + button_width), window_height - border - button_height, button_width, button_height, hwnd, (HMENU)1025, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1025, WM_SETFONT, (WPARAM)font_std, TRUE);
    CreateWindow("btn_exit", "Закрыть", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, window_width - border - button_width, window_height - border - button_height, button_width, button_height, hwnd, (HMENU)1026, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1026, WM_SETFONT, (WPARAM)font_std, TRUE);*/
	
	int win_width = 500;  // Ширина окна
	
	int label_X = 5; // Горизонтальное расположение label
	int label_Y = 5;  // Вертикальное расположение label
	int label_width = win_width - label_X * 2;  // Ширина label
	int label_height = 100;  // Высота label
	
	int win_height = label_height + 200;  // Высота окна
	
	hwnd = CreateWindow(
		ClassName, // Имя окна
		Title,  // Текст в заголовке окна
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,  //  Стиль окна
		50, // Горизонтальное расположение окна
		200,  // Вертикальное расположение окна
		win_width,  // Ширина окна
		win_height,  // Высота окна
		HWND_DESKTOP, // Дескриптор (родительского) окна владельца
		NULL, // Дескриптор меню или ID дочернего окна
		hThisInst,  // Дескриптор экземпляра приложения
		NULL);  // Указатель на данные создания окна



	hwnd_label = CreateWindow(
		"label",
		"hhhhhhhhhhhhh",
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		label_X,  // Горизонтальное расположение label
		label_Y,  // Вертикальное расположение label
		label_width, // Ширина label
		label_height,  // Высота label
		hwnd,
		(HMENU)1024,
		hThisInst,
		NULL);
	HFONT font_mono = (HFONT)GetStockObject(OEM_FIXED_FONT);
	SendDlgItemMessage(hwnd, 1024, WM_SETFONT, (WPARAM)font_mono, TRUE);
    ShowWindow(hwnd, nWinMode);
    UpdateWindow(hwnd);
    
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
