@rem call "D:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x64
set CWD=%~dp0
MSBuild "%CWD%\..\myeditor.sln" /t:rebuild /p:ContinueOnError="ErrorAndStop" /p:Configuration=Debug_60 /p:Platform=Win32 /fl /flp:logfile=MyEditor_Error.log;errorsonly
MSBuild "%CWD%\..\myeditor.sln" /t:rebuild /p:ContinueOnError="ErrorAndStop" /p:Configuration=Release_60 /p:Platform=Win32
MSBuild "%CWD%\..\myeditor.sln" /t:rebuild /p:ContinueOnError="ErrorAndStop" /p:Configuration=Debug_60 /p:Platform=x64 /fl /flp:logfile=MyEditor_Error_64.log;errorsonly
MSBuild "%CWD%\..\myeditor.sln" /t:rebuild /p:ContinueOnError="ErrorAndStop" /p:Configuration=Release_60 /p:Platform=x64