#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <Windows.h>

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

short Encode(float value) {
	int cnt = 0;
	while (value != floor(value)) {
		value *= 10.0;
		cnt++;
	}
	return (short)((cnt << 12) + (short)value);
}

float Decode(short value) {
	int cnt = value >> 12;
	float result = value & 0xfff;
	while (cnt > 0) {
		result /= 10.0;
		cnt--;
	}
	return result;
}

#include <intrin.h>
#include <string.h>

// Get processor brand string
// This seems to be working for both Intel & AMD vendors
void get_cpu_brand_string(char *p_dst_brand_string, size_t maxlen)
{
	int data[4];
	int j = 0;
	p_dst_brand_string[maxlen - 1] = '\0';
	for (int i = 0x80000002; i < 0x80000005; i++, j += 4) {
		__cpuid(data, i);
		((int *)p_dst_brand_string)[j + 0] = data[0];
		((int *)p_dst_brand_string)[j + 1] = data[1];
		((int *)p_dst_brand_string)[j + 2] = data[2];
		((int *)p_dst_brand_string)[j + 3] = data[3];
	}
}

struct serial_id
{
	struct {
		long long part1;
		long long part2;
		long long part3;
		long long part4;
	} disk_information;

	struct {
		long long part1;
		long long part2;
		long long part3;
		long long part4;
	} cpuinformation;

	struct {
		long long part1;
		long long part2;
	} memoryinformation;
};

#define GOVNO(x)
GOVNO(sizeof(struct serial_id))

// 
// generate_serial
// 
// |  Serial
// |  [0 Byte]	  ( Disk serial	  )
// |  [8 Byte]	  ( Disk serial	  )
// |  [16 byte]	  ( Disk serial	  )
// |  [32 byte]   ( Disk revision )
// |  
// 
// 32 -- 64 bytes Cpu brand string
// 64 -- 66 dwProcessorType
// 66 -- 68 dwNumberOfProcessors
// 68 -- 70 wProcessorArchitecture
// 
// 
// 
// 
#define SERIAL_XOR_MAGIC 0xC

void generate_serial(struct serial_id *p_dstdata)
{
	memset(p_dstdata, 0, sizeof(*p_dstdata));
	
	// query hard disk information
	HANDLE h_dev = CreateFileA("\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (h_dev) {
		STORAGE_PROPERTY_QUERY storage_property_query;
		STORAGE_DESCRIPTOR_HEADER storage_desc_header;
		DWORD dwBytesReturned = 0;
		storage_property_query.PropertyId = StorageDeviceProperty,
		storage_property_query.QueryType = PropertyStandardQuery;
		if (DeviceIoControl(h_dev, IOCTL_STORAGE_QUERY_PROPERTY, &storage_property_query, sizeof(STORAGE_PROPERTY_QUERY), &storage_desc_header, sizeof(STORAGE_DESCRIPTOR_HEADER), &dwBytesReturned, NULL)) {
			const DWORD dwOutBufferSize = storage_desc_header.Size;
			PBYTE pBuffer = (PBYTE)calloc(dwOutBufferSize, sizeof(BYTE));
			if (DeviceIoControl(h_dev, IOCTL_STORAGE_QUERY_PROPERTY, &storage_property_query, sizeof(STORAGE_PROPERTY_QUERY), pBuffer, dwOutBufferSize, &dwBytesReturned, NULL)) {
				STORAGE_DEVICE_DESCRIPTOR* p_dev_desc = (STORAGE_DEVICE_DESCRIPTOR*)pBuffer;
				PBYTE p_serial = (PBYTE)pBuffer + p_dev_desc->SerialNumberOffset;
				PBYTE p_rev = (PBYTE)pBuffer + p_dev_desc->ProductRevisionOffset;
				p_dstdata->disk_information.part1 = ((long long *)p_serial)[0];
				p_dstdata->disk_information.part2 = ((long long *)p_serial)[1];
				p_dstdata->disk_information.part3 = ((long long *)p_serial)[2];
				p_dstdata->disk_information.part4 = ((long long *)p_rev)[2];
				free(pBuffer);
			}
		}
		CloseHandle(h_dev);
	}

	// query cpu information
	char brand_string[64];
	get_cpu_brand_string(brand_string, sizeof(brand_string));
	p_dstdata->cpuinformation.part1 = ((long long *)brand_string)[0];
	p_dstdata->cpuinformation.part2 = ((long long *)brand_string)[1];
	p_dstdata->cpuinformation.part3 = ((long long *)brand_string)[2];
	p_dstdata->cpuinformation.part4 = ((long long *)brand_string)[3];

	// memory information
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);

	PBYTE p_data;
	p_data = (PBYTE)&p_dstdata->memoryinformation.part1;
	((short *)p_data)[0] = (short)system_info.dwProcessorType; //0 - 2
	((short *)p_data)[1] = (short)system_info.dwNumberOfProcessors; //2 - 4
	((short *)p_data)[2] = (short)system_info.wProcessorArchitecture; //4 - 6
	((short *)p_data)[3] = (short)0; //6 - 8 //UNUSED

	p_data = (PBYTE)p_dstdata;
	for (size_t i = 0; i < sizeof(struct serial_id); i++)
		p_data[i] ^= SERIAL_XOR_MAGIC;
}

void print_hex(void *p_data, size_t size)
{
	printf("---------\n");
	unsigned char *p_udata = (unsigned char *)p_data;
	for (size_t i = 0; i < size; i++) {
		printf("%X", p_udata[i]);
	}
	printf("\n---------\n");
}

void print_ascii(void *p_data, size_t size)
{
	printf("---------\n");
	unsigned char *p_udata = (unsigned char *)p_data;
	for (size_t i = 0; i < size; i++) {
		printf("%c", p_udata[i]);
	}
	printf("\n---------\n");
}

int main()
{
	//eax
	//ebx
	//ecx
	//edx
	int data[4];
	__cpuid(data, 10);

	//char text[32];
	//memset(text, 0, sizeof(text));
	//(*(int *)&text[0]) = data[1];
	//(*(int *)&text[4]) = data[3];
	//(*(int *)&text[8]) = data[2];
	//printf("%s\n", text);

	struct serial_id serial;
	generate_serial(&serial);
	printf("%s\n", (char *)&serial);
	print_hex(&serial, sizeof(serial));
	print_ascii(&serial, sizeof(serial));

	return 0;
}