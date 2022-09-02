#include "memfile.h"

#define SIZE_T_MAX ((size_t)-1)

MEMFILE_RESULT mf_alloc_ex(memfile_t *p_dstmemfile, int flags, size_t start_size)
{
	mf_init(p_dstmemfile);
	//p_dstmemfile->

	return MF_SUCCESS;
}

MEMFILE_RESULT mf_free_ex(memfile_t *p_dstmemfile, int flags)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_file_to_memory_ex(memfile_t *p_dstmemfile, int flags, size_t additional_bytes, char *p_filename)
{
	MEMFILE_RESULT result;
	fpos_t fsize;
	fpos_t fsize_with_ab;

	mf_init(p_dstmemfile);
	result = MF_SUCCESS;
	p_dstmemfile->abytes = additional_bytes;
	FILE *fp = fopen(p_filename, "rb");
	if (!fp)
		return MF_FAILED_TO_OPEN;

	fseek(fp, 0, SEEK_END);
	fgetpos(fp, &fsize);
	rewind(fp);

	// clamp file size
	if (fsize > SIZE_T_MAX) {
		if (flags & MFF_CLAMP_SIZE) {
			fsize = SIZE_T_MAX;
			result = MF_CLAMPED;
		}
		else {
			fclose(fp);
			return MF_OUT_OF_MEMORY;
		}
	}

	// clamp file size with additional bytes
	fsize_with_ab = fsize + additional_bytes;
	if (fsize_with_ab > SIZE_T_MAX) {
		if (flags & MFF_CLAMP_SIZE) {
			fsize_with_ab = SIZE_T_MAX;
			result = MF_CLAMPED;
		}
		else {
			fclose(fp);
			return MF_OUT_OF_MEMORY;
		}
	}

	// allocate memory
	p_dstmemfile->size = (size_t)fsize_with_ab; //long long to size_t
	if (!(p_dstmemfile->p_data = (unsigned char *)malloc(p_dstmemfile->size))) {
		fclose(fp);
		return MF_OUT_OF_MEMORY;
	}

	// clear memory
	if (flags & MFF_CLEAR_MEMORY)
		memset(&p_dstmemfile->p_data[p_dstmemfile->size - p_dstmemfile->abytes], 0, p_dstmemfile->abytes);

	// read bytes to memory
	if (fread(p_dstmemfile->p_data, 1, fsize, fp) != fsize) {
		fclose(fp);
		return MF_RDERR;
	}
	fclose(fp);
	return result;
}

MEMFILE_RESULT mf_file_to_memory_u_ex(memfile_t *p_dstmemfile, int flags, size_t additional_bytes, wchar_t *p_filename)
{
	assert(0);
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_memory_to_file(memfile_t *p_dstmemfile, int flags, const char *p_filename)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_memory_to_file_u(memfile_t *p_dstmemfile, int flags, const wchar_t *p_filename)
{
	assert(0);
	return MF_SUCCESS;
}

// ---------------------------

MEMFILE_RESULT mf_write_byte(memfile_t *p_dstmemfile, int byte)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_read_byte(int *p_dstbyte, memfile_t *p_dstmemfile)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_write_short(memfile_t *p_dstmemfile, int wbyte)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_read_short(int *p_dstbytes, memfile_t *p_dstmemfile)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_write_int(memfile_t *p_dstmemfile, int qbyte)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_read_int(int *p_dstbytes, memfile_t *p_dstmemfile)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_write_float(memfile_t *p_dstmemfile, float value)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_read_float(float *p_dstflt, memfile_t *p_dstmemfile)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_write_string(memfile_t *p_dstmemfile, const char *p_string)
{
	return MF_SUCCESS;
}

MEMFILE_RESULT mf_read_string(char *p_dststr, size_t maxlen, memfile_t *p_dstmemfile)
{
	return MF_SUCCESS;
}
