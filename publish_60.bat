@SET SELF_PATH=%~dp0

@echo "Removing...[Output60]"
@cd %SELF_PATH%
@rd /s /q %SELF_PATH%Output60
@md %SELF_PATH%Output60

@echo "MSBUILD Solutions..."
@cd %SELF_PATH%Windows\MSBUILD
@call Build_Dependency.bat
@call Build_Engine_60.bat
@call Build_Tool_60.bat
@call Build_Editor_60.bat

@echo "Renaming x64 files..."
@cd %SELF_PATH%Windows\x64\Release_60
@ren WowModelExplorer.exe WowModelExplorer-64.exe
@ren WowTexViewer.exe WowTexViewer-64.exe

@echo "Building Shaders..."
@cd %SELF_PATH%Windows\Win32\Debug_60
DXShaderBuilder.exe "..\..\..\Resource\Data\Shaders"
@if %errorlevel%==0 (echo "Building Shaders Successful:)") else (pause)
@cd %SELF_PATH%

@echo "Copy to Output60..."
@cd %SELF_PATH%
Xcopy ".\Windows\Win32\Release_60\AvalonDock.dll" 	".\Output60\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release_60\mywow_dll.dll" 	".\Output60\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release_60\mywowNet.dll" 	".\Output60\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release_60\WowModelExplorer.exe" 	".\Output60\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release_60\WowTexViewer.exe" 	".\Output60\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release_60\WpfPropertyGrid.dll" 	".\Output60\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release_60\*.pdb" 	".\Output60Symbols\x86\" /E /R /Y /Q

Xcopy ".\Windows\x64\Release_60\AvalonDock.dll" 	".\Output60\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release_60\mywow_dll.dll" 	".\Output60\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release_60\mywowNet.dll" 	".\Output60\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release_60\WowModelExplorer-64.exe" 	".\Output60\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release_60\WowTexViewer-64.exe" 	".\Output60\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release_60\WpfPropertyGrid.dll" 	".\Output60\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release_60\*.pdb" 	".\Output60Symbols\x64\" /E /R /Y /Q

Xcopy ".\Resource\*.*" 	".\Output60\" /E /R /Y /Q

@echo "Publish 60 Success:)"
@pause
