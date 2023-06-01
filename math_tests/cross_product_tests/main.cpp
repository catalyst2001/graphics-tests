#include <stdio.h>
#include <string.h>
#include "../../common/glmath.h"

void barycenter(vec3 *p_dst, const vec3 *p_src, unsigned int count)
{
	vec3 center = { 0.f, 0.f, 0.f };
	while (count--)
		center += p_src[count];

	center /= (float)count;
	*p_dst = center;
}

float tri_area_squared(const vec3 *p_vt0, const vec3 *p_vt1, const vec3 *p_vt2)
{
	/* compute length of triangle sides */
	float a = length(*p_vt0 - *p_vt1);
	float b = length(*p_vt1 - *p_vt2);
	float c = length(*p_vt2 - *p_vt0);
	float p = (a + b + c) / 2.f;
	return p * (p - a) * (p - b) * (p - c);
}

float poly_area_squared(const vec3 *p_src_barycenter, const vec3 *p_src, unsigned int count)
{
	float area = 0.f;
	for (unsigned int i = 0; i < count; i++)
		area += tri_area_squared(p_src_barycenter, &p_src[i], &p_src[(i + 1) % count]);

	return area;
}

bool poly_plane_normal(vec3 *p_dst, const vec3 *p_src, unsigned int count)
{
	vec3 normal, curr_normal;
	if (count < 3)
		return false;

	cross(normal, p_src[0], p_src[1]);
	normalize(normal);

	for (unsigned int i = 1; i < count - 1; i++) {
		cross(curr_normal, p_src[i], p_src[(i + 1) % count]);
		normalize(normal);
		if (curr_normal != normal)
			return false;
	}
	*p_dst = curr_normal;
	return true;
}

enum POLYRT
{
	POLYRT_OK = 0,
	POLYRT_NOPLANE,
	POLYRT_NOONPLANE,
	POLYRT_NOINTERSECT,
	POLYRT_OUTERINTERSECT
};

//int PlaneIntersection2(CPlane3 &plane, vec3 &intersectionpoint)
//{
//	// assuming vectors are all normalized
//	float denom = -dot(plane.m_normal, m_direction);
//	if (denom > 1e-6) {
//		vec3 p0l0 = plane.m_origin - m_origin;
//		float t = -dot(p0l0, plane.m_normal) / denom;
//		if (t >= 0) {
//			intersectionpoint = m_origin + m_direction * t;//правильно
//			//intersectionpoint = ray_evaluate(*this, t);
//			return 1;
//		}
//	}
//	return 0;
//}

POLYRT polygon_ray_test(vec3 *p_dst, const vec3 *p_src, unsigned int count, const vec3 *p_ray_pos, const vec3 *p_ray_dir)
{
	float denom;
	float poly_area;
	float subtris_areas;
	vec3 normal;
	vec3 center;
	vec3 intersect;

	if (!poly_plane_normal(&normal, p_src, count))
		return POLYRT_NOPLANE;

	denom = -dot(normal, *p_ray_dir);
	if (denom > 1e-6) { //1*10^-6
		barycenter(&center, p_src, count);
		float t = -dot(center - *p_ray_pos, normal) / denom;
		if (t >= 0.f) {
			intersect = *p_ray_pos + *p_ray_dir * t;
			if (p_dst)
				*p_dst = intersect;

			subtris_areas = 0.f;
			poly_area = poly_area_squared(&center, p_src, count); //TODO: проверку на нулевую площадь
			for (size_t i = 0; i < count - 1; i++)
				subtris_areas += tri_area_squared(&intersect, &p_src[i], &p_src[(i + 1) % count]);

			if(poly_area == subtris_areas)
				return POLYRT_OK;

			return POLYRT_OUTERINTERSECT;
		}
		return POLYRT_NOONPLANE;
	}
	return POLYRT_NOONPLANE; //вектора направлений луча и плоскости сонаправлены
}

struct poly_s {
	unsigned int num_verts;
	vec3 verts[64];
};

struct polyhedron_s {
	unsigned int num_sides;
	poly_s sides[64];
};

bool polyhedron_point_test(const polyhedron_s *p_phn, const vec3 *p_point)
{
	POLYRT status;
	poly_s *p_side;
	int intersects_count;
	vec3 raypos, raydir;

	raypos = *p_point;
	raydir = vec3(p_point->x + 1.f, p_point->y, p_point->z);
	normalize(raydir);
	intersects_count = 0;
	for (unsigned int i = 0; i < p_phn->num_sides; i++) {
		p_side = (poly_s *)&p_phn->sides[i];
		status = polygon_ray_test(NULL, p_side->verts, p_side->num_verts, &raypos, &raydir);
		if (status == POLYRT_OK)
			intersects_count++;
	}

	return intersects_count & 1;
}

#pragma region TEST1
void test1()
{
	vec3 verts[] = {
		vec3(1, 0, 3),
		vec3(3, 0, 33),
		vec3(9, 0, 45),
		vec3(16, 0, 7),
		vec3(25, 0, 57),
		vec3(5, 0, 37),
		vec3(4, 2, 3),
	};

	vec3 normal, curr_normal;
	cross(normal, verts[0], verts[1]);
	normalize(normal);

	const int count = (sizeof(verts) / sizeof(verts[0]));
	for (size_t i = 1; i < count - 1; i++) {
		cross(curr_normal, verts[i], verts[(i + 1) % count]);
		normalize(normal);
		if (curr_normal != normal) {
			printf("This is not plane\n");
			return;
		}
	}
	printf("OK");
}
#pragma endregion

int main()
{
	//test1();

	//for (int i = 0; i < 5; i++)
	//	if (i & 1)
	//		printf("%d ", i);

	return 0;
}