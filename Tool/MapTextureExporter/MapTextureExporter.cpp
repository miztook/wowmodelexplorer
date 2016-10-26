#include "MapTextureExporter.h"
#include "mywow.h"
#include "CFileWDT.h"
#include "CFileADT.h"
#include "CFileWMO.h"
#include "TGAImageWriter.h"

#pragma comment(lib, "mywow.lib")

void exportMapTextures(const char* dirname);

void exportWmoTextures(const char* dirname);

int main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	SWindowInfo wndInfo = Engine::createWindow("MapTextureExporter", dimension2du(800,600), 1.0f, false, true);
	HWND hwnd = wndInfo.hwnd;

	SEngineInitParam param;
	createEngine(param, wndInfo);

	bool success = g_Engine->initDriver(EDT_OPENGL, 0, false, true, 1, true);

	g_Engine->getWowDatabase()->buildMaps();
	g_Engine->getWowDatabase()->buildWmos();

	string512 dirbase = g_Engine->getFileSystem()->getBaseDirectory();
	dirbase.append("output/");
	g_Engine->getFileSystem()->createDirectory(dirbase.c_str());

	//exportMapTextures(dirbase.c_str());
	exportWmoTextures(dirbase.c_str());

	destroyEngine();

	getchar();
	return 0;
}

typedef std::set<string512, std::less<string512>, qzone_allocator<string512> > T_FileNameSet;

void exportMapTextures(const char* dirname)
{
	wowDatabase* database = g_Engine->getWowDatabase();
	IFileSystem* fs = g_Engine->getFileSystem();

	for(u32 iMap = 0; iMap < database->getNumMaps(); ++iMap)
	{
		const SMapRecord* mapRecord = database->getMap(iMap);
		if(mapRecord->type != 2)
			continue;

		c8 mapname[MAX_PATH];
		sprintf_s(mapname, MAX_PATH, "World\\Maps\\%s\\%s.wdt", mapRecord->name, mapRecord->name);

		CFileWDT* fileWDT = static_cast<CFileWDT*>(g_Engine->getResourceLoader()->loadWDT(mapname, mapRecord->id));
		if (!fileWDT)
			continue;

		string512 wdtDir = dirname;
		wdtDir.append(mapRecord->name);
		wdtDir.append("/");

		u32 nTiles = fileWDT->getTileCount();
		T_FileNameSet texFileNameSet;
		for (u32 i=0; i<nTiles; ++i)
		{
			STile* tile = fileWDT->getTile(i);
			if(fileWDT->loadADTTextures(tile))
			{
				CFileADT* fileADT = static_cast<CFileADT*>(tile->fileAdt);
				if (fileADT && fileADT->getNumTextures() > 0)
				{
					//string512 adtDir;
					//adtDir.format("%s%d_%d/", wdtDir.c_str(), (int)tile->row, (int)tile->col);
					//fs->createDirectory(adtDir.c_str());

					for (u32 t=0; t<fileADT->getNumTextures(); ++t)
					{
						const char* texName = fileADT->getTextureName(t);
						texFileNameSet.insert(texName);
					}
				}

				fileWDT->unloadADT(tile);
			}
		}

		fileWDT->drop();

		if (!texFileNameSet.empty())
			fs->createDirectory(wdtDir.c_str());

		int nTextures = 0;
		for (T_FileNameSet::iterator itr = texFileNameSet.begin(); itr != texFileNameSet.end(); ++itr)
		{
			string512 texName = *itr;
			IImage* image = g_Engine->getResourceLoader()->loadBLPAsImage(texName.c_str(), true);
			if (image)
			{
				ASSERT(image->getColorFormat() == ECF_A8R8G8B8);
				dimension2du size = image->getDimension();

				++nTextures;
				string512 texDir;
				texDir.format("%s%d.tga", wdtDir.c_str(), nTextures);

				u32 dataSize = size.Width * size.Height * getBytesPerPixelFromFormat(ECF_R8G8B8);
				u8* data = (u8*)Z_AllocateTempMemory(dataSize);
				image->copyToScaling(data, size.Width, size.Height, ECF_R8G8B8);

				bool ret = TGAWriteFile(texDir.c_str(), size.Width, size.Height, TGA_FORMAT_BGR, data);
				ASSERT(ret);

				Z_FreeTempMemory(data);

				image->drop();

				printf("%s created.\n", texDir.c_str());
			}
		}
	}
}

void exportWmoTextures(const char* dirname)
{
	wowDatabase* database = g_Engine->getWowDatabase();
	IFileSystem* fs = g_Engine->getFileSystem();

	for (u32 iWmo = 0; iWmo < database->getNumWmos(); ++iWmo)
	{
		if (iWmo<=4000)
			continue;

		const char* wmoname = database->getWmoFileName(iWmo);

		CFileWMO* fileWMO = static_cast<CFileWMO*>(g_Engine->getResourceLoader()->loadWMO(wmoname, false));
		if (!fileWMO)
			continue;

		c8 shortname[256];
		getFileNameNoExtensionA(wmoname, shortname, 256);

		string512 wmoDir = dirname;
		wmoDir.append(shortname);
		wmoDir.append("/");

		T_FileNameSet texFileNameSet;
		for (u32 i=0; i<fileWMO->getNumGroups(); ++i)
		{
			CWMOGroup* group = &fileWMO->Groups[i];
			for (u32 c=0; c<group->NumBatches; ++c)
			{
				const SWMOBatch* batch = &group->Batches[c];
				u16 matId = batch->matId;
				const SWMOMaterial* material = &fileWMO->Materials[matId]; 
				if (material->texture0)
				{
					texFileNameSet.insert(material->texture0->getFileName());
				}
			}
		}

		fileWMO->drop();

		if (!texFileNameSet.empty())
			fs->createDirectory(wmoDir.c_str());

		int nTextures = 0;
		for (T_FileNameSet::iterator itr = texFileNameSet.begin(); itr != texFileNameSet.end(); ++itr)
		{
			string512 texName = *itr;
			IImage* image = g_Engine->getResourceLoader()->loadBLPAsImage(texName.c_str(), false);
			if (image)
			{
				ASSERT(image->getColorFormat() == ECF_A8R8G8B8);
				dimension2du size = image->getDimension();

				++nTextures;
				string512 texDir;
				texDir.format("%s%d.tga", wmoDir.c_str(), nTextures);

				//u32 dataSize = size.Width * size.Height * getBytesPerPixelFromFormat(ECF_R8G8B8);
				//u8* data = (u8*)Z_AllocateTempMemory(dataSize);
				//image->copyToScaling(data, size.Width, size.Height, ECF_R8G8B8);

				bool ret = TGAWriteFile(texDir.c_str(), size.Width, size.Height, TGA_FORMAT_BGRA, image->getData());
				ASSERT(ret);

				//Z_FreeTempMemory(data);

				image->drop();

				printf("%s created.\n", texDir.c_str());
			}
		}
	}
}