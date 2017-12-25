#pragma once

#include "main.h"
#include "Block.h"
#include "Creator.h"
#include "OBLoader.h"
#include <thread>

#ifndef _NO_SCALE
#define _SCALE
#endif

#ifndef _BASIC_SHAPES
#define _ADVANCED_SHAPES
#endif

enum MoveType {
	MV_DIRECT,
	MV_SAFE,
	MV_SEPARATE
};

class Machine
{
private:
	struct Order;
	std::list<Order> _order_list;
	GLboolean _pause;
	draw::Point3D _block_size;
	Block *_block;
	GLfloat _block_mult;
	Creator *_creator;
	std::thread *_block_draw_th;

	struct Order {
		draw::Point3D pos;
		draw::Point3D uAxis;
		Order() {}
		Order(draw::Point3D pos_, draw::Point3D uAxis_) : pos(pos_), uAxis(uAxis_) {}
	};

	struct {
		GLfloat h;
		GLfloat max_r;
		GLfloat a;
		GLfloat b;
		GLfloat da;
		GLfloat db;
		GLfloat dR;
		GLfloat dr;
		GLfloat angle;
		GLfloat h_safe;
		GLfloat h_min;
		GLfloat min_r;
		inline GLfloat fullLen() {
			return max_r + dR + dr;
		}
	} _arm;

	struct {
		GLint n;
		GLfloat h;
		GLfloat r;
		GLfloat d;
	} _tower;

	struct {
		GLfloat a;
		GLfloat b;
		GLfloat h;
	} _engine;

	struct {
		GLfloat r;
		GLfloat hD;
		GLfloat a;
		GLfloat h;
		GLfloat H;
		GLfloat hH;
		GLfloat pin_max;
		GLfloat h_min;
		GLfloat h_safe;
	} _drill;

	struct {
		Order ord;
		GLfloat da;
		GLfloat dr;
		GLfloat dh;
		GLint speed_rate;

		GLboolean busy;
		GLboolean working;
	} _move;

	struct {
		GLuint floor;
		GLuint wall1;
		GLuint wall;
		GLuint clock;
	}_texture;

	struct {
		GLuint tsec;
		GLuint sec() { return tsec % 60; };
		GLuint minutes() { return tsec / 60; };
		GLuint sec_angle() { return 360 * (1 - (sec() / 60.0f)); }
		GLuint min_angle() { return 360 * (1 - (minutes() / 60.0f)); }
	}_clock;

public:
    Machine();
	~Machine();
	void allocBlock();

public:
    void draw(draw::Point3D rotate = draw::Point3D());
	void lowerArm(GLfloat dh);
	void rotateArm(GLfloat da);
	void retractArm(GLfloat dr);
private:
	void drawClock();
    void drawBase();
    void drawTower();
	void drawArm();
	void drawEngine();
	void drawDrill();
	void drawBlock();
public:
	void loadTextures();
private:
	void setStartUpPos();
public:
	draw::Point3D getPos();
private:
	draw::Point3D calcPos(GLfloat r, GLfloat angle);
public:
	void calcParam(GLfloat *r, GLfloat *angle, GLfloat *h, draw::Point3D pos);
	void moveTo(draw::Point3D pos, draw::Point3D uAxis = draw::Point3D(1, 1, 1), MoveType mt = MV_DIRECT);
private:
	void syncPos();
	void syncPosA(GLfloat v, GLfloat dMov, GLboolean &sync);
	void syncPosR(GLfloat v, GLfloat dMov, GLboolean &sync);
	void syncPosH(GLfloat v, GLfloat dMov, GLboolean &sync);
	public:
	GLfloat getFullArmLen();
	void setFullArmLen(GLfloat r);
	void execOrders();
	GLboolean getBusy();
	void setPause(GLboolean pause);
	GLboolean getPause();
	GLfloat getAngle();
	GLfloat getFL();
	draw::Point3D getDes();
	GLboolean getWorking();
	int getQueue();
	void resetQueue();
	void OpenCreator();
	GLint getSpeedRate();
	void setSpeedRate(GLint val);
	GLfloat getDrillR();
	void setDrillR(GLfloat r);
	GLfloat getBlockMult();
	void setBlockMult(GLfloat val);
	GLfloat getBlockHMult();
	void setBlockHMult(GLfloat val);
	void translateJob(std::vector<draw::Point3D> job);
	std::vector<GLfloat> transposeOX(std::vector<GLfloat> in);
	void tic();
	void resetClock();
	/* jobs */
	void doJobs();
};
