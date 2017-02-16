@rem call "D:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x64
set CWD=%~dp0
MSBuild "%CWD%\..\tools.sln" /t:rebuild /p:ContinueOnError="ErrorAndStop" /p:Configuration=Debug_60 /p:Platform=Win32 /fl /flp:logfile=Tool_Error.log;errorsonly