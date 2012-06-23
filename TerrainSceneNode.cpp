#include "dat.h"

TerrainSceneNode::TerrainSceneNode(Context *m) : ISceneNode(m->scenemgr->getRootSceneNode(), m->scenemgr, -1)
{
	bbox.reset(0, 0, 0);
	setPosition(core::vector3df());
	updateAbsolutePosition();

	scene::IAnimatedMesh *mesh = m->scenemgr->addHillPlaneMesh("ground",
		core::dimension2d<f32>(1000.0f, 1000.0f), core::dimension2d<u32>(100, 100));
	mesh->getMeshBuffer(0)->getMaterial().EmissiveColor.set(255, 33, 33, 33);

	for (int i=0; i < 3; i++) for (int j=0; j < 3; j++) {
		core::vector3df position((i-1)*100000, 0, (j-1)*100000);

		terrains[i][j] = m->scenemgr->addMeshSceneNode(mesh, this, -1, position);
		terrains[i][j]->updateAbsolutePosition();

		bbox.addInternalBox(terrains[i][j]->getTransformedBoundingBox());
	}

	//std::cerr << getBoundingBox().MaxEdge.X - getBoundingBox().MinEdge.X << std::endl;

	// create triangle selector for the terrain     
	scene::ITriangleSelector* selector = m->scenemgr->createTriangleSelectorFromBoundingBox(this);
	this->setTriangleSelector(selector);

	// create collision response animator and attach it to the camera
	scene::ISceneNodeAnimator* anim = m->scenemgr->createCollisionResponseAnimator(selector, m->camera);
	selector->drop();
	m->camera->addAnimator(anim);
	anim->drop();
}

TerrainSceneNode::~TerrainSceneNode()
{
	for (int i=0; i < 3; i++) for (int j=0; j < 3; j++)
		terrains[i][j]->remove();
}

void TerrainSceneNode::render()
{
	for (int i=0; i < 3; i++) for (int j=0; j < 3; j++)
		terrains[i][j]->render();
}

const core::aabbox3d<float>& TerrainSceneNode::getBoundingBox() const
{
	return bbox;
}

void TerrainSceneNode::centerOn(scene::ISceneNode *in, int x, int y) {
	if ((x == 1) && (y == 1))
		return;

//	std::cerr << x << " " << y << std::endl;

	core::vector3df position(in->getPosition());

	switch(x) {
	case 0:
		position.X += 100000;
		break;
	case 2:
		position.X -= 100000;
	default:
		break;
	}

	switch(y) {
	case 0:
		position.Z += 100000;
		break;
	case 2:
		position.Z -= 100000;
	default:
		break;
	}

	in->setPosition(position);
}

void TerrainSceneNode::shift(ISceneNode *in)
{
	float miny = terrains[1][1]->getTransformedBoundingBox().MinEdge.Y;
	float maxy = terrains[1][1]->getTransformedBoundingBox().MaxEdge.Y;
	float avgy = miny + (maxy - miny) / 2.f;
	core::vector3df point = in->getPosition();
	point.Y = avgy;

	int i = 0;
	for (; i < 9; i++)
	{
		if (terrains[i/3][i%3]->getTransformedBoundingBox().isPointInside(point))
		{
			break;
		}
	}

	int j = i%3;
	i /= 3;

	// not intersecting with any terrain(?)
	if (i == 3)
		return;

	centerOn(in, i, j);
}

core::vector3df TerrainSceneNode::getTerrainCenter() {
	return getBoundingBox().getCenter();
}

