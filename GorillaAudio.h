#pragma once

//Cross platform audio library


#ifndef GORILLA_AUDIO
#define GORILLA_AUDIO
#include <gorilla\ga.h>
#include <gorilla\gau.h>

class GorillaAudio
{
public:
	GorillaAudio();
	bool initGorillaAudio(std::string audiofile);
	void PlayAudio();
	void StopAudio();
	void updateMgr();
	void destroy();
	~GorillaAudio();

	void VolumeUp();
	void VolumeDown();

private:

	std::string AudioFilePath;
	gau_Manager* mgr;
	ga_Mixer* mixer;
	ga_StreamManager* streamMgr;
	ga_Sound* sound;
	ga_Handle* handle;
	float Volume = 0.05;
};

#endif