#include "stdafx.h"
#include "editor_AudioPlayer.h"

bool AudioPlayer_loadMp3( const c8* filename, u32* index )
{
	ISound* sound = g_Engine->getAudioPlayer()->getEmptyMp3Sound();
	if (!sound)
		return false;

	*index = sound->getIndex();

	return sound->load(filename);
}

bool AudioPlayer_playMp3( u32 index, bool loop )
{
	ISound* sound = g_Engine->getAudioPlayer()->getMp3Sound(index);
	if (!sound)
		return false;

	return sound->play(loop);
}

bool AudioPlayer_playMp3Callback( u32 index, FN_SOUND_CALLBACK callback )
{
	ISound* sound = g_Engine->getAudioPlayer()->getMp3Sound(index);
	if (!sound)
		return false;

	return sound->play(false, callback);
}

bool AudioPlayer_isPlaying( u32 index )
{
	ISound* sound = g_Engine->getAudioPlayer()->getMp3Sound(index);
	if (!sound)
		return false;

	return sound->isPlaying();
}

void AudioPlayer_fadeoutMp3(u32 index, u32 fadetime)
{
	ISound* sound = g_Engine->getAudioPlayer()->getMp3Sound(index);
	if (sound)
		g_Engine->getAudioPlayer()->fadeoutSound(sound, fadetime);
}

void AudioPlayer_stopMp3( u32 index )
{
	ISound* sound = g_Engine->getAudioPlayer()->getMp3Sound(index);
	if (sound)
		sound->stop();
}

void AudioPlayer_unloadMp3( u32 index )
{
	ISound* sound = g_Engine->getAudioPlayer()->getMp3Sound(index);
	if (sound)
		sound->unload();
}
