#pragma once

#include "main.h"

#define S_RGB(x) ((x) / 255.0f)
#define D2R(x) ((x) * (M_PI/180.0f))
#define R2D(x) ((x) * (180.0f/M_PI))

namespace draw {
	struct Point3D {
		GLfloat x, y, z;
		inline Point3D(GLfloat x_ = 0, GLfloat y_ = 0, GLfloat z_ = 0) : x(x_), y(y_), z(z_) {}
		inline Point3D(GLfloat arr[3]) { set3fv(arr); }
		Point3D operator+(const Point3D &p) {
			return Point3D(x + p.x, y + p.y, z + p.z);
		}
		Point3D operator-(const Point3D &p) {
			return Point3D(x - p.x, y - p.y, z - p.z);
		}
		Point3D& operator+=(const Point3D &p) {
			x += p.x;
			y += p.y;
			z += p.z;
			return *this;
		}
		Point3D& operator-=(const Point3D &p) {
			x -= p.x;
			y -= p.y;
			z -= p.z;
			return *this;
		}
		Point3D operator*(const Point3D &p) {
			return Point3D(x * p.x, y * p.y, z * p.z);
		}
		Point3D& operator*=(const Point3D &p) {
			x *= p.x;
			y *= p.y;
			z *= p.z;
			return *this;
		}
		Point3D operator/(const Point3D &p) {
			if (p.x != 0 && p.y != 0 && p.z != 0)
				return Point3D(x / p.x, y / p.y, z / p.z);
			return Point3D();
		}
		bool operator==(const Point3D &p) {
			if (x == p.x && y == p.y && z == p.z)
				return true;
			return false;
		}
		bool operator!=(const Point3D &p) {
			return !(*this == p);
		}
		bool operator<(const Point3D &p) {
			return x < p.x && y < p.y && z < p.z;
		}
		bool operator<=(const Point3D &p) {
			return x <= p.x && y <= p.y && z <= p.z;
		}
		bool operator>(const Point3D &p) {
			return x > p.x && y > p.y && z > p.z;
		}
		bool operator>=(const Point3D &p) {
			return x >= p.x && y >= p.y && z >= p.z;
		}
		GLfloat val() {
			return x * y * z;
		}
		void set3fv(GLfloat arr[3]) {
			x = arr[0];
			y = arr[1];
			z = arr[2];
		}
		GLfloat* v() {
			GLfloat v[3];
			v[0] = x;
			v[1] = y;
			v[2] = z;
			return v;
		}
	};
	/* Koło */
	Point3D* Circle(GLfloat r = 10.0f, Point3D ctr = Point3D(0, 0, 0), GLint n = 16);
	void Circle(Point3D *points, Point3D ctr, GLfloat h, GLint n);
	/* Ściany boczne figur */
	void SideWall(Point3D *points, GLint n, GLfloat h, Point3D bgn = Point3D());
	Point3D* Rectangle(GLfloat a, GLfloat b, Point3D bgn, Point3D &normal,  GLboolean center = true, GLboolean draw = true);
	void Rectangle(GLfloat a, GLfloat b, Point3D bgn, GLboolean tex, GLuint tex_id = 0);
	void Circle(GLfloat r, GLfloat n, Point3D ctr, GLboolean tex, GLuint tex_id = 0);
	void Polygon(Point3D *points, Point3D bgn, GLint n, Point3D normal);

	void Axis(Point3D pos = Point3D());

	void setMaterial(GLfloat* ambient, GLfloat* diffuse, GLfloat* specular, GLfloat shine);
	
	void ReduceToUnit(float vector[3]);
	void calcNormal(float v[3][3], float out[3]);
	Point3D calcNormalQ(Point3D *v);
}