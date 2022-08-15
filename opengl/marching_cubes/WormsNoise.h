#pragma once
#include "../../common/SimplexNoise.h"
#include "../../common/glmath.h"

class WormsNoise
{
	SimplexNoise noise;
	vec3 current_noise_pos;
	vec3 offset_pos;
	vec3 segment_pos;
public:
	WormsNoise() {}
	~WormsNoise() {}




};

