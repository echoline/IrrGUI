#include "dat.h"

void resetCamera(Context *m)
{
	m->camera->setPosition(m->origin + core::vector3df(0, 400, -800));
	m->camera->setTarget(m->origin + core::vector3df(0, 400, 0));
}

int main()
{
	Context m;
	m.running = true;

	// Event Receiver
	EventReceiver receiver;

	// start up the engine
	m.device = createDevice(video::EDT_OPENGL,
		core::dimension2d<u32>(1680,1050), 16, true, // fullscreen
					 false, false, &receiver);

	m.driver = m.device->getVideoDriver();
	m.scenemgr = m.device->getSceneManager();

	// add a first person shooter style user controlled camera
	m.camera = m.scenemgr->addCameraSceneNodeFPS(0, 100.0f, .3f, -1, 0, 0, true, 3.f);

	// add terrain scene node
	TerrainSceneNode* terrain = new TerrainSceneNode(&m);

	m.origin = terrain->getTerrainCenter();
	resetCamera(&m);

	// File manager listing
	m.filemgr = new FileManagerSceneNode(&m);
	m.filemgr->drop();

	// disable mouse cursor
	m.device->getCursorControl()->setVisible(false);

	u32 frames = 0;
	// draw everything
	while(m.device->run() && m.driver && m.running)
	{
		if (receiver.IsKeyDown(KEY_KEY_Q))
			m.running = false;

		if (receiver.IsKeyDown(KEY_ESCAPE))
			resetCamera(&m);

		core::stringw caption =(L"FPS: ");
		caption += m.driver->getFPS();
		m.device->setWindowCaption(caption.c_str());
		m.driver->beginScene(true, true, video::SColor(255,133,133,255));
		m.scenemgr->drawAll();

		for (u32 i = 0; i != m.cleanup.size(); i++) {
			m.camera->removeAnimator(m.cleanup[i]->anim);
			m.cleanup[i]->anim->drop();
			m.cleanup[i]->drop();
		}
		m.cleanup.clear();

		for (u32 i = 0; i != m.videos.size(); i++) {
			if (m.videos[i]->videoPlayer->psVideostate == Playing) {  // play active 
				if (!m.videos[i]->videoPlayer->refresh()) {  // no more AV 
					m.videos[i]->videoPlayer->goToFrame(0);
				} else {
					m.videos[i]->videoPlayer->drawVideoTexture();
				}
			}
		}

		m.driver->endScene();

		terrain->shift(m.camera);
	}

	// delete device
	m.device->drop();
	return 0;
}

