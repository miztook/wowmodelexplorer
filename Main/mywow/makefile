#====================================
# define
#====================================
COMPILE_OPTIONS = -I./interface -I../../Dependency/stormlib -I../../Dependency/freetype/include -I../../Dependency/libvorbis -I../../Dependency/libogg -I../../Dependency/libmad -I../../Dependency/PVRTexLib/include -I./GLES/Include  -O0 -g3 -w -c -fmessage-length=0 -finput-charset=GBK -MMD -MP

OBJECT_FILE_LIST = \
./make/CCollisionServices.o \
./make/CConfigs.o \
./make/CCoordSceneNode.o \
./make/CD3D11DrawServices.o \
./make/CD3D11Driver_base.o \
./make/CD3D11Driver_draw.o \
./make/CD3D11Driver_Renderer.o \
./make/CD3D11HardwareBufferServices.o \
./make/CD3D11Helper.o \
./make/CD3D11ManualTextureServices.o \
./make/CD3D11MaterialRenderer.o \
./make/CD3D11MaterialRenderServices.o \
./make/CD3D11RenderTarget.o \
./make/CD3D11ResourceLoader.o \
./make/CD3D11SceneStateServices.o \
./make/CD3D11Shader.o \
./make/CD3D11ShaderServices.o \
./make/CD3D11Texture.o \
./make/CD3D11TextureWriteServices.o \
./make/CD3D11VertexDeclaration.o \
./make/CD3D11_PS40.o \
./make/CD3D11_PS50.o \
./make/CD3D11_VS40.o \
./make/CD3D11_VS50.o \
./make/CD3D9DrawServices.o \
./make/CD3D9Driver_Renderer.o \
./make/CD3D9TextureWriteServices.o \
./make/CD3D9_PS30.o \
./make/CD3D9RenderTarget.o \
./make/CD3D9VertexDeclaration.o \
./make/CD3D9_VS30.o \
./make/CDSAudioPlayer.o \
./make/CDSSound.o \
./make/CBLPImage.o \
./make/CFileADTLoader.o \
./make/CFileM2Loader.o \
./make/CFileWDTLoader.o \
./make/CFileWMOLoader.o \
./make/CGLES2DrawServices.o \
./make/CGLES2Driver_base.o \
./make/CGLES2Driver_draw.o \
./make/CGLES2Driver_Renderer.o \
./make/CGLES2Extension.o \
./make/CGLES2HardwareBufferServices.o \
./make/CGLES2Helper.o \
./make/CGLES2ManualTextureServices.o \
./make/CGLES2MaterialRenderer.o \
./make/CGLES2MaterialRenderServices.o \
./make/CGLES2RenderTarget.o \
./make/CGLES2ResourceLoader.o \
./make/CGLES2SceneStateServices.o \
./make/CGLES2Shader.o \
./make/CGLES2ShaderServices.o \
./make/CGLES2Texture.o \
./make/CGLES2TextureWriteServices.o \
./make/CGLES2VertexDeclaration.o \
./make/CGLES2_PS.o \
./make/CGLES2_VS.o \
./make/CImageLoaderPVR.o \
./make/CKTXImage.o \
./make/CMemDbg.o \
./make/CMemFile.o \
./make/CMeshDecalRenderer.o \
./make/CMeshDecalServices.o \
./make/CMiniDump.o \
./make/CMP3Input.o \
./make/COpenGLDrawServices.o \
./make/COpenGLDriver_base.o \
./make/COpenGLDriver_draw.o \
./make/COpenGLDriver_Renderer.o \
./make/COpenGLExtension.o \
./make/COpenGLHardwareBufferServices.o \
./make/COpenGLHelper.o \
./make/COpenGLManualTextureServices.o \
./make/COpenGLMaterialRenderer.o \
./make/COpenGLMaterialRenderServices.o \
./make/COpenGLRenderTarget.o \
./make/COpenGLResourceLoader.o \
./make/COpenGLSceneStateServices.o \
./make/COpenGLShader.o \
./make/COpenGLShaderServices.o \
./make/COpenGLTexture.o \
./make/COpenGLTextureWriteServices.o \
./make/COpenGLVertexDeclaration.o \
./make/COpenGL_PS12.o \
./make/COpenGL_PS15.o \
./make/COpenGL_VS12.o \
./make/COpenGL_VS15.o \
./make/CPVRImage.o \
./make/CResourceLoader.o \
./make/CRibbonRenderer.o \
./make/CSceneEnvironment.o \
./make/CSkySceneNode.o \
./make/CSpecialTextureServices.o \
./make/CSysUtility.o \
./make/CTimer.o \
./make/CParticleRenderer.o \
./make/CTransluscentDecalRenderer.o \
./make/CVorbisInput.o \
./make/CWavInput.o \
./make/CBlit.o \
./make/CCamera.o \
./make/CD3D9Driver_base.o \
./make/CD3D9Driver_draw.o \
./make/CD3D9SceneStateServices.o \
./make/CFTFont.o \
./make/CD3D9HardwareBufferServices.o \
./make/CD3D9Helper.o \
./make/CD3D9ManualTextureServices.o \
./make/CD3D9MaterialRenderer.o \
./make/CD3D9MaterialRenderServices.o \
./make/CD3D9_PS20.o \
./make/CD3D9ResourceLoader.o \
./make/CD3D9Shader.o \
./make/CD3D9ShaderServices.o \
./make/CD3D9Texture.o \
./make/CD3D9_VS20.o \
./make/CFileADT.o \
./make/CFileM2.o \
./make/CFileSystem.o \
./make/CFileWDT.o \
./make/CFileWMO.o \
./make/CGeometryCreator.o \
./make/CImage.o \
./make/CImageLoaderBLP.o \
./make/CDxInputReader.o \
./make/CM2SceneNode.o \
./make/CManualMeshServices.o \
./make/CMapEnvironment.o \
./make/CMapTileSceneNode.o \
./make/CMesh.o \
./make/CMeshRenderer.o \
./make/CMeshSceneNode.o \
./make/CTerrainRenderer.o \
./make/CParticleSystemSceneNode.o \
./make/CParticleSystemServices.o \
./make/CReadFile.o \
./make/CRibbonEmitterServices.o \
./make/CRibbonSceneNode.o \
./make/CSceneManager.o \
./make/CSceneRenderServices.o \
./make/CTerrainServices.o \
./make/CTransluscentRenderer.o \
./make/CWDTSceneNode.o \
./make/CWmoRenderer.o \
./make/CWMOSceneNode.o \
./make/CWriteFile.o \
./make/CX2AudioPlayer.o \
./make/CX2Sound.o \
./make/ddslib.o \
./make/engine.o \
./make/enginesetting.o \
./make/ftsystem_memory.o \
./make/ftsystem_mpq.o \
./make/mpq_stormlib.o \
./make/mywow.o \
./make/pvrlib.o \
./make/q_memory.o \
./make/stdafx.o \
./make/stormlib_memory.o \
./make/wowEnvironment.o \
./make/wow_m2Action.o \
./make/wow_m2FSM.o \
./make/wow_m2appearance.o \
./make/wow_m2instance.o \
./make/wow_database.o \
./make/wow_dbc.o \
./make/wow_m2Move.o \
./make/wow_m2spell.o \
./make/wow_particle.o \
./make/wow_tileScene.o \
./make/wow_wdtScene.o \
./make/wow_wmoScene.o 

#====================================
# target file
#====================================
./make/libmywowengine.a : prepare $(OBJECT_FILE_LIST) 
	ar -rc ./make/libmywowengine.a $(OBJECT_FILE_LIST);\
rm -f $(SolutionDir)$(Platform)/$(Configuration)/libmywowengine.a;\
cp ./make/libmywowengine.a $(SolutionDir)$(Platform)/$(Configuration)/libmywowengine.a 


#====================================
# object files
#====================================
./make/CCollisionServices.o : CCollisionServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CCollisionServices.d" -MT"./make/CCollisionServices.d" -o"./make/CCollisionServices.o" "CCollisionServices.cpp" 2>> ./make/result.txt 

./make/CConfigs.o : CConfigs.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CConfigs.d" -MT"./make/CConfigs.d" -o"./make/CConfigs.o" "CConfigs.cpp" 2>> ./make/result.txt 

./make/CCoordSceneNode.o : CCoordSceneNode.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CCoordSceneNode.d" -MT"./make/CCoordSceneNode.d" -o"./make/CCoordSceneNode.o" "CCoordSceneNode.cpp" 2>> ./make/result.txt 

./make/CD3D11DrawServices.o : CD3D11DrawServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11DrawServices.d" -MT"./make/CD3D11DrawServices.d" -o"./make/CD3D11DrawServices.o" "CD3D11DrawServices.cpp" 2>> ./make/result.txt 

./make/CD3D11Driver_base.o : CD3D11Driver_base.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11Driver_base.d" -MT"./make/CD3D11Driver_base.d" -o"./make/CD3D11Driver_base.o" "CD3D11Driver_base.cpp" 2>> ./make/result.txt 

./make/CD3D11Driver_draw.o : CD3D11Driver_draw.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11Driver_draw.d" -MT"./make/CD3D11Driver_draw.d" -o"./make/CD3D11Driver_draw.o" "CD3D11Driver_draw.cpp" 2>> ./make/result.txt 

./make/CD3D11Driver_Renderer.o : CD3D11Driver_Renderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11Driver_Renderer.d" -MT"./make/CD3D11Driver_Renderer.d" -o"./make/CD3D11Driver_Renderer.o" "CD3D11Driver_Renderer.cpp" 2>> ./make/result.txt 

./make/CD3D11HardwareBufferServices.o : CD3D11HardwareBufferServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11HardwareBufferServices.d" -MT"./make/CD3D11HardwareBufferServices.d" -o"./make/CD3D11HardwareBufferServices.o" "CD3D11HardwareBufferServices.cpp" 2>> ./make/result.txt 

./make/CD3D11Helper.o : CD3D11Helper.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11Helper.d" -MT"./make/CD3D11Helper.d" -o"./make/CD3D11Helper.o" "CD3D11Helper.cpp" 2>> ./make/result.txt 

./make/CD3D11ManualTextureServices.o : CD3D11ManualTextureServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11ManualTextureServices.d" -MT"./make/CD3D11ManualTextureServices.d" -o"./make/CD3D11ManualTextureServices.o" "CD3D11ManualTextureServices.cpp" 2>> ./make/result.txt 

./make/CD3D11MaterialRenderer.o : CD3D11MaterialRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11MaterialRenderer.d" -MT"./make/CD3D11MaterialRenderer.d" -o"./make/CD3D11MaterialRenderer.o" "CD3D11MaterialRenderer.cpp" 2>> ./make/result.txt 

./make/CD3D11MaterialRenderServices.o : CD3D11MaterialRenderServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11MaterialRenderServices.d" -MT"./make/CD3D11MaterialRenderServices.d" -o"./make/CD3D11MaterialRenderServices.o" "CD3D11MaterialRenderServices.cpp" 2>> ./make/result.txt 

./make/CD3D11RenderTarget.o : CD3D11RenderTarget.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11RenderTarget.d" -MT"./make/CD3D11RenderTarget.d" -o"./make/CD3D11RenderTarget.o" "CD3D11RenderTarget.cpp" 2>> ./make/result.txt 

./make/CD3D11ResourceLoader.o : CD3D11ResourceLoader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11ResourceLoader.d" -MT"./make/CD3D11ResourceLoader.d" -o"./make/CD3D11ResourceLoader.o" "CD3D11ResourceLoader.cpp" 2>> ./make/result.txt 

./make/CD3D11SceneStateServices.o : CD3D11SceneStateServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11SceneStateServices.d" -MT"./make/CD3D11SceneStateServices.d" -o"./make/CD3D11SceneStateServices.o" "CD3D11SceneStateServices.cpp" 2>> ./make/result.txt 

./make/CD3D11Shader.o : CD3D11Shader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11Shader.d" -MT"./make/CD3D11Shader.d" -o"./make/CD3D11Shader.o" "CD3D11Shader.cpp" 2>> ./make/result.txt 

./make/CD3D11ShaderServices.o : CD3D11ShaderServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11ShaderServices.d" -MT"./make/CD3D11ShaderServices.d" -o"./make/CD3D11ShaderServices.o" "CD3D11ShaderServices.cpp" 2>> ./make/result.txt 

./make/CD3D11Texture.o : CD3D11Texture.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11Texture.d" -MT"./make/CD3D11Texture.d" -o"./make/CD3D11Texture.o" "CD3D11Texture.cpp" 2>> ./make/result.txt 

./make/CD3D11TextureWriteServices.o : CD3D11TextureWriteServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11TextureWriteServices.d" -MT"./make/CD3D11TextureWriteServices.d" -o"./make/CD3D11TextureWriteServices.o" "CD3D11TextureWriteServices.cpp" 2>> ./make/result.txt 

./make/CD3D11VertexDeclaration.o : CD3D11VertexDeclaration.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11VertexDeclaration.d" -MT"./make/CD3D11VertexDeclaration.d" -o"./make/CD3D11VertexDeclaration.o" "CD3D11VertexDeclaration.cpp" 2>> ./make/result.txt 

./make/CD3D11_PS40.o : CD3D11_PS40.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11_PS40.d" -MT"./make/CD3D11_PS40.d" -o"./make/CD3D11_PS40.o" "CD3D11_PS40.cpp" 2>> ./make/result.txt 

./make/CD3D11_PS50.o : CD3D11_PS50.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11_PS50.d" -MT"./make/CD3D11_PS50.d" -o"./make/CD3D11_PS50.o" "CD3D11_PS50.cpp" 2>> ./make/result.txt 

./make/CD3D11_VS40.o : CD3D11_VS40.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11_VS40.d" -MT"./make/CD3D11_VS40.d" -o"./make/CD3D11_VS40.o" "CD3D11_VS40.cpp" 2>> ./make/result.txt 

./make/CD3D11_VS50.o : CD3D11_VS50.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D11_VS50.d" -MT"./make/CD3D11_VS50.d" -o"./make/CD3D11_VS50.o" "CD3D11_VS50.cpp" 2>> ./make/result.txt 

./make/CD3D9DrawServices.o : CD3D9DrawServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9DrawServices.d" -MT"./make/CD3D9DrawServices.d" -o"./make/CD3D9DrawServices.o" "CD3D9DrawServices.cpp" 2>> ./make/result.txt 

./make/CD3D9Driver_Renderer.o : CD3D9Driver_Renderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9Driver_Renderer.d" -MT"./make/CD3D9Driver_Renderer.d" -o"./make/CD3D9Driver_Renderer.o" "CD3D9Driver_Renderer.cpp" 2>> ./make/result.txt 

./make/CD3D9TextureWriteServices.o : CD3D9TextureWriteServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9TextureWriteServices.d" -MT"./make/CD3D9TextureWriteServices.d" -o"./make/CD3D9TextureWriteServices.o" "CD3D9TextureWriteServices.cpp" 2>> ./make/result.txt 

./make/CD3D9_PS30.o : CD3D9_PS30.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9_PS30.d" -MT"./make/CD3D9_PS30.d" -o"./make/CD3D9_PS30.o" "CD3D9_PS30.cpp" 2>> ./make/result.txt 

./make/CD3D9RenderTarget.o : CD3D9RenderTarget.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9RenderTarget.d" -MT"./make/CD3D9RenderTarget.d" -o"./make/CD3D9RenderTarget.o" "CD3D9RenderTarget.cpp" 2>> ./make/result.txt 

./make/CD3D9VertexDeclaration.o : CD3D9VertexDeclaration.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9VertexDeclaration.d" -MT"./make/CD3D9VertexDeclaration.d" -o"./make/CD3D9VertexDeclaration.o" "CD3D9VertexDeclaration.cpp" 2>> ./make/result.txt 

./make/CD3D9_VS30.o : CD3D9_VS30.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9_VS30.d" -MT"./make/CD3D9_VS30.d" -o"./make/CD3D9_VS30.o" "CD3D9_VS30.cpp" 2>> ./make/result.txt 

./make/CDSAudioPlayer.o : CDSAudioPlayer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CDSAudioPlayer.d" -MT"./make/CDSAudioPlayer.d" -o"./make/CDSAudioPlayer.o" "CDSAudioPlayer.cpp" 2>> ./make/result.txt 

./make/CDSSound.o : CDSSound.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CDSSound.d" -MT"./make/CDSSound.d" -o"./make/CDSSound.o" "CDSSound.cpp" 2>> ./make/result.txt 

./make/CBLPImage.o : CBLPImage.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CBLPImage.d" -MT"./make/CBLPImage.d" -o"./make/CBLPImage.o" "CBLPImage.cpp" 2>> ./make/result.txt 

./make/CFileADTLoader.o : CFileADTLoader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CFileADTLoader.d" -MT"./make/CFileADTLoader.d" -o"./make/CFileADTLoader.o" "CFileADTLoader.cpp" 2>> ./make/result.txt 

./make/CFileM2Loader.o : CFileM2Loader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CFileM2Loader.d" -MT"./make/CFileM2Loader.d" -o"./make/CFileM2Loader.o" "CFileM2Loader.cpp" 2>> ./make/result.txt 

./make/CFileWDTLoader.o : CFileWDTLoader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CFileWDTLoader.d" -MT"./make/CFileWDTLoader.d" -o"./make/CFileWDTLoader.o" "CFileWDTLoader.cpp" 2>> ./make/result.txt 

./make/CFileWMOLoader.o : CFileWMOLoader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CFileWMOLoader.d" -MT"./make/CFileWMOLoader.d" -o"./make/CFileWMOLoader.o" "CFileWMOLoader.cpp" 2>> ./make/result.txt 

./make/CGLES2DrawServices.o : CGLES2DrawServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2DrawServices.d" -MT"./make/CGLES2DrawServices.d" -o"./make/CGLES2DrawServices.o" "CGLES2DrawServices.cpp" 2>> ./make/result.txt 

./make/CGLES2Driver_base.o : CGLES2Driver_base.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2Driver_base.d" -MT"./make/CGLES2Driver_base.d" -o"./make/CGLES2Driver_base.o" "CGLES2Driver_base.cpp" 2>> ./make/result.txt 

./make/CGLES2Driver_draw.o : CGLES2Driver_draw.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2Driver_draw.d" -MT"./make/CGLES2Driver_draw.d" -o"./make/CGLES2Driver_draw.o" "CGLES2Driver_draw.cpp" 2>> ./make/result.txt 

./make/CGLES2Driver_Renderer.o : CGLES2Driver_Renderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2Driver_Renderer.d" -MT"./make/CGLES2Driver_Renderer.d" -o"./make/CGLES2Driver_Renderer.o" "CGLES2Driver_Renderer.cpp" 2>> ./make/result.txt 

./make/CGLES2Extension.o : CGLES2Extension.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2Extension.d" -MT"./make/CGLES2Extension.d" -o"./make/CGLES2Extension.o" "CGLES2Extension.cpp" 2>> ./make/result.txt 

./make/CGLES2HardwareBufferServices.o : CGLES2HardwareBufferServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2HardwareBufferServices.d" -MT"./make/CGLES2HardwareBufferServices.d" -o"./make/CGLES2HardwareBufferServices.o" "CGLES2HardwareBufferServices.cpp" 2>> ./make/result.txt 

./make/CGLES2Helper.o : CGLES2Helper.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2Helper.d" -MT"./make/CGLES2Helper.d" -o"./make/CGLES2Helper.o" "CGLES2Helper.cpp" 2>> ./make/result.txt 

./make/CGLES2ManualTextureServices.o : CGLES2ManualTextureServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2ManualTextureServices.d" -MT"./make/CGLES2ManualTextureServices.d" -o"./make/CGLES2ManualTextureServices.o" "CGLES2ManualTextureServices.cpp" 2>> ./make/result.txt 

./make/CGLES2MaterialRenderer.o : CGLES2MaterialRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2MaterialRenderer.d" -MT"./make/CGLES2MaterialRenderer.d" -o"./make/CGLES2MaterialRenderer.o" "CGLES2MaterialRenderer.cpp" 2>> ./make/result.txt 

./make/CGLES2MaterialRenderServices.o : CGLES2MaterialRenderServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2MaterialRenderServices.d" -MT"./make/CGLES2MaterialRenderServices.d" -o"./make/CGLES2MaterialRenderServices.o" "CGLES2MaterialRenderServices.cpp" 2>> ./make/result.txt 

./make/CGLES2RenderTarget.o : CGLES2RenderTarget.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2RenderTarget.d" -MT"./make/CGLES2RenderTarget.d" -o"./make/CGLES2RenderTarget.o" "CGLES2RenderTarget.cpp" 2>> ./make/result.txt 

./make/CGLES2ResourceLoader.o : CGLES2ResourceLoader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2ResourceLoader.d" -MT"./make/CGLES2ResourceLoader.d" -o"./make/CGLES2ResourceLoader.o" "CGLES2ResourceLoader.cpp" 2>> ./make/result.txt 

./make/CGLES2SceneStateServices.o : CGLES2SceneStateServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2SceneStateServices.d" -MT"./make/CGLES2SceneStateServices.d" -o"./make/CGLES2SceneStateServices.o" "CGLES2SceneStateServices.cpp" 2>> ./make/result.txt 

./make/CGLES2Shader.o : CGLES2Shader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2Shader.d" -MT"./make/CGLES2Shader.d" -o"./make/CGLES2Shader.o" "CGLES2Shader.cpp" 2>> ./make/result.txt 

./make/CGLES2ShaderServices.o : CGLES2ShaderServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2ShaderServices.d" -MT"./make/CGLES2ShaderServices.d" -o"./make/CGLES2ShaderServices.o" "CGLES2ShaderServices.cpp" 2>> ./make/result.txt 

./make/CGLES2Texture.o : CGLES2Texture.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2Texture.d" -MT"./make/CGLES2Texture.d" -o"./make/CGLES2Texture.o" "CGLES2Texture.cpp" 2>> ./make/result.txt 

./make/CGLES2TextureWriteServices.o : CGLES2TextureWriteServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2TextureWriteServices.d" -MT"./make/CGLES2TextureWriteServices.d" -o"./make/CGLES2TextureWriteServices.o" "CGLES2TextureWriteServices.cpp" 2>> ./make/result.txt 

./make/CGLES2VertexDeclaration.o : CGLES2VertexDeclaration.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2VertexDeclaration.d" -MT"./make/CGLES2VertexDeclaration.d" -o"./make/CGLES2VertexDeclaration.o" "CGLES2VertexDeclaration.cpp" 2>> ./make/result.txt 

./make/CGLES2_PS.o : CGLES2_PS.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2_PS.d" -MT"./make/CGLES2_PS.d" -o"./make/CGLES2_PS.o" "CGLES2_PS.cpp" 2>> ./make/result.txt 

./make/CGLES2_VS.o : CGLES2_VS.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGLES2_VS.d" -MT"./make/CGLES2_VS.d" -o"./make/CGLES2_VS.o" "CGLES2_VS.cpp" 2>> ./make/result.txt 

./make/CImageLoaderPVR.o : CImageLoaderPVR.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CImageLoaderPVR.d" -MT"./make/CImageLoaderPVR.d" -o"./make/CImageLoaderPVR.o" "CImageLoaderPVR.cpp" 2>> ./make/result.txt 

./make/CKTXImage.o : CKTXImage.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CKTXImage.d" -MT"./make/CKTXImage.d" -o"./make/CKTXImage.o" "CKTXImage.cpp" 2>> ./make/result.txt 

./make/CMemDbg.o : CMemDbg.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMemDbg.d" -MT"./make/CMemDbg.d" -o"./make/CMemDbg.o" "CMemDbg.cpp" 2>> ./make/result.txt 

./make/CMemFile.o : CMemFile.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMemFile.d" -MT"./make/CMemFile.d" -o"./make/CMemFile.o" "CMemFile.cpp" 2>> ./make/result.txt 

./make/CMeshDecalRenderer.o : CMeshDecalRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMeshDecalRenderer.d" -MT"./make/CMeshDecalRenderer.d" -o"./make/CMeshDecalRenderer.o" "CMeshDecalRenderer.cpp" 2>> ./make/result.txt 

./make/CMeshDecalServices.o : CMeshDecalServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMeshDecalServices.d" -MT"./make/CMeshDecalServices.d" -o"./make/CMeshDecalServices.o" "CMeshDecalServices.cpp" 2>> ./make/result.txt 

./make/CMiniDump.o : CMiniDump.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMiniDump.d" -MT"./make/CMiniDump.d" -o"./make/CMiniDump.o" "CMiniDump.cpp" 2>> ./make/result.txt 

./make/CMP3Input.o : CMP3Input.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMP3Input.d" -MT"./make/CMP3Input.d" -o"./make/CMP3Input.o" "CMP3Input.cpp" 2>> ./make/result.txt 

./make/COpenGLDrawServices.o : COpenGLDrawServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLDrawServices.d" -MT"./make/COpenGLDrawServices.d" -o"./make/COpenGLDrawServices.o" "COpenGLDrawServices.cpp" 2>> ./make/result.txt 

./make/COpenGLDriver_base.o : COpenGLDriver_base.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLDriver_base.d" -MT"./make/COpenGLDriver_base.d" -o"./make/COpenGLDriver_base.o" "COpenGLDriver_base.cpp" 2>> ./make/result.txt 

./make/COpenGLDriver_draw.o : COpenGLDriver_draw.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLDriver_draw.d" -MT"./make/COpenGLDriver_draw.d" -o"./make/COpenGLDriver_draw.o" "COpenGLDriver_draw.cpp" 2>> ./make/result.txt 

./make/COpenGLDriver_Renderer.o : COpenGLDriver_Renderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLDriver_Renderer.d" -MT"./make/COpenGLDriver_Renderer.d" -o"./make/COpenGLDriver_Renderer.o" "COpenGLDriver_Renderer.cpp" 2>> ./make/result.txt 

./make/COpenGLExtension.o : COpenGLExtension.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLExtension.d" -MT"./make/COpenGLExtension.d" -o"./make/COpenGLExtension.o" "COpenGLExtension.cpp" 2>> ./make/result.txt 

./make/COpenGLHardwareBufferServices.o : COpenGLHardwareBufferServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLHardwareBufferServices.d" -MT"./make/COpenGLHardwareBufferServices.d" -o"./make/COpenGLHardwareBufferServices.o" "COpenGLHardwareBufferServices.cpp" 2>> ./make/result.txt 

./make/COpenGLHelper.o : COpenGLHelper.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLHelper.d" -MT"./make/COpenGLHelper.d" -o"./make/COpenGLHelper.o" "COpenGLHelper.cpp" 2>> ./make/result.txt 

./make/COpenGLManualTextureServices.o : COpenGLManualTextureServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLManualTextureServices.d" -MT"./make/COpenGLManualTextureServices.d" -o"./make/COpenGLManualTextureServices.o" "COpenGLManualTextureServices.cpp" 2>> ./make/result.txt 

./make/COpenGLMaterialRenderer.o : COpenGLMaterialRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLMaterialRenderer.d" -MT"./make/COpenGLMaterialRenderer.d" -o"./make/COpenGLMaterialRenderer.o" "COpenGLMaterialRenderer.cpp" 2>> ./make/result.txt 

./make/COpenGLMaterialRenderServices.o : COpenGLMaterialRenderServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLMaterialRenderServices.d" -MT"./make/COpenGLMaterialRenderServices.d" -o"./make/COpenGLMaterialRenderServices.o" "COpenGLMaterialRenderServices.cpp" 2>> ./make/result.txt 

./make/COpenGLRenderTarget.o : COpenGLRenderTarget.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLRenderTarget.d" -MT"./make/COpenGLRenderTarget.d" -o"./make/COpenGLRenderTarget.o" "COpenGLRenderTarget.cpp" 2>> ./make/result.txt 

./make/COpenGLResourceLoader.o : COpenGLResourceLoader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLResourceLoader.d" -MT"./make/COpenGLResourceLoader.d" -o"./make/COpenGLResourceLoader.o" "COpenGLResourceLoader.cpp" 2>> ./make/result.txt 

./make/COpenGLSceneStateServices.o : COpenGLSceneStateServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLSceneStateServices.d" -MT"./make/COpenGLSceneStateServices.d" -o"./make/COpenGLSceneStateServices.o" "COpenGLSceneStateServices.cpp" 2>> ./make/result.txt 

./make/COpenGLShader.o : COpenGLShader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLShader.d" -MT"./make/COpenGLShader.d" -o"./make/COpenGLShader.o" "COpenGLShader.cpp" 2>> ./make/result.txt 

./make/COpenGLShaderServices.o : COpenGLShaderServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLShaderServices.d" -MT"./make/COpenGLShaderServices.d" -o"./make/COpenGLShaderServices.o" "COpenGLShaderServices.cpp" 2>> ./make/result.txt 

./make/COpenGLTexture.o : COpenGLTexture.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLTexture.d" -MT"./make/COpenGLTexture.d" -o"./make/COpenGLTexture.o" "COpenGLTexture.cpp" 2>> ./make/result.txt 

./make/COpenGLTextureWriteServices.o : COpenGLTextureWriteServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLTextureWriteServices.d" -MT"./make/COpenGLTextureWriteServices.d" -o"./make/COpenGLTextureWriteServices.o" "COpenGLTextureWriteServices.cpp" 2>> ./make/result.txt 

./make/COpenGLVertexDeclaration.o : COpenGLVertexDeclaration.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGLVertexDeclaration.d" -MT"./make/COpenGLVertexDeclaration.d" -o"./make/COpenGLVertexDeclaration.o" "COpenGLVertexDeclaration.cpp" 2>> ./make/result.txt 

./make/COpenGL_PS12.o : COpenGL_PS12.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGL_PS12.d" -MT"./make/COpenGL_PS12.d" -o"./make/COpenGL_PS12.o" "COpenGL_PS12.cpp" 2>> ./make/result.txt 

./make/COpenGL_PS15.o : COpenGL_PS15.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGL_PS15.d" -MT"./make/COpenGL_PS15.d" -o"./make/COpenGL_PS15.o" "COpenGL_PS15.cpp" 2>> ./make/result.txt 

./make/COpenGL_VS12.o : COpenGL_VS12.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGL_VS12.d" -MT"./make/COpenGL_VS12.d" -o"./make/COpenGL_VS12.o" "COpenGL_VS12.cpp" 2>> ./make/result.txt 

./make/COpenGL_VS15.o : COpenGL_VS15.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/COpenGL_VS15.d" -MT"./make/COpenGL_VS15.d" -o"./make/COpenGL_VS15.o" "COpenGL_VS15.cpp" 2>> ./make/result.txt 

./make/CPVRImage.o : CPVRImage.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CPVRImage.d" -MT"./make/CPVRImage.d" -o"./make/CPVRImage.o" "CPVRImage.cpp" 2>> ./make/result.txt 

./make/CResourceLoader.o : CResourceLoader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CResourceLoader.d" -MT"./make/CResourceLoader.d" -o"./make/CResourceLoader.o" "CResourceLoader.cpp" 2>> ./make/result.txt 

./make/CRibbonRenderer.o : CRibbonRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CRibbonRenderer.d" -MT"./make/CRibbonRenderer.d" -o"./make/CRibbonRenderer.o" "CRibbonRenderer.cpp" 2>> ./make/result.txt 

./make/CSceneEnvironment.o : CSceneEnvironment.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CSceneEnvironment.d" -MT"./make/CSceneEnvironment.d" -o"./make/CSceneEnvironment.o" "CSceneEnvironment.cpp" 2>> ./make/result.txt 

./make/CSkySceneNode.o : CSkySceneNode.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CSkySceneNode.d" -MT"./make/CSkySceneNode.d" -o"./make/CSkySceneNode.o" "CSkySceneNode.cpp" 2>> ./make/result.txt 

./make/CSpecialTextureServices.o : CSpecialTextureServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CSpecialTextureServices.d" -MT"./make/CSpecialTextureServices.d" -o"./make/CSpecialTextureServices.o" "CSpecialTextureServices.cpp" 2>> ./make/result.txt 

./make/CSysUtility.o : CSysUtility.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CSysUtility.d" -MT"./make/CSysUtility.d" -o"./make/CSysUtility.o" "CSysUtility.cpp" 2>> ./make/result.txt 

./make/CTimer.o : CTimer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CTimer.d" -MT"./make/CTimer.d" -o"./make/CTimer.o" "CTimer.cpp" 2>> ./make/result.txt 

./make/CParticleRenderer.o : CParticleRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CParticleRenderer.d" -MT"./make/CParticleRenderer.d" -o"./make/CParticleRenderer.o" "CParticleRenderer.cpp" 2>> ./make/result.txt 

./make/CTransluscentDecalRenderer.o : CTransluscentDecalRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CTransluscentDecalRenderer.d" -MT"./make/CTransluscentDecalRenderer.d" -o"./make/CTransluscentDecalRenderer.o" "CTransluscentDecalRenderer.cpp" 2>> ./make/result.txt 

./make/CVorbisInput.o : CVorbisInput.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CVorbisInput.d" -MT"./make/CVorbisInput.d" -o"./make/CVorbisInput.o" "CVorbisInput.cpp" 2>> ./make/result.txt 

./make/CWavInput.o : CWavInput.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CWavInput.d" -MT"./make/CWavInput.d" -o"./make/CWavInput.o" "CWavInput.cpp" 2>> ./make/result.txt 

./make/CBlit.o : CBlit.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CBlit.d" -MT"./make/CBlit.d" -o"./make/CBlit.o" "CBlit.cpp" 2>> ./make/result.txt 

./make/CCamera.o : CCamera.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CCamera.d" -MT"./make/CCamera.d" -o"./make/CCamera.o" "CCamera.cpp" 2>> ./make/result.txt 

./make/CD3D9Driver_base.o : CD3D9Driver_base.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9Driver_base.d" -MT"./make/CD3D9Driver_base.d" -o"./make/CD3D9Driver_base.o" "CD3D9Driver_base.cpp" 2>> ./make/result.txt 

./make/CD3D9Driver_draw.o : CD3D9Driver_draw.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9Driver_draw.d" -MT"./make/CD3D9Driver_draw.d" -o"./make/CD3D9Driver_draw.o" "CD3D9Driver_draw.cpp" 2>> ./make/result.txt 

./make/CD3D9SceneStateServices.o : CD3D9SceneStateServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9SceneStateServices.d" -MT"./make/CD3D9SceneStateServices.d" -o"./make/CD3D9SceneStateServices.o" "CD3D9SceneStateServices.cpp" 2>> ./make/result.txt 

./make/CFTFont.o : CFTFont.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CFTFont.d" -MT"./make/CFTFont.d" -o"./make/CFTFont.o" "CFTFont.cpp" 2>> ./make/result.txt 

./make/CD3D9HardwareBufferServices.o : CD3D9HardwareBufferServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9HardwareBufferServices.d" -MT"./make/CD3D9HardwareBufferServices.d" -o"./make/CD3D9HardwareBufferServices.o" "CD3D9HardwareBufferServices.cpp" 2>> ./make/result.txt 

./make/CD3D9Helper.o : CD3D9Helper.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9Helper.d" -MT"./make/CD3D9Helper.d" -o"./make/CD3D9Helper.o" "CD3D9Helper.cpp" 2>> ./make/result.txt 

./make/CD3D9ManualTextureServices.o : CD3D9ManualTextureServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9ManualTextureServices.d" -MT"./make/CD3D9ManualTextureServices.d" -o"./make/CD3D9ManualTextureServices.o" "CD3D9ManualTextureServices.cpp" 2>> ./make/result.txt 

./make/CD3D9MaterialRenderer.o : CD3D9MaterialRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9MaterialRenderer.d" -MT"./make/CD3D9MaterialRenderer.d" -o"./make/CD3D9MaterialRenderer.o" "CD3D9MaterialRenderer.cpp" 2>> ./make/result.txt 

./make/CD3D9MaterialRenderServices.o : CD3D9MaterialRenderServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9MaterialRenderServices.d" -MT"./make/CD3D9MaterialRenderServices.d" -o"./make/CD3D9MaterialRenderServices.o" "CD3D9MaterialRenderServices.cpp" 2>> ./make/result.txt 

./make/CD3D9_PS20.o : CD3D9_PS20.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9_PS20.d" -MT"./make/CD3D9_PS20.d" -o"./make/CD3D9_PS20.o" "CD3D9_PS20.cpp" 2>> ./make/result.txt 

./make/CD3D9ResourceLoader.o : CD3D9ResourceLoader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9ResourceLoader.d" -MT"./make/CD3D9ResourceLoader.d" -o"./make/CD3D9ResourceLoader.o" "CD3D9ResourceLoader.cpp" 2>> ./make/result.txt 

./make/CD3D9Shader.o : CD3D9Shader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9Shader.d" -MT"./make/CD3D9Shader.d" -o"./make/CD3D9Shader.o" "CD3D9Shader.cpp" 2>> ./make/result.txt 

./make/CD3D9ShaderServices.o : CD3D9ShaderServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9ShaderServices.d" -MT"./make/CD3D9ShaderServices.d" -o"./make/CD3D9ShaderServices.o" "CD3D9ShaderServices.cpp" 2>> ./make/result.txt 

./make/CD3D9Texture.o : CD3D9Texture.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9Texture.d" -MT"./make/CD3D9Texture.d" -o"./make/CD3D9Texture.o" "CD3D9Texture.cpp" 2>> ./make/result.txt 

./make/CD3D9_VS20.o : CD3D9_VS20.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CD3D9_VS20.d" -MT"./make/CD3D9_VS20.d" -o"./make/CD3D9_VS20.o" "CD3D9_VS20.cpp" 2>> ./make/result.txt 

./make/CFileADT.o : CFileADT.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CFileADT.d" -MT"./make/CFileADT.d" -o"./make/CFileADT.o" "CFileADT.cpp" 2>> ./make/result.txt 

./make/CFileM2.o : CFileM2.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CFileM2.d" -MT"./make/CFileM2.d" -o"./make/CFileM2.o" "CFileM2.cpp" 2>> ./make/result.txt 

./make/CFileSystem.o : CFileSystem.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CFileSystem.d" -MT"./make/CFileSystem.d" -o"./make/CFileSystem.o" "CFileSystem.cpp" 2>> ./make/result.txt 

./make/CFileWDT.o : CFileWDT.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CFileWDT.d" -MT"./make/CFileWDT.d" -o"./make/CFileWDT.o" "CFileWDT.cpp" 2>> ./make/result.txt 

./make/CFileWMO.o : CFileWMO.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CFileWMO.d" -MT"./make/CFileWMO.d" -o"./make/CFileWMO.o" "CFileWMO.cpp" 2>> ./make/result.txt 

./make/CGeometryCreator.o : CGeometryCreator.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CGeometryCreator.d" -MT"./make/CGeometryCreator.d" -o"./make/CGeometryCreator.o" "CGeometryCreator.cpp" 2>> ./make/result.txt 

./make/CImage.o : CImage.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CImage.d" -MT"./make/CImage.d" -o"./make/CImage.o" "CImage.cpp" 2>> ./make/result.txt 

./make/CImageLoaderBLP.o : CImageLoaderBLP.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CImageLoaderBLP.d" -MT"./make/CImageLoaderBLP.d" -o"./make/CImageLoaderBLP.o" "CImageLoaderBLP.cpp" 2>> ./make/result.txt 

./make/CDxInputReader.o : CDxInputReader.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CDxInputReader.d" -MT"./make/CDxInputReader.d" -o"./make/CDxInputReader.o" "CDxInputReader.cpp" 2>> ./make/result.txt 

./make/CM2SceneNode.o : CM2SceneNode.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CM2SceneNode.d" -MT"./make/CM2SceneNode.d" -o"./make/CM2SceneNode.o" "CM2SceneNode.cpp" 2>> ./make/result.txt 

./make/CManualMeshServices.o : CManualMeshServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CManualMeshServices.d" -MT"./make/CManualMeshServices.d" -o"./make/CManualMeshServices.o" "CManualMeshServices.cpp" 2>> ./make/result.txt 

./make/CMapEnvironment.o : CMapEnvironment.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMapEnvironment.d" -MT"./make/CMapEnvironment.d" -o"./make/CMapEnvironment.o" "CMapEnvironment.cpp" 2>> ./make/result.txt 

./make/CMapTileSceneNode.o : CMapTileSceneNode.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMapTileSceneNode.d" -MT"./make/CMapTileSceneNode.d" -o"./make/CMapTileSceneNode.o" "CMapTileSceneNode.cpp" 2>> ./make/result.txt 

./make/CMesh.o : CMesh.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMesh.d" -MT"./make/CMesh.d" -o"./make/CMesh.o" "CMesh.cpp" 2>> ./make/result.txt 

./make/CMeshRenderer.o : CMeshRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMeshRenderer.d" -MT"./make/CMeshRenderer.d" -o"./make/CMeshRenderer.o" "CMeshRenderer.cpp" 2>> ./make/result.txt 

./make/CMeshSceneNode.o : CMeshSceneNode.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CMeshSceneNode.d" -MT"./make/CMeshSceneNode.d" -o"./make/CMeshSceneNode.o" "CMeshSceneNode.cpp" 2>> ./make/result.txt 

./make/CTerrainRenderer.o : CTerrainRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CTerrainRenderer.d" -MT"./make/CTerrainRenderer.d" -o"./make/CTerrainRenderer.o" "CTerrainRenderer.cpp" 2>> ./make/result.txt 

./make/CParticleSystemSceneNode.o : CParticleSystemSceneNode.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CParticleSystemSceneNode.d" -MT"./make/CParticleSystemSceneNode.d" -o"./make/CParticleSystemSceneNode.o" "CParticleSystemSceneNode.cpp" 2>> ./make/result.txt 

./make/CParticleSystemServices.o : CParticleSystemServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CParticleSystemServices.d" -MT"./make/CParticleSystemServices.d" -o"./make/CParticleSystemServices.o" "CParticleSystemServices.cpp" 2>> ./make/result.txt 

./make/CReadFile.o : CReadFile.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CReadFile.d" -MT"./make/CReadFile.d" -o"./make/CReadFile.o" "CReadFile.cpp" 2>> ./make/result.txt 

./make/CRibbonEmitterServices.o : CRibbonEmitterServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CRibbonEmitterServices.d" -MT"./make/CRibbonEmitterServices.d" -o"./make/CRibbonEmitterServices.o" "CRibbonEmitterServices.cpp" 2>> ./make/result.txt 

./make/CRibbonSceneNode.o : CRibbonSceneNode.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CRibbonSceneNode.d" -MT"./make/CRibbonSceneNode.d" -o"./make/CRibbonSceneNode.o" "CRibbonSceneNode.cpp" 2>> ./make/result.txt 

./make/CSceneManager.o : CSceneManager.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CSceneManager.d" -MT"./make/CSceneManager.d" -o"./make/CSceneManager.o" "CSceneManager.cpp" 2>> ./make/result.txt 

./make/CSceneRenderServices.o : CSceneRenderServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CSceneRenderServices.d" -MT"./make/CSceneRenderServices.d" -o"./make/CSceneRenderServices.o" "CSceneRenderServices.cpp" 2>> ./make/result.txt 

./make/CTerrainServices.o : CTerrainServices.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CTerrainServices.d" -MT"./make/CTerrainServices.d" -o"./make/CTerrainServices.o" "CTerrainServices.cpp" 2>> ./make/result.txt 

./make/CTransluscentRenderer.o : CTransluscentRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CTransluscentRenderer.d" -MT"./make/CTransluscentRenderer.d" -o"./make/CTransluscentRenderer.o" "CTransluscentRenderer.cpp" 2>> ./make/result.txt 

./make/CWDTSceneNode.o : CWDTSceneNode.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CWDTSceneNode.d" -MT"./make/CWDTSceneNode.d" -o"./make/CWDTSceneNode.o" "CWDTSceneNode.cpp" 2>> ./make/result.txt 

./make/CWmoRenderer.o : CWmoRenderer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CWmoRenderer.d" -MT"./make/CWmoRenderer.d" -o"./make/CWmoRenderer.o" "CWmoRenderer.cpp" 2>> ./make/result.txt 

./make/CWMOSceneNode.o : CWMOSceneNode.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CWMOSceneNode.d" -MT"./make/CWMOSceneNode.d" -o"./make/CWMOSceneNode.o" "CWMOSceneNode.cpp" 2>> ./make/result.txt 

./make/CWriteFile.o : CWriteFile.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CWriteFile.d" -MT"./make/CWriteFile.d" -o"./make/CWriteFile.o" "CWriteFile.cpp" 2>> ./make/result.txt 

./make/CX2AudioPlayer.o : CX2AudioPlayer.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CX2AudioPlayer.d" -MT"./make/CX2AudioPlayer.d" -o"./make/CX2AudioPlayer.o" "CX2AudioPlayer.cpp" 2>> ./make/result.txt 

./make/CX2Sound.o : CX2Sound.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/CX2Sound.d" -MT"./make/CX2Sound.d" -o"./make/CX2Sound.o" "CX2Sound.cpp" 2>> ./make/result.txt 

./make/ddslib.o : ddslib.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/ddslib.d" -MT"./make/ddslib.d" -o"./make/ddslib.o" "ddslib.cpp" 2>> ./make/result.txt 

./make/engine.o : engine.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/engine.d" -MT"./make/engine.d" -o"./make/engine.o" "engine.cpp" 2>> ./make/result.txt 

./make/enginesetting.o : enginesetting.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/enginesetting.d" -MT"./make/enginesetting.d" -o"./make/enginesetting.o" "enginesetting.cpp" 2>> ./make/result.txt 

./make/ftsystem_memory.o : ftsystem_memory.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/ftsystem_memory.d" -MT"./make/ftsystem_memory.d" -o"./make/ftsystem_memory.o" "ftsystem_memory.cpp" 2>> ./make/result.txt 

./make/ftsystem_mpq.o : ftsystem_mpq.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/ftsystem_mpq.d" -MT"./make/ftsystem_mpq.d" -o"./make/ftsystem_mpq.o" "ftsystem_mpq.cpp" 2>> ./make/result.txt 

./make/mpq_stormlib.o : mpq_stormlib.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/mpq_stormlib.d" -MT"./make/mpq_stormlib.d" -o"./make/mpq_stormlib.o" "mpq_stormlib.cpp" 2>> ./make/result.txt 

./make/mywow.o : mywow.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/mywow.d" -MT"./make/mywow.d" -o"./make/mywow.o" "mywow.cpp" 2>> ./make/result.txt 

./make/pvrlib.o : pvrlib.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/pvrlib.d" -MT"./make/pvrlib.d" -o"./make/pvrlib.o" "pvrlib.cpp" 2>> ./make/result.txt 

./make/q_memory.o : q_memory.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/q_memory.d" -MT"./make/q_memory.d" -o"./make/q_memory.o" "q_memory.cpp" 2>> ./make/result.txt 

./make/stdafx.o : stdafx.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/stdafx.d" -MT"./make/stdafx.d" -o"./make/stdafx.o" "stdafx.cpp" 2>> ./make/result.txt 

./make/stormlib_memory.o : stormlib_memory.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/stormlib_memory.d" -MT"./make/stormlib_memory.d" -o"./make/stormlib_memory.o" "stormlib_memory.cpp" 2>> ./make/result.txt 

./make/wowEnvironment.o : wowEnvironment.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wowEnvironment.d" -MT"./make/wowEnvironment.d" -o"./make/wowEnvironment.o" "wowEnvironment.cpp" 2>> ./make/result.txt 

./make/wow_m2Action.o : wow_m2Action.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_m2Action.d" -MT"./make/wow_m2Action.d" -o"./make/wow_m2Action.o" "wow_m2Action.cpp" 2>> ./make/result.txt 

./make/wow_m2FSM.o : wow_m2FSM.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_m2FSM.d" -MT"./make/wow_m2FSM.d" -o"./make/wow_m2FSM.o" "wow_m2FSM.cpp" 2>> ./make/result.txt 

./make/wow_m2appearance.o : wow_m2appearance.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_m2appearance.d" -MT"./make/wow_m2appearance.d" -o"./make/wow_m2appearance.o" "wow_m2appearance.cpp" 2>> ./make/result.txt 

./make/wow_m2instance.o : wow_m2instance.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_m2instance.d" -MT"./make/wow_m2instance.d" -o"./make/wow_m2instance.o" "wow_m2instance.cpp" 2>> ./make/result.txt 

./make/wow_database.o : wow_database.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_database.d" -MT"./make/wow_database.d" -o"./make/wow_database.o" "wow_database.cpp" 2>> ./make/result.txt 

./make/wow_dbc.o : wow_dbc.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_dbc.d" -MT"./make/wow_dbc.d" -o"./make/wow_dbc.o" "wow_dbc.cpp" 2>> ./make/result.txt 

./make/wow_m2Move.o : wow_m2Move.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_m2Move.d" -MT"./make/wow_m2Move.d" -o"./make/wow_m2Move.o" "wow_m2Move.cpp" 2>> ./make/result.txt 

./make/wow_m2spell.o : wow_m2spell.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_m2spell.d" -MT"./make/wow_m2spell.d" -o"./make/wow_m2spell.o" "wow_m2spell.cpp" 2>> ./make/result.txt 

./make/wow_particle.o : wow_particle.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_particle.d" -MT"./make/wow_particle.d" -o"./make/wow_particle.o" "wow_particle.cpp" 2>> ./make/result.txt 

./make/wow_tileScene.o : wow_tileScene.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_tileScene.d" -MT"./make/wow_tileScene.d" -o"./make/wow_tileScene.o" "wow_tileScene.cpp" 2>> ./make/result.txt 

./make/wow_wdtScene.o : wow_wdtScene.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_wdtScene.d" -MT"./make/wow_wdtScene.d" -o"./make/wow_wdtScene.o" "wow_wdtScene.cpp" 2>> ./make/result.txt 

./make/wow_wmoScene.o : wow_wmoScene.cpp
	g++ $(COMPILE_OPTIONS) -MF"./make/wow_wmoScene.d" -MT"./make/wow_wmoScene.d" -o"./make/wow_wmoScene.o" "wow_wmoScene.cpp" 2>> ./make/result.txt 


#====================================
# prepare
#====================================
prepare : 
	rm -r -f ./make;mkdir ./make

#====================================
# clean
#====================================
clean : 
	rm -r -f ./make
