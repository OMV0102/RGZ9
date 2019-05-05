#include <windows.h>
#include <string>
#include <sstream>
#include <locale.h>
#define LIB_NAME "lib.dll"

using namespace std;

LPCSTR ClassName = "class_form";
LPCSTR Title = "Сетевое подключение";

string text_net;
string text_HT;
string text_error;

HWND hwnd;

DWORD WINAPI ThreadFunc(void*)
{
	setlocale(LC_ALL, "Russian");
	HINSTANCE hinstLib = LoadLibrary(TEXT(LIB_NAME)); 
	if (hinstLib != NULL) 
	{
		typedef int(*ImportFunction1)();
		ImportFunction1 is_connect = (ImportFunction1)GetProcAddress(hinstLib, "is_connect");
		text_net = "Сетевое подключение:   ";
		if (is_connect != NULL)
		{
			int flag_connect = is_connect();
			if (flag_connect == 1)
				text_net += "ЕСТЬ.";
			else
				text_net += "НЕТ.";
		}
		else 
		{
			text_error = "";
			text_error += "Не удалось определить наличие сетевого подключения!";
			text_net += "неопределенно.";
			MessageBox(hwnd, text_error.c_str(), "Ошибка", MB_OK | MB_ICONERROR);
		}

			

		typedef int(*ImportFunction2)();
		ImportFunction2 is_HT = (ImportFunction2)GetProcAddress(hinstLib, "is_Hyper_Threading");
		text_HT = "Технология Hyper Threading:   ";
		if (is_HT != NULL)
		{
			int flag_HT = is_HT();

			if (flag_HT == 1)
				text_HT += "поддерживается.";
			else
				text_HT += "НЕ поддерживается.";

		}
		else
		{
			text_error = "";
			text_error += "Не удалось определить поддержку технологии Hyper Threading!";
			text_HT += "неопределенно.";
			MessageBox(hwnd, text_error.c_str(), "Ошибка", MB_OK | MB_ICONERROR);
		}
			
		FreeLibrary(hinstLib);
	}
	else 
	{
		text_error = "";
		text_error += LIB_NAME; text_error += " не найден!\n";
		text_error += "Поместите файл "; text_error += LIB_NAME; text_error += " в папку с программой\n";
		text_error += "и запустите программу еще раз.";
		MessageBox(hwnd, text_error.c_str(), "Ошибка", MB_OK | MB_ICONERROR);
	} 
	return 0;
}

LRESULT CALLBACK WindowFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) // Message processor
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg)
	{
		case WM_COMMAND:
		{
			if (LOWORD(wParam) == 1025)
			{
				HANDLE hThread;
				DWORD IDThread;
				hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
				CloseHandle(hThread);
				UpdateWindow(hwnd);

			}
			if (LOWORD(wParam) == 1026)
				PostQuitMessage(0);
			break;
		}
		
		case WM_PAINT: 
		{ 
			hdc = BeginPaint(hwnd, &ps);
			TextOut(hdc, 10, 120, text_net.c_str(), text_net.length());
			TextOut(hdc, 10, 140, text_HT.c_str(), text_HT.length());
			EndPaint(hwnd, &ps);
			break;
		}
					   
		case WM_DESTROY: 
		{
			PostQuitMessage(0);
			break;
		}
			
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
		return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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
	int window_width = 700;
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

	HWND label = CreateWindow("static", "Click \"Run\" to get values", WS_CHILD | WS_VISIBLE | WS_BORDER,
		border, border, window_width - 2 * border, (window_height - button_height - 4 * border) / 2,
		hwnd, (HMENU)1024, hInstance, NULL);

	SendDlgItemMessage(hwnd, 1024, WM_SETFONT, (WPARAM)font_mono, TRUE);
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	CloseHandle(hThread);//Buttons below
	CreateWindow("button", "Run", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, window_width - 2 * (border + button_width), window_height - border - button_height, button_width, button_height, hwnd, (HMENU)1025, hInstance, NULL);
	SendDlgItemMessage(hwnd, 1025, WM_SETFONT, (WPARAM)font_std, TRUE);
	CreateWindow("button", "Закрыть", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, window_width - border - button_width, window_height - border - button_height, button_width, button_height, hwnd, (HMENU)1026, hInstance, NULL);
	SendDlgItemMessage(hwnd, 1026, WM_SETFONT, (WPARAM)font_std, TRUE);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))//Loop
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
