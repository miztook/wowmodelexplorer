using System;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using System.Security;
using System.Security.Permissions;

namespace WowModelExplorer.Utility
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

        [DllImport("user32.dll", EntryPoint = "GetAsyncKeyState")]
        public static extern short GetAsyncKeyState(int vKey);

        public static bool IsKeyPressed(int vKey)
        {
            return (GetAsyncKeyState(vKey) & 0x8000) != 0;
        }

        #region Malloc

        [ComImport, SuppressUnmanagedCodeSecurity, Guid("00000002-0000-0000-c000-000000000046"),
         InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        public interface IMalloc
        {
            [PreserveSig]
            IntPtr Alloc(int cb);

            [PreserveSig]
            IntPtr Realloc(IntPtr pv, int cb);

            [PreserveSig]
            void Free(IntPtr pv);

            [PreserveSig]
            int GetSize(IntPtr pv);

            [PreserveSig]
            int DidAlloc(IntPtr pv);

            [PreserveSig]
            void HeapMinimize();
        }

        [SecurityCritical]
        public static IMalloc GetSHMalloc()
        {
            IMalloc[] ppMalloc = new IMalloc[1];
            SHGetMalloc(ppMalloc);
            return ppMalloc[0];
        }

        [SecurityCritical, DllImport("shell32.dll")]
        private static extern int SHGetMalloc([Out, MarshalAs(UnmanagedType.LPArray)] IMalloc[] ppMalloc);

        #endregion

        #region Folders

        [SecurityCritical, DllImport("shell32.dll")]
        public static extern int SHGetFolderLocation(IntPtr hwndOwner, Int32 nFolder, IntPtr hToken, uint dwReserved,
            out IntPtr ppidl);

        [SecurityCritical, DllImport("shell32.dll")]
        public static extern int SHParseDisplayName([MarshalAs(UnmanagedType.LPWStr)] string pszName, IntPtr pbc,
            out IntPtr ppidl, uint sfgaoIn, out uint psfgaoOut);

        [SecurityCritical, DllImport("shell32.dll")]
        public static extern IntPtr SHBrowseForFolder(ref BROWSEINFO lbpi);

        [SecurityCritical, DllImport("shell32.dll", CharSet = CharSet.Auto)]
        public static extern bool SHGetPathFromIDList(IntPtr pidl, IntPtr pszPath);

        #endregion

        public enum ShowCommands
        {
            SW_HIDE = 0,
            SW_SHOWNORMAL = 1,
            SW_NORMAL = 1,
            SW_SHOWMINIMIZED = 2,
            SW_SHOWMAXIMIZED = 3,
            SW_MAXIMIZE = 3,
            SW_SHOWNOACTIVATE = 4,
            SW_SHOW = 5,
            SW_MINIMIZE = 6,
            SW_SHOWMINNOACTIVE = 7,
            SW_SHOWNA = 8,
            SW_RESTORE = 9,
            SW_SHOWDEFAULT = 10,
            SW_FORCEMINIMIZE = 11,
            SW_MAX = 11
        }

        [DllImport("shell32.dll")]
        public static extern IntPtr ShellExecute(
            IntPtr hwnd,
            string lpOperation,
            string lpFile,
            string lpParameters,
            string lpDirectory,
            ShowCommands nShowCmd);

        public delegate IntPtr WndProc(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam);

        [StructLayout(LayoutKind.Sequential)]
        public struct BROWSEINFO
        {
            /// <summary>
            ///     Handle to the owner window for the dialog box.
            /// </summary>
            public IntPtr HwndOwner;

            /// <summary>
            ///     Pointer to an item identifier list (PIDL) specifying the
            ///     location of the root folder from which to start browsing.
            /// </summary>
            public IntPtr Root;

            /// <summary>
            ///     Address of a buffer to receive the display name of the
            ///     folder selected by the user.
            /// </summary>
            [MarshalAs(UnmanagedType.LPStr)]
            public string DisplayName;

            /// <summary>
            ///     Address of a null-terminated string that is displayed
            ///     above the tree view control in the dialog box.
            /// </summary>
            [MarshalAs(UnmanagedType.LPStr)]
            public string Title;

            /// <summary>
            ///     Flags specifying the options for the dialog box.
            /// </summary>
            public uint Flags;

            /// <summary>
            ///     Address of an application-defined function that the
            ///     dialog box calls when an event occurs.
            /// </summary>
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public WndProc Callback;

            /// <summary>
            ///     Application-defined value that the dialog box passes to
            ///     the callback function
            /// </summary>
            public int LParam;

            /// <summary>
            ///     Variable to receive the image associated with the selected folder.
            /// </summary>
            public int Image;
        }

        [Flags]
        public enum FolderBrowserOptions
        {
            /// <summary>
            ///     None.
            /// </summary>
            None = 0,

            /// <summary>
            ///     For finding a folder to start document searching
            /// </summary>
            FolderOnly = 0x0001,

            /// <summary>
            ///     For starting the Find Computer
            /// </summary>
            FindComputer = 0x0002,

            /// <summary>
            ///     Top of the dialog has 2 lines of text for BROWSEINFO.lpszTitle and
            ///     one line if this flag is set.  Passing the message
            ///     BFFM_SETSTATUSTEXTA to the hwnd can set the rest of the text.
            ///     This is not used with BIF_USENEWUI and BROWSEINFO.lpszTitle gets
            ///     all three lines of text.
            /// </summary>
            ShowStatusText = 0x0004,
            ReturnAncestors = 0x0008,

            /// <summary>
            ///     Add an editbox to the dialog
            /// </summary>
            ShowEditBox = 0x0010,

            /// <summary>
            ///     insist on valid result (or CANCEL)
            /// </summary>
            ValidateResult = 0x0020,

            /// <summary>
            ///     Use the new dialog layout with the ability to resize
            ///     Caller needs to call OleInitialize() before using this API
            /// </summary>
            UseNewStyle = 0x0040,
            UseNewStyleWithEditBox = (UseNewStyle | ShowEditBox),

            /// <summary>
            ///     Allow URLs to be displayed or entered. (Requires BIF_USENEWUI)
            /// </summary>
            AllowUrls = 0x0080,

            /// <summary>
            ///     Add a UA hint to the dialog, in place of the edit box. May not be
            ///     combined with BIF_EDITBOX.
            /// </summary>
            ShowUsageHint = 0x0100,

            /// <summary>
            ///     Do not add the "New Folder" button to the dialog.  Only applicable
            ///     with BIF_NEWDIALOGSTYLE.
            /// </summary>
            HideNewFolderButton = 0x0200,

            /// <summary>
            ///     don't traverse target as shortcut
            /// </summary>
            GetShortcuts = 0x0400,

            /// <summary>
            ///     Browsing for Computers.
            /// </summary>
            BrowseComputers = 0x1000,

            /// <summary>
            ///     Browsing for Printers.
            /// </summary>
            BrowsePrinters = 0x2000,

            /// <summary>
            ///     Browsing for Everything
            /// </summary>
            BrowseFiles = 0x4000,

            /// <summary>
            ///     sharable resources displayed (remote shares, requires BIF_USENEWUI)
            /// </summary>
            BrowseShares = 0x8000
        }
    }
}
