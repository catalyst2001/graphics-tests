#pragma once

struct vec2_t {
	float x, y;
};

/*
=================================
 vec2_add

 add two vectors
=================================
*/
inline void vec2_add(vec2_t *p_dst, const vec2_t *p_veca, const vec2_t *p_vecb)
{
	p_dst->x = p_veca->x + p_vecb->x;
	p_dst->y = p_veca->y + p_vecb->y;
}

/*
=================================
 vec2_divs

 divide vector by scalar
=================================
*/
inline void vec2_divs(vec2_t *p_dst, vec2_t *p_vec, float scalar)
{
	p_dst->x = p_vec->x / scalar;
	p_dst->y = p_vec->y / scalar;
}

inline void compute_centroid(vec2_t *p_dst, const vec2_t *p_polypoints, int num_of_points)
{
	vec2_t sum = { 0.f, 0.f };
	for (int i = 0; i < num_of_points; i++)
		vec2_add(&sum, &sum, &p_polypoints[i]);

	vec2_divs(&sum, &sum, (float)num_of_points);
	*p_dst = sum;
}

inline bool point_in_circle(vec2_t *p_circle_center, const vec2_t *p_point, float radius)
{
	vec2_t circle;
	circle.x = p_circle_center->x - p_point->x;
	circle.y = p_circle_center->y - p_point->y;
	return ((circle.x * circle.x + circle.y * circle.y) <= (radius * radius)); //x^2 + y^2 <= R^2
}