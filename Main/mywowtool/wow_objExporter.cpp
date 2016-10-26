#include "stdafx.h"
#include "wow_objExporter.h"
#include "mywow.h"
#include "wow_exportUtility.h"

#include "CFileM2.h"
#include "CFileWMO.h"

wowObjExporter::wowObjExporter()
{

}

wowObjExporter::~wowObjExporter()
{

}

bool wowObjExporter::exportM2SceneNode( IM2SceneNode* node, const c8* filename )
{
	const CFileM2* pFile = static_cast<const CFileM2*>(node->getFileM2());
	if (!pFile)
		return false;

	wow_m2instance* pM2Instance = node->getM2Instance();
	if (!pM2Instance)
		return false;

	//Create Folder
	char szfilename[MAX_PATH];
	getFileDirA(filename, szfilename, MAX_PATH);
	string512 strExportFolder = szfilename;
	strExportFolder.normalizeDir();

	AUX_CreateDirectory(strExportFolder.c_str());

	IWriteFile* pObjFileToSave = g_Engine->getFileSystem()->createAndWriteFile(filename, false);
	if (!pObjFileToSave)
		return false;

	string512 strMtlFileName = filename;
	strMtlFileName.changeExt("obj", "mtl");
	IWriteFile* pMtlFileToSave = g_Engine->getFileSystem()->createAndWriteFile(strMtlFileName.c_str(), false);
	if (!pMtlFileToSave)
	{
		delete pObjFileToSave;
		return false;
	}

	if (!exportFileM2(pObjFileToSave, pMtlFileToSave, pFile, pM2Instance))
	{
		delete pMtlFileToSave;
		delete pObjFileToSave;
		return false;
	}

	delete pMtlFileToSave;
	delete pObjFileToSave;
	return true;


	return true;
}

bool wowObjExporter::exportWMOSceneNode( IWMOSceneNode* node, const c8* filename )
{
	const CFileWMO* pFile = static_cast<const CFileWMO*>(node->getFileWMO());
	if (!pFile)
		return false;

	//Create Folder
	char szfilename[MAX_PATH];
	getFileDirA(filename, szfilename, MAX_PATH);
	string512 strExportFolder = szfilename;
	strExportFolder.normalizeDir();

	AUX_CreateDirectory(strExportFolder.c_str());

	IWriteFile* pObjFileToSave = g_Engine->getFileSystem()->createAndWriteFile(filename, false);
	if (!pObjFileToSave)
		return false;

	string512 strMtlFileName = filename;
	strMtlFileName.changeExt("obj", "mtl");
	IWriteFile* pMtlFileToSave = g_Engine->getFileSystem()->createAndWriteFile(strMtlFileName.c_str(), false);
	if (!pMtlFileToSave)
	{
		delete pObjFileToSave;
		return false;
	}

	if (!exportFileWMO(pObjFileToSave, pMtlFileToSave, pFile))
	{
		delete pMtlFileToSave;
		delete pObjFileToSave;
		return false;
	}

	delete pMtlFileToSave;
	delete pObjFileToSave;
	return true;
}

bool wowObjExporter::exportWMOSceneNodeGroups(IWMOSceneNode* node, const c8* filename)
{
	const CFileWMO* pFile = static_cast<const CFileWMO*>(node->getFileWMO());
	if (!pFile)
		return false;

	//Create Folder
	char szfilename[MAX_PATH];
	getFileDirA(filename, szfilename, MAX_PATH);
	string512 strExportFolder = szfilename;
	strExportFolder.normalizeDir();

	AUX_CreateDirectory(strExportFolder.c_str());

	//Create Group SubFolder
	u32 numGroup = pFile->getNumGroups();
	for (u32 i=0; i<numGroup; ++i)
	{
		string512 strGroupFolder;
		strGroupFolder.format("%sgroup%u", strExportFolder.c_str(), i);
		strGroupFolder.normalizeDir();
		AUX_CreateDirectory(strGroupFolder.c_str());

		getFileNameNoExtensionA(filename, szfilename, MAX_PATH);
		string512 strFileName;
		strFileName.format("%s%s_%u.obj", strGroupFolder.c_str(), szfilename, i);

		if(!exportFileWMOGroup(strFileName.c_str(), pFile, i))
			return false;
	}

	return true;
}

bool wowObjExporter::exportFileM2(IWriteFile* pFileObj, IWriteFile* pFileMtl, const CFileM2* pFileM2, wow_m2instance* pM2Instance)
{
	char	szLine[AFILE_LINEMAXLEN]; 

	Q_sprintf(szLine, AFILE_LINEMAXLEN, "# Exported from mywow Engine");
	pFileObj->writeLine(szLine);

	char szfilename[MAX_PATH];
	getFileNameA(pFileMtl->getFileName(), szfilename, MAX_PATH);

	Q_sprintf(szLine, AFILE_LINEMAXLEN, "mtllib %s\n", szfilename);
	pFileObj->writeLine(szLine);

	//Create Textures Folder
	getFileDirA(pFileMtl->getFileName(), szfilename, MAX_PATH);
	string512 strTextureFolder = szfilename;
	strTextureFolder.append("/Textures/");
	AUX_CreateDirectory(strTextureFolder.c_str());

	if (!exportFileM2Vertices(pFileObj, pFileM2))
	{
		CSysUtility::outputDebug("wowObjExporter::exportFileM2 failed at exportFileM2Vertices");
		return false;
	}

	if (!exportFileM2Materials(pFileMtl, pFileM2, pM2Instance))
	{
		CSysUtility::outputDebug("wowObjExporter::exportFileM2 failed at exportFileM2Materials");
		return false;
	}

	return true;
}

bool wowObjExporter::exportFileM2Vertices(IWriteFile* pFile, const CFileM2* pFileM2)
{
	char	szLine[AFILE_LINEMAXLEN]; 

	CFileSkin* pFileSkin = pFileM2->Skin;
	if (!pFileSkin)
		return false;

	for (u32 i=0; i<pFileSkin->NumGeosets; ++i)
	{
		CGeoset* pGeoSet = &pFileSkin->Geosets[i];

		const STexUnit* texUnit = pGeoSet->getTexUnit(0);
		s16 rfIndex = texUnit->rfIndex;
		if (rfIndex == -1 || pFileM2->RenderFlags[rfIndex].invisible)
			continue;

		string256 strName;
		strName.format("mesh_geoset%u", i);
		int nVerts =  (int)pGeoSet->VCount;
		int nFaces = (int)pGeoSet->ICount / 3;

		string256 strMatName;
		strMatName.format("mesh_geoset%u", i);
		bool b2Side = !pGeoSet->BillBoard;
		if (b2Side)
			strMatName.append("_Dbl");

		//name
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "o %s", strName.c_str());
		pFile->writeLine(szLine);

		//v
		for (int n=0; n<nVerts; ++n)
		{
			SVertex_PNT2W& v = pFileM2->GVertices[pGeoSet->VStart + n];
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "v %0.6f %0.6f %0.6f", v.Pos.X, v.Pos.Y, v.Pos.Z);
			pFile->writeLine(szLine);
		}

		//vt
		for (int n=0; n<nVerts; ++n)
		{
			SVertex_PNT2W& v = pFileM2->GVertices[pGeoSet->VStart + n];
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "vt %0.6f %0.6f", v.TCoords0.X, 1.0f - v.TCoords0.Y);
			pFile->writeLine(szLine);
		}

		//vn
		for (int n=0; n<nVerts; ++n)
		{
			SVertex_PNT2W& v = pFileM2->GVertices[pGeoSet->VStart + n];
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "vn %0.6f %0.6f %0.6f", v.Normal.X, v.Normal.Y, v.Normal.Z);
			pFile->writeLine(szLine);
		}

		//group, usemtl, smooth, face
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "g %s", strName.c_str());
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "usemtl %s", strMatName.c_str());
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "s 1");
		pFile->writeLine(szLine);

		for(int n=0; n<nFaces; ++n)
		{
			int idx0 = (int)pFileSkin->Indices[pGeoSet->IStart + n*3] + 1;
			int idx1 = (int)pFileSkin->Indices[pGeoSet->IStart + n*3 + 1] + 1;
			int idx2 = (int)pFileSkin->Indices[pGeoSet->IStart + n*3 + 2] + 1;

			ASSERT(idx0 > 0 && idx0 <= (int)(pGeoSet->VStart + pGeoSet->VCount));
			ASSERT(idx1 > 0 && idx1 <= (int)(pGeoSet->VStart + pGeoSet->VCount));
			ASSERT(idx2 > 0 && idx2 <= (int)(pGeoSet->VStart + pGeoSet->VCount));

			Q_sprintf(szLine, AFILE_LINEMAXLEN, "f %d/%d/%d %d/%d/%d %d/%d/%d", idx0, idx0, idx0, idx1, idx1, idx1, idx2, idx2, idx2);
			pFile->writeLine(szLine);
		}

		pFile->writeLine("");			//\n
	}

	return true;
}

bool wowObjExporter::exportFileM2Materials(IWriteFile* pFile, const CFileM2* pFileM2, wow_m2instance* pM2Instance)
{
	char	szLine[AFILE_LINEMAXLEN]; 

	CFileSkin* pFileSkin = pFileM2->Skin;
	if (!pFileSkin)
		return false;

	for (u32 i=0; i<pFileSkin->NumGeosets; ++i)
	{
		CGeoset* pGeoSet = &pFileSkin->Geosets[i];

		const STexUnit* texUnit = pGeoSet->getTexUnit(0);
		s16 rfIndex = texUnit->rfIndex;
		if (rfIndex == -1 || pFileM2->RenderFlags[rfIndex].invisible)
			continue;

		const IFileM2::SRenderFlag& renderflag = pFileM2->RenderFlags[rfIndex];

		string256 strMatName;
		strMatName.format("mesh_geoset%u", i);
		bool b2Side = !pGeoSet->BillBoard;
		if (b2Side)
			strMatName.append("_Dbl");

		Q_sprintf(szLine, AFILE_LINEMAXLEN, "newmtl %s", strMatName.c_str());
		pFile->writeLine(szLine);

		Q_sprintf(szLine, AFILE_LINEMAXLEN, "illum 2");
		pFile->writeLine(szLine);

		SColorf cAmbient(1.0f, 1.0f, 1.0f);
		SColorf cDiffuse(1.0f, 1.0f, 1.0f);
		SColorf cSpecular(0.0f, 0.0f, 0.0f);
		SColorf cEmissive(0.0f, 0.0f, 0.0f);
		float fPower = 1.0f;

		Q_sprintf(szLine, AFILE_LINEMAXLEN, "d 1");
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ka %0.6f %0.6f %0.6f", cAmbient.r, cAmbient.g, cAmbient.b);
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "Kd %0.6f %0.6f %0.6f", cDiffuse.r, cDiffuse.g, cDiffuse.b);
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ks %0.6f %0.6f %0.6f", cSpecular.r, cSpecular.g, cSpecular.b);
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ke %0.6f %0.6f %0.6f", cEmissive.r, cEmissive.g, cEmissive.b);
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ns %0.6f", fPower);
		pFile->writeLine(szLine);

		ITexture* tex = NULL;		//get texture 0
		{
			s16 texID = pGeoSet->getTexUnit(0)->TexID;

			if (texID != -1)
			{
				ETextureTypes texType = pFileM2->TextureTypes[texID];
				if (texType == TEXTURE_FILENAME)
					tex = pFileM2->getTexture(texID);
				else
					tex = pM2Instance->ReplaceTextures[(u32)texType];
			}
		}

		char szfilename[MAX_PATH] = {0};
		if (tex)
			getFileNameA(tex->getFileName(), szfilename, MAX_PATH);
		string512 strTexture = "Textures/";
		strTexture.append(szfilename);
		strTexture.changeExt("blp", "tga");

		Q_sprintf(szLine, AFILE_LINEMAXLEN, "map_Ki %s", strTexture.c_str());
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "map_Kd %s", strTexture.c_str());
		pFile->writeLine(szLine);

		pFile->writeLine("");			//\n

		//Copy & Transform Textures
		char szdirname[MAX_PATH] = {0};
		getFileDirA(pFile->getFileName(), szdirname, MAX_PATH);
		string512 strTextureSrc;
		if(tex)
			strTextureSrc = tex->getFileName();

		string512 strTextureDest = szdirname;
		strTextureDest.append("/Textures/");
		strTextureDest.append(szfilename);
		strTextureDest.changeExt("blp", "tga");
		AUX_ExportBlpAsTga(strTextureSrc.c_str(), strTextureDest.c_str(), true);
		//AUX_CopyFile(strTextureDest.c_str(), strTextureSrc.c_str());
	}

	return true;
}

bool wowObjExporter::exportFileWMO(IWriteFile* pFileObj, IWriteFile* pFileMtl, const CFileWMO* Wmo)
{
	char	szLine[AFILE_LINEMAXLEN]; 

	Q_sprintf(szLine, AFILE_LINEMAXLEN, "# Exported from mywow Engine");
	pFileObj->writeLine(szLine);

	char szfilename[MAX_PATH];
	getFileNameA(pFileMtl->getFileName(), szfilename, MAX_PATH);

	Q_sprintf(szLine, AFILE_LINEMAXLEN, "mtllib %s\n", szfilename);
	pFileObj->writeLine(szLine);

	//Create Textures Folder
	getFileDirA(pFileMtl->getFileName(), szfilename, MAX_PATH);
	string512 strTextureFolder = szfilename;
	strTextureFolder.append("/Textures/");
	AUX_CreateDirectory(strTextureFolder.c_str());

	if (!exportWMOVertices(pFileObj, Wmo))
	{
		CSysUtility::outputDebug("wowObjExporter::exportFileWMO failed at exportWMOVertices");
		return false;
	}

	if (!exportWMOMaterials(pFileMtl, Wmo))
	{
		CSysUtility::outputDebug("wowObjExporter::exportFileWMO failed at exportWMOMaterials");
		return false;
	}

	return true;
}

bool wowObjExporter::exportWMOVertices(IWriteFile* pFile, const CFileWMO* Wmo)
{
	char	szLine[AFILE_LINEMAXLEN]; 

	for (u32 i=0; i<Wmo->Header.nGroups; ++i)
	{
		const CWMOGroup* group = &Wmo->Groups[i];
		for (u32 c=0; c<group->NumBatches; ++c)
		{
			const SWMOBatch* batch = &group->Batches[c];
			const SWMOMaterial* material = &Wmo->Materials[batch->matId]; 

			string256 strName;
			strName.format("mesh_group%u_%u", i, c);
			int nVerts =  (int)batch->getVertexCount();
			int nFaces = (int)batch->indexCount / 3;

			string256 strMatName;
			strMatName.format("mat_group_%u_%u", i, c);
			bool b2Side = (material->flags & 0x4) == 0;
			if (b2Side)
				strMatName.append("_Dbl");

			//name
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "o %s", strName.c_str());
			pFile->writeLine(szLine);

			//v
			for (int n=0; n<nVerts; ++n)
			{
				SVertex_PNCT2& v = Wmo->Vertices[group->VStart + batch->vertexStart + n];
				Q_sprintf(szLine, AFILE_LINEMAXLEN, "v %0.6f %0.6f %0.6f", v.Pos.X, v.Pos.Y, v.Pos.Z);
				pFile->writeLine(szLine);
			}

			//vt
			for (int n=0; n<nVerts; ++n)
			{
				SVertex_PNCT2& v = Wmo->Vertices[group->VStart + batch->vertexStart + n];
				Q_sprintf(szLine, AFILE_LINEMAXLEN, "vt %0.6f %0.6f", v.TCoords0.X, 1.0f - v.TCoords0.Y);
				pFile->writeLine(szLine);
			}

			//vn
			for (int n=0; n<nVerts; ++n)
			{
				SVertex_PNCT2& v = Wmo->Vertices[group->VStart + batch->vertexStart + n];
				Q_sprintf(szLine, AFILE_LINEMAXLEN, "vn %0.6f %0.6f %0.6f", v.Normal.X, v.Normal.Y, v.Normal.Z);
				pFile->writeLine(szLine);
			}

			//group, usemtl, smooth, face
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "g %s", strName.c_str());
			pFile->writeLine(szLine);
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "usemtl %s", strMatName.c_str());
			pFile->writeLine(szLine);
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "s 1");
			pFile->writeLine(szLine);

			for(int n=0; n<nFaces; ++n)
			{
				int idx0 = (int)Wmo->Indices[group->IStart + batch->indexStart + n*3] + 1;
				int idx1 = (int)Wmo->Indices[group->IStart + batch->indexStart + n*3 + 1] + 1;
				int idx2 = (int)Wmo->Indices[group->IStart + batch->indexStart + n*3 + 2] + 1;

				ASSERT(idx0 > 0 && idx0 <= (int)(group->VStart + batch->vertexStart + batch->getVertexCount()));
				ASSERT(idx1 > 0 && idx1 <= (int)(group->VStart + batch->vertexStart + batch->getVertexCount()));
				ASSERT(idx2 > 0 && idx2 <= (int)(group->VStart + batch->vertexStart + batch->getVertexCount()));

				Q_sprintf(szLine, AFILE_LINEMAXLEN, "f %d/%d/%d %d/%d/%d %d/%d/%d", idx0, idx0, idx0, idx1, idx1, idx1, idx2, idx2, idx2);
				pFile->writeLine(szLine);
			}

			pFile->writeLine("");			//\n
		}
	}

	return true;
}

bool wowObjExporter::exportWMOMaterials(IWriteFile* pFile, const CFileWMO* Wmo)
{
	char	szLine[AFILE_LINEMAXLEN]; 

	for (u32 i=0; i<Wmo->Header.nGroups; ++i)
	{
		const CWMOGroup* group = &Wmo->Groups[i];
		for (u32 c=0; c<group->NumBatches; ++c)
		{
			const SWMOBatch* batch = &group->Batches[c];
			const SWMOMaterial* material = &Wmo->Materials[batch->matId]; 

			string256 strMatName;
			strMatName.format("mat_group_%u_%u", i, c);
			bool b2Side = (material->flags & 0x4) == 0;
			if (b2Side)
				strMatName.append("_Dbl");

			Q_sprintf(szLine, AFILE_LINEMAXLEN, "newmtl %s", strMatName.c_str());
			pFile->writeLine(szLine);

			Q_sprintf(szLine, AFILE_LINEMAXLEN, "illum 2");
			pFile->writeLine(szLine);

			SColorf cAmbient(1.0f, 1.0f, 1.0f);
			SColorf cDiffuse(1.0f, 1.0f, 1.0f);
			SColorf cSpecular(0.0f, 0.0f, 0.0f);
			SColorf cEmissive(0.0f, 0.0f, 0.0f);
			float fPower = 1.0f;

			Q_sprintf(szLine, AFILE_LINEMAXLEN, "d 1");
			pFile->writeLine(szLine);
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ka %0.6f %0.6f %0.6f", cAmbient.r, cAmbient.g, cAmbient.b);
			pFile->writeLine(szLine);
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "Kd %0.6f %0.6f %0.6f", cDiffuse.r, cDiffuse.g, cDiffuse.b);
			pFile->writeLine(szLine);
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ks %0.6f %0.6f %0.6f", cSpecular.r, cSpecular.g, cSpecular.b);
			pFile->writeLine(szLine);
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ke %0.6f %0.6f %0.6f", cEmissive.r, cEmissive.g, cEmissive.b);
			pFile->writeLine(szLine);
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ns %0.6f", fPower);
			pFile->writeLine(szLine);

			char szfilename[MAX_PATH] = {0};
			if (material->texture0)
				getFileNameA(material->texture0->getFileName(), szfilename, MAX_PATH);
			string512 strTexture = "Textures/";
			strTexture.append(szfilename);
			strTexture.changeExt("blp", "tga");

			Q_sprintf(szLine, AFILE_LINEMAXLEN, "map_Ki %s", strTexture.c_str());
			pFile->writeLine(szLine);
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "map_Kd %s", strTexture.c_str());
			pFile->writeLine(szLine);

			pFile->writeLine("");			//\n

			//Copy & Transform Textures
			char szdirname[MAX_PATH] = {0};
			getFileDirA(pFile->getFileName(), szdirname, MAX_PATH);
			string512 strTextureSrc;
			if(material->texture0)
				strTextureSrc = material->texture0->getFileName();
			
			string512 strTextureDest = szdirname;
			strTextureDest.append("/Textures/");
			strTextureDest.append(szfilename);
			strTextureDest.changeExt("blp", "tga");
			AUX_ExportBlpAsTga(strTextureSrc.c_str(), strTextureDest.c_str(), true);
			//AUX_CopyFile(strTextureDest.c_str(), strTextureSrc.c_str());
		}
	}

	return true;
}

bool wowObjExporter::exportFileWMOGroup(const c8* filename, const CFileWMO* Wmo, u32 iGroup)
{
	IWriteFile* pFileObj = g_Engine->getFileSystem()->createAndWriteFile(filename, false);
	if (!pFileObj)
		return false;

	string512 strMtlFileName = filename;
	strMtlFileName.changeExt("obj", "mtl");
	IWriteFile* pFileMtl = g_Engine->getFileSystem()->createAndWriteFile(strMtlFileName.c_str(), false);
	if (!pFileMtl)
	{
		delete pFileObj;
		return false;
	}

	{
		char	szLine[AFILE_LINEMAXLEN]; 

		Q_sprintf(szLine, AFILE_LINEMAXLEN, "# Exported from mywow Engine");
		pFileObj->writeLine(szLine);

		char szfilename[MAX_PATH];
		getFileNameA(pFileMtl->getFileName(), szfilename, MAX_PATH);

		Q_sprintf(szLine, AFILE_LINEMAXLEN, "mtllib %s\n", szfilename);
		pFileObj->writeLine(szLine);

		//Create Textures Folder
		getFileDirA(pFileMtl->getFileName(), szfilename, MAX_PATH);
		string512 strTextureFolder = szfilename;
		strTextureFolder.append("/Textures/");
		AUX_CreateDirectory(strTextureFolder.c_str());

		if (!exportWMOGroupVertices(pFileObj, Wmo, iGroup))
		{
			CSysUtility::outputDebug("wowObjExporter::exportFileWMO failed at exportWMOVertices");
			delete pFileMtl;
			delete pFileObj;
			return false;
		}

		if (!exportWMOGroupMaterials(pFileMtl, Wmo, iGroup))
		{
			CSysUtility::outputDebug("wowObjExporter::exportFileWMO failed at exportWMOMaterials");
			delete pFileMtl;
			delete pFileObj;
			return false;
		}
	}

	delete pFileMtl;
	delete pFileObj;
	return true;
}

bool wowObjExporter::exportWMOGroupVertices(IWriteFile* pFile, const CFileWMO* Wmo, u32 iGroup)
{
	char	szLine[AFILE_LINEMAXLEN]; 

	const CWMOGroup* group = &Wmo->Groups[iGroup];
	for (u32 c=0; c<group->NumBatches; ++c)
	{
		const SWMOBatch* batch = &group->Batches[c];
		const SWMOMaterial* material = &Wmo->Materials[batch->matId]; 

		string256 strName;
		strName.format("mesh_group%u_%u", iGroup, c);
		int nVerts =  (int)batch->getVertexCount();
		int nFaces = (int)batch->indexCount / 3;

		string256 strMatName;
		strMatName.format("mat_group_%u_%u", iGroup, c);
		bool b2Side = (material->flags & 0x4) == 0;
		if (b2Side)
			strMatName.append("_Dbl");

		//name
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "o %s", strName.c_str());
		pFile->writeLine(szLine);

		//v
		for (int n=0; n<nVerts; ++n)
		{
			SVertex_PNCT2& v = Wmo->Vertices[group->VStart + batch->vertexStart + n];
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "v %0.6f %0.6f %0.6f", v.Pos.X, v.Pos.Y, v.Pos.Z);
			pFile->writeLine(szLine);
		}

		//vt
		for (int n=0; n<nVerts; ++n)
		{
			SVertex_PNCT2& v = Wmo->Vertices[group->VStart + batch->vertexStart + n];
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "vt %0.6f %0.6f", v.TCoords0.X, 1.0f - v.TCoords0.Y);
			pFile->writeLine(szLine);
		}

		//vn
		for (int n=0; n<nVerts; ++n)
		{
			SVertex_PNCT2& v = Wmo->Vertices[group->VStart + batch->vertexStart + n];
			Q_sprintf(szLine, AFILE_LINEMAXLEN, "vn %0.6f %0.6f %0.6f", v.Normal.X, v.Normal.Y, v.Normal.Z);
			pFile->writeLine(szLine);
		}

		//group, usemtl, smooth, face
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "g %s", strName.c_str());
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "usemtl %s", strMatName.c_str());
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "s 1");
		pFile->writeLine(szLine);

		for(int n=0; n<nFaces; ++n)
		{
			int idx0 = (int)Wmo->Indices[group->IStart + batch->indexStart + n*3] - group->VStart + 1;
			int idx1 = (int)Wmo->Indices[group->IStart + batch->indexStart + n*3 + 1] - group->VStart + 1;
			int idx2 = (int)Wmo->Indices[group->IStart + batch->indexStart + n*3 + 2] - group->VStart + 1;

			ASSERT(idx0 > 0 && idx0 <= (int)(batch->vertexStart + batch->getVertexCount()));
			ASSERT(idx1 > 0 && idx1 <= (int)(batch->vertexStart + batch->getVertexCount()));
			ASSERT(idx2 > 0 && idx2 <= (int)(batch->vertexStart + batch->getVertexCount()));

			Q_sprintf(szLine, AFILE_LINEMAXLEN, "f %d/%d/%d %d/%d/%d %d/%d/%d", idx0, idx0, idx0, idx1, idx1, idx1, idx2, idx2, idx2);
			pFile->writeLine(szLine);
		}

		pFile->writeLine("");			//\n
	}

	return true;
}

bool wowObjExporter::exportWMOGroupMaterials(IWriteFile* pFile, const CFileWMO* Wmo, u32 iGroup)
{
	char	szLine[AFILE_LINEMAXLEN]; 

	const CWMOGroup* group = &Wmo->Groups[iGroup];
	for (u32 c=0; c<group->NumBatches; ++c)
	{
		const SWMOBatch* batch = &group->Batches[c];
		const SWMOMaterial* material = &Wmo->Materials[batch->matId]; 

		string256 strMatName;
		strMatName.format("mat_group_%u_%u", iGroup, c);
		bool b2Side = (material->flags & 0x4) == 0;
		if (b2Side)
			strMatName.append("_Dbl");

		Q_sprintf(szLine, AFILE_LINEMAXLEN, "newmtl %s", strMatName.c_str());
		pFile->writeLine(szLine);

		Q_sprintf(szLine, AFILE_LINEMAXLEN, "illum 2");
		pFile->writeLine(szLine);

		SColorf cAmbient(1.0f, 1.0f, 1.0f);
		SColorf cDiffuse(1.0f, 1.0f, 1.0f);
		SColorf cSpecular(0.0f, 0.0f, 0.0f);
		SColorf cEmissive(0.0f, 0.0f, 0.0f);
		float fPower = 1.0f;

		Q_sprintf(szLine, AFILE_LINEMAXLEN, "d 1");
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ka %0.6f %0.6f %0.6f", cAmbient.r, cAmbient.g, cAmbient.b);
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "Kd %0.6f %0.6f %0.6f", cDiffuse.r, cDiffuse.g, cDiffuse.b);
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ks %0.6f %0.6f %0.6f", cSpecular.r, cSpecular.g, cSpecular.b);
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ke %0.6f %0.6f %0.6f", cEmissive.r, cEmissive.g, cEmissive.b);
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "Ns %0.6f", fPower);
		pFile->writeLine(szLine);

		char szfilename[MAX_PATH] = {0};
		if (material->texture0)
			getFileNameA(material->texture0->getFileName(), szfilename, MAX_PATH);
		string512 strTexture = "Textures/";
		strTexture.append(szfilename);
		strTexture.changeExt("blp", "tga");

		Q_sprintf(szLine, AFILE_LINEMAXLEN, "map_Ki %s", strTexture.c_str());
		pFile->writeLine(szLine);
		Q_sprintf(szLine, AFILE_LINEMAXLEN, "map_Kd %s", strTexture.c_str());
		pFile->writeLine(szLine);

		pFile->writeLine("");			//\n

		//Copy & Transform Textures
		char szdirname[MAX_PATH] = {0};
		getFileDirA(pFile->getFileName(), szdirname, MAX_PATH);
		string512 strTextureSrc;
		if(material->texture0)
			strTextureSrc = material->texture0->getFileName();

		string512 strTextureDest = szdirname;
		strTextureDest.append("/Textures/");
		strTextureDest.append(szfilename);
		strTextureDest.changeExt("blp", "tga");
		AUX_ExportBlpAsTga(strTextureSrc.c_str(), strTextureDest.c_str(), true);
		//AUX_CopyFile(strTextureDest.c_str(), strTextureSrc.c_str());
	}

	return true;
}
