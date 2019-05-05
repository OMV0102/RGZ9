#include <windows.h>

// функци¤ определ¤юща¤ наличие сетевого соединени¤
extern "C" __declspec(dllexport) int is_connect() 
{
    int net = GetSystemMetrics(SM_NETWORK);
	__asm
	{
		MOV EAX, net   // помещаем значение в регистр EAX
		AND EAX, 00000001h
		MOV net, EAX
	}
	return net;
}

// функци¤ определ¤юща¤ наличие технологии Hyper Threating
extern "C" __declspec(dllexport) int is_Hyper_Threading()
{
    int HT;
    __asm
    {
        mov EAX, 1 // выполн¤ем EAX =1
        cpuid
        and EDX, 10000000h // A bit mask
        shr EDX, 28
        mov HT, EDX
    }
    return HT;
}