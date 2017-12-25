#include "DestroyEffect.h"
#include "Block.h"

Block * DestroyEffect::_block = nullptr;

DestroyEffect::DestroyEffect()
{
	_tmin = 0;
	_tmax = 500;
	_tinc = 0.5;
}

DestroyEffect::~DestroyEffect()
{
}

void DestroyEffect::addEffect(draw::Point3D pos, unsigned char d)
{
	auto f = [](GLfloat t, GLfloat angle, draw::Point3D c, draw::Point3D posT_1)->draw::Point3D {
		draw::Point3D p;
		p.x = c.x * t * sin(D2R(angle));
		p.y = c.y * t * cos(D2R(angle));
		p.z = -t*t + c.z * t;

		glColor3f(1.0f, 1.0f, S_RGB(rand(0, 255)));
		return p;
	};

	auto dfz = [](GLfloat t, GLfloat angle, draw::Point3D c, draw::Point3D posT_1)->GLfloat {
		return -2 * t + c.z;
	};
	GLfloat angle = rand(0, 360);
	GLint qmin = 2;
	GLint qmax = 5;
	GLint q = rand(qmin, qmax);

	GLint cxy_min = 4;
	GLint cxy_max = 8;

	GLint cz_min = 8;
	GLint cz_max = 17;

	for (GLint i = 0; i <= q; i++) {
		draw::Point3D c(rand(cxy_min, cxy_max), rand(cxy_min, cxy_max), rand(cz_min, cz_max));
		Particle p(pos, GLint(angle + (360 / q) * i), c, _tmin, _tmax, _tinc, f, dfz);
		_particles.push_back(p);
	}
}

void DestroyEffect::draw()
{
	cutToLimit();

	for (int i = 0; i < _particles.size(); i++) {
		if (!_particles[i].ifExist()) {
			_particles.erase(_particles.begin() + i);
			i--;
			continue;
		}
		_particles[i].calcPos();
		_particles[i].draw();
	}
}

void DestroyEffect::cutToLimit()
{
	GLint limit = 5000;
	GLint size = _particles.size();
	GLint diff = size - limit;
	if (diff <= 0)
		return;

	_particles.erase(_particles.begin(), _particles.begin() + diff);
}

DestroyEffect::Particle::Particle(draw::Point3D pos, GLfloat angle, draw::Point3D c, GLfloat tmin, GLfloat tmax, GLfloat tinc,
	draw::Point3D(*f)(GLfloat t, GLfloat angle, draw::Point3D c, draw::Point3D posT_1),
	GLfloat(*dfz)(GLfloat t, GLfloat angle, draw::Point3D c, draw::Point3D posT_1)) :
	_pos0(pos), _angle(angle), _c(c), _shape_angle(0), _t(tmin), _tmin(tmin), _tmax(tmax), _tinc(tinc), _f(f), _dfz(dfz)
{
	_r = 0.5f;
	_h = 1.5f;
	_blocked = false;
}

void DestroyEffect::Particle::calcPos()
{
	GLfloat h_real = 0.0f;
	GLfloat zdec = 0.8f;
	draw::Point3D ds = _block->getDSize();
	draw::Point3D s = _block->getSize();
	draw::Point3D q = _block->getQuantity();

	if (ifExist() && _pos.z > h_real && !_blocked)
		_posT = _f(_t, _angle, _c, _posT);

	draw::Point3D pos = _pos0 + _posT;
	GLint indthis = _block->Pos2DtoInd(_pos);
	draw::Point3D indnext2D = _block->Pos2DtoInd2D(pos);

	if (indnext2D.x >= q.x || indnext2D.y >= q.y || indnext2D.x < 0 || indnext2D.y < 0)
		goto skip;

	GLint indnext = _block->_2DtoL2D(indnext2D);
	if (_block->at(indnext).size.z > pos.z) {
		auto bthis = _block->at(indthis);
		pos = _pos;
		pos.z = (pos.z - zdec) < bthis.size.z ? bthis.size.z : pos.z - zdec;
		_blocked = true;
	}

	skip:
	if (pos.z < h_real)
		pos.z = h_real;

	_pos = pos;

	if (_t < _tmax)
		_t += _tinc;
}

void DestroyEffect::Particle::calcShapeAngle()
{
	_shape_angle = 90 - R2D(atan(_dfz(_t, _angle, _c, _pos)));
}

void DestroyEffect::Particle::draw()
{
	calcShapeAngle();

	glPushMatrix(); /* rotate */
	glTranslatef(_pos.x, _pos.y, _pos.z);
	glRotatef(_shape_angle * sin(D2R(_angle)), 1, 0, 0);
	glRotatef(_shape_angle * cos(D2R(_angle)), 0, 1, 0);

	GLfloat m1_ambient[] = { 0.25f, 0.25f, 0.25f };
	GLfloat m1_diffuse[] = { 0.4f, 0.4f, 0.4f };
	GLfloat m1_specular[] = { 0.774597f, 0.774597f, 0.774597f };
	GLfloat m1_shine = 76.8f;
	draw::setMaterial(m1_ambient, m1_diffuse, m1_specular, m1_shine);

	GLint n = 8;
	draw::Point3D bgn;
	draw::Point3D *points = draw::Circle(_r, bgn, n);
	draw::Circle(points, draw::Point3D(0, 0, _h), _h, n);

	GLfloat m2_ambient[] = { 0.23125f, 0.23125f, 0.23125f, 1.0f };
	GLfloat m2_diffuse[] = { 0.2775f, 0.2775f, 0.2775f, 1.0f };
	GLfloat m2_specular[] = { 0.773911f, 0.773911f, 0.773911f, 1.0f };
	GLfloat m2_shine = 89.6f;
	draw::setMaterial(m2_ambient, m2_diffuse, m2_specular, m2_shine);

	draw::SideWall(points, n, _h);
	delete[] points;

	glPopMatrix();
}

GLboolean DestroyEffect::Particle::ifExist()
{
	return _t < _tmax;
}

void DestroyEffect::setBlock(Block *block)
{
	_block = block;
}