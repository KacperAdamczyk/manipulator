#include "machine.h"

Machine::Machine() : _block_mult(5.0f), _pause(false), _block_size(120, 80, 60), _block(nullptr)
{
	_tower.n = 36;
	_tower.h = 120.0f;
	_tower.r = 12.0f;
	_tower.d = 130.0f;

	_arm.h = 100.0f;
	_arm.max_r = 55.0f;
	_arm.a = 15.0f;
	_arm.b = 15.0f;
	_arm.da = 4.0f;
	_arm.db = 4.0f;
	_arm.dR = _arm.max_r;
	_arm.dr = _arm.max_r;
	_arm.angle = 0.0f;
	_arm.min_r = 1.0f;

	_engine.a = _arm.a;
	_engine.b = _arm.b;
	_engine.h = _arm.a;

	_drill.r = 1.25f;
	_drill.hD = 14.0f;
	_drill.a = 1.5f;
	_drill.h = 1.0f;
	_drill.H = 60.0f;
	_drill.hH = _drill.h + _drill.H;

	_move.da = 0.25f;
	_move.dr = 0.25f;
	_move.dh = 0.5f;
	_move.speed_rate = 2;
	_move.ord.pos = draw::Point3D(0, 0, 70);
	_move.ord.uAxis = draw::Point3D(1, 1, 1);

	_move.busy = true;
	_move.working = true;

	/* zale¿noœci */
	_arm.h_min = _block_size.z + _drill.h + _arm.b / 2;
	_arm.h_safe = _arm.h_min + _drill.hD;
	_drill.pin_max = _arm.h_min - _drill.hD -_engine.b / 2;
	_drill.h_min = _block_size.z - _drill.hD;
	_drill.h_safe = _drill.h_min + _drill.hD + _drill.h;

	_clock.tsec = 0;

	allocBlock();
	setStartUpPos();
}

Machine::~Machine()
{
	if (_block != nullptr)
		delete _block;
	if (_creator != nullptr)
		delete _creator;
}

void Machine::allocBlock()
{
	if (_block != nullptr)
		delete _block;
	_block = new Block(draw::Point3D(0, 0, 0), _block_size, draw::Point3D(12 * _block_mult, 8 * _block_mult, 6 * _block_mult));
	_block->setAllExist();
}

void Machine::draw(draw::Point3D rotate)
{
	
	GLfloat nRange = 100.0f;
	GLfloat lightPos0[] = { 0.0f, 0.0f, 150.0f, 1.0f };
	GLfloat lightPos1[] = { 0.0f, 50.0f, 300.0f, 1.0f };
	GLfloat lightPos2[] = { 0.0f, 0.0f, 150.0f, 1.0f };
	GLfloat lightPos00[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	drawClock();
	glPushMatrix(); /* Translate (everything down) */
	glTranslatef(0, -nRange / 1.5f, -200.0f);
    glPushMatrix(); /* Rotate (everything) */
    glRotatef(-rotate.x, 1, 0, 0);
    glRotatef(-rotate.y, 0, 1, 0);
    glRotatef(-rotate.z, 0, 0, 1);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	glPopMatrix();
	glLightfv(GL_LIGHT2, GL_POSITION, lightPos2);
    drawBase();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f); /* Kolor swiatla */
	drawBlock();
	//draw::Axis();
    glPushMatrix(); /* Translate (tower) */
    glTranslatef(0, _tower.d, 0);
    drawTower();
	glPushMatrix(); /* Translate, Rotate (arm) */
	glRotatef(-_arm.angle, 0, 0, 1);
	glTranslatef(0, -_tower.r, _arm.h);
	glRotatef(90, 1, 0, 0);
	drawArm();
	glPushMatrix(); /* Translate (engine) */
	glTranslatef(0, 0, _arm.fullLen());
	drawEngine();
	glPushMatrix(); /* Rotate, Translate (drill) */
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, -_engine.h / 2, -_engine.b / 2);
	drawDrill();
	glPopMatrix(); /* Rotate, Translate (drill) */
	glPopMatrix(); /* Translate (engine) */
	glPopMatrix(); /* Rotate (arm) */
	glPopMatrix(); /* Translate, Rotate (base) */
    glPopMatrix(); /* Rotate (everything) */
	glPopMatrix(); /* Translate (everything down) */
}

void Machine::lowerArm(GLfloat dh)
{
	/* if designation is allowed */
	draw::Point3D pos = getPos();
	if (pos.z + dh < 0) 
		dh = -pos.z;
	if (!_block->isPosValid(pos + draw::Point3D(0, 0, dh), _drill.r, 1))
		return;

	GLfloat arm_max = _tower.h - _arm.a / 2 - 5.0f;
	GLfloat min = 0.0f;

	if (dh > 0) {
		if (_drill.h - dh >= min)
			_drill.h -= dh;
		else if (_arm.h + dh <= arm_max)
			_arm.h += dh;
	}
	if (dh < 0) {
		if (_arm.h + dh >= _arm.h_min)
			_arm.h += dh;
		else if (_drill.h - dh <= _drill.pin_max)
			_drill.h -= dh;
	}
}

void Machine::rotateArm(GLfloat da)
{
	/* if designation is allowed */
	if (!_block->isPosValid(calcPos(getFullArmLen(), _arm.angle + da), _drill.r, da >= 0 ? 1 : -1))
		return;

	GLfloat max = 90.f;
	GLfloat min = -90.0f;
	if (_arm.angle + da <= max && _arm.angle + da >= min)
		_arm.angle += da;
}

void Machine::retractArm(GLfloat dr)
{
	/* if designation is allowed */
	if (!_block->isPosValid(calcPos(getFullArmLen() + dr, _arm.angle), _drill.r, dr >= 0 ? 1 : -1))
		return;

	if (dr > 0) {
		if (_arm.dR + dr <= _arm.max_r)
			_arm.dR += dr;
		else if (_arm.dr + dr <= _arm.max_r)
			_arm.dr += dr;
	}
	if (dr < 0) {
		if (_arm.dr + dr >= _arm.min_r)
			_arm.dr += dr;
		else if (_arm.dR + dr >= _arm.min_r)
			_arm.dR += dr;
	}
}

void Machine::drawClock()
{
	draw::Point3D ctr(160, -70, 140);
	GLfloat r = 25;
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	draw::Circle(r, 32, ctr, true, _texture.clock);
	glColor3f(0, 0, 0);
	float ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float diffuse[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shine = 0;
	draw::setMaterial(ambient, diffuse, specular, shine);
	draw::Circle(r + 2, 32, ctr, false);
	float ambient_2[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float diffuse_2[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float specular_2[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float shine_2 = 100.0f;
	draw::setMaterial(ambient_2, diffuse_2, specular_2, shine_2);
	glPushMatrix();
	glTranslatef(ctr.x, ctr.y, ctr.z + 1);
	glPushMatrix();
	glRotatef(_clock.sec_angle(), 0, 0, 1);
	glTranslatef(0, (r - 1) / 2, 0);
	draw::Rectangle(1.5f, r - 1, draw::Point3D(), false);
	glPopMatrix();
	glRotatef(_clock.min_angle(), 0, 0, 1);
	glTranslatef(0, (r - 7) / 2, 0);
	draw::Rectangle(2.0f, r - 7, draw::Point3D(), false);
	glPopMatrix();
	draw::Circle(1.5f, 8, ctr + draw::Point3D(0, 0, 2), false);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void Machine::drawBase()
{
	float ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float shine = 120.8f;
	draw::setMaterial(ambient, diffuse, specular, shine);

	//glColor3f(S_RGB(255), S_RGB(255), S_RGB(255));
	draw::Point3D bgn;
	GLfloat x = 800;
	GLfloat y = 800;
    draw::Rectangle(x, y, bgn, true, _texture.floor);
	draw::Rectangle(x, y, draw::Point3D(0, 0, y / 2), true, _texture.floor);
	glPushMatrix();
	glRotatef(-90.0f, 0, 1, 0);
	glRotatef(-90.0f, 0, 0, 1);
	glPushMatrix();
	glTranslatef(0, y / 4, -x / 2);
	draw::Rectangle(x, y / 2, bgn, true, _texture.wall);
	glPopMatrix();
	glTranslatef(0, y / 4, x / 2);
	glRotatef(180.0f, 0, 1, 0);
	draw::Rectangle(x, y / 2, bgn, true, _texture.wall);
	glPopMatrix();
	glPushMatrix();
	glRotatef(90.0f, 1, 0, 0);
	glPushMatrix();
	glTranslatef(0, y / 4, -x / 2);
	draw::Rectangle(x, y / 2, bgn, true, _texture.wall);
	glPopMatrix();
	glTranslatef(0, y / 4, x / 2);
	draw::Rectangle(x, y / 2, bgn, true, _texture.wall);
	glPopMatrix();
}

void Machine::drawTower()
{
	static OBLoader t("resources/tower.obj");
	t.draw();
}

void Machine::drawArm()
{
	static OBLoader a1("resources/arm1.obj");
	static OBLoader a2("resources/arm2.obj");
	static OBLoader a3("resources/arm3.obj");

	float mat_ambient[] = { 0.67f,0.67f,0.67f,1.0f };
	float mat_diffuse[] = { 0.89f,0.89f,0.89f,1.0f };
	float mat_specular[] = { 0.93f,0.93f,0.93f,1.0f };
	float shine = 128.0f;
	glPushMatrix();
	a1.draw(true);
	glTranslatef(0.0f, 0.0f, _arm.dR);
	draw::setMaterial(mat_ambient, mat_diffuse, mat_specular, shine);
	a2.draw(false);
	glTranslatef(0.0f, 0.0f, _arm.dr);
	a3.draw(false);
	glPopMatrix();
}

void Machine::drawEngine()
{
	static OBLoader e("resources/engine.obj");
	e.draw();
}

void Machine::drawDrill()
{
	static OBLoader b("resources/bolt.obj");
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -_drill.h);
	b.draw();

	float m1_ambient[] = { 0.48f, 0.4f, 0.14f, 1.0f };
	float m1_diffuse[] = { 0.95164f, 0.80648f, 0.42648f, 1.0f };
	float m1_specular[] = { 0.928281f, 0.855802f, 0.666065f, 1.0f };
	float m1_shine = 128.0f;
	draw::setMaterial(m1_ambient, m1_diffuse, m1_specular, m1_shine);

	draw::Point3D bgn;
	GLint n = 32;
	draw::Point3D *points = draw::Circle(_drill.r, bgn, n);
	draw::Circle(points, bgn, -_drill.hD, n);
	draw::SideWall(points, n, -_drill.hD);
	delete[] points;
	glPopMatrix();
}

void Machine::drawBlock()
{
	_block->setDrillR(_drill.r);
	_block->setDrillPos(getPos());
	_block->draw();
}

void Machine::loadTextures()
{

	BITMAPINFOHEADER	bitmapInfoHeader;
	unsigned char*		bitmapData;

	glGenTextures(1, &_texture.floor);
	bitmapData = LoadBitmapFile("resources/floor.bmp", &bitmapInfoHeader);
	glBindTexture(GL_TEXTURE_2D, _texture.floor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);
	if (bitmapData)
		free(bitmapData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glGenTextures(1, &_texture.wall);
	bitmapData = LoadBitmapFile("resources/wall.bmp", &bitmapInfoHeader);
	glBindTexture(GL_TEXTURE_2D, _texture.wall);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);
	if (bitmapData)
		free(bitmapData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glGenTextures(1, &_texture.clock);
	bitmapData = LoadBitmapFile("resources/clock_face.bmp", &bitmapInfoHeader);
	glBindTexture(GL_TEXTURE_2D, _texture.clock);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);
	if (bitmapData)
		free(bitmapData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void Machine::setStartUpPos()
{
	_arm.dR = _arm.dr = _arm.min_r;
	_arm.angle = 45;
	_arm.h = _arm.h_safe;
}

draw::Point3D Machine::getPos()
{
	return calcPos(getFullArmLen(), _arm.angle);
}

draw::Point3D Machine::calcPos(GLfloat r, GLfloat angle)
{
	GLfloat relative_r = r - _tower.d;
	angle = D2R(angle);
	GLfloat x = r * sin(angle);
	GLfloat y = r * cos(angle) - _tower.d;
	GLfloat z = _arm.h - _arm.b / 2 - _drill.h - _drill.hD;
	return draw::Point3D(x, y, z);
}

void Machine::calcParam(GLfloat *r, GLfloat *angle, GLfloat *h, draw::Point3D pos)
{
	GLfloat y = _tower.d + pos.y;
	*r = sqrt(pos.x * pos.x + y * y);
	*angle = atan2(y, pos.x);
	*h = pos.z + _arm.b / 2 + _drill.h + _drill.hD;
	*angle = 90 - R2D(*angle);
}

void Machine::moveTo(draw::Point3D pos, draw::Point3D uAxis, MoveType mt)
{
	switch (mt) {
	case MV_DIRECT:
		_order_list.push_front(Order(pos, uAxis));
		break;
	case MV_SAFE:
		if ((_drill.h + _drill.hD) <= _drill.h_safe)
			_order_list.push_front(Order(draw::Point3D(0, 0, _drill.h_safe), draw::Point3D(0, 0, 1)));
		_order_list.push_front(Order(draw::Point3D(pos.x, pos.y, 0), draw::Point3D(1, 1, 0)));
		_order_list.push_front(Order(draw::Point3D(0, 0, pos.z), draw::Point3D(0, 0, 1)));
		break;
	case MV_SEPARATE:
		_order_list.push_front(Order(draw::Point3D(pos.x, 0, 0), draw::Point3D(1, 0, 0)));
		_order_list.push_front(Order(draw::Point3D(0, pos.y, 0), draw::Point3D(0, 1, 0)));
		_order_list.push_front(Order(draw::Point3D(0, 0, pos.z), draw::Point3D(0, 0, 1)));
	}
}

void Machine::syncPos()
{
	static GLboolean sR = false;
	static GLboolean sAngle = false;
	static GLboolean sH = false;

	if (!_move.working)
		return;
	_move.busy = true;

	if (!_move.ord.uAxis.x)
		sAngle = true;
	if (!_move.ord.uAxis.y)
		sR = true;
	if (!_move.ord.uAxis.z)
		sH = true;

	GLfloat r, angle, h;
	calcParam(&r, &angle, &h, _move.ord.pos);

	if (sAngle && sR && sH) {
		_move.busy = false;

		if (_move.ord.uAxis.x != 0.0f)
			_arm.angle = angle;
		if (_move.ord.uAxis.y != 0.0f)
			setFullArmLen(r);

		sAngle = sR = sH = false;

		return;
	}

	draw::Point3D mov;
	for (int i = 1; i <= _move.speed_rate; i++) {
		if (_move.ord.uAxis.x != 0) {
			mov = _move.ord.pos - getPos() * _move.ord.uAxis;
			syncPosA(mov.x, _move.da, sAngle);
		}
		if (_move.ord.uAxis.y != 0) {
			mov = _move.ord.pos - getPos() * _move.ord.uAxis;
			syncPosR(mov.y, _move.dr, sR);
		}
		if (_move.ord.uAxis.z != 0) {
			mov = _move.ord.pos - getPos() * _move.ord.uAxis;
			syncPosH(mov.z, _move.dh, sH);
		}
	}
}

void Machine::syncPosA(GLfloat v, GLfloat dMov, GLboolean &sync)
{
	GLfloat cInd = 0.5f;
	if (abs(v) < 2 * dMov) {
		dMov = abs(v * cInd);
		sync = true;
	} else {
		sync = false;
	}
	rotateArm((v / abs(v)) * dMov);
}

void Machine::syncPosR(GLfloat v, GLfloat dMov, GLboolean &sync)
{
	GLfloat cInd = 0.8f;
	if (abs(v) < 2 * dMov) {
		dMov = abs(v * cInd);
		sync = true;
	} else {
		sync = false;
	}
	retractArm((v / abs(v)) * dMov);
}

void Machine::syncPosH(GLfloat v, GLfloat dMov, GLboolean & sync)
{
	GLfloat cInd = 0.5f;
	if (abs(v) < dMov) {
		dMov = abs(v * cInd);
		sync = true;
	} else {
		sync = false;
	}
	lowerArm((v / abs(v)) * dMov);
}

GLfloat Machine::getFullArmLen()
{
	return _tower.r + _arm.fullLen() + _arm.b / 2;
}

void Machine::setFullArmLen(GLfloat r)
{
	retractArm(r - getFullArmLen());
}

void Machine::execOrders()
{
	if (_creator != nullptr) {
		if (_creator->getJobReady())
			translateJob(_creator->getJob());
	}

	if ((_order_list.size() == 0 && !_move.busy) || _pause) {
		_move.working = false;
		return;
	}
	_move.working = true;
	
	if (!_move.busy && _order_list.size() > 0) {
		_move.ord = _order_list.back();
		_order_list.pop_back();
	}
	syncPos();
}

GLboolean Machine::getBusy()
{
	return _move.busy;
}

void Machine::setPause(GLboolean pause)
{
	_pause = pause;
}

GLboolean Machine::getPause()
{
	return _pause;
}

GLfloat Machine::getAngle()
{
	return _arm.angle;
}

GLfloat Machine::getFL()
{
	return _arm.fullLen();
}

draw::Point3D Machine::getDes()
{
	return _move.ord.pos;
}

GLboolean Machine::getWorking()
{
	return _move.working;
}

int Machine::getQueue()
{
	return _order_list.size();
}

void Machine::resetQueue()
{
	_order_list.clear();
}

void Machine::OpenCreator()
{
	if (_creator != nullptr)
		delete _creator;
	_creator = new Creator(_block->getQuantity(), transposeOX(_block->createHeightMap()),
							_block->getDSize().z * _block->getQuantity().z);
}

GLint Machine::getSpeedRate()
{
	return _move.speed_rate;
}

void Machine::setSpeedRate(GLint val)
{
	_move.speed_rate = val;
}

GLfloat Machine::getDrillR()
{
	return _drill.r;
}

void Machine::setDrillR(GLfloat r)
{
	_drill.r = r;
}

GLfloat Machine::getBlockMult()
{
	return _block_mult;
}

void Machine::setBlockMult(GLfloat val)
{
	_block_mult = val;
}

GLfloat Machine::getBlockHMult()
{
	return _block->getHMult();
}

void Machine::setBlockHMult(GLfloat val)
{
	_block->setHMult(val);
}

void Machine::translateJob(std::vector<draw::Point3D> job)
{
	using draw::Point3D;
	Point3D fa(1, 1, 1);
	Point3D s = _block->getSize();
	Point3D ds = _block->getDSize();

	for (Point3D p : job) {
		moveTo(Point3D(s.x / 2 - (0.5 + p.x) * ds.x, -s.y / 2 + (0.5 + p.y) * ds.y, p.z), fa, MV_SAFE);
	}
	moveTo(Point3D(0, 0, 70), fa, MV_SAFE);
}

std::vector<GLfloat> Machine::transposeOX(std::vector<GLfloat> in)
{
	GLint len = in.size();
	draw::Point3D s = _block->getQuantity();
	std::vector<GLfloat> out;
	out.resize(len, 0);
	
	for (GLint i = 0; i < len; i++) {
		draw::Point3D p2D = _block->L2Dto2D(i);
		p2D.y = s.y - p2D.y - 1;
		GLint ind = _block->_2DtoL2D(p2D);
		out[_block->_2DtoL2D(p2D)] = in[i];
	}

	return out;
}

void Machine::tic()
{
	static GLuint before = time(NULL);
	GLuint now = time(NULL);
	if (_move.working)
		_clock.tsec += now - before;
	before = now;
}

void Machine::resetClock()
{
	_clock.tsec = 0;
}
