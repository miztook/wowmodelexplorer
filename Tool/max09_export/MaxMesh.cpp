#include "stdafx.h"
#include "MaxMesh.h"

CMaxMesh::CMaxMesh()
{
	m_pINode = NULL;
	m_pTriObject = NULL;
	m_pModifier = NULL;
	m_eModifier = MODIFIER_NONE;

	m_nNumFace = 0;
	m_nNumVert = 0;
}

bool CMaxMesh::create(INode* node)
{
	m_pINode = node;

	m_pModifier = findSkinModifier(node);
	if (!m_pModifier)
	{
		_ASSERT(false);
		m_eModifier = MODIFIER_NONE;
	}
	else
	{
		m_eModifier = MODIFIER_SKIN;
	}

}

Modifier* CMaxMesh::findSkinModifier( INode* node )
{
	Object* obj = node->GetObjectRef();
	if (!obj)
		return NULL;

	const c8* name = node->GetName();
	bool bGetVal = false;
	Modifier* pModifier = NULL;

	while(obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* derivedObj = (IDerivedObject*)obj;

		int num = derivedObj->NumModifiers();
		for (int i=0; i<num; ++i)
		{
			pModifier = derivedObj->GetModifier(i);

			if (pModifier->ClassID() == SKIN_CLASSID)
			{
				if (i == num - 1)
				{
					bGetVal = true;
					break;
				}
				else
				{
					m_InitOsState = derivedObj->Eval(0, i+1);
					return pModifier;
				}
			}
		}

		//next
		obj = derivedObj->GetObjRef();

		if (bGetVal && obj)
		{
			if (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
			{
				m_InitOsState = ((IDerivedObject*)obj)->Eval(0, 0);
			}
			else
			{
				m_InitOsState = obj->Eval(0);
			}
			return pModifier;
		}
	}

	return NULL;
}
