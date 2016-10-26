#pragma once

#include "CSceneManager.h"

class CEditM2SceneNode;
class CEditWMOSceneNode;

class CEditSceneManager : public CSceneManager
{
public:
	CEditSceneManager();
	~CEditSceneManager();

public:
	virtual IM2SceneNode* addM2SceneNode(IFileM2* mesh, ISceneNode* parent, bool npc = false) override;
	virtual IWMOSceneNode* addWMOSceneNode(IFileWMO* wmo, ISceneNode* parent) override;
	virtual IWDTSceneNode* addWDTSceneNode(IFileWDT* wdt, ISceneNode* parent) override;
	virtual IMapTileSceneNode* addMapTileSceneNode(IFileWDT* wdt, STile* tile, ISceneNode* parent) override;
	virtual void removeSceneNode(ISceneNode* node, bool del) override;
	virtual void drawAll(bool foreground) override;
	virtual void onWindowSizeChanged(const dimension2du& size) override;

public:
	void setBackImage(const c8* filename);
	const c8* getBackImageFileName() const { return BackImageFileName.c_str(); }

	void setCenterTexture(const c8* filename);

	IM2SceneNode*	DebugM2Node;
	IWMOSceneNode*	DebugWMONode;
	IWDTSceneNode*		DebugWDTNode;
	IMapTileSceneNode*		DebugMapTileNode;
	bool		ShowDebugM2Geosets;
	bool		TextureOpaque;

private:
	void drawM2GeosetsDebugInfo(CEditM2SceneNode* node);

	void drawBackTexture();

	void drawCenterTexture();

	void renderNormal();
	void renderRT();

	void doRender();

private:
	c16	DebugMsg[512];
	
	matrix4		OldView;
	matrix4		OldProjection;

	string256		BackImageFileName;
	IImage*		BackImage;
	ITexture*		BackTexture;
	ITexture*		CenterTexture;
};