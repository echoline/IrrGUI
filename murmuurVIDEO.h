#ifndef __MURMUUR_VIDEO_H__ /////////////////////////////////////////////////////////////////////////////////
#define __MURMUUR_VIDEO_H__  ////////////////////////////////////////////////////////////////////////////////
//#define SOUND_MULTITHREADED  // comment this out to not multi thread the audio //////////////////////////////
#define SOUND_OPENAL  // comment this out for no sound //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Original code sources from juliusctw, Hansel, stepan1117
// Heavily Modified/Merged by theSpecial1
// Glitch freedom obtained with the help of briaj, randomMesh & polho
// Version 0.8
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


// constants ////////////////////////////////////////////////////////////////////////////////////////////////
const int NUM_BUFFERS = 3;
const int BUFFER_SIZE = 19200;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


// includes /////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef SOUND_MULTITHREADED
   #include <boost/bind.hpp>
   #include <boost/thread/thread.hpp>
   #include <boost/thread/mutex.hpp>
   #include <boost/shared_ptr.hpp>
#endif
#include <irrlicht.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
extern "C" {
   #define UINT64_C uint64_t
   #include <string.h>
   #include <libavcodec/avcodec.h>
   #include <libavformat/avformat.h>
   #include <libswscale/swscale.h>
#ifdef SOUND_OPENAL
   #include <signal.h>
   #include <AL/al.h>
   #include <AL/alc.h>
   #include <AL/alut.h>
#endif
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// structures ///////////////////////////////////////////////////////////////////////////////////////////////
typedef struct MyFile *FilePtr;
typedef struct MyStream *StreamPtr;
struct MyStream {
   AVCodecContext *CodecCtx;
   int StreamIdx;

   char *Data;
   size_t DataSize;
   size_t DataSizeMax;
   char *DecodedData;
   size_t DecodedDataSize;

   FilePtr parent;
};

struct MyFile {
   AVFormatContext *FmtCtx;
   StreamPtr *Streams;
   size_t StreamsSize;
};

enum ePlaystate { Closed, Playing, Paused, Stopped };
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


// main class definition ////////////////////////////////////////////////////////////////////////////////////
class murmuurVIDEO {
private: ////////////////////////////////////////////////////////////////////////////////////////////////////
   irr::ITimer *_itTimer;
    irr::video::IVideoDriver *_vdVideoDriver;
    irr::video::IImage *_imCurrentImage;
    irr::video::ITexture *_txCurrentTexture;

   FilePtr _fpFile;
   irr::core::stringc _sCurrentFile;
    StreamPtr _spStreamA, _spStreamV;
   bool _bHasAudio, _bHasVideo;
   int _iDesiredH;
   int _iDesiredW;
   bool _first_time;
   struct SwsContext *_img_convert_ctx;
   int _currentX;
   int _currentY;
   double _dSecondsPerFrame;
   int _iVideoSynchAmount;
   float _fFramerate;
   float _fDuration;
   int _iActualFrame;

    unsigned long _lLastTime;
   std::vector<AVFrame> _frFrame_Buffer;
   bool _bFrameDisplayed;
   AVFrame *_frFrame;
    AVFrame *_frFrameRGB;
    int _iNumBytes;
    uint8_t *_iBuffer;
   irr::s32* _p;
    irr::s32* _pimage;

#ifdef SOUND_OPENAL
    ALuint _aiSource;
    ALint _aiState;
   ALenum _aeOldFormat;
   ALenum _aeFormat;
#endif
    int _iBuffCount;
    int _iOld_rate;
    int _iChannels;
    int _iBits;
    int _iRate;
    int _iBasetime;

   float _fVolume;
   bool _bMute;

   bool _initAV(void);
   FilePtr _openAVFile(const char *fname);
   void _closeAVFile(FilePtr file);
   void _DumpFrame(AVFrame *pFrame, int width, int height, bool needResize);
   StreamPtr _getAVAudioStream(FilePtr file, int streamnum);
   StreamPtr _getAVVideoStream(FilePtr file, int streamnum);
   int _getAVAudioInfo(StreamPtr stream, int *rate, int *channels, int *bits);
   bool _getNextPacket(FilePtr file, int streamidx);
   int _getAVAudioData(StreamPtr stream, void *data, int length);
   AVFrame *_getNextFrame(void);

#ifdef SOUND_MULTITHREADED
   boost::shared_ptr<boost::thread> _m_thread;
   boost::mutex _m_mutex;
   bool _m_finished;
#endif
   bool _refreshAudio(void);

   Context *ctx;
public: /////////////////////////////////////////////////////////////////////////////////////////////////////
   ePlaystate psVideostate;
   irr::scene::IMeshSceneNode *mnOutputMesh;
   irr::scene::IBillboardSceneNode *mnOutputBillboard;
   bool bVideoLoaded;

   murmuurVIDEO(irr::video::IVideoDriver *irrVideoDriver, irr::ITimer *timer, int desiredW, int desiredH, Context*);
   murmuurVIDEO(irr::video::IVideoDriver *irrVideoDriver, irr::ITimer *timer, int desiredW, int desiredH, Context*, irr::scene::IMeshSceneNode *outputMesh);
   murmuurVIDEO(irr::video::IVideoDriver *irrVideoDriver, irr::ITimer *timer, int desiredW, int desiredH, Context*, irr::scene::IBillboardSceneNode *);

   bool open(irr::core::stringc sFileName, int iStartIndex);
   bool refresh(void);

   void drawVideoTexture(void);
   void close(void);

   void changeResolution(int w, int h);
   bool goToFrame(int iNumFrame);

   irr::core::stringw getTitleBarStatus(void);

   float getVolumeLevel(void);   void setVolumeLevel(float fVol);
                        void setVolumeMute(void);
   int getVideoSpeed(void);   void setVideoSpeed(int iSpeed);
   int getCurrentFrame(void);

    ~murmuurVIDEO();
}; //////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif //////////////////////////////////////////////////////////////////////////////////////////////////////
