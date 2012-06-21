#include "dat.h"

FileManagerSceneNode::FileManagerSceneNode(Context *m) : ISceneNode(m->scenemgr->getRootSceneNode(), m->scenemgr, -1)
{
	setPosition(m->origin);

	core::vector3df offset(0, 0, 0);

	io::IFileList *filelist = m->device->getFileSystem()->createFileList();
	for (u32 i = 0; i != filelist->getFileCount(); i++) {
		FileSceneNode *file = new FileSceneNode(filelist, i, offset, this, m);
		file->drop();

		files.push_back(file);
		bbox.addInternalBox(file->getBoundingBox());

		offset.X += 100;
	}

	m_this = m;
}

FileManagerSceneNode::~FileManagerSceneNode()
{
	//std::cout << "delete FileManagerSceneNode\n";
	if (m_this->running) for (u32 index = 0; index != files.size(); index++) {
		files[index]->grab();
		files[index]->anim->grab();
		m_this->cleanup.push_back(files[index]);
	}
	files.clear();
	removeAll();
}

void FileManagerSceneNode::render()
{
	for (u32 index = 0; index != files.size(); index++)
		files[index]->render();
}

const core::aabbox3d<float>& FileManagerSceneNode::getBoundingBox() const {
	return bbox;
}

