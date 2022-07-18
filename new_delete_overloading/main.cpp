#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>

#define USE_OVERLOADING

#ifdef USE_OVERLOADING
void *operator new(size_t size)
{
	return malloc(size);
}

void operator delete(void *p_data)
{
	free(p_data);
}
#endif

struct message {
	int userid;
	char string[128];
};

int main()
{
	int number_of_elements;
	scanf("%d", &number_of_elements);
	
	message *p_msg;
	p_msg = new message[number_of_elements];
	for (int i = 0; i < number_of_elements; i++) {
		sprintf_s(p_msg[i].string, sizeof(p_msg[i].string), "User%d", i);
		p_msg[i].userid = i;
	}
	delete[] p_msg;


	return 0;
}