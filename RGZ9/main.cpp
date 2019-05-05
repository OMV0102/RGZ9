#include <windows.h>
#include <string>
#include <sstream>
#include <locale.h>
#define LIB_NAME "lib.dll"

using namespace std;

LPCSTR ClassName = "class_form"; // имя класса формы
LPCSTR Title = "Сетевое подключение"; // имя окна (заголовок)

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
	MSG msg;
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
	int width = 700;  // ширина окна
	int height = 300;  // высота окна
	
	// Создание потока для запуска функции ThreadFunc
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	CloseHandle(hThread); // закрытие потока

	hwnd = CreateWindow(
		ClassName,  // имя класса формы
		Title,   // имя окна (заголовок)
		WS_OVERLAPPED | // стиль окна: перекрывающее окно
		WS_CAPTION | // стиль окна: наличие области заголовка
		WS_SYSMENU | // стиль окна: наличие меню окна и кнопок: свернуть, закрыть
		WS_MINIMIZEBOX |// стиль окна: наличие кнопки свернуть
		WS_CLIPCHILDREN | // стиль окна: не прорисовыевает области для дочерних элементов
		WS_CLIPSIBLINGS,  // стиль окна: антипрорисовка перекрывающих друг друга дочерних элементов
		200,  //  горизонтальная позиция окна относительно рабочеко стола
		200,  //  вертикальная позиция окна относительно рабочеко стола
		width,  // ширина окна
		height,  // высота окна
		HWND_DESKTOP,  // дескриптор родительского окна (рабочий стол)
		NULL, // дескриптор дочернего окна или меню - нет
		hInstance, // дескриптор эксземпляра приложения
		NULL); // указатель на данные создания окна

	

	CreateWindow(
		"button",  // имя класса
		"Закрыть",  // надпись
		WS_CHILD | // стиль кнопки: 
		WS_VISIBLE | // стиль кнопки: кнопка видима
		BS_PUSHBUTTON, // стиль кнопки: при нажатие будет обработка в WM_COMMAND
		10,  //  горизонтальная позиция относительно окна
		50,  //  вертикальная позиция относительно окна
		70,  // ширина кнопки
		30,  // высота кнопки
		hwnd,  // дескриптор родительского окна (окно программы)
		(HMENU)1026,  // дескриптор меню
		hInstance, // дескриптор эксземпляра приложения
		NULL); // указатель на данные создания окна

	//Активизирует окно и показывает на экране
	ShowWindow(hwnd, SW_SHOW);

	//UpdateWindow(hwnd);
	// постоянное ожидание сообщений
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
