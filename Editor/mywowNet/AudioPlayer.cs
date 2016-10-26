using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class AudioPlayer : INative
    {
        public AudioPlayer(IntPtr raw) : base(raw) { }

        public bool LoadMp3(string filename, out uint index)
        {
            return AudioPlayer_loadMp3(filename, out index);
        }

        public bool PlayMp3(uint index, bool loop)
        {
            return AudioPlayer_playMp3(index, loop);
        }

        public bool PlayMp3Callback(uint index, FnSoundCallback callback)
        {
            return AudioPlayer_playMp3Callback(index, callback);
        }

        public bool IsPlaying(uint index)
        {
            return AudioPlayer_isPlaying(index);
        }

        public void FadeoutMp3(uint index, uint fadetime)
        {
            AudioPlayer_fadeoutMp3(index, fadetime);
        }

        public void StopMp3(uint index)
        {
            AudioPlayer_stopMp3(index);
        }

        public void UnloadMp3(uint index)
        {
            AudioPlayer_unloadMp3(index);
        }
    }
}