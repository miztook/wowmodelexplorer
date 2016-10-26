using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public delegate void FnSoundCallback();

    public partial class AudioPlayer
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "AudioPlayer_loadMp3", CharSet = CharSet.Ansi)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool AudioPlayer_loadMp3(
            [MarshalAs(UnmanagedType.LPStr)]string filename,
            out uint index);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "AudioPlayer_playMp3")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool AudioPlayer_playMp3(
            uint index,
            [MarshalAsAttribute(UnmanagedType.I1)] bool loop);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "AudioPlayer_playMp3Callback")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool AudioPlayer_playMp3Callback(
            uint index,
            FnSoundCallback callback);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "AudioPlayer_isPlaying")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool AudioPlayer_isPlaying(
            uint index );

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "AudioPlayer_fadeoutMp3")]
        public static extern void AudioPlayer_fadeoutMp3(
            uint index,
            uint fadetime);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "AudioPlayer_stopMp3")]
        public static extern void AudioPlayer_stopMp3(
            uint index);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "AudioPlayer_unloadMp3")]
        public static extern void AudioPlayer_unloadMp3(
            uint index);
    }
}