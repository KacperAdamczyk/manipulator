#pragma once

#include "main.h"
#include "shapes.h"
#include "DestroyEffect.h"

class Block
{
private:
	struct dBlock;
	draw::Point3D _center_pos; /* center */
	draw::Point3D _pos;        /* corner */
	draw::Point3D _size;
	draw::Point3D _dsize;
	draw::Point3D _quantity;
	GLfloat _h_mult;
	draw::Point3D _drill_pos;
	GLfloat _drill_r;
	std::vector<dBlock> _blocks;
	DestroyEffect _destroy_effect;

	struct dBlock {
		draw::Point3D pos;
		draw::Point3D size;
		GLboolean exist;

		dBlock(draw::Point3D pos_, draw::Point3D size_);
		dBlock();

		GLboolean contains(draw::Point3D p, draw::Point3D s = draw::Point3D(1.0f, 1.0f, 1.0f));
		inline GLboolean intersects(draw::Point3D p, GLfloat r);
	};

public:
	Block(draw::Point3D _center_pos, draw::Point3D _size, draw::Point3D quantity);
	~Block();
private:
	void alloc();
	unsigned char checkNeighbours(int i);
public:
	void draw();
	void setDrillPos(draw::Point3D pos);
	void setDrillR(GLfloat r);
	draw::Point3D getDSize();
	draw::Point3D getCenterPos();
	draw::Point3D getSize();
	draw::Point3D getQuantity();
	GLfloat getHMult();
	void setHMult(GLfloat h);
	draw::Point3D L2Dto2D(GLint i);
	draw::Point3D L3Dto3D(GLint i);
	GLint L3DtoL2D(GLint i);
	GLint _2DtoL2D(draw::Point3D pos);
	GLboolean isPosValid(draw::Point3D pos, const GLfloat r, GLint sign);
	void setAllExist();
	std::vector<GLfloat> createHeightMap();
	dBlock at(int ind);
	draw::Point3D Pos2DtoInd2D(draw::Point3D pos);
	GLint Pos2DtoInd(draw::Point3D pos);
};

