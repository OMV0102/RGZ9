#include <windows.h>
#include <string>
#include <sstream>
#include <locale.h>
#define LIB_NAME "lib.dll"

using namespace std;

LPCSTR ClassName = "class_form"; // ��� ������ �����
LPCSTR Title = "������� �����������"; // ��� ���� (���������)

string text_net;
string text_HT;
string text_error;

HWND hwnd;

DWORD WINAPI ThreadFunc(void*)
{
	//setlocale(LC_ALL, "Russian");
	HINSTANCE hinstLib = LoadLibrary(TEXT(LIB_NAME));
	if (hinstLib != NULL)
	{
		typedef int(*ImportFunction1)();
		ImportFunction1 is_connect = (ImportFunction1)GetProcAddress(hinstLib, "is_connect");
		text_net = "������� �����������:   ";
		if (is_connect != NULL)
		{
			int flag_connect = is_connect();
			if (flag_connect == 1)
				text_net += "����.";
			else
				text_net += "���.";
		}
		else
		{
			text_error = "";
			text_error += "�� ������� ���������� ������� �������� �����������!";
			text_net += "�������������.";
			MessageBox(hwnd, text_error.c_str(), "������", MB_OK | MB_ICONERROR);
		}



		typedef int(*ImportFunction2)();
		ImportFunction2 is_HT = (ImportFunction2)GetProcAddress(hinstLib, "is_Hyper_Threading");
		text_HT = "���������� Hyper Threading:   ";
		if (is_HT != NULL)
		{
			int flag_HT = is_HT();

			if (flag_HT == 1)
				text_HT += "��������������.";
			else
				text_HT += "�� ��������������.";

		}
		else
		{
			text_error = "";
			text_error += "�� ������� ���������� ��������� ���������� Hyper Threading!";
			text_HT += "�������������.";
			MessageBox(hwnd, text_error.c_str(), "������", MB_OK | MB_ICONERROR);
		}

		FreeLibrary(hinstLib);
	}
	else
	{
		text_error = "";
		text_error += LIB_NAME; text_error += " �� ������!\n";
		text_error += "��������� ���� "; text_error += LIB_NAME; text_error += " � ����� � ����������\n";
		text_error += "� ��������� ��������� ��� ���.";
		MessageBox(hwnd, text_error.c_str(), "������", MB_OK | MB_ICONERROR);
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
	int width = 700;  // ������ ����
	int height = 300;  // ������ ����
	
	// �������� ������ ��� ������� ������� ThreadFunc
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	CloseHandle(hThread); // �������� ������

	hwnd = CreateWindow(
		ClassName,  // ��� ������ �����
		Title,   // ��� ���� (���������)
		WS_OVERLAPPED | // ����� ����: ������������� ����
		WS_CAPTION | // ����� ����: ������� ������� ���������
		WS_SYSMENU | // ����� ����: ������� ���� ���� � ������: ��������, �������
		WS_MINIMIZEBOX |// ����� ����: ������� ������ ��������
		WS_CLIPCHILDREN | // ����� ����: �� �������������� ������� ��� �������� ���������
		WS_CLIPSIBLINGS,  // ����� ����: �������������� ������������� ���� ����� �������� ���������
		200,  //  �������������� ������� ���� ������������ �������� �����
		200,  //  ������������ ������� ���� ������������ �������� �����
		width,  // ������ ����
		height,  // ������ ����
		HWND_DESKTOP,  // ���������� ������������� ���� (������� ����)
		NULL, // ���������� ��������� ���� ��� ���� - ���
		hInstance, // ���������� ����������� ����������
		NULL); // ��������� �� ������ �������� ����

	

	CreateWindow(
		"button",  // ��� ������
		"�������",  // �������
		WS_CHILD | // ����� ������: 
		WS_VISIBLE | // ����� ������: ������ ������
		BS_PUSHBUTTON, // ����� ������: ��� ������� ����� ��������� � WM_COMMAND
		10,  //  �������������� ������� ������������ ����
		50,  //  ������������ ������� ������������ ����
		70,  // ������ ������
		30,  // ������ ������
		hwnd,  // ���������� ������������� ���� (���� ���������)
		(HMENU)1026,  // ���������� ����
		hInstance, // ���������� ����������� ����������
		NULL); // ��������� �� ������ �������� ����

	//������������ ���� � ���������� �� ������
	ShowWindow(hwnd, SW_SHOW);

	//UpdateWindow(hwnd);
	// ���������� �������� ���������
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
