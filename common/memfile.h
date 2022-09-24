#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef enum MEMFILE_RESULT_E
{
	MF_SUCCESS = 0,		  //ok
	MF_FAILED_TO_OPEN,	  //failed to open file
	MF_OUT_OF_MEMORY,	  //out of memory
	MF_CLAMPED,			  //file size was clamped to arch support length
	MF_RDERR,			  //read error
	MF_WRERR,			  //write error
	MF_EOF,				  //end of file
	MF_INVOP			  //invalid operation
} MEMFILE_RESULT;

typedef struct memfile_s {
	size_t size;
	size_t pos;
	size_t abytes;
	unsigned char *p_data;
} memfile_t;

#define mf_init(p) \
	(p)->size = 0; \
	(p)->pos = 0; \
	(p)->abytes = 0; \
	(p)->p_data = 0;

#define MFF_CLAMP_SIZE (1 << 1)
#define MFF_CLEAR_MEMORY (1 << 2)
#define MFF_DYNAMIC (1 << 3)

MEMFILE_RESULT mf_alloc_ex(memfile_t *p_dstmemfile, int flags, size_t start_size);
MEMFILE_RESULT mf_free_ex(memfile_t *p_dstmemfile, int flags);

MEMFILE_RESULT mf_file_to_memory_ex(memfile_t *p_dstmemfile, int flags, size_t additional_bytes, const char *p_filename);
MEMFILE_RESULT mf_file_to_memory_u_ex(memfile_t *p_dstmemfile, int flags, size_t additional_bytes, const wchar_t *p_filename); //TODO: unicode version unimplemented
MEMFILE_RESULT mf_memory_to_file(memfile_t *p_dstmemfile, int flags, const char *p_filename);
MEMFILE_RESULT mf_memory_to_file_u(memfile_t *p_dstmemfile, int flags, const wchar_t *p_filename); //TODO: unicode version unimplemented

MEMFILE_RESULT mf_write_byte(memfile_t *p_dstmemfile, int byte);
MEMFILE_RESULT mf_read_byte(int *p_dstbyte, memfile_t *p_dstmemfile);
MEMFILE_RESULT mf_write_short(memfile_t *p_dstmemfile, int wbyte);
MEMFILE_RESULT mf_read_short(int *p_dstbytes, memfile_t *p_dstmemfile);
MEMFILE_RESULT mf_write_int(memfile_t *p_dstmemfile, int qbyte);
MEMFILE_RESULT mf_read_int(int *p_dstbytes, memfile_t *p_dstmemfile);
MEMFILE_RESULT mf_write_float(memfile_t *p_dstmemfile, float value);
MEMFILE_RESULT mf_read_float(float *p_dstflt, memfile_t *p_dstmemfile);
MEMFILE_RESULT mf_write_string(memfile_t *p_dstmemfile, const char *p_string);
MEMFILE_RESULT mf_read_string(char *p_dststr, size_t maxlen, memfile_t *p_dstmemfile);
MEMFILE_RESULT mf_write_bytes(memfile_t *p_dstmemfile, void *p_data, size_t size);
MEMFILE_RESULT mf_read_bytes(void *p_data, size_t size, memfile_t *p_dstmemfile);