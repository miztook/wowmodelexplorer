#pragma once

#include "max.h"
#include "base.h"

class CMaxMesh
{
public:
	enum E_MODIFIER
	{
		MODIFIER_NONE = 0,
		MODIFIER_SKIN,
	};

public:
	CMaxMesh();
	virtual ~CMaxMesh() {}

public:
	bool create(INode* node);

	Modifier*	findSkinModifier(INode* node);

private:
	INode*	m_pINode;
	TriObject*		m_pTriObject;
	Modifier*		m_pModifier;
	E_MODIFIER	m_eModifier;	
	ObjectState	m_InitOsState;

	u32		m_nNumFace;
	u32		m_nNumVert;
};