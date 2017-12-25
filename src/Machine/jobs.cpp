#include "machine.h"

void Machine::doJobs()
{
	using namespace draw;
	Point3D fa(1, 1, 1);
	Point3D s = _block->getSize();
	Point3D ds = _block->getDSize();
	Point3D c = _block->getCenterPos();
	Point3D q = _block->getQuantity();

	GLfloat gx = 1.5f * ds.x;
	GLfloat gy = 1.5f;
	GLfloat gz = 1.f;

	for (int j = 0; j < q.z - gz; j++) {
		for (int i = 1; i < q.y - gy; i += 2) {
			if (i == 1)
				moveTo(Point3D(s.x / 2 - gx, -s.y / 2 + ds.y * (i + 0.5f)   , s.z - ds.z / 2 - ds.z * j), fa, MV_SAFE);
			moveTo(Point3D( s.x / 2 - gx, -s.y / 2 + ds.y * (i + 0.5f), s.z - ds.z / 2 - ds.z * j), fa, MV_DIRECT);
			moveTo(Point3D(-s.x / 2 + gx, -s.y / 2 + ds.y * (i + 0.5f), s.z - ds.z / 2 - ds.z * j), fa, MV_DIRECT);
			moveTo(Point3D(-s.x / 2 + gx, -s.y / 2 + ds.y * (i + 1.5f), s.z - ds.z / 2 - ds.z * j), fa, MV_DIRECT);
			moveTo(Point3D( s.x / 2 - gx, -s.y / 2 + ds.y * (i + 1.5f), s.z - ds.z / 2 - ds.z * j), fa, MV_DIRECT);
		}
	}
	moveTo(Point3D(0, 0, 60), fa, MV_SAFE);
}