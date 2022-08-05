#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* typedefs */
typedef signed char int8;
typedef signed char i8;
typedef unsigned char uint8;
typedef unsigned char ui8;
typedef signed short int16;
typedef signed short i16;
typedef unsigned short uint16;
typedef unsigned short ui16;
typedef signed int int32;
typedef signed int i32;
typedef unsigned int uint32;
typedef unsigned int ui32;
typedef unsigned long ulong;
typedef unsigned long ul;
typedef unsigned long long ulonglong;
typedef unsigned long long ull;
typedef unsigned long long uint64;
typedef unsigned long long ui64;
typedef signed long long int64;
typedef signed long long i64;
typedef float f32;
typedef double f64;
typedef void *pvoid;
typedef void *handle;

#define ODEF_OK 0
#define ODEF_IGNORE 1

#ifdef _DEBUG
int __odef_dbgmessage() {
	return 0;
}

#else


#endif