#include "dat.h"
#include <iostream>

class FileSceneNodeCollisionCallback : public scene::ICollisionCallback {
private:
	FileSceneNode *caller;

public:
	FileSceneNodeCollisionCallback(FileSceneNode *c) {
		caller = c;
	}

	bool onCollision(const scene::ISceneNodeAnimatorCollisionResponse &animator) {
		caller->collided();

		return true;
	}
};

FileSceneNode::FileSceneNode(io::IFileList *list, u32 index, core::vector3df &offset, scene::ISceneNode *parent, Context *m) : scene::ISceneNode(parent, m->scenemgr, -1)
{
	videoPlayer = NULL;
	anim = NULL;

	io::IFileSystem *fs = m->device->getFileSystem();
	file = fs->createAndOpenFile(list->getFileName(index));

	if (!file) {
		file = new EReadFile(list->getFileName(index));
	}

	core::vector3df position = offset;
	height = (float)(1.0 + file->getSize()/65536.0);
	if (height > 5.0f)
		height = 5.0f;
	position.Y += height * 20.0f;

	setPosition(position);
	updateAbsolutePosition();

	cube = m->scenemgr->addCubeSceneNode(40, this, 1, core::vector3df(), core::vector3df(),
		core::vector3df(1, height, 1));

	cube->setMaterialFlag(video::EMF_ANTI_ALIASING, video::EAAM_FULL_BASIC);
	
	scene::ITriangleSelector *selector = m->scenemgr->createTriangleSelectorFromBoundingBox(cube);
	cube->setTriangleSelector(selector);

	anim = m->scenemgr->createCollisionResponseAnimator(
		selector, m->camera, core::vector3df(10,80,10), core::vector3df(0,0,0));
	selector->drop();

	anim->setCollisionCallback(new FileSceneNodeCollisionCallback(this));
	m->camera->addAnimator(anim);
	anim->drop();

	core::stringw absname(file->getFileName());
	s32 lastslash = absname.findLast('/') + 1;
	s32 len = absname.size();
	core::stringw fname(absname.subString(lastslash, len - lastslash));

	isopen = false;
	isadir = false;
	if ((index == -1) || (file->getSize() == -1))
		cube->getMaterial(0).EmissiveColor.set(255, 255, 0, 0);
	else if (list->isDirectory(index)) {
		cube->getMaterial(0).EmissiveColor.set(255, 0, 0, 255);
		isadir = true;
	} else if (isimg(fname))
		cube->getMaterial(0).EmissiveColor.set(255, 133, 133, 55);
	else if (isvid(fname))
		cube->getMaterial(0).EmissiveColor.set(255, 200, 55, 55);
	else if (isanimesh(fname))
		cube->getMaterial(0).EmissiveColor.set(255, 0, 0, 0);
	else
		cube->getMaterial(0).EmissiveColor.set(255, 55, 55, 55);

	label = m->scenemgr->addBillboardTextSceneNode(NULL, fname.c_str(), this, 
		core::dimension2d<f32>(fname.size() * 10.0f, 10.0f),
		core::vector3df(0, height * 40.0f, 0), -1);

	m_this = m;
}

FileSceneNode::~FileSceneNode()
{
//	std::cout << "delete FileSceneNode\n";
	if (videoPlayer)
		videoPlayer->close();
	s32 index = m_this->videos.binary_search(this);
	if (index != -1)
		m_this->videos.erase(index);
	removeAll();
}

void FileSceneNode::render()
{
	cube->render();
	label->render();
}

const core::aabbox3d<float>& FileSceneNode::getBoundingBox() const
{
	return cube->getBoundingBox();
}

bool FileSceneNode::isvid(core::stringc name) {
	if ((name.subString(name.size() - 4, 4).equals_ignore_case(".avi")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".mpg")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".mkv")) ||
	    (name.subString(name.size() - 3, 3).equals_ignore_case(".rm")) ||
	    (name.subString(name.size() - 5, 5).equals_ignore_case(".mpeg")) ||
	    (name.subString(name.size() - 5, 5).equals_ignore_case(".divx")))
		return true;
	return false;
}

bool FileSceneNode::isimg(core::stringc name) {
	if ((name.subString(name.size() - 4, 4).equals_ignore_case(".bmp")) || 
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".jpg")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".tga")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".pcx")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".png")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".psd")))
		return true;
	return false;
}

bool FileSceneNode::isanimesh(core::stringc name) {
	if ((name.subString(name.size() - 4, 4).equals_ignore_case(".md2")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".md3")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".obj")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".bsp")) ||
	    (name.subString(name.size() - 5, 5).equals_ignore_case(".my3d")) ||
	    (name.subString(name.size() - 5, 5).equals_ignore_case(".lmts")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".csm")) ||
	    (name.subString(name.size() - 4, 4).equals_ignore_case(".oct")))
		return true;
	return false;
}

// things to do after user walks into a FileSceneNode
void FileSceneNode::collided()
{
	m_this->camera->setPosition(m_this->camera->getPosition() + ((m_this->camera->getPosition() - m_this->camera->getTarget()).normalize() * 100.0f));
	m_this->camera->updateAbsolutePosition();

	if (isadir) {
		m_this->filemgr->remove();

		m_this->device->getFileSystem()->changeWorkingDirectoryTo(file->getFileName());

		m_this->filemgr = new FileManagerSceneNode(m_this);
		m_this->filemgr->drop();
	} else if (file->getSize() != 0) {
		core::stringc name = file->getFileName();
		if (isimg(name)) {
			if (!isopen) {
				video::ITexture *texture = m_this->scenemgr->getVideoDriver()
							->getTexture(name);
				if (texture) {
					core::dimension2d<f32> size(texture->getOriginalSize());
					display = m_this->scenemgr->addBillboardSceneNode(this, size,
								core::vector3df(0, height * 40 + size.Height, 0));
					if (display) {
						display->setMaterialTexture(0, texture);
						display->setMaterialType(video::EMT_DETAIL_MAP);
						display->setMaterialFlag(video::EMF_LIGHTING, false);
						isopen = true;
					}
				}
			} else {
				display->remove();
				isopen = false;
			}
		} else if (isvid(name)) {
			if (!isopen) {
				core::dimension2d<f32> size(256, 256);
				display = m_this->scenemgr->addBillboardSceneNode(this, size,
							core::vector3df(0, height * 40 + size.Height, 0));
				if (display) {
					display->setMaterialType(video::EMT_DETAIL_MAP);
					display->setMaterialFlag(video::EMF_LIGHTING, false);

					videoPlayer = new murmuurVIDEO(m_this->driver, m_this->device->getTimer(), 256, 128, m_this, display);
					if (videoPlayer) {
						if (videoPlayer->open(name, 0)) {
							m_this->videos.push_back(this);
							m_this->videos.sort();
							isopen = true;
						} else {
							videoPlayer->close();
							videoPlayer = NULL;
						}
					}
					if (!isopen)
						display->remove();
				}
			} else {
				videoPlayer->close();
				videoPlayer = NULL;
				s32 index = m_this->videos.binary_search(this);
				if (index != -1)
					m_this->videos.erase(index);
				display->remove();
				isopen = false;
			}
		} else if (isanimesh(name)) {
			if (!isopen) {
				scene::IAnimatedMesh *mesh = m_this->scenemgr->getMesh(name);
				if (mesh) {
					mesh->getMeshBuffer(0)->getMaterial().EmissiveColor.set(255, 133, 133, 133);
					model = m_this->scenemgr->addAnimatedMeshSceneNode(
						mesh, this, -1, core::vector3df(0, height * 60, 0));
					if (model) {
						isopen = true;
					}
				}
			} else {
				model->remove();
				isopen = false;
			}
		}
	}
}
