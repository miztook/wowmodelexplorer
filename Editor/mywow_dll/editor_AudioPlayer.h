#pragma once

#include "editor_base.h"

MW_API bool AudioPlayer_loadMp3(const c8* filename, u32* index);
MW_API bool AudioPlayer_playMp3(u32 index, bool loop);
MW_API bool AudioPlayer_playMp3Callback(u32 index, FN_SOUND_CALLBACK callback);
MW_API bool AudioPlayer_isPlaying(u32 index);
MW_API void AudioPlayer_fadeoutMp3(u32 index, u32 fadetime);
MW_API void AudioPlayer_stopMp3(u32 index);
MW_API void AudioPlayer_unloadMp3(u32 index);