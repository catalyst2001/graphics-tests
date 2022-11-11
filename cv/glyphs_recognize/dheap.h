#pragma once
#include <malloc.h>
#include <assert.h>
#define NULL 0

//04.12.2021 0:49
//автоматически перераспределяемая куча

//объявление структуры где нибудь
#define DHEAP_DECLARE(type, name)\
	struct {\
		type *ptr;\
		int capacity;\
		int offset;\
		int reserve;\
	} name;// = { NULL, 0, 0, 1 };

//инициализировать (выделить память, указать начальный размер и сколько байт нужно добавлять при перераспределении)
#define DHEAP_INIT(type, name, _startsize, _reserve) \
	{\
		(name)->reserve = _reserve;\
		(name)->offset = 0;\
		(name)->capacity = _startsize;\
		(name)->ptr = (type *)malloc(sizeof(type) * (name)->capacity);\
		assert((name)->ptr);\
	}

#define DHEAP_SET(name, index, element) ((name)->ptr[index] = element) //записать элемент в массив
#define DHEAP_GET(name, index) ((name)->ptr[index]) //получить элемент в массиве

//записать в конец
#define DHEAP_PUSH(type, name, element)\
    {\
		if(!(name)->ptr)\
			DHEAP_INIT(type, name, 1, 1);\
			                                        \
		if(((name)->offset + 1) == (name)->capacity) {\
			(name)->capacity += (name)->reserve;\
			(name)->ptr = (type *)realloc((name)->ptr, sizeof(type) * (name)->capacity);\
			assert((name)->ptr);\
		}\
		(name)->ptr[(name)->offset] = element;\
		(name)->offset++;\
	}

#define DHEAP_POP(name) ((name)->ptr[--name.offset]) //достать последний элемент
#define DHEAP_OFFSET(name) ((name)->offset) //получить смещение относительно адреса начала
#define DHEAP_CAPACITY(name) ((name)->capacity) //получить размер
#define DHEAP_DATA(name) ((name)->ptr) //получить данные
#define DHEAP_CLEAR(name) (name)->offset = 0;

#define DHEAP_FREE(name) free((name)->ptr) //освободить память


////////////////////
//  STACK
////////////////////
#define STACK_FIXED_DECLARE(type, name, size) \
	struct { \
		int position; \
		type stack[size]; \
	} name;

#define STACK_FIXED_SET_POS(name, pos) (name)->position = pos;
#define STACK_FIXED_PUSH(name, data) (name)->stack[(name)->position++] = data;
#define STACK_FIXED_POP(name) (name)->stack[(name)->position--];