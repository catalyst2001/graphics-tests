#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "../../common/rutility/rmemory.h"
#include "../../common/glmath.h"

#define ALLOC_ONE(type) ((type *)calloc(1, sizeof(type)))

enum RENDER_STATUS
{
	RSTATUS_SUCCESS = 0,
	RSTATUS_OUT_OF_MEMORY,
};

