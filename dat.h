#include <irrlicht.h>
using namespace irr;

typedef struct Context Context;
typedef class FileSceneNode FileSceneNode;

class FileManagerSceneNode : public scene::ISceneNode {
private:
	core::aabbox3d<f32> bbox;
	core::array<FileSceneNode*> files;
	Context *m_this;

public:
	FileManagerSceneNode(Context*);
	~FileManagerSceneNode();
	void render();
	const core::aabbox3d<f32>& getBoundingBox() const;
};

class FileSceneNode : public scene::ISceneNode {
private:
	scene::ISceneNode *cube;
	scene::IBillboardTextSceneNode *label;
	io::IReadFile *file;
	Context *m_this;
	bool isadir;
	f32 height;
	bool isopen;
	scene::IBillboardSceneNode *display;
	scene::ISceneNode *model;

public:
	scene::ISceneNodeAnimatorCollisionResponse* anim;

	FileSceneNode(io::IFileList*, u32, core::vector3df&, scene::ISceneNode*, Context*);
	~FileSceneNode();
	void render();
	const core::aabbox3d<f32>& getBoundingBox() const;
	void collided();
	bool isimg(core::stringc);
	bool isanimesh(core::stringc);
};

class TerrainSceneNode : public scene::ISceneNode {
private:
	core::aabbox3d<f32> bbox;
	scene::IMeshSceneNode *terrains[3][3];

public:
	TerrainSceneNode(Context*);
	~TerrainSceneNode();
	void render();
	const core::aabbox3d<f32>& getBoundingBox() const;
	scene::IMetaTriangleSelector* createTriangleSelector(scene::ISceneManager *scenemgr, s32 LOD);
	void centerOn(scene::ISceneNode *in, int x, int y);
	void shift(scene::ISceneNode *in);
	core::vector3df getTerrainCenter();
};

class EventReceiver : public IEventReceiver {
private:
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
	bool LeftButtonDown;
	bool RightButtonDown;

public:
	bool OnEvent(const SEvent&);
	bool IsKeyDown(EKEY_CODE keyCode) const;
	bool IsLeftButtonDown() const;
	bool IsRightButtonDown() const;
	EventReceiver();
};

class EReadFile : public io::IReadFile {
private:
	io::path name;

public:
	EReadFile(const io::path&);
	s32 read(void*, u32);
	bool seek(long int, bool);
	long int getSize() const;
	long int getPos() const;
	const io::path& getFileName() const;
};

struct Context {
	IrrlichtDevice *device;
	scene::ISceneManager *scenemgr;
	core::vector3df origin;
	FileManagerSceneNode *filemgr;
	scene::ICameraSceneNode *camera;
	core::array<FileSceneNode*> cleanup;
	bool running;
};

