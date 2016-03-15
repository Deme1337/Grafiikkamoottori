#include "stdafx.h"
#include "GorillaAudio.h"


GorillaAudio::GorillaAudio()
{
}

bool GorillaAudio::initGorillaAudio(std::string audiofile)
{
	this->AudioFilePath = audiofile;

	gc_initialize(0);

	mgr = gau_manager_create();

	mixer = gau_manager_mixer(mgr);

	streamMgr = gau_manager_streamManager(mgr);
	sound = gau_load_sound_file(AudioFilePath.c_str(), "wav");
	
	handle = gau_create_handle_sound(mixer, sound, &gau_on_finish_destroy, 0, 0);

	return true;
}

void GorillaAudio::VolumeDown()
{
	if (Volume > 0)
	{
		Volume -= 0.01;
		ga_handle_setParamf(handle, GA_HANDLE_PARAM_GAIN, Volume);
	}

}

void GorillaAudio::VolumeUp()
{
	if (Volume < 1.0)
	{
		Volume += 0.01;
		ga_handle_setParamf(handle, GA_HANDLE_PARAM_GAIN, Volume);

	}

}

void GorillaAudio::PlayAudio()
{
	if (handle->state > 0)
	{
		ga_handle_setParamf(handle, GA_HANDLE_PARAM_GAIN, Volume);
		ga_handle_play(handle);
	}
	else
	{
		handle = gau_create_handle_sound(mixer, sound, &gau_on_finish_destroy, 0, 0);
		ga_handle_setParamf(handle, GA_HANDLE_PARAM_GAIN, Volume);
		ga_handle_play(handle);
	}
	
}

void GorillaAudio::StopAudio()
{
	ga_handle_stop(handle);
}

void GorillaAudio::updateMgr()
{
	gau_manager_update(mgr);
	
}

void GorillaAudio::destroy()
{
	gau_manager_destroy(mgr);
	gc_shutdown();
}


GorillaAudio::~GorillaAudio()
{
}
