#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <time.h>
#include "../../common/gldl.h"
#include "../../common/glmath.h"

using std::vector;

gldl_dt_t *p_gldl;

struct Vertex {
	double x, y, z;
	Vertex(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
};

struct Edge {
	int v1, v2;
	Edge(int v1 = 0, int v2 = 0) : v1(v1), v2(v2) {}
};

struct Poly {
	vector<int> vertices;
	Poly(const vector<int>& vertices) : vertices(vertices) {}
};

vector<Vertex> vertices;
vector<Edge> edges;
vector<Poly> polygons;

float frandom(float minVal, float maxVal) {
	float randValue = (float)rand() / RAND_MAX;
	return minVal + randValue * (maxVal - minVal);
}

int irandom(int minVal, int maxVal) {
	int randValue = (int)rand() / RAND_MAX;
	return minVal + randValue * (maxVal - minVal);
}

void generateConvexPolyhedron(int n) {
	// Создайте массив вершин
	for (int i = 0; i < n; i++)
		vertices.push_back(Vertex(frandom(-10, 10), frandom(-10, 10), frandom(-10, 10)));

	// Найдите центр масс всех вершин
	Vertex c(0, 0, 0);
	for (Vertex v : vertices) {
		c.x += v.x;
		c.y += v.y;
		c.z += v.z;
	}
	c.x /= n;
	c.y /= n;
	c.z /= n;

	// Отсортируйте вершины по углу относительно центра масс
	sort(vertices.begin(), vertices.end(), [&](const Vertex& a, const Vertex& b) {
		return atan2(a.y - c.y, a.x - c.x) < atan2(b.y - c.y, b.x - c.x);
	});

	// Создайте ребра и полигоны
	for (int i = 0; i < n; i++) {
		int j = (i + 1) % n;
		edges.push_back(Edge(i, j));
		vector<int> v;
		for (int k = 0; k < n; k++) {
			int l = (k + 1) % n;
			if ((l != j) && (atan2(vertices[l].y - c.y, vertices[l].x - c.x) - atan2(vertices[k].y - c.y, vertices[k].x - c.x) > 0))
				v.push_back(k);
		}
		polygons.push_back(Poly(v));
	}
}

struct polyhedron {
	vector<vec3> verts;


};

/* Ax + By + Cz + D = 0 */
bool vertex_on_plane(const vec3 &plane_pos, const vec3 &plane_normal, const vec3 &vertex)
{
	float d = length(plane_pos); // расчет свободного коэффициента D
	return (plane_normal.x * vertex.x + plane_normal.y * vertex.y + plane_normal.z * vertex.z + d) == 0.f;
}

void generate_convex_polyhedron(polyhedron &dst_polyhedron, int num_verts)
{
	/* generate one side of polyhedron */
	int i;
	const float min_distance = 10.f;
	vec3 up_vec = vec3(0.f, 1.f, 0.f);

	vec3 one_side_normal = vec3(frandom(-1.f, 1.f), frandom(-1.f, 1.f), frandom(-1.f, 1.f));
	int side_one_num_verts = irandom(0, num_verts);
	for (i = 0; i < side_one_num_verts; i++) {

	}

	/* generate two side of polyhedron */
	vec3 two_side_normal = vec3(frandom(-1.f, 1.f), frandom(-1.f, 1.f), frandom(-1.f, 1.f));
	int side_two_num_verts = irandom(0, num_verts);
	for (i = 0; i < side_two_num_verts; i++) {

	}
}

vec3 polyhedron1[] = {
	vec3(5, -8, 4), vec3(4, -8, 0), vec3(5, -8, 4), vec3(-3, -8, 4), vec3(-4, -8, 0), vec3(-3, -8, 1), 
	vec3(4, 3, -4), vec3(6, 3, 0), vec3(4, 3, 4), vec3(-4, 3, 4), vec3(-2, 3, 0), vec3(-4, 3, -4),

	vec3(5, 0, -4), vec3(4, 3, -4), vec3(6, 3, 0), vec3(4, -8, 0),
	vec3(4, -8, 0), vec3(6, 3, 0), vec3(4, 3, 4), vec3(5, -8, 4),

	vec3(5, -8, 4), vec3(4, 3, 4), vec3(-4, 3, 4), vec3(-3, -8, 4),

	vec3(-3, 0, -4), vec3(-4, 3, -4), vec3(-2, 3, 0), vec3(-4, -8, 0),
	vec3(-4, -8, 0), vec3(-2, 3, 0), vec3(-4, 3, 4), vec3(-3, -8, 4),

	vec3(-5, 0, 4), vec3(-3, 0, -4), vec3(-4, 3, -4), vec3(4, 3, -4)
};

int main()
{
	//vec3 planepos = vec3(0.f, 0.f, 0.f);
	//vec3 planenorm = vec3(0.5f, 0.5f, 0.5f);
	//vec3 vert = vec3(0, 0, 0);
	//printf("%s\n", vertex_on_plane(planepos, planenorm, vert) ? "ON PLANE" : "NO");

	return 0;

	p_gldl = gldl_initialize(-1, -1, 800, 600, 24, 32, "Random convex polyhedron generation", NULL);
	if (!p_gldl) {
		printf("Error: %d\n", gldl_last_error());
		return 1;
	}

	srand(time(NULL));
	generateConvexPolyhedron(60); // Генерация многогранника с 6 вершинами

	printf("Num vertices: %zd\n", vertices.size());
	printf("Num edges: %zd\n", edges.size());
	printf("Num polys: %zd\n", polygons.size());

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);

	while (p_gldl->IsWindowOpened()) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glLoadIdentity();

		glTranslatef(0.f, 0.f, -50.f);

		// draw polyhedron
		for (size_t i = 0; i < polygons.size(); i++) {
			Poly *p_curr_poly = &polygons[i];
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			glDrawElements(GL_POLYGON, p_curr_poly->vertices.size(), GL_UNSIGNED_INT, p_curr_poly->vertices.data());
		}
	}
	gldl_shutdown(p_gldl);
	return 0;
}