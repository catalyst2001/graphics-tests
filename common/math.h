#pragma once
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

#define math_abs(x) (x < 0) ? -x : x
#define FLOAT_PI 3.141592653589793238462643
#define RTOD 57.2957795
#define RAD2DEG(rad) ((rad * 180.f) / FLOAT_PI)
#define DTOR 0.0174532925
#define DEG2RAD(deg) ((deg * FLOAT_PI) / 180.f)
#define EPSILON 0.0000001f

// Float vector3 ( 12 bytes )
typedef struct vector3_s {
	union {
		struct { float pitch, yaw, roll; };
		struct { float x, y, z; };
	};
} vector3_t;

typedef struct vector3i_s {
	int x, y, z;
} vector3i_t;

// Byte vector3 ( 3 bytes )
// Range -128 / +127
typedef struct vector3b_s {
	char x, y, z;
} vector3b_t;

typedef struct quat_s {
	float x, y, z, w;
} quat_t;

/**
* Quaternions
*/
#define QUAT_FROM_VEC3(q, v) \
	(q)->x = (v)->x;\
	(q)->y = (v)->y;\
	(q)->z = (v)->z;\
	(q)->w = 0.f;

#define QUAT_SET(q, _x, _y, _z, _w) \
	(q)->x = _x; \
	(q)->y = _y; \
	(q)->z = _z; \
	(q)->w = _w;
	
void  quat_compute_w(const quat_t *p_quat);
float quat_length_sq(const quat_t *p_quat);
float quat_length(const quat_t *p_quat);
float quat_dot(const quat_t *p_quata, const quat_t *p_quatb);
void  quat_nomalize(quat_t *p_dst, const quat_t *p_src);
void  quat_mul(quat_t *p_dst, const quat_t *p_quatA, const quat_t *p_quatB);
void  quat_mulvec(quat_t *p_dst, const quat_t *p_quatA, const vector3_t *p_vecB);
void  quat_conjugate(quat_t *p_dst, const quat_t *p_src);

void  quat_slerp(quat_t *p_dst, const quat_t *p_quatA, const quat_t *p_quatB, float time);
void  quat_from_angle_axis(quat_t *p_dst, float angle, const vector3_t *p_axis);

/**
* Vectors
*/
#define VEC3FLOAT2BYTE(byte_dest, float_src) \
	(byte_dest)->x = *(char *)&((float_src)->x); \
	(byte_dest)->y = *(char *)&((float_src)->y); \
	(byte_dest)->z = *(char *)&((float_src)->z);

#define VEC3_SET(v, _x, _y, _z) \
	(v)->x = _x; \
	(v)->y = _y; \
	(v)->z = _z;

#define VEC3(x, y, z)              ((vector3_t){ x, y, z })
#define vec3equ(pva, pvb)          ((pva)->x == (pvb)->x && (pva)->y == (pvb)->y && (pva)->z == (pvb)->z)
#define vec3les(pva, pvb)          ((pva)->x < (pvb)->x && (pva)->y < (pvb)->y && (pva)->z < (pvb)->z)
#define vec3les_or_eq(pva, pvb)    ((pva)->x <= (pvb)->x && (pva)->y <= (pvb)->y && (pva)->z <= (pvb)->z)
#define vec3gtr(pva, pvb)          ((pva)->x > (pvb)->x && (pva)->y > (pvb)->y && (pva)->z > (pvb)->z)
#define vec3gtr_or_eq(pva, pvb)    ((pva)->x >= (pvb)->x && (pva)->y >= (pvb)->y && (pva)->z >= (pvb)->z)

void vec3_add(vector3_t *p_dst, const vector3_t *p_vec_a, const vector3_t *p_vec_b);
void vec3_sub(vector3_t *p_dst, const vector3_t *p_vec_a, const vector3_t *p_vec_b);
void vec3_mul(vector3_t *p_dst, const vector3_t *p_vec_a, const vector3_t *p_vec_b);
void vec3_scale(vector3_t *p_dst, const vector3_t *p_vec, float scalar);
void vec3_div(vector3_t *p_dst, const vector3_t *p_vec_a, const vector3_t *p_vec_b);
void vec3_divsc(vector3_t *p_dst, const vector3_t *p_vec_a, float scalar);

void vec3i_add(vector3i_t *p_dst, const vector3i_t *p_vec_a, const vector3i_t *p_vec_b);
void vec3i_sub(vector3i_t *p_dst, const vector3i_t *p_vec_a, const vector3i_t *p_vec_b);
void vec3i_mul(vector3i_t *p_dst, const vector3i_t *p_vec_a, const vector3i_t *p_vec_b);
void vec3i_scale(vector3i_t *p_dst, const vector3i_t *p_vec, int scalar);
void vec3i_div(vector3i_t *p_dst, const vector3i_t *p_vec_a, const vector3i_t *p_vec_b);
void vec3i_divsc(vector3i_t *p_dst, const vector3i_t *p_vec_a, int scalar);

void vec3_cross(vector3_t *p_dst, const vector3_t *p_vec_a, const vector3_t *p_vec_b);
void vec3_normalize(vector3_t *p_vec);
float vec3_length(const vector3_t *p_dst);
float vec3_dot(const vector3_t *p_vec_a, const vector3_t *p_vec_b);
float vec3_distance(const vector3_t *p_vec_a, const vector3_t *p_vec_b);

float vec3_cos_angle(const vector3_t *p_vec_a, const vector3_t *p_vec_b);
#define cos_angle_to_deg(cos_angle) (RAD2DEG(acosf(cos_angle)))
float vec3_angle_between_vectors(const vector3_t *p_vec_a, const vector3_t *p_vec_b);
void  vec3_rotate_with_quat(vector3_t *p_dst, const vector3_t *p_src, const quat_t *p_quat);

/**
* ray
*/
typedef struct ray_s {
	vector3_t pos;
	vector3_t dir;
	float length;
} ray_t;

typedef struct bouding_sphere_s {
	vector3_t pos;
	float radius;
} bouding_sphere_t;

typedef struct bouding_box_s {
	vector3_t min;
	vector3_t max;
} bouding_box_t;

typedef struct bouding_boxi_s {
	vector3i_t min;
	vector3i_t max;
} bouding_boxi_t;

#define point_in_bbox(ppt, pbb) ((pbb)->min.x < ppt.x && (pbb)->min.y < ppt.y && (pbb)->min.z < ppt.z  &&  (pbb)->max.x > ppt.x && (pbb)->max.y > ppt.y && (pbb)->max.z > ppt.z)
#define point_in_bbox_or_on(ppt, pbb) ((pbb)->min.x <= ppt.x && (pbb)->min.y <= ppt.y && (pbb)->min.z <= ppt.z  &&  (pbb)->max.x >= ppt.x && (pbb)->max.y >= ppt.y && (pbb)->max.z >= ppt.z)
bool point_in_sphere(const bouding_sphere_t *p_sphere, const vector3_t *p_point);
bool ray_sphere_intersect(const ray_t *p_ray, const bouding_sphere_t *p_sphere);
bool ray_sphere_intersect_ilya(const ray_t *p_ray, const bouding_sphere_t *p_sphere);
bool ray_bound_box_intersect(const ray_t *p_ray, const bouding_box_t *p_bbox);

// >= 0.f - intersect
float ray_plane_intersect(const ray_t *p_ray, const vector3_t *p_origin, const vector3_t *p_normal);

static bool PlaneIntersection5(vector3_t *planeP, vector3_t *planeN, vector3_t *rayP, vector3_t *rayD, vector3_t *intersect)
{
	float d = -vec3_dot(planeP, planeN);
	float t = -(d + rayP->z * planeN->z + rayP->y * planeN->y + rayP->x * planeN->x) / (rayD->z * planeN->z + rayD->y * planeN->y + rayD->x * planeN->x);
	if (t > 0.f) {
		vector3_t ipt;
		vec3_scale(&ipt, rayD, t);
		vec3_add(&ipt, rayP, &ipt);
		*intersect = ipt;
		return true;
	}
	return false;
}

/* 2D math */
typedef struct vec2i_s {
	int x, y;
} vec2i_t;

typedef struct vec2_s {
	float x, y;
} vec2_t;

typedef vec2i_t point_t;
typedef vec2_t pointf_t;

float vec2_length_squared(const vec2_t *p_vec2);
float vec2_length(const vec2_t *p_vec2);
void vec2_normalize(vec2_t *p_vec2dst, const vec2_t *p_vec2);
float vec2_dot(const vec2_t *p_vec_a, const vec2_t *p_vec_b);
void vec2_scale(vec2_t *p_vecdst, const vec2_t *p_vecsrc, float scalar);
void vec2_add(vec2_t *p_vecdst, const vec2_t *p_veca, vec2_t *p_vecb);
void vec2_sub(vec2_t *p_vecdst, const vec2_t *p_veca, vec2_t *p_vecb);

#define VEC3_TO_VEC2_XY(pvec2, pvec3) \
	(pvec2)->x = (pvec3)->x; \
	(pvec2)->y = (pvec3)->y;

#define VEC3_TO_VEC2_XZ(pvec2, pvec3) \
	(pvec2)->x = (pvec3)->x; \
	(pvec2)->y = (pvec3)->z;

typedef struct polygon_s {
	int num_of_points;
	vec2_t *p_points;
} polygon_t;

bool line_line_intersection(vec2_t *p_dst_isct_pt, const vec2_t *p_linea_from, const vec2_t *p_linea_to, const vec2_t *p_lineb_from, const vec2_t *p_lineb_to);
bool line_line_intersection2(vec2_t *p_dst_isct_pt, const vec2_t A, const vec2_t B, const vec2_t C, const vec2_t D);
bool point_in_polygon(const polygon_t *p_poly, const vec2_t *p_point);

#ifdef __cplusplus
}
#endif