#include <stdio.h>
extern "C" {
#include "../../common/camera.h"
#include "../../common/gldl.h"
};
#include "../../common/glmath.h"
#include "../../common/camera2.h"

//CCamera camera(45.f, vec3(0.f, 0.f, 0.f));
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

#define DTOR 0.0174532925    
#define RTOD 57.2957795 

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// camera Attributes
	vec3 Position;
	vec3 Front;
	vec3 Up;
	vec3 Right;
	vec3 WorldUp;
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// constructor with vectors
	Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = vec3(posX, posY, posZ);
		WorldUp = vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard()
	{
		float velocity = MovementSpeed;
		if (GetAsyncKeyState('W'))
			Position += Front * velocity;
		if (GetAsyncKeyState('S'))
			Position -= Front * velocity;
		if (GetAsyncKeyState('A'))
			Position -= Right * velocity;
		if (GetAsyncKeyState('D'))
			Position += Right * velocity;
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

	void Look(HWND hwnd)
	{
		POINT pt;
		RECT window_rect;
		GetWindowRect(hwnd, &window_rect);
		GetCursorPos(&pt);

		POINT cpt;
		cpt.x = (window_rect.right / 2.f);
		cpt.y = (window_rect.bottom / 2.f);
		float xdiff = pt.x - cpt.x;
		float ydiff = pt.y - cpt.y;

		ProcessKeyboard();
		ProcessMouseMovement(xdiff, -ydiff, true);
		
		vec3 dir = Position + Front;
		gluLookAt(Position.x, Position.y, Position.z, dir.x, dir.y, dir.z, 0.f, 1.f, 0.f);
		SetCursorPos(cpt.x, cpt.y);
	}

private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		vec3 front;
		front.x = cosf(Yaw * DTOR) * cosf(Pitch * DTOR);
		front.y = sinf(Pitch * DTOR);
		front.z = sinf(Yaw  * DTOR) * cosf(Pitch * DTOR);
		Front = normalize(front);
		// also re-calculate the Right and Up vector
		Right = normalize(cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = normalize(cross(Right, Front));
	}
};

gldl_dt_t *pwnd;
//Camera cam;

GLUquadric *quadric;

int main()
{


	if (!(pwnd = gldl_initialize(-1, -1, 800, 600, 24, 32, "ray sphere intersect", NULL))) {
		printf("Failed to create window!\n");
		return 1;
	}

	quadric = gluNewQuadric();

	glViewport(0, 0, pwnd->GetWindowWidth(), pwnd->GetWindowHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, pwnd->GetWindowWidth() / (double)pwnd->GetWindowHeight(), 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	camera2_t cam2;
	//memset(&cam2, 0, sizeof(cam2));
	camera2_init(&cam2, pwnd->GetWindow(), { 0, 10, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, { 0, 1, 0 }, 0.1f, 0.5f, 0);
	ShowCursor(FALSE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (pwnd->IsWindowOpened()) {
		if (pwnd->IsButtonPressedOne(27))
			break;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		camera2_update(&cam2);
		
		vec3 o = vec3(0.f, 10.f, 0.f);
		vec3 vec1 = vec3(0.f, 10.f, 10.f);
		vec3 vec2 = vec3(10.f, 10.f, 0.f);

		glLineWidth(2.f);
		glPushAttrib(GL_CURRENT_BIT);
		glColor3ub(0, 0, 255);
		glBegin(GL_LINES);

		glVertex3f(o.x, o.y, o.z);
		glVertex3f(vec1.x, vec1.y, vec1.z);
		
		glVertex3f(o.x, o.y, o.z);
		glVertex3f(vec2.x, vec2.y, vec2.z);

		ray_t ray;
		ray.pos = cam2.origin;
		ray.dir = cam2.direction;
		printf("%f %f %f\n", cam2.direction.x, cam2.direction.y, cam2.direction.z);

		vector3_t plane_origin = {0.f, 0.f, 0.f};
		vector3_t plane_normal = {0.f, 1.f, 0.f};
		//float k = ray_plane_intersect(&ray, &plane_origin, &plane_normal);
		//if (k >= 0.f) {
		//	vector3_t intersect_point;
		//	vec3_add(&intersect_point, &ray.pos, &ray.dir);
		//	vec3_mulsc(&intersect_point, &intersect_point, k);

		//	glBegin(GL_LINE);
		//	glVertex3f(intersect_point.x, 0.f, intersect_point.z);
		//	glVertex3f(intersect_point.x, 10.f, intersect_point.z);
		//	glEnd();
		//}
		
		glEnd();
		glPopAttrib();
		glLineWidth(1.f);

		pwnd->Draw3DAxises(2.f, 2.f);
		glColor4ub(255, 255, 255, 50);
		pwnd->Draw3DGird(0, 0, 0, 100, 5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		vector3_t intersect_point;
		if (PlaneIntersection5(&plane_origin, &plane_normal, &ray.pos, &ray.dir, &intersect_point)) {
			//intersect_point.x = roundf(intersect_point.x / 10.f) * 10.f;
			//intersect_point.y = roundf(intersect_point.y / 10.f) * 10.f;
			//intersect_point.z = roundf(intersect_point.z / 10.f) * 10.f;

			//glBegin(GL_LINES);
			//glVertex3f(0.f, 0.f, 0.f);
			//glVertex3f(intersect_point.x, intersect_point.y, intersect_point.z);
			//glEnd();

			glPushMatrix();
			glTranslatef(intersect_point.x, intersect_point.y, intersect_point.z);
			gluSphere(quadric, 0.2, 20, 20);
			glPopMatrix();
		}

		bouding_sphere_t sphere;
		sphere.pos = { 0.f, 0.f, 0.f };
		sphere.radius = 5.f;

		glPushAttrib(GL_CURRENT_BIT);
		
		if(ray_sphere_intersect(&ray, &sphere))
			glColor3ub(50, 50, 0);
		else glColor3ub(50, 50, 500);

		glPushMatrix();
		glTranslatef(sphere.pos.x, sphere.pos.y, sphere.pos.z);
		gluSphere(quadric, sphere.radius, 20, 20);
		glPopMatrix();
		glPopAttrib();
	}
	gldl_shutdown(pwnd);
	ShowCursor(TRUE);
	return 0;
}