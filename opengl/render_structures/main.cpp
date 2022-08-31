#include <stdio.h>

int main()
{
	__asm {
		push eax
		push ebx
		push ecx

		push 1
		call __acrt_iob_func
		pop ecx

		push ecx
		push 'o'
		call putc
		pop eax

		pop ecx
		pop ebx
		pop eax
	};
	return 0;
}