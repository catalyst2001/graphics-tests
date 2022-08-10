#pragma once
#include <stdint.h>

namespace rutl
{
	// 
	// Basic RGB color class
	// 
	class rcolor3
	{
		union {
			struct { uint8_t r, g, b; };
			struct { uint8_t cv[3]; };
		};
	public:
		rcolor3() : r(0), g(0), b(0) {}
		rcolor3(uint8_t __r, uint8_t __g, uint8_t __b) : r(__r), g(__g), b(__b) {}
		~rcolor3() {}

		uint8_t &operator[](uint8_t index) { return cv[index]; }
	};

	// 
	// Basic RGBA color class
	// 
	class rcolor4
	{
		union {
			struct { uint8_t r, g, b, a; };
			struct { uint8_t cv[4]; };
		};
	public:
		rcolor4() : r(0), g(0), b(0), a(0) {}
		rcolor4(uint8_t __r, uint8_t __g, uint8_t __b, uint8_t __a) : r(__r), g(__g), b(__b), a(__a) {}
		~rcolor4() {}

		uint8_t &operator[](uint8_t index) { return cv[index]; }
	};
};
