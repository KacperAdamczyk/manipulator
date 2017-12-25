#pragma once
#include "main.h"
#include "shapes.h"

class Block;

class DestroyEffect
{
	class Particle
	{
		draw::Point3D _pos0;
		draw::Point3D _posT;
		draw::Point3D _pos;
		draw::Point3D _c;
		GLfloat _angle;
		GLfloat _shape_angle;
		GLfloat _t;
		GLfloat _tmin;
		GLfloat _tmax;
		GLfloat _tinc;
		GLboolean _blocked;

		draw::Point3D(*_f)(GLfloat t, GLfloat angle, draw::Point3D c, draw::Point3D posT_1);
		GLfloat(*_dfz)(GLfloat t, GLfloat angle, draw::Point3D c, draw::Point3D posT_1);

		/* Shape */
		GLfloat _r;
		GLfloat _h;

	public:
		Particle(draw::Point3D pos, GLfloat angle, draw::Point3D c, GLfloat _tmin, GLfloat _tmax, GLfloat tinc, 
			     draw::Point3D(*_f)(GLfloat t, GLfloat angle, draw::Point3D c, draw::Point3D posT_1), 
				 GLfloat(*dfz)(GLfloat t, GLfloat angle, draw::Point3D c, draw::Point3D posT_1));
		void calcPos();
		void calcShapeAngle();
		void draw();
		GLboolean ifExist();
	};
	static Block *_block;
	std::vector<Particle> _particles;
	GLfloat _tmin;
	GLfloat _tmax;
	GLfloat _tinc;

public:
	DestroyEffect();
	~DestroyEffect();

	void addEffect(draw::Point3D pos, unsigned char d);
	void draw();
	static void setBlock(Block *block);

private:
	void cutToLimit();
};

