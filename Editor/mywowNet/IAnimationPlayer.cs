using System;

namespace mywowNet
{
    public interface IAnimationPlayer
    {
        int CurrentAnimationIndex { get; }
        float CurrentFrame { get; set; }
        bool IsPlaying { get; }
        bool Loop { get; set; }

        void Pause();
        void Resume();
        void Stop();
        void Step(float frame);
    }
}
