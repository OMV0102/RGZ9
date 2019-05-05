#include <windows.h>
#include <string>
#include <sstream>
#include <locale.h>
#define LIB_NAME "lib.dll" // ��� ����������

using namespace std;

LPCSTR ClassName = "class_window"; // ��� ������ �����
LPCSTR Title = "������� ����������� � Hyper Threading"; // ��� ���� (���������)

string text_net; // ������ � ����������� � ������� �����������
string text_HT; // ������ � ����������� � ��������� Hyper_Threading
string text_error; // ������ � ����������� � ��������� ������

HWND hwnd; // ���������� ����

// ������� ���������� �������, ������������ � ������������ ����������
DWORD WINAPI ThreadFunc(void*)
{
	// �������� ������������ ����������
	HINSTANCE library = LoadLibrary(TEXT(LIB_NAME));
	if (library != NULL)
	{
		// �������� ����� ������ 1, ���� ��, 0, ���� ���, -1, ���� �������� ������������� 
		int flag_connect; // ������� �����������
		int flag_HT; // ��������� HT

		text_error = "";

		typedef int(*ImportFunction1)();  // ��� ��������� �� �������1
		typedef int(*ImportFunction2)();// ��� ��������� �� �������2
		// �������� ��������, ������������ � ����������
		ImportFunction1 is_connect = (ImportFunction1)GetProcAddress(library, "is_connect");
		ImportFunction2 is_HT = (ImportFunction2)GetProcAddress(library, "is_Hyper_Threading");

		text_net = "������� �����������:   ";
		text_HT = "���������� Hyper Threading:   ";
		
		// ���� ����� ������� ������� (������� �����������)
		if (is_connect != NULL)
		{
			// �������� ������������ �������� �������
			// �� ������������ �������
			flag_connect = is_connect();
			// ���� �������� ����� �������
			// ������� ����������� ����
			// ����� ���
			if (flag_connect == 1)
				text_net += "����.";
			else
				text_net += "���.";
		}
		else // ���� ����� ������� �� �������
		{
			// ��������� ��������������� ����� ������
			flag_connect = -1;
			text_error += "�� ������� ���������� ������� �������� �����������!\n";
			// ���������� � ����������� �������������
			text_net += "�������������.";
		}

		// ���� ����� ������� ������� (��������� HT)
		if (is_HT != NULL)
		{
			// �������� ������������ �������� �������
			// �� ������������ �������
			flag_HT = is_HT();
			// ���� �������� ����� �������
			// ��������� ���������� ����
			// ����� ���
			if (flag_HT == 1)
				text_HT += "��������������.";
			else
				text_HT += "�� ��������������.";
		}
		else// ���� ����� ������� �� �������
		{
			// ��������� ��������������� ����� ������
			flag_HT = -1;
			text_error += "�� ������� ���������� ��������� ���������� Hyper Threading!";
			// ���������� � ��������� ������������
			text_HT += "�������������.";
		}

		// ���� �����-�� �� ������� �� ����������� �� ����������
		if (flag_connect == -1 || flag_HT == -1) 
		{
			// ������� ���������� ����, ���������� �� ������
			MessageBox(hwnd, text_error.c_str(), "������", MB_OK | MB_ICONERROR);
		}

		FreeLibrary(library); // ��������� ������������ ����������, ���������� ����������
	}
	else
	{
		text_error = "���� ";
		text_error += LIB_NAME; text_error += " �� ������!\n";
		text_error += "��������� ���� "; text_error += LIB_NAME; text_error += " � ����� � ����������\n";
		text_error += "� ��������� ��������� ��� ���.";
		MessageBox(hwnd, text_error.c_str(), "������", MB_OK | MB_ICONERROR);
	}
	return 0;
}

// ������� �������������� ���������
LRESULT CALLBACK WindowFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	PAINTSTRUCT ps; // ��������� ��� ���������
	HDC hdc; // �������� ����������
	switch (msg)
	{
		case WM_COMMAND:
		{
			// ������������ ������� ������ "�������"
			PostQuitMessage(0);  // ��������� ���������
			break;
		}

		case WM_PAINT:
		{
			// ��������� ������ � ����
			hdc = BeginPaint(hwnd, &ps);
			TextOut(hdc, 10, 10, text_net.c_str(), text_net.length());
			TextOut(hdc, 10, 30, text_HT.c_str(), text_HT.length());
			EndPaint(hwnd, &ps);
			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);  // �������� ���������
			break;
		}

		// ��������� ��������� �� ���������
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
	MSG msg;
	WNDCLASS wcl;
	HANDLE hThread; // ���������� ������
	DWORD IDThread; // ������������� ������

	// �������� ������ ��� ������� ������� ThreadFunc
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	WaitForSingleObject(hThread, INFINITE);  // �������� ���������� ������
	CloseHandle(hThread); // �������� ����������� ������

	wcl.hInstance = hThisInst; // ���������� ����������, ������� �������� ������� ��������� ��� ������
	wcl.lpszClassName = ClassName; // ��� ������ ����
	wcl.lpfnWndProc = WindowFunc; // ��������� �� ������� ���������
	wcl.style = CS_HREDRAW | CS_VREDRAW; // ����������� ����� ���� ��� ��������� ��������
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // ������ ��������� �� ���������
	wcl.hCursor = LoadCursor(hThisInst, IDC_ARROW); // ������� ������ ����
	wcl.lpszMenuName = NULL;
	wcl.cbWndExtra = 0; // ����� ���. ����
	wcl.cbClsExtra = 0; // ����� ���. ����
	wcl.hbrBackground = CreateSolidBrush(RGB(255, 255, 255)); // ���� ���� ���� 

	// ������������ ����� ���� ��� ������������ ������������� ��� ������ ������� CreateWindow
	RegisterClass(&wcl);
	
	// �������� ����
	hwnd = CreateWindow(
		ClassName,  // ��� ������ ����
		Title,   // ��� ���� (���������)
		WS_OVERLAPPED | // ����� ����: ������������� ����
		WS_CAPTION | // ����� ����: ������� ������� ���������
		WS_SYSMENU | // ����� ����: ������� ���� ���� � ������: ��������, �������
		WS_MINIMIZEBOX |// ����� ����: ������� ������ ��������
		WS_CLIPCHILDREN | // ����� ����: �� �������������� ������� ��� �������� ���������
		WS_CLIPSIBLINGS,  // ����� ����: �������������� ������������� ���� ����� �������� ���������
		600,  //  �������������� ������� ���� ������������ �������� �����
		400,  //  ������������ ������� ���� ������������ �������� �����
		380,  // ������ ����
		150,  // ������ ����
		HWND_DESKTOP,  // ���������� ������������� ���� (������� ����)
		NULL, // ���������� ��������� ���� ��� ����
		hThisInst, // ���������� ����������� ����������
		NULL); // ��������� �� ������ �������� ����

	//�������� ������
	CreateWindow(
		"button",  // ��� ������
		"�������",  // �������
		WS_CHILD | // ����� ������: 
		WS_VISIBLE | // ����� ������: ������ ������
		BS_PUSHBUTTON, // ����� ������: ��� ������� ����� ��������� � WM_COMMAND
		150,  //  �������������� ������� ������������ ����
		65,  //  ������������ ������� ������������ ����
		70,  // ������ ������
		30,  // ������ ������
		hwnd,  // ���������� ������������� ���� (���� ���������)
		NULL, // ���������� ��������� ���� ��� ����
		hThisInst, // ���������� ����������� ����������
		NULL); // ��������� �� ������ �������� ����

	//������������ ���� � ���������� �� ������
	ShowWindow(hwnd, SW_SHOW);

	// ���������� �������� ���������
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
