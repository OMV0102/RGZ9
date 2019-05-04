#include <windows.h>

// функци€ определ€юща€ наличие сетевого соединени€
extern "C" __declspec(dllexport) int is_connect() 
{
    int net = GetSystemMetrics(SM_NETWORK);
	__asm // Receive a first bit
	{
		mov edx, net
		and edx, 1 // A very simple bit mask
		mov net, edx
	}
	return net;
}

// функци€ определ€юща€ наличие Hyper Threating Technology
extern "C" __declspec(dllexport) int is_Hyper_Threading()
{
    int htt;
    __asm
    {
        mov eax, 1
        cpuid
        and edx, 10000000h // A bit mask
        shr edx, 28
        mov htt, edx
    }
    return htt;
}