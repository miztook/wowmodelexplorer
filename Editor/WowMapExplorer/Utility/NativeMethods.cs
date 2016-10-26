using System;
using System.Windows.Interop;
using System.Runtime.InteropServices;

namespace WowMapExplorer.Utility
{
    public class NativeMethods
    {
        [DllImport("user32.dll", EntryPoint = "SetWindowLongA")]
        public static extern int SetWindowLongA(
            [In] System.IntPtr hWnd,
            int nIndex,
            int dwNewLong);

        [DllImport("user32.dll", EntryPoint = "GetWindowLongA")]
        public static extern int GetWindowLongA(
            [In] System.IntPtr hWnd,
            int nIndex);

        [DllImport("user32.dll", EntryPoint = "SetParent")]
        public static extern System.IntPtr SetParent(
            [In] System.IntPtr hWndChild,
            [In] System.IntPtr hWndNewParent);

        [DllImport("user32.dll", EntryPoint = "GetParent")]
        public static extern System.IntPtr GetParent(
            [In] System.IntPtr hWnd);

        [DllImport("user32.dll", EntryPoint = "SendMessageA")]
        [return: MarshalAs(UnmanagedType.SysInt)]
        public static extern int SendMessageA(
            [In] System.IntPtr hWnd,
            uint Msg,
            [MarshalAs(UnmanagedType.SysUInt)] uint wParam,
            [MarshalAs(UnmanagedType.SysInt)] int lParam);

        [DllImport("user32.dll", EntryPoint = "SetFocus")]
        public static extern IntPtr SetFocus([In] IntPtr hWnd);

        [DllImport("user32.dll", EntryPoint = "CloseWindow")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CloseWindow([In] System.IntPtr hWnd);

        [DllImport("user32.dll", EntryPoint = "DestroyWindow")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool DestroyWindow([In] System.IntPtr hWnd);

        [DllImport("user32.dll", EntryPoint = "SetActiveWindow")]
        public static extern IntPtr SetActiveWindow([In] System.IntPtr hWnd);

        [DllImport("user32.dll", EntryPoint = "GetAsyncKeyState")]
        public static extern short GetAsyncKeyState(int vKey);

        public static bool IsKeyPressed(int vKey)
        {
            return (GetAsyncKeyState(vKey) & 0x8000) != 0;
        }

    }
}
