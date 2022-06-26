#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>

inline short compress_float(float flt)
{
	flt /= FLT_MAX;
	return *((short *)&flt);
}

inline float decompress_float(short fcval)
{
	float val = *((float *)&fcval);
	return (val * FLT_MAX);
}

short Encode(double value) {
	int cnt = 0;
	while (value != floor(value)) {
		value *= 10.0;
		cnt++;
	}
	return (short)((cnt << 12) + (int)value);
}

double Decode(short value) {
	int cnt = value >> 12;
	double result = value & 0xfff;
	while (cnt > 0) {
		result /= 10.0;
		cnt--;
	}
	return result;
}

int main()
{
	printf("Max value: %f\n", FLT_MAX);
	return 0;
}