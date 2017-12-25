#include "shapes.h"


draw::Point3D* draw::Circle(GLfloat r, Point3D ctr, GLint n)
{
    Point3D *points = new Point3D [n];

    for (int i = 0; i < n; i++) {
		GLfloat angle = GLfloat(2 * M_PI * ((GLfloat)i / n));
        points[i] = Point3D(r * cos(angle) + ctr.x, r * sin(angle) + ctr.y, ctr.z);
    }
    Circle(points, ctr, 0, n);

    return points;
}

void draw::Circle(Point3D * points, Point3D ctr, GLfloat h, GLint n)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(ctr.x, ctr.y, ctr.z);
	for (int i = 0; i <= n; i++) {
		glVertex3f(points[i % n].x, points[i % n].y, points[i % n].z + h);
	}
    glEnd();
}

void draw::SideWall(Point3D *points, GLint n, GLfloat h, draw::Point3D bgn)
{
    for (int i = 0; i < n; i++) {
        glBegin(GL_QUADS);
        int in = (i + 1) % n;
		Point3D v[4];
		v[0] = Point3D(points[i].x + bgn.x, points[i].y + bgn.y, points[i].z + bgn.z);
		v[1] = Point3D(points[i].x + bgn.x, points[i].y + bgn.y, points[i].z + h);
		v[2] = Point3D(points[in].x + bgn.x, points[in].y + bgn.y, points[in].z + h);
		v[3] = Point3D(points[in].x + bgn.x, points[in].y + bgn.y, points[in].z + bgn.z);
        glVertex3f(v[0].x, v[0].y, v[0].z);
        glVertex3f(v[1].x, v[1].y, v[1].z);
        glVertex3f(v[2].x, v[2].y, v[2].z);
        glVertex3f(v[3].x, v[3].y, v[3].z);
		Point3D n = calcNormalQ(v);
		glNormal3fv(n.v());
        glEnd();
    }
}

draw::Point3D * draw::Rectangle(GLfloat a, GLfloat b, Point3D bgn, Point3D &out_normal, GLboolean center, GLboolean draw)
{
    GLint n = 4;
    Point3D *points = new Point3D [n];
	points[0] = center ? bgn + Point3D(-a / 2, -b / 2, 0) : bgn;
	points[1] = center ? bgn + Point3D(-a / 2, b / 2, 0) : bgn + Point3D(0, b, 0);
	points[2] = center ? bgn + Point3D(a / 2, b / 2, 0) : bgn + Point3D(a, b, 0);
    points[3] = center ? bgn + Point3D(a / 2, -b / 2, 0) : bgn + Point3D(a, 0, 0);

	out_normal = calcNormalQ(points);
	if (draw)
		Polygon(points, Point3D(), n, out_normal);

    return points;
}

void draw::Polygon(Point3D *points, Point3D bgn, GLint n, Point3D normal)
{
	Point3D point;
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++) {
	    point = points[i] + bgn;
		glVertex3f(point.x, point.y, point.z);
	}
	glNormal3fv(normal.v());
	glEnd();
}

void draw::Rectangle(GLfloat a, GLfloat b, Point3D bgn, GLboolean tex, GLuint tex_id)
{
	Point3D points[4];
	points[0] =  bgn + Point3D(-a / 2, -b / 2, 0);
	points[1] =  bgn + Point3D(-a / 2, b / 2, 0);
	points[2] =  bgn + Point3D(a / 2, b / 2, 0);
	points[3] =  bgn + Point3D(a / 2, -b / 2, 0);

	Point3D normal = calcNormalQ(points);

	if (tex) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex_id);
	}
	glBegin(GL_QUADS);
	glNormal3fv(normal.v());
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(points[0].v());
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(points[1].v());
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(points[2].v());
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(points[3].v());
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void draw::Circle(GLfloat r, GLfloat n, Point3D ctr, GLboolean tex, GLuint tex_id)
{
	if (tex) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex_id);
	}

	Point3D *points = new Point3D[n];
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++) {
		GLfloat angle = GLfloat(2 * M_PI * ((GLfloat)i / n));
		GLfloat s = sin(angle);
		GLfloat c = cos(angle);
		GLfloat x = r * c + ctr.x;
		GLfloat y = r * s + ctr.y;
		glTexCoord2f((c + 1) / 2, (s + 1) / 2);
		glVertex3f(x, y, ctr.z);
		
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	delete points;
}

void draw::Axis(Point3D pos)
{
	glColor3f(1.0f, 1.0f, 0);
	glBegin(GL_LINES);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(-100, pos.y, pos.z);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(pos.x, -100, pos.z);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(pos.x, pos.y, 200);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(100, pos.y, pos.z);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(pos.x, 100, pos.z);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(pos.x, pos.y, -200);
	glEnd();

}

void draw::setMaterial(GLfloat * ambient, GLfloat * diffuse, GLfloat * specular, GLfloat shine)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
}

void draw::ReduceToUnit(float vector[3])
{
	float length;
	// Calculate the length of the vector		
	length = (float)sqrt((vector[0] * vector[0]) +
		(vector[1] * vector[1]) +
		(vector[2] * vector[2]));
	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if (length == 0.0f)
		length = 1.0f;
	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}
// Points p1, p2, & p3 specified in counter clock-wise order
void draw::calcNormal(float v[3][3], float out[3])
{
	float v1[3], v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;
	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];
	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y] * v2[z] - v1[z] * v2[y];
	out[y] = v1[z] * v2[x] - v1[x] * v2[z];
	out[z] = v1[x] * v2[y] - v1[y] * v2[x];
	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
}

draw::Point3D draw::calcNormalQ(Point3D *v)
{
	Point3D v1 = Point3D(v[1]) - Point3D(v[0]);
	Point3D v2 = Point3D(v[2]) - Point3D(v[0]);
	
	//Point3D v1(v[0].x - v[1].x, v[0].y - v[1].y, v[0].z - v[1].z);
	//Point3D v2(v[1].x - v[2].x, v[1].y - v[2].y, v[1].z - v[2].z);
	Point3D n(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
	GLfloat nv[] = { n.x, n.y, n.z };
	ReduceToUnit(nv);
	Point3D nn;
	nn.set3fv(nv);
	return nn  * Point3D(1, 1, -1);
}
