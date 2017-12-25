#include "Block.h"

Block::dBlock::dBlock(draw::Point3D pos_, draw::Point3D size_) : pos(pos_), size(size_), exist(true) {}

Block::dBlock::dBlock() {};

GLboolean Block::dBlock::contains(draw::Point3D p, draw::Point3D s)
{
	return ((pos.x - 0.5f * s.x * size.x) <= p.x && p.x <= (pos.x + 0.5f * s.x * size.x) &&
		(pos.y - 0.5f * s.y * size.y) <= p.y && p.y <= (pos.y + 0.5f * s.y * size.y) &&
		pos.z <= p.z && p.z <= (pos.z + s.z * size.z));
}

GLboolean Block::dBlock::intersects(draw::Point3D p, GLfloat r)
{
	if (!(pos.z <= p.z && p.z <= (pos.z + size.z))) return false;

	draw::Point3D circleDist(abs(p.x - pos.x), abs(p.y - pos.y), 0);

	if (circleDist.x > (size.x / 2 + r)) return false;
	if (circleDist.y > (size.y / 2 + r)) return false;

	if (circleDist.x <= (size.x / 2)) return true;
	if (circleDist.y <= (size.y / 2)) return true;

	GLfloat cornerDist_sq = (circleDist.x - size.x / 2) * (circleDist.x - size.x / 2) +
		(circleDist.y - size.y / 2) * (circleDist.y - size.y / 2);

	return (cornerDist_sq <= r * r);
}

Block::Block(draw::Point3D center_pos, draw::Point3D size, draw::Point3D quantity) : _center_pos(center_pos), _size(size), 
	_quantity(quantity), _h_mult(5.0f)
{
	_pos = _center_pos - _size * draw::Point3D(0.5f, 0.5f, 0.0f);
	_dsize = _size / quantity;

	alloc();
	DestroyEffect::setBlock(this);
}

Block::~Block()
{
}

void Block::alloc()
{
	draw::Point3D d;
	GLfloat len = _quantity.x * _quantity.y;
	dBlock db;
	for (int i = 0; i < len; i++) {
		db.pos = _pos + d + draw::Point3D(_dsize.x / 2, _dsize.y / 2, 0);
		db.size = _dsize * draw::Point3D(1, 1, _quantity.z);
		_blocks.push_back(db);

		d.x += _dsize.x;
		if (d.x >= _size.x) {
			d.x = 0;
			d.y += _dsize.y;
		}
		if (d.y >= _size.y) {
			d.y = 0;
			d.z += _dsize.z;
		}
	}
}

unsigned char Block::checkNeighbours(int i)
{
	unsigned char T = 0x1;
	unsigned char R = 0x2;
	unsigned char B = 0x4;
	unsigned char L = 0x8;
	unsigned char TR = 0x10;
	unsigned char BR = 0x20;
	unsigned char BL = 0x40;
	unsigned char TL = 0x80;
	unsigned char c = 0;
	/*
	*	TL T  TR
	*	L  x  R
	*	BL B  BR
	*/
	int q = _quantity.x * _quantity.y;
	int ind;
	dBlock b = _blocks[i];
	// T
	ind = i - _size.x;
	if (ind >= 0 && ind < q) {
		if (_blocks[ind].size.z <= b.size.z)
			c |= T;
	} else {
		c |= T;
	}
	// TR
	ind = i - _size.x + 1;
	if (ind >= 0 && ind < q) {
		if (_blocks[ind].size.z <= b.size.z)
			c |= TR;
	} else {
		c |= TR;
	}
	// R
	ind = i + 1;
	if (ind >= 0 && ind < q) {
		if (_blocks[ind].size.z <= b.size.z)
			c |= R;
	} else {
		c |= R;
	}
	// BR
	ind = i + _size.x + 1;
	if (ind >= 0 && ind < q) {
		if (_blocks[ind].size.z <= b.size.z)
			c |= BR;
	} else {
		c |= BR;
	}
	// B
	ind = i + _size.x;
	if (ind >= 0 && ind < q) {
		if (_blocks[ind].size.z <= b.size.z)
			c |= B;
	} else {
		c |= B;
	}
	// BL
	ind = i + _size.x - 1;
	if (ind >= 0 && ind < q) {
		if (_blocks[ind].size.z <= b.size.z)
			c |= BL;
	} else {
		c |= BL;
	}
	// L
	ind = i - 1;
	if (ind >= 0 && ind < q) {
		if (_blocks[ind].size.z <= b.size.z)
			c |= L;
	} else {
		c |= L;
	}
	// TL
	ind = i - _size.x - 1;
	if (ind >= 0 && ind < q) {
		if (_blocks[ind].size.z <= b.size.z)
			c |= TL;
	} else {
		c |= TL;
	}

	return c;
}

void Block::draw()
{
	for (GLint i = 0; i < _blocks.size(); i++) {
		dBlock &b = _blocks[i];
		if (b.exist) {
			if (b.intersects(_drill_pos, _drill_r)) {
				if (b.size.z != _drill_pos.z) {
					draw::Point3D pos(b.pos.x, b.pos.y, b.size.z);
					_destroy_effect.addEffect(pos, checkNeighbours(i));
				}
				b.size.z = _drill_pos.z;
				if (b.size.z <= 0) {
					b.exist = false;
				}
			}
		}
		if (!b.exist)
			continue;

		draw::Point3D bgn = b.pos;
		draw::Point3D norm;
		GLint n = 4;

		GLfloat m1_ambient[] = { 0.46f, 0.46f, 0.46f, 1.0f };
		GLfloat m1_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
		GLfloat m1_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
		GLfloat m1_shine = 89.0f;
		draw::setMaterial(m1_ambient, m1_diffuse, m1_specular, m1_shine);
		//glColor3f(S_RGB(197), S_RGB(183), S_RGB(167));
		draw::Point3D *points = draw::Rectangle(b.size.x, b.size.y, bgn, norm);
		draw::Polygon(points, draw::Point3D(0, 0, b.size.z), n, norm);

		GLfloat m2_ambient[] = { 0.28f, 0.28f, 0.28f, 1.0f };
		GLfloat m2_diffuse[] = { 0.4775f, 0.4775f, 0.4775f, 1.0f };
		GLfloat m2_specular[] = { 0.773911f, 0.773911f, 0.773911f, 1.0f };
		GLfloat m2_shine = 65.0f;
		draw::setMaterial(m2_ambient, m2_diffuse, m2_specular, m2_shine);
		//glColor3f(S_RGB(177), S_RGB(163), S_RGB(147));
		draw::SideWall(points, n, b.size.z);

		delete[] points;
	}

	/* Destroy effects */
	_destroy_effect.draw();
}

void Block::setDrillPos(draw::Point3D pos)
{
	_drill_pos = pos * draw::Point3D(-1.0f, -1.0f, 1.0f);
}

void Block::setDrillR(GLfloat r)
{
	_drill_r = r;
}

draw::Point3D Block::getDSize()
{
	return _dsize;
}

draw::Point3D Block::getCenterPos()
{
	return _center_pos;
}

draw::Point3D Block::getSize()
{
	return _size;
}

draw::Point3D Block::getQuantity()
{
	return _quantity;
}

GLfloat Block::getHMult()
{
	return _h_mult;
}

void Block::setHMult(GLfloat h)
{
	_h_mult = h;
}

draw::Point3D Block::L2Dto2D(GLint i)
{
	GLint x = i % GLint(_quantity.x);
	GLint y = i / GLint(_quantity.x);
	return draw::Point3D(x, y, 0);
}

draw::Point3D Block::L3Dto3D(GLint i)
{
	GLint x = (i % GLint(_quantity.x * _quantity.y)) % GLint(_quantity.x);
	GLint y = (i % GLint(_quantity.x * _quantity.y)) / GLint(_quantity.x);
	GLint z = i / GLint(_quantity.x * _quantity.y);
	return draw::Point3D(x, y, z);
}

GLint Block::L3DtoL2D(GLint i)
{
	return i % GLint(_quantity.x * _quantity.y);
}

GLint Block::_2DtoL2D(draw::Point3D pos)
{
	return GLint(pos.x) + GLint(pos.y) * GLint(_quantity.x);
}

GLboolean Block::isPosValid(draw::Point3D pos, const GLfloat r, GLint sign)
{
	bool found = false;
	int i = sign >= 0 ? 0 : _quantity.x * _quantity.y - 1;
	auto f = [&]()->bool { 
		if (sign >= 0)
			return i < _quantity.x * _quantity.y;
		else
			return i >= 0; 
	};
	for (i; f(); sign >= 0 ? i++ : i--) {
		/* normalizacja uk³adu odniesienia */
		if (_blocks[i].intersects(pos * draw::Point3D(-1.0f, -1.0f, 0), r)) {
			found = true;
			break;
		}
	}

	if (!found) return true;
	if (_blocks[i].size.z - _h_mult * _dsize.z <= pos.z) return true;

	return false;
}

void Block::setAllExist()
{
	for (dBlock &b : _blocks)
		b.exist = true;
}

std::vector<GLfloat> Block::createHeightMap()
{
	std::vector<GLfloat> arr;

	for (dBlock b : _blocks) {
		arr.push_back(b.size.z);
	}

	return arr;
}

Block::dBlock Block::at(int ind)
{
	return _blocks[ind];
}

draw::Point3D Block::Pos2DtoInd2D(draw::Point3D pos)
{
	return draw::Point3D ((pos.x + _size.x / 2) / _dsize.x, (pos.y + _size.y / 2) / _dsize.y, 0);
}

GLint Block::Pos2DtoInd(draw::Point3D pos)
{
	return _2DtoL2D(Pos2DtoInd2D(pos));
}
