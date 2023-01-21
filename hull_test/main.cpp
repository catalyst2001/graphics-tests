#include <stdio.h>
#include <math.h>
#include <float.h>

struct vec2_t {
	float x, y;
};

inline bool vec2_equals(const vec2_t *p_veca, const vec2_t *p_vecb)
{
	return (p_veca->x == p_vecb->x && p_veca->y == p_vecb->y);
}

float vec2_length(const vec2_t *p_src);
void vec2_normalize(vec2_t *p_dst);
void vec2_add(vec2_t *p_dst, const vec2_t *p_veca, const vec2_t *p_vecb);
void vec2_divs(vec2_t *p_dst, vec2_t *p_vec, float scalar);
float vec2_dot(const vec2_t *p_veca, const vec2_t *p_vecb);
float vec2_dotn(const vec2_t *p_veca, const vec2_t *p_vecb);
void compute_centroid(vec2_t *p_dst, const vec2_t *p_polypoints, int num_of_points);

#define DTOR 0.0174444444444444f
#define RTOD 57.32484076433121f

#define max(a, b) (a > b) ? a : b

#define min(a, b) (a < b) ? a : b

float clamp(float val, float minval, float maxval)
{
	if (val < minval)
		return minval;

	if (val > maxval)
		return maxval;

	return val;
}

template<typename _Type>
void swap(_Type *p_ea, _Type *p_eb)
{
	_Type temp = *p_ea;
	*p_ea = *p_eb;
	*p_eb = temp;
}

void concave_hull(vec2_t *p_points, int count)
{
	vec2_t centroid;
	int j = 0;
	int currpt = 0;
	float cos_angle, max_rad = -9999999.f;
	compute_centroid(&centroid, p_points, count);
	vec2_normalize(&centroid);
	while (currpt < count) {
		for (; j < count; j++) {
			vec2_t norm_point = p_points[j];
			vec2_normalize(&norm_point);
			cos_angle = vec2_dot(&norm_point, &centroid);
			float rad = acosf(clamp(cos_angle, -1.f, 1.f));
			printf("concave_hull rad: %f\n", rad);
			if ((rad > max_rad)) {
				
				if (max_rad > rad)
					max_rad = rad;

				swap<vec2_t>(&p_points[count - 1], &p_points[j]);
				currpt++;
				goto repeat;
			}
		}
	repeat:;
	}
}

// Jarvis march algorithm
bool convex_hull(vec2_t *p_dst, vec2_t *p_points, int count)
{
	int i;
	float ox_max_angle = 0.f;
	int lowest_idx = 0;
	int dst_idx = 0;

	if (count < 2)
		return false; // one point cannot describe any shape

	/* find lowest Y coord */
	vec2_t ox = { 1, 0 };
	for (i = 0; i < count; i++) {
		float ox_angle = acosf(clamp(vec2_dotn(&ox, &p_points[i]), -1.f, 1.f));
		if (ox_angle > ox_max_angle) {
			ox_max_angle = ox_angle;
			lowest_idx = i;
		}
	}

	float pt_angle;
	int min_pt_angle_idx = 0;
	float min_pt_angle = 3.14f;
	for (i = 0; i < count; i++) {	
		pt_angle = acosf(clamp(vec2_dotn(&p_points[lowest_idx], &p_points[i]), -1.f, 1.f));
		if (pt_angle < min_pt_angle && i != min_pt_angle_idx && lowest_idx != i) {
			min_pt_angle = pt_angle;
			min_pt_angle_idx = i;
		}
	}
	printf("MIN ANGLE %f TO POINT ( %f %f )  ( %f %f )\n", min_pt_angle,
		p_points[min_pt_angle_idx].x, p_points[min_pt_angle_idx].y,
		p_points[lowest_idx].x, p_points[lowest_idx].y
	);

	p_dst[dst_idx++] = p_points[lowest_idx]; //save lowest point

	int curr_pt_idx = (lowest_idx + 1) % count;
	while (curr_pt_idx != lowest_idx && dst_idx < count) {
		int min_pt_angle_idx = 0;
		float min_rad_angle = 3.14159265358;
		for (i = 0; i < count; i++) {

			/* skip calculating the angle between the same point */
			if (curr_pt_idx == i)
				continue;

			float cos_angle = vec2_dotn(&p_points[curr_pt_idx], &p_points[i]);
			float rad_angle = acosf(clamp(cos_angle, -1.f, 1.f));
			if (min_rad_angle > rad_angle) {
				min_rad_angle = rad_angle;
				min_pt_angle_idx = i;
				printf("min angle: %f\n", min_rad_angle * RTOD);
			}
		}

		/* store the point between which the largest angle */
		p_dst[dst_idx++] = p_points[min_pt_angle_idx];

		/* set this point index to current point index */
		curr_pt_idx = min_pt_angle_idx;

		printf("MIN ANGLE: %f\n", min_rad_angle * RTOD);
	}
	return true;
}

void print_polygon(const vec2_t *p_src, int count)
{
	for (int i = 0; i < count; i++)
		printf(" ( %f %f )", p_src[i].x, p_src[i].y);

	printf("\n");
}

int main()
{
	vec2_t veca = { 1.f, 1.f };
	vec2_t vecb = { 10.f, 10.f };
	vec2_normalize(&veca);
	vec2_normalize(&vecb);
	float cosa = vec2_dot(&veca, &vecb);
	float rad = acosf(clamp(cosa, -1.f, 1.f));
	printf("COSA=%f  RAD=%f  DEG=%f\n", cosa, rad, rad * RTOD);
	//return 0;

	vec2_t poly[] = {
		{ 274.f, 24.f },
		{ 444.f, 264.f },
		{ 184.f, 117.f },
		{ 771.f, 143.f },
		{ 130.f, 371.f },
		{ 481.f, 24.f },
		{ 560.f, 347.f }
	};
#define size(x) (sizeof(x) / sizeof(x[0]))

	printf("Source mixed polygon: \n");
	print_polygon(poly, size(poly));

	vec2_t out_poly[size(poly)];
	printf("Polygon with sort: \n");
	convex_hull(out_poly, poly, size(poly));
	print_polygon(out_poly, size(out_poly));
	return 0;
}

float vec2_length(const vec2_t *p_src)
{
	return sqrtf(p_src->x * p_src->x + p_src->y * p_src->y);
}

void vec2_normalize(vec2_t *p_dst)
{
	float length = vec2_length(p_dst);
	if (length < FLT_EPSILON)
		length = 1.f;

	p_dst->x /= length;
	p_dst->y /= length;
}

void vec2_add(vec2_t *p_dst, const vec2_t *p_veca, const vec2_t *p_vecb)
{
	p_dst->x = p_veca->x + p_vecb->x;
	p_dst->y = p_veca->y + p_vecb->y;
}

void vec2_divs(vec2_t *p_dst, vec2_t *p_vec, float scalar)
{
	p_dst->x = p_vec->x / scalar;
	p_dst->y = p_vec->y / scalar;
}

void compute_centroid(vec2_t *p_dst, const vec2_t *p_polypoints, int num_of_points)
{
	vec2_t sum = { 0.f, 0.f };
	for (int i = 0; i < num_of_points; i++)
		vec2_add(&sum, &sum, &p_polypoints[i]);

	vec2_divs(&sum, &sum, (float)num_of_points);
	*p_dst = sum;
}

float vec2_dot(const vec2_t *p_veca, const vec2_t *p_vecb)
{
	return p_veca->x * p_vecb->x + p_veca->y * p_vecb->y;
}

float vec2_dotn(const vec2_t *p_veca, const vec2_t *p_vecb)
{
	vec2_t vec_a = *p_veca;
	vec2_t vec_b = *p_vecb;
	vec2_normalize(&vec_a);
	vec2_normalize(&vec_b);
	return vec2_dot(&vec_a, &vec_b);
}
