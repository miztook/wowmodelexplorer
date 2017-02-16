@SET SELF_PATH=%~dp0

@echo "Removing...[Output]"
@cd %SELF_PATH%
@rd /s /q %SELF_PATH%Output
@md %SELF_PATH%Output

@echo "MSBUILD Solutions..."
@cd %SELF_PATH%Windows\MSBUILD
@call Rebuild_Dependency.bat
@call Rebuild_Engine.bat
@call Rebuild_Tool.bat
@call Rebuild_Editor.bat

@echo "Renaming x64 files..."
@cd %SELF_PATH%Windows\x64\Release
@ren WowModelExplorer.exe WowModelExplorer-64.exe
@ren WowTexViewer.exe WowTexViewer-64.exe

@echo "Building Shaders..."
@cd %SELF_PATH%Windows\Win32\Debug
DXShaderBuilder.exe "..\..\..\Resource\Data\Shaders"
@if %errorlevel%==0 (echo "Building Shaders Successful:)") else (pause)
@cd %SELF_PATH%

@echo "Copy to Output..."
@cd %SELF_PATH%
Xcopy ".\Windows\Win32\Release\AvalonDock.dll" 	".\Output\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release\mywow_dll.dll" 	".\Output\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release\mywowNet.dll" 	".\Output\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release\WowModelExplorer.exe" 	".\Output\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release\WowTexViewer.exe" 	".\Output\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release\WpfPropertyGrid.dll" 	".\Output\bin\x86\" /E /R /Y /Q
Xcopy ".\Windows\Win32\Release\*.pdb" 	".\OutputSymbols\x86\" /E /R /Y /Q

Xcopy ".\Windows\x64\Release\AvalonDock.dll" 	".\Output\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release\mywow_dll.dll" 	".\Output\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release\mywowNet.dll" 	".\Output\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release\WowModelExplorer-64.exe" 	".\Output\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release\WowTexViewer-64.exe" 	".\Output\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release\WpfPropertyGrid.dll" 	".\Output\bin\x64\" /E /R /Y /Q
Xcopy ".\Windows\x64\Release\*.pdb" 	".\OutputSymbols\x64\" /E /R /Y /Q

Xcopy ".\Resource\*.*" 	".\Output\" /E /R /Y /Q

del ".\Output60\Data\listfile60.txt"

@echo "Publish Success:)"
@pause


