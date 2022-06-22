#include "math.h"

void quat_compute_w(quat_t *p_quat)
{
	float t = 1.0f - (p_quat->x * p_quat->x) - (p_quat->y * p_quat->y) - (p_quat->z * p_quat->z);
	p_quat->w = (t < 0.0f) ? 0.0f : -sqrt(t);
}

float quat_length_sq(const quat_t *p_quat)
{
	return (p_quat->x * p_quat->x) + (p_quat->y * p_quat->y) + (p_quat->z * p_quat->z) + (p_quat->w * p_quat->w);
}

float quat_length(const quat_t * p_quat)
{
	return sqrt((p_quat->x * p_quat->x) + (p_quat->y * p_quat->y) + (p_quat->z * p_quat->z) + (p_quat->w * p_quat->w));
}

float quat_dot(const quat_t *p_quata, const quat_t *p_quatb)
{
	return ((p_quata->x * p_quatb->x) + (p_quata->y * p_quatb->y) + (p_quata->z * p_quatb->z) + (p_quata->w * p_quatb->w));
}

void quat_nomalize(quat_t *p_dst, const quat_t *p_src)
{
	float n = quat_length(p_src);
	p_dst->x = p_src->x / n;
	p_dst->y = p_src->y / n;
	p_dst->z = p_src->z / n;
	p_dst->w = p_src->w / n;
}

void quat_mul(quat_t *p_dst, const quat_t *p_quatA, const quat_t *p_quatB)
{
	p_dst->x = p_quatA->x * p_quatB->w + p_quatA->y * p_quatB->z - p_quatA->z * p_quatB->y + p_quatA->w * p_quatB->x;
	p_dst->y = -p_quatA->x * p_quatB->z + p_quatA->y * p_quatB->w + p_quatA->z * p_quatB->x + p_quatA->w * p_quatB->y;
	p_dst->z = p_quatA->x * p_quatB->y - p_quatA->y * p_quatB->x + p_quatA->z * p_quatB->w + p_quatA->w * p_quatB->z;
	p_dst->w = -p_quatA->x * p_quatB->x - p_quatA->y * p_quatB->y - p_quatA->z * p_quatB->z + p_quatA->w * p_quatB->w;
}

void quat_mulvec(quat_t *p_dst, const quat_t *p_quatA, const vector3_t *p_vecB)
{
	p_dst->w = -(p_quatA->x * p_vecB->x) - (p_quatA->y * p_vecB->y) - (p_quatA->z * p_vecB->z);
	p_dst->x = (p_quatA->w * p_vecB->x) + (p_quatA->y * p_vecB->z) - (p_quatA->z * p_vecB->y);
	p_dst->y = (p_quatA->w * p_vecB->y) + (p_quatA->z * p_vecB->x) - (p_quatA->x * p_vecB->z);
	p_dst->z = (p_quatA->w * p_vecB->z) + (p_quatA->x * p_vecB->y) - (p_quatA->y * p_vecB->x);
}

void quat_conjugate(quat_t *p_dst, const quat_t *p_src)
{
	p_dst->w = p_src->w;
	p_dst->x = -p_src->x;
	p_dst->y = -p_src->y;
	p_dst->z = -p_src->z;
}

void quat_slerp(quat_t *p_dst, const quat_t *p_quatA, const quat_t *p_quatB, float time)
{
	/* Check for out-of range parameter and return edge points if so */
	if (time <= 0.0) {
		*p_dst = *p_quatA;
		return;
	}

	if (time >= 1.0) {
		*p_dst = *p_quatB;
		return;
	}

	/* Compute "cosine of angle between quaternions" using dot product */
	float cosOmega = quat_dot(p_quatA, p_quatB);

	/* If negative dot, use -q1.	Two quaternions q and -q
		 represent the same rotation, but may produce
		 different slerp.	We chose q or -q to rotate using
		 the acute angle. */
	float q1w = p_quatB->w;
	float q1x = p_quatB->x;
	float q1y = p_quatB->y;
	float q1z = p_quatB->z;
	if (cosOmega < 0.0f) {
		q1w = -q1w;
		q1x = -q1x;
		q1y = -q1y;
		q1z = -q1z;
		cosOmega = -cosOmega;
	}

	/* We should have two unit quaternions, so dot should be <= 1.0 */
	//assert(cosOmega < 1.1f);

	/* Compute interpolation fraction, checking for quaternions almost exactly the same */
	float k0, k1;
	if (cosOmega > 0.9999f)
	{
		/* Very close - just use linear interpolation, which will protect againt a divide by zero */
		k0 = 1.0f - time;
		k1 = time;
	}
	else
	{
		/* Compute the sin of the angle using the trig identity sin^2(omega) + cos^2(omega) = 1 */
		float sinOmega = sqrt(1.0f - (cosOmega * cosOmega));

		/* Compute the angle from its sin and cosine */
		float omega = atan2(sinOmega, cosOmega);

		/* Compute inverse of denominator, so we only have to divide once */
		float oneOverSinOmega = 1.0f / sinOmega;

		/* Compute interpolation parameters */
		k0 = sin((1.0f - time) * omega) * oneOverSinOmega;
		k1 = sin(time * omega) * oneOverSinOmega;
	}

	/* Interpolate and return new quaternion */
	p_dst->w = (k0 * p_quatA->w) + (k1 * q1w);
	p_dst->x = (k0 * p_quatA->x) + (k1 * q1x);
	p_dst->y = (k0 * p_quatA->y) + (k1 * q1y);
	p_dst->z = (k0 * p_quatA->z) + (k1 * q1z);
}

void quat_from_angle_axis(quat_t *p_dst, float angle, const vector3_t *p_axis)
{
	vector3_t result;
	float theta = angle * DTOR;
	vec3_mulsc(&result, p_axis, -sin(theta / 2.f));
	p_dst->x = result.x;
	p_dst->y = result.y;
	p_dst->z = result.z;
	p_dst->w = cos(theta / 2.f);
}

// vectors
void vec3_add(vector3_t *p_dst, const vector3_t *p_vec_a, const vector3_t *p_vec_b)
{
	p_dst->x = p_vec_a->x + p_vec_b->x;
	p_dst->y = p_vec_a->y + p_vec_b->y;
	p_dst->z = p_vec_a->z + p_vec_b->z;
}

void vec3_sub(vector3_t *p_dst, const vector3_t *p_vec_a, const vector3_t *p_vec_b)
{
	p_dst->x = p_vec_a->x - p_vec_b->x;
	p_dst->y = p_vec_a->y - p_vec_b->y;
	p_dst->z = p_vec_a->z - p_vec_b->z;
}

void vec3_mul(vector3_t *p_dst, const vector3_t *p_vec_a, const vector3_t *p_vec_b)
{
	p_dst->x = p_vec_a->x * p_vec_b->x;
	p_dst->y = p_vec_a->y * p_vec_b->y;
	p_dst->z = p_vec_a->z * p_vec_b->z;
}

void vec3_mulsc(vector3_t *p_dst, const vector3_t *p_vec, float scalar)
{
	p_dst->x = p_vec->x * scalar;
	p_dst->y = p_vec->y * scalar;
	p_dst->z = p_vec->z * scalar;
}

void vec3_div(vector3_t *p_dst, const vector3_t *p_vec_a, const vector3_t *p_vec_b)
{
	p_dst->x = p_vec_a->x / p_vec_b->x;
	p_dst->y = p_vec_a->y / p_vec_b->y;
	p_dst->z = p_vec_a->z / p_vec_b->z;
}

void vec3_divsc(vector3_t *p_dst, const vector3_t *p_vec_a, float scalar)
{
	p_dst->x = p_vec_a->x / scalar;
	p_dst->y = p_vec_a->y / scalar;
	p_dst->z = p_vec_a->z / scalar;
}

float vec3_length(const vector3_t *p_dst)
{
	return sqrtf((p_dst->x * p_dst->x) + (p_dst->y * p_dst->y) + (p_dst->z * p_dst->z));
}

float vec3_dot(const vector3_t *p_vec_a, const vector3_t *p_vec_b)
{
	return p_vec_a->x * p_vec_b->x + p_vec_a->y * p_vec_b->y + p_vec_a->z * p_vec_b->z;
}

void vec3_cross(vector3_t *p_dst, const vector3_t *p_vec_a, const vector3_t *p_vec_b)
{
	p_dst->x = p_vec_a->y * p_vec_b->z - p_vec_a->z * p_vec_b->y;
	p_dst->y = (p_vec_a->z * p_vec_b->x - p_vec_a->x * p_vec_b->z);
	p_dst->z = p_vec_a->x * p_vec_b->y - p_vec_a->y * p_vec_b->x;
}

float vec3_distance(const vector3_t *p_vec_a, const vector3_t *p_vec_b)
{
	vector3_t diff;
	diff.x = p_vec_a->x - p_vec_b->x;
	diff.y = p_vec_a->y - p_vec_b->y;
	diff.z = p_vec_a->z - p_vec_b->z;
	return vec3_length(&diff);
}

float vec3_cos_angle(const vector3_t *p_vec_a, const vector3_t *p_vec_b)
{
	float length = vec3_dot(p_vec_a, p_vec_b);
	return length / (vec3_length(p_vec_a) * vec3_length(p_vec_b));
}

float vec3_angle_between_vectors(const vector3_t *p_vec_a, const vector3_t *p_vec_b)
{
	return cos_angle_to_deg(vec3_cos_angle(p_vec_a, p_vec_b));
}

//mul(q, mul(quat(a, 0.f), conjugate(q))).vec();
void vec3_rotate_with_quat(vector3_t *p_dst, const vector3_t *p_src, const quat_t *p_quat)
{
	quat_t qa, qb;
	quat_conjugate(&qa, p_quat);
	qb.x = p_src->x;
	qb.y = p_src->y;
	qb.z = p_src->z;
	qb.w = 0.f;
	quat_mul(&qa, &qb, &qa);
	quat_mul(&qa, p_quat, &qa);
	p_dst->x = qa.x;
	p_dst->y = qa.y;
	p_dst->z = qa.z;
}

void vec3_normalize(vector3_t *p_vec)
{
	float length = sqrtf((p_vec->x * p_vec->x) + (p_vec->y * p_vec->y) + (p_vec->z * p_vec->z));
	if (length > 0.f) {
		p_vec->x /= length;
		p_vec->y /= length;
		p_vec->z /= length;
	}
}

bool point_in_sphere(const bouding_sphere_t *p_sphere, const vector3_t *p_point)
{
	vector3_t d;
	d.x = (p_sphere->pos.x - p_point->x);
	d.y = (p_sphere->pos.y - p_point->y);
	d.z = (p_sphere->pos.z - p_point->z);
	return ((d.x * d.x) + (d.y * d.y) + (d.z * d.z)) == (p_sphere->radius * p_sphere->radius);
}

//http://netlib.narod.ru/library/book0032/ch15_04.htm#:~:text=%D0%A7%D1%82%D0%BE%D0%B1%D1%8B%20%D0%BE%D0%BF%D1%80%D0%B5%D0%B4%D0%B5%D0%BB%D0%B8%D1%82%D1%8C%2C%20%D0%BF%D0%B5%D1%80%D0%B5%D1%81%D0%B5%D0%BA%D0%B0%D0%B5%D1%82%20%D0%BB%D0%B8%20%D0%BB%D1%83%D1%87,%D0%BF%D0%BE%D0%B7%D0%B2%D0%BE%D0%BB%D0%B8%D1%82%20%D0%BD%D0%B0%D0%BC%20%D0%BD%D0%B0%D0%B9%D1%82%D0%B8%20%D1%82%D0%BE%D1%87%D0%BA%D0%B8%20%D0%BF%D0%B5%D1%80%D0%B5%D1%81%D0%B5%D1%87%D0%B5%D0%BD%D0%B8%D1%8F.&text=%D0%B3%D0%B4%D0%B5%20A%20%3D%20u%20%D0%A7%20u,%E2%80%93%20c)%20%E2%80%93%20r2.
bool ray_sphere_intersect(const ray_t *p_ray, const bouding_sphere_t *p_sphere)
{
	// ray origin inside sphere
	if (point_in_sphere(p_sphere, &p_ray->pos)) {
		//printf("Point in shpere\n");
		true;
	}

	vector3_t v;
	vec3_sub(&v, &p_ray->pos, &p_sphere->pos);
	float b = 2.f * vec3_dot(&p_ray->dir, &v);
	float c = vec3_dot(&v, &v) - (p_sphere->radius * p_sphere->radius);
	float d = (b * b) - (4.f * c);
	if (d < 0.f)
		return false;

	d = sqrtf(d);
	float s0 = (-b + d) / 2.f;
	float s1 = (-b - d) / 2.f;
	if (s0 >= 0.0f || s1 >= 0.0f) {
		//printf("Intersect! s0=%f s1=%f\n", s0, s1);
		return true;
	}

	return false;
}

bool ray_sphere_intersect_ilya(const ray_t *p_ray, const bouding_sphere_t *p_sphere)
{
	vector3_t vec, cb, ab;
	vec3_sub(&cb, &p_ray->dir, &p_sphere->pos);
	vec3_sub(&ab, &p_ray->dir, &p_ray->pos);
	vec3_cross(&vec, &cb, &ab);

	float vec_length = vec3_length(&vec);
	float ab_length = vec3_length(&ab);
	float x = vec_length / ab_length;

	return (x <= p_sphere->radius); //budet
}

float ray_plane_intersect(const ray_t *p_ray, const vector3_t *p_origin, const vector3_t *p_normal)
{
	float d = -vec3_dot(p_normal, &p_ray->dir);
	if (d > EPSILON) {
		vector3_t p0;
		vec3_sub(&p0, p_origin, &p_ray->pos);
		return -vec3_dot(&p0, p_normal) / d;
	}
	return -1.f;
}
