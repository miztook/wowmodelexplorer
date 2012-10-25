#pragma once

#include "IFileWMO.h"

class CWMOLoader
{
public:
	bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "wmo"); }

	IFileWMO* loadWMO( MPQFile* file );
};

class CFileWMO : public IFileWMO
{
private:
	CFileWMO();
	~CFileWMO();

	friend class CWMOLoader;

public:
	virtual bool loadFile(MPQFile* file);
	virtual u8* getFileData() const { return FileData; }
	virtual aabbox3df getBoundingBox() const { return Box; }

	virtual bool buildVideoResources();
	virtual void releaseVideoResources();

private:
	void clear();

private:
	u8*			FileData;
	aabbox3df	Box;
};

