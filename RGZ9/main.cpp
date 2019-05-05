#include <windows.h>
#include <string>
#include <sstream>
#include <locale.h>
#define LIB_NAME "lib.dll" // имя библиотеки

using namespace std;

LPCSTR ClassName = "class_window"; // имя класса формы
LPCSTR Title = "Сетевое подключение и Hyper Threading"; // имя окна (заголовок)

string text_net; // строка с информацией о сетевом подключении
string text_HT; // строка с информацией о поддержке Hyper_Threading
string text_error; // строка с информацией о возникшей ошибке

HWND hwnd; // дескрипотр окна

// функция вызывающая функции, определенные в динамической библиотеке
DWORD WINAPI ThreadFunc(void*)
{
	// загрузка динамической библиотеки
	HINSTANCE library = LoadLibrary(TEXT(LIB_NAME));
	if (library != NULL)
	{
		// числовые флаги равные 1, если ДА, 0, если НЕТ, -1, если значение неопределенно 
		int flag_connect; // наличие подключения
		int flag_HT; // поддрежка HT

		text_error = "";

		typedef int(*ImportFunction1)();  // тип указателя на функцию1
		typedef int(*ImportFunction2)();// тип указателя на функцию2
		// загрузка функциий, определенных в библиотеке
		ImportFunction1 is_connect = (ImportFunction1)GetProcAddress(library, "is_connect");
		ImportFunction2 is_HT = (ImportFunction2)GetProcAddress(library, "is_Hyper_Threading");

		text_net = "Сетевое подключение:   ";
		text_HT = "Технология Hyper Threading:   ";
		
		// Если адрес функции получен (сетевое подключение)
		if (is_connect != NULL)
		{
			// получаем возвращаемое значение функции
			// из ассемблерной вставки
			flag_connect = is_connect();
			// Если значение равно единицу
			// сетевое подключение есть
			// иначе нет
			if (flag_connect == 1)
				text_net += "ЕСТЬ.";
			else
				text_net += "НЕТ.";
		}
		else // Если адрес функции НЕ получен
		{
			// формируем соответствующий текст ошибки
			flag_connect = -1;
			text_error += "Не удалось определить наличие сетевого подключения!\n";
			// информация о подключении неопределенна
			text_net += "неопределенно.";
		}

		// Если адрес функции получен (поддержка HT)
		if (is_HT != NULL)
		{
			// получаем возвращаемое значение функции
			// из ассемблерной вставки
			flag_HT = is_HT();
			// Если значение равно единицу
			// поддрежка технологии есть
			// иначе нет
			if (flag_HT == 1)
				text_HT += "поддерживается.";
			else
				text_HT += "НЕ поддерживается.";
		}
		else// Если адрес функции НЕ получен
		{
			// формируем соответствующий текст ошибки
			flag_HT = -1;
			text_error += "Не удалось определить поддержку технологии Hyper Threading!";
			// информация о поддержке неопределена
			text_HT += "неопределенно.";
		}

		// если какая-то из функций не загрузилась из библиотеки
		if (flag_connect == -1 || flag_HT == -1) 
		{
			// выводим диалоговое окно, сообщающее об ошибке
			MessageBox(hwnd, text_error.c_str(), "Ошибка", MB_OK | MB_ICONERROR);
		}

		FreeLibrary(library); // закрываем динамическую библиотеку, освобождая дескриптор
	}
	else
	{
		text_error = "Файл ";
		text_error += LIB_NAME; text_error += " не найден!\n";
		text_error += "Поместите файл "; text_error += LIB_NAME; text_error += " в папку с программой\n";
		text_error += "и запустите программу еще раз.";
		MessageBox(hwnd, text_error.c_str(), "Ошибка", MB_OK | MB_ICONERROR);
	}
	return 0;
}

// функция обрабатывающая сообщения
LRESULT CALLBACK WindowFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	PAINTSTRUCT ps; // структура для рисования
	HDC hdc; // контекст устройства
	switch (msg)
	{
		case WM_COMMAND:
		{
			// обрабатывает нажатие кнопки "Закрыть"
			PostQuitMessage(0);  // закрываем программу
			break;
		}

		case WM_PAINT:
		{
			// отрисовка текста в окне
			hdc = BeginPaint(hwnd, &ps);
			TextOut(hdc, 10, 10, text_net.c_str(), text_net.length());
			TextOut(hdc, 10, 30, text_HT.c_str(), text_HT.length());
			EndPaint(hwnd, &ps);
			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);  // закрытие программы
			break;
		}

		// обработка сообщений по умолчанию
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
	MSG msg;
	WNDCLASS wcl;
	HANDLE hThread; // дескриптор потока
	DWORD IDThread; // идентификатор потока

	// Создание потока для запуска функции ThreadFunc
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	WaitForSingleObject(hThread, INFINITE);  // ожидание завершения потока
	CloseHandle(hThread); // удаление дескриптора потока

	wcl.hInstance = hThisInst; // дескриптор экземпляра, который содержит оконную процедуру для класса
	wcl.lpszClassName = ClassName; // имя класса окна
	wcl.lpfnWndProc = WindowFunc; // указатель на оконную процедуру
	wcl.style = CS_HREDRAW | CS_VREDRAW; // перересовка всего окна при изменении размеров
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // иконка программы по умолчанию
	wcl.hCursor = LoadCursor(hThisInst, IDC_ARROW); // обычный курсор мыши
	wcl.lpszMenuName = NULL;
	wcl.cbWndExtra = 0; // число доп. байт
	wcl.cbClsExtra = 0; // число доп. байт
	wcl.hbrBackground = CreateSolidBrush(RGB(255, 255, 255)); // цвет фона окна 

	// регистрируем класс окна для последующего использования при вызове функции CreateWindow
	RegisterClass(&wcl);
	
	// создание окна
	hwnd = CreateWindow(
		ClassName,  // имя класса окна
		Title,   // имя окна (заголовок)
		WS_OVERLAPPED | // стиль окна: перекрывающее окно
		WS_CAPTION | // стиль окна: наличие области заголовка
		WS_SYSMENU | // стиль окна: наличие меню окна и кнопок: свернуть, закрыть
		WS_MINIMIZEBOX |// стиль окна: наличие кнопки свернуть
		WS_CLIPCHILDREN | // стиль окна: не прорисовыевает области для дочерних элементов
		WS_CLIPSIBLINGS,  // стиль окна: антипрорисовка перекрывающих друг друга дочерних элементов
		600,  //  горизонтальная позиция окна относительно рабочего стола
		400,  //  вертикальная позиция окна относительно рабочего стола
		380,  // ширина окна
		150,  // высота окна
		HWND_DESKTOP,  // дескриптор родительского окна (рабочий стол)
		NULL, // дескриптор дочернего окна или меню
		hThisInst, // дескриптор эксземпляра приложения
		NULL); // указатель на данные создания окна

	//создание кнопки
	CreateWindow(
		"button",  // имя класса
		"Закрыть",  // надпись
		WS_CHILD | // стиль кнопки: 
		WS_VISIBLE | // стиль кнопки: кнопка видима
		BS_PUSHBUTTON, // стиль кнопки: при нажатие будет обработка в WM_COMMAND
		150,  //  горизонтальная позиция относительно окна
		65,  //  вертикальная позиция относительно окна
		70,  // ширина кнопки
		30,  // высота кнопки
		hwnd,  // дескриптор родительского окна (окно программы)
		NULL, // дескриптор дочернего окна или меню
		hThisInst, // дескриптор эксземпляра приложения
		NULL); // указатель на данные создания окна

	//Активизирует окно и показывает на экране
	ShowWindow(hwnd, SW_SHOW);

	// постоянное ожидание сообщений
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
